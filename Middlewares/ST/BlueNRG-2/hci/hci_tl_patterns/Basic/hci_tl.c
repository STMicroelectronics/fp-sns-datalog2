/**
  ******************************************************************************
  * @file    hci_tl.c
  * @author  AMG - RF Application Team
  * @brief   Contains the basic functions for managing the framework required
  *          for handling the HCI interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "hci_const.h"
#include "hci.h"
#include "hci_tl.h"

#define HCI_LOG_ON                      0
#define HCI_PCK_TYPE_OFFSET             0
#define EVENT_PARAMETER_TOT_LEN_OFFSET  2

/**
  * Increase this parameter to overcome possible issues due to BLE devices crowded environment
  * or high number of incoming notifications from peripheral devices
  */
#ifndef HCI_READ_PACKET_NUM_MAX
#define HCI_READ_PACKET_NUM_MAX      (5)
#endif /* HCI_READ_PACKET_NUM_MAX */

#ifndef MIN
#define MIN(a,b)      ((a) < (b))? (a) : (b)
#endif /* MIN */
#ifndef MAX
#define MAX(a,b)      ((a) > (b))? (a) : (b)
#endif /* MAX */

list_node_t             hci_read_pkt_pool;
list_node_t             hci_read_pkt_rx_queue;
static hci_data_packet_t hci_read_packet_buffer[HCI_READ_PACKET_NUM_MAX];
static hci_context_t    hci_context;

/************************* Static internal functions **************************/

/**
  * @brief  Verify the packet type.
  *
  * @param  hci_read_packet The HCI data packet
  * @retval 0: valid packet, 1: incorrect packet, 2: wrong length (packet truncated or too long)
  */
static uint32_t verify_packet(const hci_data_packet_t *hci_read_packet)
{
  const uint8_t *hci_pckt = hci_read_packet->data_buff;

  if (hci_pckt[HCI_PCK_TYPE_OFFSET] != HCI_EVENT_PKT)
  {
    return 1;  /* Incorrect type */
  }

  if (hci_pckt[EVENT_PARAMETER_TOT_LEN_OFFSET] != hci_read_packet->data_len - (1 + HCI_EVENT_HDR_SIZE))
  {
    return 2;  /* Wrong length (packet truncated or too long) */
  }

  return 0;
}

/**
  * @brief  Send an HCI command.
  *
  * @param  ogf The Opcode Group Field
  * @param  ocf The Opcode Command Field
  * @param  plen The HCI command length
  * @param  param The HCI command parameters
  * @retval None
  */
static void send_cmd(uint16_t ogf, uint16_t ocf, uint8_t plen, void *param)
{
  uint8_t payload[HCI_MAX_PAYLOAD_SIZE];
  hci_command_hdr hc;

  hc.opcode = HTOBS(CMD_OPCODE_PACK(ogf, ocf));
  hc.plen = plen;

  payload[0] = HCI_COMMAND_PKT;
  BLUENRG_MEMCPY(payload + 1, &hc, sizeof(hc));
  BLUENRG_MEMCPY(payload + HCI_HDR_SIZE + HCI_COMMAND_HDR_SIZE, param, plen);

  hci_tl_spi_send(payload, HCI_HDR_SIZE + HCI_COMMAND_HDR_SIZE + plen);
}

/**
  * @brief  Remove the tail from a source list and insert it to the head
  *         of a destination list.
  *
  * @param  dest_list
  * @param  src_list
  * @retval None
  */
static void move_list(list_node_t *dest_list, list_node_t *src_list)
{
  p_list_node_t tmp_node;

  while (!list_is_empty(src_list))
  {
    list_remove_tail(src_list, &tmp_node);
    list_insert_head(dest_list, tmp_node);
  }
}

/**
  * @brief  Free the HCI event list.
  *
  * @param  None
  * @retval None
  */
static void free_event_list(void)
{
  hci_data_packet_t *pckt;

  while (list_get_size(&hci_read_pkt_pool) < HCI_READ_PACKET_NUM_MAX / 2)
  {
    list_remove_head(&hci_read_pkt_rx_queue, (list_node_t **)&pckt);
    list_insert_tail(&hci_read_pkt_pool, (list_node_t *)pckt);
  }
}

/***************************** Exported functions *****************************/

/**
  * @brief Reset BlueNRG module.
  *
  * @param  None
  * @retval int32_t 0
  */
WEAK_FUNCTION(int32_t hci_tl_spi_reset(void))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_tl_spi_reset\r\n");

  return 0;
}

/**
  * @brief  Writes data from local buffer to SPI.
  *
  * @param  buffer : data buffer to be written
  * @param  size   : size of first data buffer to be written
  * @retval int32_t: Number of read bytes
  */
WEAK_FUNCTION(int32_t hci_tl_spi_send(uint8_t *buffer, uint16_t size))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_tl_spi_send\r\n");

  return 0;
}

/**
  * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
  *
  * @param  buffer : Buffer where data from SPI are stored
  * @param  size   : Buffer size
  * @retval int32_t: Number of read bytes
  */
WEAK_FUNCTION(int32_t hci_tl_spi_receive(uint8_t *buffer, uint16_t size))
{
  /* NOTE : This function Should not be modified, when needed,
            the callback could be implemented in the user file
   */
  BLUENRG_PRINTF("hci_tl_spi_receive\r\n");

  return 0;
}

/*********************** HCI Transport layer functions ************************/

void hci_init(void(* user_evt_rx)(void *p_data), void *p_conf)
{
  uint8_t index;

  if (user_evt_rx != NULL)
  {
    hci_context.user_evt_rx = user_evt_rx;
  }

  /* Initialize list heads of ready and free hci data packet queues */
  list_init_head(&hci_read_pkt_pool);
  list_init_head(&hci_read_pkt_rx_queue);

  /* Initialize TL BLE layer */
  hci_tl_lowlevel_init();

  /* Initialize the queue of free hci data packets */
  for (index = 0; index < HCI_READ_PACKET_NUM_MAX; index++)
  {
    list_insert_tail(&hci_read_pkt_pool, (list_node_t *)&hci_read_packet_buffer[index]);
  }

  /* Initialize low level driver */
  hci_tl_spi_reset();
}

int32_t hci_send_req(struct hci_request *r, BOOL async)
{
  uint8_t *ptr;
  uint16_t opcode = HTOBS(CMD_OPCODE_PACK(r->ogf, r->ocf));
  hci_event_pckt *event_pckt;
  hci_spi_pckt *hci_hdr;

  hci_data_packet_t *hci_read_packet = NULL;
  list_node_t hci_temp_queue;

  list_init_head(&hci_temp_queue);

  free_event_list();

  send_cmd(r->ogf, r->ocf, r->command_len, r->cparam);

  if (async)
  {
    return 0;
  }

  while (1)
  {
    evt_cmd_complete  *cc;
    evt_cmd_status    *cs;
    evt_le_meta_event *me;
    uint32_t len;

    uint32_t tickstart = HAL_GetTick();

    while (1)
    {
      if ((HAL_GetTick() - tickstart) > HCI_DEFAULT_TIMEOUT_MS)
      {
        goto failed;
      }

      if (!list_is_empty(&hci_read_pkt_rx_queue))
      {
        break;
      }
    }

    /* Extract packet from HCI event queue. */
    list_remove_head(&hci_read_pkt_rx_queue, (list_node_t **)&hci_read_packet);

    hci_hdr = (void *)hci_read_packet->data_buff;

    if (hci_hdr->type == HCI_EVENT_PKT)
    {
      event_pckt = (void *)(hci_hdr->data);

      ptr = hci_read_packet->data_buff + (1 + HCI_EVENT_HDR_SIZE);
      len = hci_read_packet->data_len - (1 + HCI_EVENT_HDR_SIZE);

      switch (event_pckt->evt)
      {
        case EVT_CMD_STATUS:
          cs = (void *) ptr;

          if (cs->opcode != opcode)
          {
            goto failed;
          }

          if (r->event != EVT_CMD_STATUS)
          {
            if (cs->status)
            {
              goto failed;
            }
            break;
          }

          r->rlen = MIN(len, r->rlen);
          BLUENRG_MEMCPY(r->rparam, ptr, r->rlen);
          goto done;

        case EVT_CMD_COMPLETE:
          cc = (void *) ptr;

          if (cc->opcode != opcode)
          {
            goto failed;
          }

          ptr += EVT_CMD_COMPLETE_SIZE;
          len -= EVT_CMD_COMPLETE_SIZE;

          r->rlen = MIN(len, r->rlen);
          BLUENRG_MEMCPY(r->rparam, ptr, r->rlen);
          goto done;

        case EVT_LE_META_EVENT:
          me = (void *) ptr;

          if (me->subevent != r->event)
          {
            break;
          }

          len -= 1;
          r->rlen = MIN(len, r->rlen);
          BLUENRG_MEMCPY(r->rparam, me->data, r->rlen);
          goto done;

        case EVT_HARDWARE_ERROR:
          goto failed;

        default:
          break;
      }
    }

    /* If there are no more packets to be processed, be sure there is at list one
       packet in the pool to process the expected event.
       If no free packets are available, discard the processed event and insert it
       into the pool. */
    if (list_is_empty(&hci_read_pkt_pool) && list_is_empty(&hci_read_pkt_rx_queue))
    {
      list_insert_tail(&hci_read_pkt_pool, (list_node_t *)hci_read_packet);
      hci_read_packet = NULL;
    }
    else
    {
      /* Insert the packet in a different queue. These packets will be
      inserted back in the main queue just before exiting from send_req(), so that
      these events can be processed by the application.
      */
      list_insert_tail(&hci_temp_queue, (list_node_t *)hci_read_packet);
      hci_read_packet = NULL;
    }
  }

failed:
  if (hci_read_packet != NULL)
  {
    list_insert_head(&hci_read_pkt_pool, (list_node_t *)hci_read_packet);
  }
  move_list(&hci_read_pkt_rx_queue, &hci_temp_queue);

  return -1;

done:
  /* Insert the packet back into the pool.*/
  list_insert_head(&hci_read_pkt_pool, (list_node_t *)hci_read_packet);
  move_list(&hci_read_pkt_rx_queue, &hci_temp_queue);

  return 0;
}

void hci_user_evt_proc(void)
{
  hci_data_packet_t *hci_read_packet = NULL;

  /* process any pending events read */
  while (list_is_empty(&hci_read_pkt_rx_queue) == FALSE)
  {
    list_remove_head(&hci_read_pkt_rx_queue, (list_node_t **)&hci_read_packet);

    if (hci_context.user_evt_rx != NULL)
    {
      hci_context.user_evt_rx(hci_read_packet->data_buff);
    }

    list_insert_tail(&hci_read_pkt_pool, (list_node_t *)hci_read_packet);
  }
}

int32_t hci_notify_asynch_evt(void *pdata)
{
  hci_data_packet_t *hci_read_packet = NULL;
  uint8_t data_len;

  int32_t ret = 0;

  if (list_is_empty(&hci_read_pkt_pool) == FALSE)
  {
    /* Queuing a packet to read */
    list_remove_head(&hci_read_pkt_pool, (list_node_t **)&hci_read_packet);

    data_len = hci_tl_spi_receive(hci_read_packet->data_buff, HCI_READ_PACKET_SIZE);
    if (data_len > 0)
    {
      hci_read_packet->data_len = data_len;
      if (verify_packet(hci_read_packet) == 0)
      {
        list_insert_tail(&hci_read_pkt_rx_queue, (list_node_t *)hci_read_packet);
      }
      else
        list_insert_head(&hci_read_pkt_pool, (list_node_t *)hci_read_packet);
    }
    else
    {
      /* Insert the packet back into the pool*/
      list_insert_head(&hci_read_pkt_pool, (list_node_t *)hci_read_packet);
    }
  }
  else
  {
    ret = 1;
  }
  return ret;

}

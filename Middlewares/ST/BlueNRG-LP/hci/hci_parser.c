/**
  ******************************************************************************
  * @file    hci_parser.c 
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.3.0
  * @date    20-July-2023
  * @brief   HCI parser file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bluenrg_lp_types.h"
//#include "osal.h"
#include "hci_parser.h"
//#include "SDK_EVAL_Config.h"
#include "hci.h"
#include "hci_const.h"

/* Added for UART version only */
#define HCI_PACKET_SIZE         532 // Because of extended ACI commands, size can be bigger than standard HCI commands
static uint8_t hci_buffer[HCI_PACKET_SIZE];
static uint16_t hci_pckt_len = 0;

/**
* @brief Parses ACI commands, ACL and vendor packets
* @param buff Pointer to the received data
* @param len Length of the received data
* @retval None
*/
void hci_input_cmd(uint8_t *buff, uint16_t len)
{
  static hci_state state = WAITING_TYPE;
    
  static uint16_t collected_payload_len = 0;
  static uint16_t payload_len;
  static uint16_t header_len;
  uint8_t byte;
  uint16_t i = 0;
  
  while(hci_pckt_len < HCI_PACKET_SIZE && i++ < len){
        
    byte = *buff++;

    if(state == WAITING_TYPE)
      hci_pckt_len = 0;
    
    hci_buffer[hci_pckt_len++] = byte;        
        
    if(state == WAITING_TYPE){
      
      state = WAITING_HEADER;
      
      if(byte == HCI_COMMAND_PKT){
        header_len = HCI_TYPE_SIZE + HCI_COMMAND_HDR_SIZE;
      }
      else if(byte == HCI_COMMAND_EXT_PKT){
        header_len = HCI_TYPE_SIZE + HCI_COMMAND_EXT_HDR_SIZE;
      }
      else if(byte == HCI_ACLDATA_PKT){
        header_len = HCI_TYPE_SIZE + HCI_ACL_HDR_SIZE;
      }
      else if(byte == HCI_VENDOR_PKT){
        header_len = HCI_TYPE_SIZE + HCI_VENDOR_HDR_SIZE;
      }
      else {
        state = WAITING_TYPE;        
      }
    }
    else if(state == WAITING_HEADER){
      
      if(hci_pckt_len == header_len){
                    
        // The entire header has been received
        uint8_t pckt_type = hci_buffer[0];
        collected_payload_len = 0;
        payload_len = 0;
        
        if(pckt_type == HCI_COMMAND_PKT){
          hci_cmd_hdr *hdr = (hci_cmd_hdr *)(hci_buffer+HCI_TYPE_SIZE);
          payload_len = hdr->plen;
        }
        else if(pckt_type == HCI_COMMAND_EXT_PKT){
          hci_cmd_ext_hdr *hdr = (hci_cmd_ext_hdr *)(hci_buffer+HCI_TYPE_SIZE);
          payload_len = hdr->plen;
        }
        else if(pckt_type == HCI_ACLDATA_PKT){
          hci_acl_hdr *hdr = (hci_acl_hdr *)(hci_buffer+HCI_TYPE_SIZE);
          payload_len = hdr->dlen;
        }
        else if(pckt_type == HCI_VENDOR_PKT){
          hci_vendor_hdr *hdr = (hci_vendor_hdr *)(hci_buffer+HCI_TYPE_SIZE);
          payload_len = hdr->plen;
        }
        if(payload_len == 0){
          state = WAITING_TYPE;
          packet_received(hci_buffer, hci_pckt_len);
        }
        else {
          state = WAITING_PAYLOAD;                      
        }
      }      
    }
    else if(state == WAITING_PAYLOAD){
      collected_payload_len++;
      if(collected_payload_len >= payload_len){
        state = WAITING_TYPE;
        packet_received(hci_buffer, hci_pckt_len);
      }      
    }
  }
  
}

/**
* @brief Parses ACI events only
* @param buff Pointer to the received data
* @param len Length of the received data
* @retval None
*/
void hci_input_event(uint8_t *buff, uint16_t len)
{
  static hci_state state = WAITING_TYPE;
    
  static uint16_t collected_payload_len = 0;
  static uint16_t payload_len;
  static uint16_t header_len;
  uint8_t byte;
  uint16_t i = 0;
        
  if(state == WAITING_TYPE)
    hci_pckt_len = 0;
    
  while(hci_pckt_len < HCI_PACKET_SIZE && i++ < len){
        
    byte = *buff++;

    if(state == WAITING_TYPE)
      hci_pckt_len = 0;
    
    hci_buffer[hci_pckt_len++] = byte;        
        
    if(state == WAITING_TYPE){
      /* Only events are accepted. ACL packets not used. */
      
      state = WAITING_HEADER;
      
      if(byte == HCI_EVENT_PKT){
        header_len = HCI_TYPE_SIZE + HCI_EVENT_HDR_SIZE;
      }
      else if(byte == HCI_EVENT_EXT_PKT){
        header_len = HCI_TYPE_SIZE + HCI_EVENT_EXT_HDR_SIZE;
      }
      else {
        state = WAITING_TYPE;        
      }
    }
    else if(state == WAITING_HEADER){
      
      if(hci_pckt_len == header_len){
                    
        // The entire header has been received
        uint8_t pckt_type = hci_buffer[0];
        collected_payload_len = 0;
        payload_len = 0;
        
        if(pckt_type == HCI_EVENT_PKT){
          hci_event_hdr *hdr = (hci_event_hdr *)(hci_buffer+HCI_TYPE_SIZE);
          payload_len = hdr->plen;
        }
        else if(pckt_type == HCI_EVENT_EXT_PKT){
          hci_event_ext_hdr *hdr = (hci_event_ext_hdr *)(hci_buffer+HCI_TYPE_SIZE);
          payload_len = hdr->plen;
        }
        if(payload_len == 0){
          state = WAITING_TYPE;
          packet_received(hci_buffer, hci_pckt_len);
        }
        else {
          state = WAITING_PAYLOAD;                      
        }
      }      
    }
    else if(state == WAITING_PAYLOAD){
      collected_payload_len++;
      if(collected_payload_len >= payload_len){
        state = WAITING_TYPE;
        packet_received(hci_buffer, hci_pckt_len);
      }
    }
  }
}

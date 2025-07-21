/**
  ******************************************************************************
  * @file    hci_tl.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.0.0
  * @date    25-September-2024
  * @brief   Header file for framework required for handling HCI interface.
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

#ifndef HCI_TL_H
#define HCI_TL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hci_tl_interface.h"
#include "ble_types.h"
#include "ble_list.h"
#include "bluenrg_conf.h"

/**
  * @addtogroup LOW_LEVEL_INTERFACE LOW_LEVEL_INTERFACE
  * @{
  */

/**
  * @defgroup LL_HCI_TL HCI_TL
  * @{
  */

/**
  * @defgroup BASIC BASIC
  * @{
  */

/** @defgroup BASIC_Types Exported Types
  * @{
  */

/**
  * @brief Structure hosting the HCI request
  * @{
  */
struct hci_request
{
  uint8_t  ext_aci; /**< To be set to 1 if extended aci packet has to be used for this request */
  uint16_t ogf;     /**< Opcode Group Field */
  uint16_t ocf;     /**< Opcode Command Field */
  uint32_t event;   /**< HCI Event */
  void     *cparam; /**< HCI Command from MCU to Host */
  uint32_t command_len;    /**< Command Length */
  void     *rparam; /**< Response from Host to MCU */
  uint32_t rlen;    /**< Response Length */
};
/**
  * @}
  */

/**
  * @brief Structure used to read received HCI data packet
  * @{
  */
typedef struct _t_hci_data_packet
{
  list_node_t current_node;
  uint8_t data_buff[HCI_READ_PACKET_SIZE];
  uint16_t data_len;
} hci_data_packet_t;
/**
  * @}
  */

/**
  * @brief Structure used to manage the BUS IO operations.
  *        All the structure fields will point to functions defined at user level.
  * @{
  */

/**
  * @}
  */

/**
  * @brief Describe the HCI flow status
  * @{
  */
typedef enum
{
  HCI_DATA_FLOW_DISABLE = 0,
  HCI_DATA_FLOW_ENABLE,
} hci_flow_status_t;
/**
  * @}
  */

/**
  * @brief Contain the HCI context
  * @{
  */
typedef struct
{
  void (* user_evt_rx)(void *p_data);   /**< ACI events callback function pointer */
} hci_context_t;

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup BASIC_Functions Exported Functions
  * @{
  */
/**
  * @brief  Initializes the peripherals communication with the BlueNRG
  *         Expansion Board (via SPI, I2C, USART, ...)
  *
  * @param  void* Pointer to configuration struct
  * @retval int32_t Status
  */
int32_t hci_tl_spi_init(void *pConf);

/**
  * @brief  DeInitializes the peripherals communication with the BlueNRG
  *         Expansion Board (via SPI, I2C, USART, ...)
  *
  * @param  None
  * @retval int32_t 0
  */
int32_t hci_tl_spi_de_init(void);

/**
  * @brief Reset BlueNRG module.
  *
  * @param  None
  * @retval int32_t 0
  */
int32_t hci_tl_spi_reset(void);

/**
  * @brief  Writes data from local buffer to SPI.
  *
  * @param  buffer : data buffer to be written
  * @param  size   : size of first data buffer to be written
  * @retval int32_t: Number of read bytes
  */
int32_t hci_tl_spi_send(uint8_t *buffer, uint16_t size);

/**
  * @brief  Reads from BlueNRG SPI buffer and store data into local buffer.
  *
  * @param  buffer : Buffer where data from SPI are stored
  * @param  size   : Buffer size
  * @retval int32_t: Number of read bytes
  */
int32_t hci_tl_spi_receive(uint8_t *buffer, uint16_t size);

/**
  * @brief  Send an HCI request either in synchronous or in asynchronous mode.
  *
  * @param  r: The HCI request
  * @param  async: TRUE if asynchronous mode, FALSE if synchronous mode
  * @retval uint32_t: 0 when success, -1 when failure
  */
int32_t hci_send_req(struct hci_request *r, BOOL async);

/**
  * @brief  Interrupt service routine that must be called when the BlueNRG
  *         reports a packet received or an event to the host through the
  *         BlueNRG interrupt line.
  *
  * @param  pdata Packet or event pointer
  * @retval 0: packet/event processed, 1: no packet/event processed
  */
int32_t hci_notify_asynch_evt(void *pdata);

/**
  * @brief  This function resume the User Event Flow which has been stopped on return
  *         from user_evt_rx() when the User Event has not been processed.
  *
  * @param  None
  * @retval None
  */
void hci_resume_flow(void);

/**
  * @brief  This function is called when an ACI/HCI command is sent and the response
  *         is waited from the BLE core.
  *         The application shall implement a mechanism to not return from this function
  *         until the waited event is received.
  *         This is notified to the application with hci_cmd_resp_release().
  *         It is called from the same context the HCI command has been sent.
  *
  * @param  timeout: Waiting timeout
  * @retval None
  */
void hci_cmd_resp_wait(uint32_t timeout);

/**
  * @brief  This function is called when an ACI/HCI command is sent and the response is
  *         received from the BLE core.
  *
  * @param  flag: Release flag
  * @retval None
  */
void hci_cmd_resp_release(uint32_t flag);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* HCI_TL_H */

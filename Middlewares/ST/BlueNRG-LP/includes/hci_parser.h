/**
  ******************************************************************************
  * @file    hci_parser.h 
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.3.0
  * @date    20-July-2023
  * @brief   
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HCI_PARSER_H_
#define HCI_PARSER_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
//#include "SDK_EVAL_Config.h"
/* Exported macro ------------------------------------------------------------*/

/* DTM mode codes */
#define DTM_MODE_DTM_SPI        0x02
#define DTM_MODE_DTM_UART       0x01
#define DTM_MODE_UART           0x00

/* Firmware version */
#define FW_VERSION_MAJOR    1
#define FW_VERSION_MINOR    6

#define PACK_2_BYTE_PARAMETER(ptr, param)  do{\
                *((uint8_t *)ptr) = (uint8_t)(param);   \
                *((uint8_t *)ptr+1) = (uint8_t)(param)>>8; \
                }while(0)

/* Exported constants --------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

typedef enum {
  WAITING_TYPE,
  WAITING_HEADER,
  WAITING_PAYLOAD
}hci_state;

/* Exported functions ------------------------------------------------------- */

/* HCI library functions. */
void hci_input_event(uint8_t *buff, uint16_t len);
void hci_input_cmd(uint8_t *buff, uint16_t len);

/* This is a callback called when an end of packet is detected. */
void packet_received(uint8_t *packet, uint16_t pckt_len);

extern uint8_t buffer_out[];
extern uint16_t buffer_out_len;

#endif /* HCI_PARSER_H_ */

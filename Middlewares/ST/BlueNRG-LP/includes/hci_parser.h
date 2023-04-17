/**
  ******************************************************************************
  * @file    hci_parser.h 
  * @author  AMS RF Application Team
  * @date    Feb-2020
  * @brief   
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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

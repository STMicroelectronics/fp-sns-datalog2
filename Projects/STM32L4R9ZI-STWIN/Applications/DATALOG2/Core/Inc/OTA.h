/**
  ******************************************************************************
  * @file    BLEDualProgram\Inc\OTA.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.5.0
  * @date    27-Mar-2023
  * @brief   Over-the-Air Update API
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
#ifndef _OTA_H_
#define _OTA_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Max Program Size (we keep the last sector for FW status id */
#define OTA_MAX_PROG_SIZE (0x100000-0x1000)

/* when we don't have a valid Fw ID... */
#define OTA_OTA_FW_ID_NOT_VALID 0xFFFF

/* Exported functions ---------------------------------------------------------*/

/* API for preparing the Flash for receiving the Update. It defines also the Size of the Update and the CRC value expected */
extern void StartUpdateFWBlueMS(uint32_t SizeOfUpdate, uint32_t uwCRCValue);
/* API for storing chuck of data to Flash.
 * When it has received the total number of byte defined by StartUpdateFWBlueMS,
 * it computes the CRC value and if it matches the expected CRC value,
 * it writes the Magic Number in Flash for BootLoader */
extern int8_t UpdateFWBlueMS(uint32_t *SizeOfUpdateBlueFW, uint8_t *att_data, int32_t data_length,
                             uint8_t WriteMagicNum);

/* API for updating the current Fw Id saved on Flash */
extern void UpdateCurrFlashBankFwId(uint16_t FwId);

/* API for reading the Fw Ids saved on Flash */
extern void ReadFlashBanksFwId(uint16_t *FwId1, uint16_t *FwId2);

#ifdef __cplusplus
}
#endif

#endif /* _OTA_H_ */


/**
  ******************************************************************************
  * @file    BLE_BinaryContent.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.8.0
  * @date    02-December-2022
  * @brief   BLE_BinaryContent info services APIs.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/  
#ifndef _BLE_BINARY_CONTENT_H_
#define _BLE_BINARY_CONTENT_H_

#ifdef __cplusplus
 extern "C" {
#endif
   
/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventBinaryContent_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventBinaryContent_t CustomNotifyEventBinaryContent;

/* Exported functions ------------------------------------------------------- */

/**
 * @brief  Init BinaryContent info service
 * @param  None
 * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for BinaryContent info service
 */
extern BleCharTypeDef* BLE_InitBinaryContentService(void);

/**
 * @brief  BinaryContent send buffer
 * @param  uint8_t* buffer
 * @param  uint32_t len
 * @retval tBleStatus Status
 */
extern tBleStatus BLE_BinaryContentUpdate(uint8_t* buffer, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_BINARY_CONTENT_H_ */


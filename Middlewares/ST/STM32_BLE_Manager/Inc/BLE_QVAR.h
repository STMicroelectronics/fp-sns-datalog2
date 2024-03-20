/**
  ******************************************************************************
  * @file    BLE_QVAR.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   QVAR info services APIs.
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
#ifndef _BLE_QVAR_H_
#define _BLE_QVAR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventQVAR_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventQVAR_t CustomNotifyEventQVAR;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init QVAR info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for QVAR info service
  */
extern BleCharTypeDef *BLE_InitQVARService(void);

/**
  * @brief  Update The QVAR Char
  * @param  int32_t QVAR Value [LSB]
  * @param  uint8_t Flag (to be defined)
  * @param  int32_t DQVAR Value [LSB]
  * @param  uint32_t Parameter (to be defined)
  * @param  int32_t NumberElement Number of elements to send
  * @retval tBleStatus      Status
  */
extern tBleStatus BLE_QVARUpdate(int32_t QVAR, uint8_t Flag, int32_t DQVAR, uint32_t Param, int32_t NumberElement);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_QVAR_H_ */


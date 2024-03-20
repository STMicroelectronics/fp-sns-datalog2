/**
  ******************************************************************************
  * @file    BLE_EventCounter.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Event Counter info service APIs.
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
#ifndef _BLE_EVENTCOUNTER_H_
#define _BLE_EVENTCOUNTER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventCounter_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventCounter_t CustomNotifyEventCounter;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Event Counter info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for EventCounter info service
  */
extern BleCharTypeDef *BLE_InitEventCounterService(void);

/**
  * @brief  Update Event Counter characteristic
  * @param uint32_t Event Counter Number
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_EventCounterUpdate(uint32_t EventCounter);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_EVENTCOUNTER_H_ */


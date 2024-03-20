/**
  ******************************************************************************
  * @file    BLE_AccEvent.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Acceleromenter HW Event service APIs.
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
#ifndef _BLE_ACC_EVENT_H_
#define _BLE_ACC_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomReadRequestAccEvent_t)(uint16_t *Command, uint8_t *dimByte);
typedef void (*CustomNotifyEventAccEvent_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */

extern CustomReadRequestAccEvent_t CustomReadRequestAccEvent;
extern CustomNotifyEventAccEvent_t CustomNotifyEventAccEvent;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init HW Acceleromenter Event info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for HW Acceleromenter Event info service
  */
extern BleCharTypeDef *BLE_InitAccEnvService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting HW Acceleromenter Event Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetAccEnvAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update HW Acceleromenter Event characteristic value
  * @param  uint16_t Command to Send
  * @param  uint8_t Command Length
  * @retval tBleStatus: Status
  */
extern tBleStatus BLE_AccEnvUpdate(uint16_t Command, uint8_t dimByte);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_ACC_EVENT_H_ */


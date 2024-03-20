/**
  ******************************************************************************
  * @file    BLE_GasConcentration.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Gas concentration info services APIs.
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
#ifndef _BLE_GAS_CONCENTRATION_H_
#define _BLE_GAS_CONCENTRATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomReadRequestGasConcentration_t)(uint32_t *Gas);
typedef void (*CustomNotifyEventGasConcentration_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomReadRequestGasConcentration_t CustomReadRequestGasConcentration;
extern CustomNotifyEventGasConcentration_t CustomNotifyEventGasConcentration;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init gas concentration info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for gas concentration info service
  */
extern BleCharTypeDef *BLE_InitGasConcentrationService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Gas Concentration Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetGasConcentrationAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Gas Concentration Value
  * @param  uint16_t Measured Gas Concentration value
  * @retval tBleStatus   Status
  */
tBleStatus BLE_GasConcentrationStatusUpdate(uint32_t Gas);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_GAS_CONCENTRATION_H_ */


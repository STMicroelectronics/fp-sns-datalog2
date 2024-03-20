/**
  ******************************************************************************
  * @file    BLE_PedometerAlgorithm.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Pedometer Algorithm info service APIs.
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
#ifndef _BLE_PEDOMETER_ALGORITHM_H_
#define _BLE_PEDOMETER_ALGORITHM_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ---------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------- */
typedef struct
{
  uint8_t Cadence;      /* [steps/min] */
  uint32_t Nsteps;
} BLE_PM_output_t;

typedef void (*CustomReadRequestPedometerAlgorithm_t)(BLE_PM_output_t *PedometerAlgorithmData);
typedef void (*CustomNotifyEventPedometerAlgorithm_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomReadRequestPedometerAlgorithm_t CustomReadRequestPedometerAlgorithm;
extern CustomNotifyEventPedometerAlgorithm_t CustomNotifyEventPedometerAlgorithm;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Pedometer Algorithm info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Pedometer Algorithm info service
  */
extern BleCharTypeDef *BLE_InitPedometerAlgorithmService(void);

#ifndef BLE_MANAGER_SDKV2
/**
  * @brief  Setting Pedometer Algorithm Advertise Data
  * @param  uint8_t *manuf_data: Advertise Data
  * @retval None
  */
extern void BLE_SetPedometerAlgorithmAdvertiseData(uint8_t *manuf_data);
#endif /* BLE_MANAGER_SDKV2 */

/**
  * @brief  Update Pedometer Algorithm characteristic
  * @param  BLE_PM_output_t PedometerAlgorithmData Pedometer Algorithm data output
  * @retval tBleStatus   Status
  */
extern tBleStatus BLE_PedometerAlgorithmUpdate(BLE_PM_output_t *PedometerAlgorithmData);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_PEDOMETER_ALGORITHM_H_ */


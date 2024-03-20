/**
  ******************************************************************************
  * @file    BLE_MachineLearningCore.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Machine Learning Core info services APIs.
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
#ifndef _BLE_MACHINE_LEARNING_CORE_H_
#define _BLE_MACHINE_LEARNING_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
typedef void (*CustomNotifyEventMachineLearningCore_t)(BLE_NotifyEvent_t Event);
typedef void (*CustomReadMachineLearningCore_t)(uint8_t *mlc_out, uint8_t *mlc_status_mainpage);

/* Enum type Number Machine Learning Core Registers */
typedef enum
{
  BLE_MLC_4_REG = 0, /* 4 MLC Registers */
  BLE_MLC_8_REG = 1  /* 8 MLC Registers */
} BLE_MachineLearningCoreNumReg_t;

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventMachineLearningCore_t CustomNotifyEventMachineLearningCore;
extern CustomReadMachineLearningCore_t CustomReadMachineLearningCore;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Machine Learning Core info service
  * @param  BLE_MachineLearningCoreNumReg_t NumReg Number MLC registers
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Machine Learning Core info service
  */
BleCharTypeDef *BLE_InitMachineLearningCoreService(BLE_MachineLearningCoreNumReg_t NumReg);

/**
  * @brief  Update Machine Learning Core output registers characteristic
  * @param  uint8_t *mlc_out pointers to 8 MLC register
  * @param  uint8_t *mlc_status_mainpage pointer to the MLC status bits from 1 to 8
  * @retval tBleStatus Status
  */
tBleStatus BLE_MachineLearningCoreUpdate(uint8_t *mlc_out, uint8_t *mlc_status_mainpage);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MACHINE_LEARNING_CORE_H_ */


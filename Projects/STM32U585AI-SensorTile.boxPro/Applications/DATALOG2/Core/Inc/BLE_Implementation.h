/**
  ******************************************************************************
  * @file    BLE_Implementation.h
  * @author  SRA
  * @brief   BLE Implementation header template file.
  *          This file should be copied to the application folder and renamed
  *          to BLE_Implementation.h.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _BLE_IMPLEMENTATION_H_
#define _BLE_IMPLEMENTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "BLE_PnPLike.h"
#include "BLE_MachineLearningCore.h"
#include "BLE_RawPnPLControlled.h"
#include "BLE_HighSpeedDataLog.h"
#include "BLE_MachineLearningCore.h"
#include "services/syserror.h"

/* necessary for using floor on BLE_Manager.h */
#include <math.h>

/* Exported Defines --------------------------------------------------------*/


/* Firmware Package Name */
#define BLE_FW_PACKAGENAME          "HSD2v30"

/* Max Stream ID dedicated to custom data, utility stream (like advertise option byte) could be added after this */
#define MAX_CUSTOM_DATA_STREAM_ID          (SM_MAX_SENSORS)


/* Exported Variables ------------------------------------------------------- */
extern volatile uint8_t  paired;
extern volatile int PowerButtonPressed;
extern volatile uint32_t NeedToClearSecureDB;

/* Exported functions ------------------------------------------------------- */
extern void BLE_InitCustomService(void);
extern void BLE_SetCustomAdvertiseData(uint8_t *manuf_data);
extern void BLE_BluetoothInit(void);
extern uint8_t BLE_GetFWID(void);


/* Exported macro ------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _BLE_IMPLEMENTATION_H_ */


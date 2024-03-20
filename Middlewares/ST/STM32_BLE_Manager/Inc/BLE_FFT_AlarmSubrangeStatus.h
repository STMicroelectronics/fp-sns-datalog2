/**
  ******************************************************************************
  * @file    BLE_FFT_AlarmSubrangeStatus.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   FFT Alarm Subrange Status info services APIs.
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
#ifndef _BLE_FFT_ALARM_SUBRANGE_STATUS_H_
#define _BLE_FFT_ALARM_SUBRANGE_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported typedef --------------------------------------------------------- */
/**
  * @brief  X-Y-Z Generic Value in float
  */
typedef struct
{
  float x;         /* !< Generic X Value in float */
  float y;         /* !< Generic Y Value in float */
  float z;         /* !< Generic Z Value in float */
} BLE_FFT_AlarmSubrangeStatusGenericValue_t;

/**
  * @brief  Warning Alarm Datatype
  */
typedef enum
{
  BLE_GOOD_SUBRANGE          = (uint8_t)0x00,  /* !< GOOD STATUS */
  BLE_WARNING_SUBRANGE       = (uint8_t)0x01,  /* !< WARNING STATUS */
  BLE_ALARM_SUBRANGE         = (uint8_t)0x02,  /* !< ALARM STATUS */
  BLE_NONE_SUBRANGE          = (uint8_t)0x03,  /* !< RFU STATUS */
} BLE_FFT_AlarmType_t;

/**
  * @brief  STATUS for FFT Subrange Warning-Alarm
  */
typedef struct
{
  BLE_FFT_AlarmType_t STATUS_AXIS_X;   /* !< X STATUS ALARM */
  BLE_FFT_AlarmType_t STATUS_AXIS_Y;   /* !< Y STATUS ALARM */
  BLE_FFT_AlarmType_t STATUS_AXIS_Z;   /* !< Z STATUS ALARM */
} sBLE_FFT_AlarmSubrangeStatus_t;

typedef void (*CustomNotifyEventFFT_AlarmSubrangeStatus_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventFFT_AlarmSubrangeStatus_t CustomNotifyEventFFT_AlarmSubrangeStatus;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init FFT Alarm Subrange Status info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for FFT Alarm Subrange Status info service
  */
BleCharTypeDef *BLE_InitFFT_AlarmSubrangeStatusService(void);

/*
 * @brief  Update FFT Alarm Subrange RMS status value
 * @param  sBLE_FFT_AlarmSubrangeStatus_t              AlarmStatus
 * @param  BLE_FFT_AlarmSubrangeStatusGenericValue_t        SubrangeMaxValue
 * @param  BLE_FFT_AlarmSubrangeStatusGenericValue_t        SubrangeFreqMaxValue
 * @retval tBleStatus   Status
 */
tBleStatus BLE_FFT_AlarmSubrangeStatusUpdate(sBLE_FFT_AlarmSubrangeStatus_t          AlarmStatus,
                                             BLE_FFT_AlarmSubrangeStatusGenericValue_t    SubrangeMaxValue,
                                             BLE_FFT_AlarmSubrangeStatusGenericValue_t    SubrangeFreqMaxValue);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_FFT_ALARM_SUBRANGE_STATUS_H_ */


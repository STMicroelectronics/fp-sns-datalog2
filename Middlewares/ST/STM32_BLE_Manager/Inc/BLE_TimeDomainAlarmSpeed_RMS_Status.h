/**
  ******************************************************************************
  * @file    BLE_TimeDomainAlarmSpeed_RMS_Status.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Time Domain Alarm Speed RMS Status info services APIs.
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
#ifndef _BLE_TD_ALARM_SPEED_RMS_STATUS_H_
#define _BLE_TD_ALARM_SPEED_RMS_STATUS_H_

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
} BLE_TD_AlarmSpeed_RMS_StatusGenericValue_t;

/**
  * @brief  Warning Alarm Datatype
  */
typedef enum
{
  BLE_GOOD_SPEED_STATUS         = (uint8_t)0x00,  /* !< GOOD STATUS */
  BLE_WARNING_SPEED_STATUS      = (uint8_t)0x01,  /* !< WARNING STATUS */
  BLE_ALARM_SPEED_STATUS        = (uint8_t)0x02,  /* !< ALARM STATUS */
  BLE_NONE_SPEED_STATUS         = (uint8_t)0x03,  /* !< RFU STATUS */
} BLE_TD_AlarmType_t;

/**
  * @brief  STATUS for Time Domain Speed Warning-Alarm
  */
typedef struct
{
  BLE_TD_AlarmType_t STATUS_AXIS_X;   /* !< X STATUS ALARM */
  BLE_TD_AlarmType_t STATUS_AXIS_Y;   /* !< Y STATUS ALARM */
  BLE_TD_AlarmType_t STATUS_AXIS_Z;   /* !< Z STATUS ALARM */
} sBLE_TD_AlarmSpeed_RMS_Status_t;

typedef void (*CustomNotifyEventTD_AlarmSpeed_RMS_Status_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventTD_AlarmSpeed_RMS_Status_t CustomNotifyEventTD_AlarmSpeed_RMS_Status;

/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Init Time Domain Alarm Speed RMS Status info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for Time Domain Alarm Speed RMS Status info service
  */
BleCharTypeDef *BLE_InitTD_AlarmSpeed_RMS_StatusService(void);

/*
 * @brief  Update Time Domain Alarm Speed RMS status value
 * @param  sBLE_TD_AlarmSpeed_RMS_Status_t              Alarm
 * @param  BLE_TD_AlarmSpeed_RMS_StatusGenericValue_t        SpeedRmsValue
 * @retval tBleStatus   Status
 */
tBleStatus BLE_TD_AlarmSpeed_RMS_StatusUpdate(sBLE_TD_AlarmSpeed_RMS_Status_t Alarm,
                                              BLE_TD_AlarmSpeed_RMS_StatusGenericValue_t SpeedRmsValue);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_TD_ALARM_SPEED_RMS_STATUS_H_ */


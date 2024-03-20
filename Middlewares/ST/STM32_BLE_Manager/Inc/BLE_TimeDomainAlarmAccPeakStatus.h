/**
  ******************************************************************************
  * @file    BLE_TimeDomainAlarmAccPeakStatus.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 1.9.1
  * @date    10-October-2023
  * @brief   Time Domain Alarm Acc Peak Status info services APIs.
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
#ifndef _BLE_TD_ALARM_ACC_PEAK_STATUS_H_
#define _BLE_TD_ALARM_ACC_PEAK_STATUS_H_

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
} BLE_MANAGER_TD_AlarmAccPeakStatusGenericValue_t;

/**
  * @brief  Warning Alarm Datatype
  */
typedef enum
{
  BLE_GOOD_ACC_PEAK          = (uint8_t)0x00,  /* !< GOOD STATUS */
  BLE_WARNING_ACC_PEAK       = (uint8_t)0x01,  /* !< WARNING STATUS */
  BLE_ALARM_ACC_PEAK         = (uint8_t)0x02,  /* !< ALARM STATUS */
  BLE_NONE_ACC_PEAK          = (uint8_t)0x03,  /* !< RFU STATUS */
} BLE_Manager_TD_AlarmAccPeakStatusAlarmType_t;

/**
  * @brief  STATUS for FFT Acc Peak Warning-Alarm
  */
typedef struct
{
  BLE_Manager_TD_AlarmAccPeakStatusAlarmType_t STATUS_AXIS_X;   /* !< X STATUS ALARM */
  BLE_Manager_TD_AlarmAccPeakStatusAlarmType_t STATUS_AXIS_Y;   /* !< Y STATUS ALARM */
  BLE_Manager_TD_AlarmAccPeakStatusAlarmType_t STATUS_AXIS_Z;   /* !< Z STATUS ALARM */
} sBLE_Manager_TD_AlarmAccPeakStatusAlarm_t;


typedef void (*CustomNotifyEventTD_AlarmAccPeakStatus_t)(BLE_NotifyEvent_t Event);

/* Exported Variables ------------------------------------------------------- */
extern CustomNotifyEventTD_AlarmAccPeakStatus_t CustomNotifyEventTD_AlarmAccPeakStatus;

/**
  * @brief  Init FFT Alarm Acc Peak Status info service
  * @param  None
  * @retval BleCharTypeDef* BleCharPointer: Data structure pointer for FFT Alarm Acc Peak Status info service
  */
BleCharTypeDef *BLE_InitTD_AlarmAccPeakStatusService(void);

/*
 * @brief  Update FFT Alarm Acc Peak status value
 * @param  sBLE_Manager_TD_AlarmAccPeakStatusAlarm_t            Alarm
 * @param  BLE_MANAGER_TD_AlarmAccPeakStatusGenericValue_t      AccPeak
 * @retval tBleStatus   Status
 */
tBleStatus BLE_TD_AlarmAccPeakStatusUpdate(sBLE_Manager_TD_AlarmAccPeakStatusAlarm_t Alarm,
                                           BLE_MANAGER_TD_AlarmAccPeakStatusGenericValue_t AccPeak);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_TD_ALARM_ACC_PEAK_STATUS_H_ */


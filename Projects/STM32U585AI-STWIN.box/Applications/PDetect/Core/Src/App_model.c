/**
  ******************************************************************************
  * @file    App_Model.c
  * @author  SRA
  * @brief   App Application Model and PnPL Components APIs
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

/**
  ******************************************************************************
  * This file has been auto generated from the following Device Template Model:
  * dtmi:vespucci:steval_stwinbx1:FP_SNS_DATALOG2_PDetect;1
  *
  * Created by: DTDL2PnPL_cGen version 1.2.3
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include <string.h>
#include <stdio.h>
/* USER includes -------------------------------------------------------------*/
#include "services/SQuery.h"
#include "services/sysdebug.h"
#include "fx_stm32_sd_driver.h"
#include "ux_user.h"
#include "rtc.h"
#include "HardwareDetection.h"


/* USER defines --------------------------------------------------------------*/
#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DT, level, message)


/* USER private function prototypes ------------------------------------------*/
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status);

static sys_error_code_t __sc_set_sd_stream_params(AppModel_t *p_app_model, uint32_t id);
static sys_error_code_t __sc_set_usb_stream_params(AppModel_t *p_app_model, uint32_t id);
static void __sc_set_usb_enpoints(AppModel_t *p_app_model);
static void __sc_reset_stream_params(AppModel_t *p_app_model);

AppModel_t app_model;

AppModel_t *getAppModel(void)
{
  return &app_model;
}

/* Device Components APIs ----------------------------------------------------*/

/* VL53L8CX_TOF PnPL Component -----------------------------------------------*/
static SensorModel_t vl53l8cx_tof_model;

uint8_t vl53l8cx_tof_comp_init(void)
{
  vl53l8cx_tof_model.comp_name = vl53l8cx_tof_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "vl53l8cx", COM_TYPE_TOF);
  vl53l8cx_tof_model.id = id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  vl53l8cx_tof_model.stream_params.stream_id = -1;
  vl53l8cx_tof_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "[EXTERN]\0";
  vl53l8cx_tof_set_sensor_annotation(default_notes);
#if (HSD_USE_DUMMY_DATA == 1)
  vl53l8cx_tof_set_samples_per_ts(0);
#else
  vl53l8cx_tof_set_samples_per_ts(10);
#endif

  /* default values for CES demo */
  vl53l8cx_tof_set_resolution("8x8");

  app_model.s_models[id] = &vl53l8cx_tof_model;
  __stream_control(NULL, true);
  return 0;
}
char *vl53l8cx_tof_get_key(void)
{
  return "vl53l8cx_tof";
}

uint8_t vl53l8cx_tof_get_enable(bool *value)
{
  uint16_t id = vl53l8cx_tof_model.id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  *value = vl53l8cx_tof_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_odr(int32_t *value)
{
  uint16_t id = vl53l8cx_tof_model.id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  *value = vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.frequency;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_resolution(char **value)
{
  uint16_t id = vl53l8cx_tof_model.id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  uint8_t profile = vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile;

//  /* vl53l8cx ranging profiles */
//  #define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
//  #define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
//  #define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
//  #define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)

  switch (profile)
  {
    case 1:
    case 2:
      *value = "4x4";
      break;
    case 3:
    case 4:
      *value = "8x8";
      break;
    default:
      *value = "4x4";
      break;
  }
  return 0;
}
uint8_t vl53l8cx_tof_get_ranging_mode(char **value)
{
  uint16_t id = vl53l8cx_tof_model.id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  uint8_t profile = vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile;

//  /* vl53l8cx ranging profiles */
//  #define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
//  #define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
//  #define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
//  #define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)

  switch (profile)
  {
    case 1:
    case 3:
      *value = "Continuous";
      break;
    case 2:
    case 4:
      *value = "Autonomous";
      break;
    default:
      *value = "Continuous";
      break;
  }
  return 0;
}
uint8_t vl53l8cx_tof_get_integration_time(int32_t *value)
{
  uint16_t id = vl53l8cx_tof_model.id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  *value = vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.timing_budget;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_samples_per_ts(int32_t *value)
{
  *value = vl53l8cx_tof_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_ioffset(float *value)
{
  *value = vl53l8cx_tof_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_usb_dps(int32_t *value)
{
  *value = vl53l8cx_tof_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_sd_dps(int32_t *value)
{
  *value = vl53l8cx_tof_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_data_type(char **value)
{
  *value = "uint32_t";
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_sensor_annotation(char **value)
{
  uint16_t id = vl53l8cx_tof_model.id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  *value = vl53l8cx_tof_model.annotation;
  return 0;
}
uint8_t vl53l8cx_tof_get_sensor_category(int32_t *value)
{
  *value = vl53l8cx_tof_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_dim(int32_t *value)
{
  uint16_t id = vl53l8cx_tof_model.id;
  vl53l8cx_tof_model.sensor_status = SMSensorGetStatus(id);
  uint8_t profile = vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile;

//  /* vl53l8cx ranging profiles */
//  #define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
//  #define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
//  #define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
//  #define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)

  switch (profile)
  {
    case 1:
    case 2:
      *value = 128;
      break;
    case 3:
    case 4:
      *value = 512;
      break;
    default:
      *value = 128;
      break;
  }
  return 0;
}
uint8_t vl53l8cx_tof_get_stream_id(int8_t *value)
{
  *value = vl53l8cx_tof_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_get_ep_id(int8_t *value)
{
  *value = vl53l8cx_tof_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t vl53l8cx_tof_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(vl53l8cx_tof_model.id);
  }
  else
  {
    ret = SMSensorDisable(vl53l8cx_tof_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    vl53l8cx_tof_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t vl53l8cx_tof_set_odr(int32_t value)
{
  vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.frequency = value;
  sys_error_code_t ret = SMSensorSetFrequency(vl53l8cx_tof_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    vl53l8cx_tof_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t vl53l8cx_tof_set_resolution(const char *value)
{
  sys_error_code_t ret = 1;
  uint8_t resolution = 16;
//  uint8_t profile = vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile;

  ///* vl53l8cx ranging profiles */
  //#define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
  //#define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
  //#define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
  //#define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)
  ///* vl53l8cx valid resolutions */
  //#define VL53L8CX_RESOLUTION_4X4     ((uint8_t) 16U)
  //#define VL53L8CX_RESOLUTION_8X8     ((uint8_t) 64U)

  if (strcmp(value, "4x4") == 0)
  {
    resolution = 16;
//    if(profile == 1 || profile == 3)
//    {
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile = 1;
//    }
//    else
//    {
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile = 2;
//    }
  }
  else if (strcmp(value, "8x8") == 0)
  {
    resolution = 64;
//    if(profile == 1 || profile == 3)
//    {
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile = 3;
//    }
//    else
//    {
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.ranging_profile = 4;
//    }
  }
  ret = SMSensorSetResolution(vl53l8cx_tof_model.id, resolution);
  return ret;
}
uint8_t vl53l8cx_tof_set_ranging_mode(const char *value)
{
  sys_error_code_t ret = 1;
  uint32_t ranging_mode = 1;
  uint8_t mode = vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.mode;

  ///* vl53l8cx ranging profiles */
  //#define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
  //#define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
  //#define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
  //#define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)
  ///* vl53l8cx valid modes*/
  //#define VL53L8CX_MODE_BLOCKING_CONTINUOUS   (1U)
  //#define VL53L8CX_MODE_BLOCKING_ONESHOT      (2U)
  //#define VL53L8CX_MODE_ASYNC_CONTINUOUS      (3U)
  //#define VL53L8CX_MODE_ASYNC_ONESHOT         (4U)

  if (strcmp(value, "Continuous") == 0)
  {
    if (mode == 1 || mode == 2)
    {
      ranging_mode = 1;
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.mode = 1;
    }
    else
    {
      ranging_mode = 3;
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.mode = 3;
    }
  }
  else if (strcmp(value, "Autonomous") == 0)
  {
    if (mode == 1 || mode == 2)
    {
      ranging_mode = 2;
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.mode = 2;
    }
    else
    {
      ranging_mode = 4;
//      vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.mode = 4;
    }
  }
  ret = SMSensorSetRangingMode(vl53l8cx_tof_model.id, ranging_mode);
  return ret;
}
uint8_t vl53l8cx_tof_set_integration_time(int32_t value)
{
  vl53l8cx_tof_model.sensor_status.type.ranging.profile_config.timing_budget = value;
  sys_error_code_t ret = SMSensorSetIntegrationTime(vl53l8cx_tof_model.id, value);
  return ret;
}
uint8_t vl53l8cx_tof_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 60;
  if (value >= min_v && value <= max_v)
  {
    vl53l8cx_tof_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  vl53l8cx_tof_model.stream_params.spts = max_v;
  }
  else
  {
	  vl53l8cx_tof_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t vl53l8cx_tof_set_sensor_annotation(const char *value)
{
  strcpy(vl53l8cx_tof_model.annotation, value);
  return 0;
}

/* VD6283TX_ALS PnPL Component -----------------------------------------------*/
static SensorModel_t vd6283tx_als_model;

uint8_t vd6283tx_als_comp_init(void)
{
  vd6283tx_als_model.comp_name = vd6283tx_als_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "vd6283tx", COM_TYPE_ALS);
  vd6283tx_als_model.id = id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  vd6283tx_als_model.stream_params.stream_id = -1;
  vd6283tx_als_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "[EXTERN]\0";
  vd6283tx_als_set_sensor_annotation(default_notes);
#if (HSD_USE_DUMMY_DATA == 1)
  vd6283tx_als_set_samples_per_ts(0);
#else
  vd6283tx_als_set_samples_per_ts(1);
#endif

  /* default values for CES demo */

  app_model.s_models[id] = &vd6283tx_als_model;
  __stream_control(NULL, true);
  return 0;
}
char *vd6283tx_als_get_key(void)
{
  return "vd6283tx_als";
}

uint8_t vd6283tx_als_get_enable(bool *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_intermeasurement_time(int32_t *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.intermeasurement_time;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_exposure_time(int32_t *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.exposure_time;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_channel1_gain(float *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.gain[0];
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_channel2_gain(float *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.gain[1];
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_channel3_gain(float *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.gain[2];
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_channel4_gain(float *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.gain[3];
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_channel5_gain(float *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.gain[4];
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_channel6_gain(float *value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.sensor_status.type.light.gain[5];
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_samples_per_ts(int32_t *value)
{
  *value = vd6283tx_als_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_ioffset(float *value)
{
  *value = vd6283tx_als_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_usb_dps(int32_t *value)
{
  *value = vd6283tx_als_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_sd_dps(int32_t *value)
{
  *value = vd6283tx_als_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_data_type(char **value)
{
  *value = "uint32_t";
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_sensor_annotation(char **value)
{
  uint16_t id = vd6283tx_als_model.id;
  vd6283tx_als_model.sensor_status = SMSensorGetStatus(id);
  *value = vd6283tx_als_model.annotation;
  return 0;
}
uint8_t vd6283tx_als_get_sensor_category(int32_t *value)
{
  *value = vd6283tx_als_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_dim(int32_t *value)
{
  *value = 6;
  return 0;
}
uint8_t vd6283tx_als_get_stream_id(int8_t *value)
{
  *value = vd6283tx_als_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_get_ep_id(int8_t *value)
{
  *value = vd6283tx_als_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t vd6283tx_als_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(vd6283tx_als_model.id);
  }
  else
  {
    ret = SMSensorDisable(vd6283tx_als_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    vd6283tx_als_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t vd6283tx_als_set_intermeasurement_time(int32_t value)
{
  sys_error_code_t ret = SMSensorSetIntermeasurementTime(vd6283tx_als_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    vd6283tx_als_model.sensor_status.type.light.intermeasurement_time = value;
#if (HSD_USE_DUMMY_DATA != 1)
    vd6283tx_als_set_samples_per_ts((int32_t)(1000 / value));
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t vd6283tx_als_set_exposure_time(int32_t value)
{
  sys_error_code_t ret = SMSensorSetExposureTime(vd6283tx_als_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    vd6283tx_als_model.sensor_status.type.light.exposure_time = value;
    /* USER Code */
  }
  return ret;
}
uint8_t vd6283tx_als_set_channel1_gain(float value)
{
  vd6283tx_als_model.sensor_status.type.light.gain[0] = value;
  sys_error_code_t ret = SMSensorSetLightGain(vd6283tx_als_model.id, vd6283tx_als_model.sensor_status.type.light.gain[0], 1);
  return ret;
}
uint8_t vd6283tx_als_set_channel2_gain(float value)
{
  vd6283tx_als_model.sensor_status.type.light.gain[1] = value;
  sys_error_code_t ret = SMSensorSetLightGain(vd6283tx_als_model.id, vd6283tx_als_model.sensor_status.type.light.gain[1], 2);
  return ret;
}
uint8_t vd6283tx_als_set_channel3_gain(float value)
{
  vd6283tx_als_model.sensor_status.type.light.gain[2] = value;
  sys_error_code_t ret = SMSensorSetLightGain(vd6283tx_als_model.id, vd6283tx_als_model.sensor_status.type.light.gain[2], 3);
  return ret;
}
uint8_t vd6283tx_als_set_channel4_gain(float value)
{
  vd6283tx_als_model.sensor_status.type.light.gain[3] = value;
  sys_error_code_t ret = SMSensorSetLightGain(vd6283tx_als_model.id, vd6283tx_als_model.sensor_status.type.light.gain[3], 4);
  return ret;
}
uint8_t vd6283tx_als_set_channel5_gain(float value)
{
  vd6283tx_als_model.sensor_status.type.light.gain[4] = value;
  sys_error_code_t ret = SMSensorSetLightGain(vd6283tx_als_model.id, vd6283tx_als_model.sensor_status.type.light.gain[4], 5);
  return ret;
}
uint8_t vd6283tx_als_set_channel6_gain(float value)
{
  vd6283tx_als_model.sensor_status.type.light.gain[5] = value;
  sys_error_code_t ret = SMSensorSetLightGain(vd6283tx_als_model.id, vd6283tx_als_model.sensor_status.type.light.gain[5], 6);
  return ret;
}
uint8_t vd6283tx_als_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 5;
  if (value >= min_v && value <= max_v)
  {
    vd6283tx_als_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  vd6283tx_als_model.stream_params.spts = max_v;
  }
  else
  {
	  vd6283tx_als_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t vd6283tx_als_set_sensor_annotation(const char *value)
{
  strcpy(vd6283tx_als_model.annotation, value);
  return 0;
}

/* STHS34PF80_TMOS PnPL Component --------------------------------------------*/
static SensorModel_t sths34pf80_tmos_model;

uint8_t sths34pf80_tmos_comp_init(void)
{
  sths34pf80_tmos_model.comp_name = sths34pf80_tmos_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "sths34pf80", COM_TYPE_TMOS);
  sths34pf80_tmos_model.id = id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  sths34pf80_tmos_model.stream_params.stream_id = -1;
  sths34pf80_tmos_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "[EXTERN]\0";
  sths34pf80_tmos_set_sensor_annotation(default_notes);
#if (HSD_USE_DUMMY_DATA == 1)
  sths34pf80_tmos_set_samples_per_ts(0);
#else
  sths34pf80_tmos_set_samples_per_ts(15);
#endif

  /* default values for CES demo */
  sths34pf80_tmos_set_presence_threshold(120);
  sths34pf80_tmos_set_embedded_compensation(true);
  sths34pf80_tmos_set_software_compensation(false);

  app_model.s_models[id] = &sths34pf80_tmos_model;
  __stream_control(NULL, true);
  return 0;
}
char *sths34pf80_tmos_get_key(void)
{
  return "sths34pf80_tmos";
}

uint8_t sths34pf80_tmos_get_enable(bool *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_odr(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)sths34pf80_tmos_model.sensor_status.type.presence.data_frequency;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_transmittance(float *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.Transmittance;
  return 0;
}
uint8_t sths34pf80_tmos_get_avg_tobject_num(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.average_tobject;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_avg_tambient_num(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.average_tambient;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_lpf_p_m_bandwidth(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.lpf_p_m_bandwidth;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_lpf_p_bandwidth(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.lpf_p_bandwidth;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_lpf_m_bandwidth(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.lpf_m_bandwidth;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_presence_threshold(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.presence_threshold;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_presence_hysteresis(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.presence_hysteresis;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_motion_threshold(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.motion_threshold;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_motion_hysteresis(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.motion_hysteresis;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_tambient_shock_threshold(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.tambient_shock_threshold;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_tambient_shock_hysteresis(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.tambient_shock_hysteresis;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_embedded_compensation(bool *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.embedded_compensation;
  return 0;
}
uint8_t sths34pf80_tmos_get_software_compensation(bool *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.software_compensation;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_compensation_type(char **value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);

//  typedef enum
//  {
//    IPD_COMP_NONE,   /* No compensation */
//    IPD_COMP_LIN,    /* Linear compensation */
//    IPD_COMP_NONLIN  /* Non-linear compensation */
//  } IPD_comp_t;

  switch (sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.comp_type)
  {
    case 0:
      *value = "IPD_COMP_NONE";
      break;
    case 1:
      *value = "IPD_COMP_LIN";
      break;
    case 2:
      *value = "IPD_COMP_NONLIN";
      break;
    default:
      *value = "";
      break;
  }
  return 0;
}
uint8_t sths34pf80_tmos_get_sw_presence_threshold(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.pres_ths;
  return 0;
}
uint8_t sths34pf80_tmos_get_sw_motion_threshold(int32_t *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.mot_ths;
  return 0;
}
uint8_t sths34pf80_tmos_get_compensation_filter_flag(bool *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.comp_filter_flag;
  return 0;
}
uint8_t sths34pf80_tmos_get_absence_static_flag(bool *value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.abs_static_flag;
  return 0;
}
uint8_t sths34pf80_tmos_get_samples_per_ts(int32_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_ioffset(float *value)
{
  *value = sths34pf80_tmos_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_usb_dps(int32_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_sd_dps(int32_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_data_type(char **value)
{
  *value = "int16_t";
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_sensor_annotation(char **value)
{
  uint16_t id = sths34pf80_tmos_model.id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatus(id);
  *value = sths34pf80_tmos_model.annotation;
  return 0;
}
uint8_t sths34pf80_tmos_get_sensor_category(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_dim(int32_t *value)
{
  *value = 11;
  return 0;
}
uint8_t sths34pf80_tmos_get_stream_id(int8_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_get_ep_id(int8_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t sths34pf80_tmos_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(sths34pf80_tmos_model.id);
  }
  else
  {
    ret = SMSensorDisable(sths34pf80_tmos_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_odr(int32_t value)
{
  sys_error_code_t ret = SMSensorSetDataFrequency(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.data_frequency = value;
#if (HSD_USE_DUMMY_DATA != 1)
    sths34pf80_tmos_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_transmittance(float value)
{
  sys_error_code_t ret = SMSensorSetTransmittance(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.Transmittance = value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_avg_tobject_num(int32_t value)
{
  sys_error_code_t ret = SMSensorSetAverageTObject(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.average_tobject = value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_avg_tambient_num(int32_t value)
{
  sys_error_code_t ret = SMSensorSetAverageTAmbient(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.average_tambient = value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_lpf_p_m_bandwidth(int32_t value)
{
  sys_error_code_t ret = SMSensorSetLPF_P_M_Bandwidth(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.lpf_p_m_bandwidth = value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_lpf_p_bandwidth(int32_t value)
{
  sys_error_code_t ret = SMSensorSetLPF_P_Bandwidth(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.lpf_p_bandwidth = value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_lpf_m_bandwidth(int32_t value)
{
  sys_error_code_t ret = SMSensorSetLPF_M_Bandwidth(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.lpf_m_bandwidth = value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_presence_threshold(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 32767;
  if (value >= min_v && value <= max_v)
  {
    sys_error_code_t ret = SMSensorSetPresenceThreshold(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      sths34pf80_tmos_model.sensor_status.type.presence.presence_threshold = value;
      /* USER Code */
    }
    return ret;
  }
  return 0;
}
uint8_t sths34pf80_tmos_set_presence_hysteresis(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 255;
  if (value >= min_v && value <= max_v)
  {
    sys_error_code_t ret = SMSensorSetPresenceHysteresis(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      sths34pf80_tmos_model.sensor_status.type.presence.presence_hysteresis = value;
      /* USER Code */
    }
    return ret;
  }
  return 0;
}
uint8_t sths34pf80_tmos_set_motion_threshold(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 32767;
  if (value >= min_v && value <= max_v)
  {
    sys_error_code_t ret = SMSensorSetMotionThreshold(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      sths34pf80_tmos_model.sensor_status.type.presence.motion_threshold = value;
      /* USER Code */
    }
    return ret;
  }
  return 0;
}
uint8_t sths34pf80_tmos_set_motion_hysteresis(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 255;
  if (value >= min_v && value <= max_v)
  {
    sys_error_code_t ret = SMSensorSetMotionHysteresis(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      sths34pf80_tmos_model.sensor_status.type.presence.motion_hysteresis = value;
      /* USER Code */
    }
    return ret;
  }
  return 0;
}
uint8_t sths34pf80_tmos_set_tambient_shock_threshold(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 32767;
  if (value >= min_v && value <= max_v)
  {
    sys_error_code_t ret = SMSensorSetTAmbientShockThreshold(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      sths34pf80_tmos_model.sensor_status.type.presence.tambient_shock_threshold = value;
      /* USER Code */
    }
    return ret;
  }
  return 0;
}
uint8_t sths34pf80_tmos_set_tambient_shock_hysteresis(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 255;
  if (value >= min_v && value <= max_v)
  {
    sys_error_code_t ret = SMSensorSetTAmbientShockHysteresis(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      sths34pf80_tmos_model.sensor_status.type.presence.tambient_shock_hysteresis = value;
      /* USER Code */
    }
    return ret;
  }
  return 0;
}
uint8_t sths34pf80_tmos_set_embedded_compensation(bool value)
{
  sys_error_code_t ret = SMSensorSetEmbeddedCompensation(sths34pf80_tmos_model.id, (uint8_t)value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.embedded_compensation = (uint8_t)value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_software_compensation(bool value)
{
  sys_error_code_t ret = SMSensorSetSoftwareCompensation(sths34pf80_tmos_model.id, (uint8_t)value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.software_compensation = (uint8_t)value;
    /* USER Code */
  }
  return ret;
}
uint8_t sths34pf80_tmos_set_compensation_type(const char *value)
{
  //  typedef enum
  //  {
  //    IPD_COMP_NONE,   /* No compensation */
  //    IPD_COMP_LIN,    /* Linear compensation */
  //    IPD_COMP_NONLIN  /* Non-linear compensation */
  //  } IPD_comp_t;

  if (strcmp(value, "IPD_COMP_NONE") == 0)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.comp_type = 0;
  }
  else if (strcmp(value, "IPD_COMP_LIN") == 0)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.comp_type = 1;
  }
  else if (strcmp(value, "IPD_COMP_NONLIN") == 0)
  {
    sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.comp_type = 2;
  }
  SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id,
                                                 &sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig);
  return 0;
}
uint8_t sths34pf80_tmos_set_sw_presence_threshold(int32_t value)
{
  sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.pres_ths = (uint16_t)value;
  SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id,
                                                 &sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig);
  return 0;
}
uint8_t sths34pf80_tmos_set_sw_motion_threshold(int32_t value)
{
  sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.mot_ths = (uint16_t)value;
  SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id,
                                                 &sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig);
  return 0;
}
uint8_t sths34pf80_tmos_set_compensation_filter_flag(bool value)
{
  sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.comp_filter_flag = (uint8_t)value;
  SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id,
                                                 &sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig);
  return 0;
}
uint8_t sths34pf80_tmos_set_absence_static_flag(bool value)
{
  sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig.abs_static_flag = (uint8_t)value;
  SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id,
                                                 &sths34pf80_tmos_model.sensor_status.type.presence.AlgorithmConfig);
  return 0;
}
uint8_t sths34pf80_tmos_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 30;
  if (value >= min_v && value <= max_v)
  {
    sths34pf80_tmos_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  sths34pf80_tmos_model.stream_params.spts = max_v;
  }
  else
  {
	  sths34pf80_tmos_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t sths34pf80_tmos_set_sensor_annotation(const char *value)
{
  strcpy(sths34pf80_tmos_model.annotation, value);
  return 0;
}

/* IMP34DT05_MIC PnPL Component ----------------------------------------------*/
static SensorModel_t imp34dt05_mic_model;

uint8_t imp34dt05_mic_comp_init(void)
{
  imp34dt05_mic_model.comp_name = imp34dt05_mic_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "imp34dt05", COM_TYPE_MIC);
  imp34dt05_mic_model.id = id;
  imp34dt05_mic_model.sensor_status = SMSensorGetStatus(id);
  imp34dt05_mic_model.stream_params.stream_id = -1;
  imp34dt05_mic_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  imp34dt05_mic_set_sensor_annotation(default_notes);
  /* USER Code */
  imp34dt05_mic_set_enable(false);
  imp34dt05_mic_set_odr(16000);
#if (HSD_USE_DUMMY_DATA == 1)
  imp34dt05_mic_set_samples_per_ts(0);
#else
  imp34dt05_mic_set_samples_per_ts(1000);
#endif
  app_model.s_models[id] = &imp34dt05_mic_model;
  __stream_control(NULL, true);
  return 0;
}

char *imp34dt05_mic_get_key(void)
{
  return "imp34dt05_mic";
}

uint8_t imp34dt05_mic_get_odr(int32_t *value)
{
  uint16_t id = imp34dt05_mic_model.id;
  imp34dt05_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp34dt05_mic_model.sensor_status.type.audio.frequency;
  return 0;
}
uint8_t imp34dt05_mic_get_aop(float *value)
{
  *value = 122.5;
  return 0;
}
uint8_t imp34dt05_mic_get_enable(bool *value)
{
  uint16_t id = imp34dt05_mic_model.id;
  imp34dt05_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp34dt05_mic_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_get_volume(int32_t *value)
{
  uint16_t id = imp34dt05_mic_model.id;
  imp34dt05_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp34dt05_mic_model.sensor_status.type.audio.volume;
  return 0;
}
uint8_t imp34dt05_mic_get_resolution(int32_t *value)
{
  uint16_t id = imp34dt05_mic_model.id;
  imp34dt05_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp34dt05_mic_model.sensor_status.type.audio.resolution;
  return 0;
}
uint8_t imp34dt05_mic_get_samples_per_ts(int32_t *value)
{
  *value = imp34dt05_mic_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}
uint8_t imp34dt05_mic_get_ioffset(float *value)
{
  *value = imp34dt05_mic_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_get_usb_dps(int32_t *value)
{
  *value = imp34dt05_mic_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_get_sd_dps(int32_t *value)
{
  *value = imp34dt05_mic_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_get_sensitivity(float *value)
{
  *value = 0.000030517578125; //2/(2^imp34dt05_mic_model.sensor_status.type.audio.resolution);
  return 0;
}
uint8_t imp34dt05_mic_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t imp34dt05_mic_get_sensor_annotation(char **value)
{
  uint16_t id = imp34dt05_mic_model.id;
  imp34dt05_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp34dt05_mic_model.annotation;
  return 0;
}
uint8_t imp34dt05_mic_get_sensor_category(int32_t *value)
{
  *value = imp34dt05_mic_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_get_stream_id(int8_t *value)
{
  *value = imp34dt05_mic_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_get_ep_id(int8_t *value)
{
  *value = imp34dt05_mic_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t imp34dt05_mic_set_odr(int32_t value)
{
  sys_error_code_t ret = SMSensorSetFrequency(imp34dt05_mic_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    imp34dt05_mic_model.sensor_status.type.audio.frequency = value;
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t imp34dt05_mic_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(imp34dt05_mic_model.id);
  }
  else
  {
    ret = SMSensorDisable(imp34dt05_mic_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    imp34dt05_mic_model.sensor_status.is_active = value;
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t imp34dt05_mic_set_volume(int32_t value)
{
  sys_error_code_t ret = SMSensorSetVolume(imp34dt05_mic_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    imp34dt05_mic_model.sensor_status.type.audio.volume = value;
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t imp34dt05_mic_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 1000;
  /* USER Code */
  if (value >= min_v && value <= max_v)
  {
    imp34dt05_mic_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  imp34dt05_mic_model.stream_params.spts = max_v;
  }
  else
  {
	  imp34dt05_mic_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t imp34dt05_mic_set_sensor_annotation(const char *value)
{
  strcpy(imp34dt05_mic_model.annotation, value);
  return 0;
}

/* ISM330DHCX_ACC PnPL Component ---------------------------------------------*/
static SensorModel_t ism330dhcx_acc_model;

uint8_t ism330dhcx_acc_comp_init(void)
{
  ism330dhcx_acc_model.comp_name = ism330dhcx_acc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_ACC);
  ism330dhcx_acc_model.id = id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  ism330dhcx_acc_model.stream_params.stream_id = -1;
  ism330dhcx_acc_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  ism330dhcx_acc_set_sensor_annotation(default_notes);
  /* USER Code */
  ism330dhcx_acc_set_enable(false);
  ism330dhcx_acc_set_odr(104);
#if (HSD_USE_DUMMY_DATA == 1)
  ism330dhcx_acc_set_samples_per_ts(0);
#else
  ism330dhcx_acc_set_samples_per_ts(104);
#endif
  app_model.s_models[id] = &ism330dhcx_acc_model;
  __stream_control(NULL, true);
  return 0;
}

char *ism330dhcx_acc_get_key(void)
{
  return "ism330dhcx_acc";
}

uint8_t ism330dhcx_acc_get_odr(float *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_fs(int32_t *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)ism330dhcx_acc_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_enable(bool *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_samples_per_ts(int32_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_dim(int32_t *value)
{
  *value = 3;
  return 0;
}
uint8_t ism330dhcx_acc_get_ioffset(float *value)
{
  *value = ism330dhcx_acc_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_measodr(float *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_usb_dps(int32_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_sd_dps(int32_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_sensitivity(float *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t ism330dhcx_acc_get_sensor_annotation(char **value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.annotation;
  return 0;
}
uint8_t ism330dhcx_acc_get_sensor_category(int32_t *value)
{
  *value = ism330dhcx_acc_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_stream_id(int8_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_ep_id(int8_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(ism330dhcx_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_acc_model.sensor_status.type.mems.odr = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    ism330dhcx_acc_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_acc_set_fs(int32_t value)
{
  sys_error_code_t ret = SMSensorSetFS(ism330dhcx_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_acc_model.sensor_status.type.mems.fs = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
  }
  return ret;
}
uint8_t ism330dhcx_acc_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(ism330dhcx_acc_model.id);
  }
  else
  {
    ret = SMSensorDisable(ism330dhcx_acc_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_acc_model.sensor_status.is_active = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_acc_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 1000;
  if (value >= min_v && value <= max_v)
  {
    ism330dhcx_acc_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  ism330dhcx_acc_model.stream_params.spts = max_v;
  }
  else
  {
	  ism330dhcx_acc_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t ism330dhcx_acc_set_sensor_annotation(const char *value)
{
  strcpy(ism330dhcx_acc_model.annotation, value);
  return 0;
}

/* ISM330DHCX_GYRO PnPL Component --------------------------------------------*/
static SensorModel_t ism330dhcx_gyro_model;

uint8_t ism330dhcx_gyro_comp_init(void)
{
  ism330dhcx_gyro_model.comp_name = ism330dhcx_gyro_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_GYRO);
  ism330dhcx_gyro_model.id = id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  ism330dhcx_gyro_model.stream_params.stream_id = -1;
  ism330dhcx_gyro_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  ism330dhcx_gyro_set_sensor_annotation(default_notes);
  /* USER Code */
  ism330dhcx_gyro_set_enable(false);
  ism330dhcx_gyro_set_odr(104);
#if (HSD_USE_DUMMY_DATA == 1)
  ism330dhcx_gyro_set_samples_per_ts(0);
#else
  ism330dhcx_gyro_set_samples_per_ts(104);
#endif
  app_model.s_models[id] = &ism330dhcx_gyro_model;
  __stream_control(NULL, true);
  return 0;
}

char *ism330dhcx_gyro_get_key(void)
{
  return "ism330dhcx_gyro";
}

uint8_t ism330dhcx_gyro_get_odr(float *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_fs(int32_t *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)ism330dhcx_gyro_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_enable(bool *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_samples_per_ts(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_dim(int32_t *value)
{
  *value = 3;
  return 0;
}
uint8_t ism330dhcx_gyro_get_ioffset(float *value)
{
  *value = ism330dhcx_gyro_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_measodr(float *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_usb_dps(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_sd_dps(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_sensitivity(float *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t ism330dhcx_gyro_get_sensor_annotation(char **value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.annotation;
  return 0;
}
uint8_t ism330dhcx_gyro_get_sensor_category(int32_t *value)
{
  *value = ism330dhcx_gyro_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_stream_id(int8_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_ep_id(int8_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(ism330dhcx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_gyro_model.sensor_status.type.mems.odr = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    ism330dhcx_gyro_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_gyro_set_fs(int32_t value)
{
  sys_error_code_t ret = SMSensorSetFS(ism330dhcx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_gyro_model.sensor_status.type.mems.fs = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
  }
  return ret;
}
uint8_t ism330dhcx_gyro_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(ism330dhcx_gyro_model.id);
  }
  else
  {
    ret = SMSensorDisable(ism330dhcx_gyro_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_gyro_model.sensor_status.is_active = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_gyro_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 1000;
  if (value >= min_v && value <= max_v)
  {
    ism330dhcx_gyro_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  ism330dhcx_gyro_model.stream_params.spts = max_v;
  }
  else
  {
	  ism330dhcx_gyro_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t ism330dhcx_gyro_set_sensor_annotation(const char *value)
{
  strcpy(ism330dhcx_gyro_model.annotation, value);
  return 0;
}

/* IIS2ICLX_ACC PnPL Component -----------------------------------------------*/
static SensorModel_t iis2iclx_acc_model;

uint8_t iis2iclx_acc_comp_init(void)
{
  iis2iclx_acc_model.comp_name = iis2iclx_acc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "iis2iclx", COM_TYPE_ACC);
  iis2iclx_acc_model.id = id;
  iis2iclx_acc_model.sensor_status = SMSensorGetStatus(id);
  iis2iclx_acc_model.stream_params.stream_id = -1;
  iis2iclx_acc_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  iis2iclx_acc_set_sensor_annotation(default_notes);
  /* USER Code */
  iis2iclx_acc_set_enable(false);
  iis2iclx_acc_set_odr(10);
#if (HSD_USE_DUMMY_DATA == 1)
  iis2iclx_acc_set_samples_per_ts(0);
#else
  iis2iclx_acc_set_samples_per_ts(10);
#endif
  app_model.s_models[id] = &iis2iclx_acc_model;
  __stream_control(NULL, true);
  return 0;
}

char *iis2iclx_acc_get_key(void)
{
  return "iis2iclx_acc";
}

uint8_t iis2iclx_acc_get_odr(float *value)
{
  uint16_t id = iis2iclx_acc_model.id;
  iis2iclx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2iclx_acc_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_fs(float *value)
{
  uint16_t id = iis2iclx_acc_model.id;
  iis2iclx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2iclx_acc_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_enable(bool *value)
{
  uint16_t id = iis2iclx_acc_model.id;
  iis2iclx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2iclx_acc_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_samples_per_ts(int32_t *value)
{
  *value = iis2iclx_acc_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_dim(int32_t *value)
{
  *value = 2;
  return 0;
}
uint8_t iis2iclx_acc_get_ioffset(float *value)
{
  *value = iis2iclx_acc_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_measodr(float *value)
{
  uint16_t id = iis2iclx_acc_model.id;
  iis2iclx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2iclx_acc_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_usb_dps(int32_t *value)
{
  *value = iis2iclx_acc_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_sd_dps(int32_t *value)
{
  *value = iis2iclx_acc_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_sensitivity(float *value)
{
  uint16_t id = iis2iclx_acc_model.id;
  iis2iclx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2iclx_acc_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t iis2iclx_acc_get_sensor_annotation(char **value)
{
  uint16_t id = iis2iclx_acc_model.id;
  iis2iclx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2iclx_acc_model.annotation;
  return 0;
}
uint8_t iis2iclx_acc_get_sensor_category(int32_t *value)
{
  *value = iis2iclx_acc_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_stream_id(int8_t *value)
{
  *value = iis2iclx_acc_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_get_ep_id(int8_t *value)
{
  *value = iis2iclx_acc_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t iis2iclx_acc_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(iis2iclx_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    iis2iclx_acc_model.sensor_status.type.mems.odr = value;
#if (HSD_USE_DUMMY_DATA != 1)
    iis2iclx_acc_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t iis2iclx_acc_set_fs(float value)
{
  sys_error_code_t ret = SMSensorSetFS(iis2iclx_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    iis2iclx_acc_model.sensor_status.type.mems.fs = value;
    /* USER Code */
  }
  return ret;
}
uint8_t iis2iclx_acc_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(iis2iclx_acc_model.id);
  }
  else
  {
    ret = SMSensorDisable(iis2iclx_acc_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    iis2iclx_acc_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t iis2iclx_acc_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 800;
  if (value >= min_v && value <= max_v)
  {
    iis2iclx_acc_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  iis2iclx_acc_model.stream_params.spts = max_v;
  }
  else
  {
	  iis2iclx_acc_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t iis2iclx_acc_set_sensor_annotation(const char *value)
{
  strcpy(iis2iclx_acc_model.annotation, value);
  return 0;
}


/* IIS2MDC_MAG PnPL Component ------------------------------------------------*/
static SensorModel_t iis2mdc_mag_model;

uint8_t iis2mdc_mag_comp_init(void)
{
  iis2mdc_mag_model.comp_name = iis2mdc_mag_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "iis2mdc", COM_TYPE_MAG);
  iis2mdc_mag_model.id = id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  iis2mdc_mag_model.stream_params.stream_id = -1;
  iis2mdc_mag_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  iis2mdc_mag_set_sensor_annotation(default_notes);
  /* USER Code */
  iis2mdc_mag_set_enable(false);
  iis2mdc_mag_set_odr(10);
#if (HSD_USE_DUMMY_DATA == 1)
  iis2mdc_mag_set_samples_per_ts(0);
#else
  iis2mdc_mag_set_samples_per_ts(10);
#endif
  app_model.s_models[id] = &iis2mdc_mag_model;
  __stream_control(NULL, true);
  return 0;
}

char *iis2mdc_mag_get_key(void)
{
  return "iis2mdc_mag";
}

uint8_t iis2mdc_mag_get_odr(int32_t *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)iis2mdc_mag_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_fs(int32_t *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)iis2mdc_mag_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_enable(bool *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_samples_per_ts(int32_t *value)
{
  *value = iis2mdc_mag_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_dim(int32_t *value)
{
  *value = 3;
  return 0;
}
uint8_t iis2mdc_mag_get_ioffset(float *value)
{
  *value = iis2mdc_mag_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_measodr(float *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_usb_dps(int32_t *value)
{
  *value = iis2mdc_mag_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_sd_dps(int32_t *value)
{
  *value = iis2mdc_mag_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_sensitivity(float *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t iis2mdc_mag_get_sensor_annotation(char **value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.annotation;
  return 0;
}
uint8_t iis2mdc_mag_get_sensor_category(int32_t *value)
{
  *value = iis2mdc_mag_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_stream_id(int8_t *value)
{
  *value = iis2mdc_mag_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_ep_id(int8_t *value)
{
  *value = iis2mdc_mag_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_set_odr(int32_t value)
{
  sys_error_code_t ret = SMSensorSetODR(iis2mdc_mag_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    iis2mdc_mag_model.sensor_status.type.mems.odr = value;
#if (HSD_USE_DUMMY_DATA != 1)
    iis2mdc_mag_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t iis2mdc_mag_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(iis2mdc_mag_model.id);
  }
  else
  {
    ret = SMSensorDisable(iis2mdc_mag_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    iis2mdc_mag_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t iis2mdc_mag_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 100;
  if (value >= min_v && value <= max_v)
  {
    iis2mdc_mag_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  iis2mdc_mag_model.stream_params.spts = max_v;
  }
  else
  {
	  iis2mdc_mag_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t iis2mdc_mag_set_sensor_annotation(const char *value)
{
  strcpy(iis2mdc_mag_model.annotation, value);
  return 0;
}

/* STTS22H_TEMP PnPL Component -----------------------------------------------*/
static SensorModel_t stts22h_temp_model;

uint8_t stts22h_temp_comp_init(void)
{
  stts22h_temp_model.comp_name = stts22h_temp_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "stts22h", COM_TYPE_TEMP);
  stts22h_temp_model.id = id;
  stts22h_temp_model.sensor_status = SMSensorGetStatus(id);
  stts22h_temp_model.stream_params.stream_id = -1;
  stts22h_temp_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  stts22h_temp_set_sensor_annotation(default_notes);
  /* USER Code */
  stts22h_temp_set_enable(false);
  stts22h_temp_set_odr(1);
#if (HSD_USE_DUMMY_DATA == 1)
  stts22h_temp_set_samples_per_ts(0);
#else
  stts22h_temp_set_samples_per_ts(1);
#endif
  app_model.s_models[id] = &stts22h_temp_model;
  __stream_control(NULL, true);
  return 0;
}

char *stts22h_temp_get_key(void)
{
  return "stts22h_temp";
}

uint8_t stts22h_temp_get_odr(int32_t *value)
{
  uint16_t id = stts22h_temp_model.id;
  stts22h_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)stts22h_temp_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_fs(int32_t *value)
{
  uint16_t id = stts22h_temp_model.id;
  stts22h_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)stts22h_temp_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_enable(bool *value)
{
  uint16_t id = stts22h_temp_model.id;
  stts22h_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = stts22h_temp_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_samples_per_ts(int32_t *value)
{
  *value = stts22h_temp_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}
uint8_t stts22h_temp_get_ioffset(float *value)
{
  *value = stts22h_temp_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_measodr(float *value)
{
  uint16_t id = stts22h_temp_model.id;
  stts22h_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = stts22h_temp_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_usb_dps(int32_t *value)
{
  *value = stts22h_temp_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_sd_dps(int32_t *value)
{
  *value = stts22h_temp_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_sensitivity(float *value)
{
  uint16_t id = stts22h_temp_model.id;
  stts22h_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = stts22h_temp_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_data_type(char **value)
{
  *value = "float";
  return 0;
}
uint8_t stts22h_temp_get_sensor_annotation(char **value)
{
  uint16_t id = stts22h_temp_model.id;
  stts22h_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = stts22h_temp_model.annotation;
  return 0;
}
uint8_t stts22h_temp_get_sensor_category(int32_t *value)
{
  *value = stts22h_temp_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_stream_id(int8_t *value)
{
  *value = stts22h_temp_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_get_ep_id(int8_t *value)
{
  *value = stts22h_temp_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t stts22h_temp_set_odr(int32_t value)
{
  sys_error_code_t ret = SMSensorSetODR(stts22h_temp_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    stts22h_temp_model.sensor_status.type.mems.odr = value;
#if (HSD_USE_DUMMY_DATA != 1)
    stts22h_temp_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t stts22h_temp_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(stts22h_temp_model.id);
  }
  else
  {
    ret = SMSensorDisable(stts22h_temp_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    stts22h_temp_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t stts22h_temp_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 200;
  if (value >= min_v && value <= max_v)
  {
    stts22h_temp_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  stts22h_temp_model.stream_params.spts = max_v;
  }
  else
  {
	  stts22h_temp_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t stts22h_temp_set_sensor_annotation(const char *value)
{
  strcpy(stts22h_temp_model.annotation, value);
  return 0;
}

/* ILPS22QS_PRESS PnPL Component ---------------------------------------------*/
static SensorModel_t ilps22qs_press_model;

uint8_t ilps22qs_press_comp_init(void)
{
  ilps22qs_press_model.comp_name = ilps22qs_press_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ilps22qs", COM_TYPE_PRESS);
  ilps22qs_press_model.id = id;
  ilps22qs_press_model.sensor_status = SMSensorGetStatus(id);
  ilps22qs_press_model.stream_params.stream_id = -1;
  ilps22qs_press_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  ilps22qs_press_set_sensor_annotation(default_notes);
  /* USER Code */
  ilps22qs_press_set_enable(false);
  ilps22qs_press_set_odr(1);
#if (HSD_USE_DUMMY_DATA == 1)
  ilps22qs_press_set_samples_per_ts(0);
#else
  ilps22qs_press_set_samples_per_ts(1);
#endif
  app_model.s_models[id] = &ilps22qs_press_model;
  __stream_control(NULL, true);
  return 0;
}

char *ilps22qs_press_get_key(void)
{
  return "ilps22qs_press";
}

uint8_t ilps22qs_press_get_odr(int32_t *value)
{
  uint16_t id = ilps22qs_press_model.id;
  ilps22qs_press_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)ilps22qs_press_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_fs(int32_t *value)
{
  uint16_t id = ilps22qs_press_model.id;
  ilps22qs_press_model.sensor_status = SMSensorGetStatus(id);
  *value = (int32_t)ilps22qs_press_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_enable(bool *value)
{
  uint16_t id = ilps22qs_press_model.id;
  ilps22qs_press_model.sensor_status = SMSensorGetStatus(id);
  *value = ilps22qs_press_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_samples_per_ts(int32_t *value)
{
  *value = ilps22qs_press_model.stream_params.spts;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}
uint8_t ilps22qs_press_get_ioffset(float *value)
{
  *value = ilps22qs_press_model.stream_params.ioffset;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_measodr(float *value)
{
  uint16_t id = ilps22qs_press_model.id;
  ilps22qs_press_model.sensor_status = SMSensorGetStatus(id);
  *value = ilps22qs_press_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_usb_dps(int32_t *value)
{
  *value = ilps22qs_press_model.stream_params.usb_dps;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_sd_dps(int32_t *value)
{
  *value = ilps22qs_press_model.stream_params.sd_dps;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_sensitivity(float *value)
{
  uint16_t id = ilps22qs_press_model.id;
  ilps22qs_press_model.sensor_status = SMSensorGetStatus(id);
  *value = ilps22qs_press_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_data_type(char **value)
{
  *value = "float";
  return 0;
}
uint8_t ilps22qs_press_get_sensor_annotation(char **value)
{
  uint16_t id = ilps22qs_press_model.id;
  ilps22qs_press_model.sensor_status = SMSensorGetStatus(id);
  *value = ilps22qs_press_model.annotation;
  return 0;
}
uint8_t ilps22qs_press_get_sensor_category(int32_t *value)
{
  *value = ilps22qs_press_model.sensor_status.isensor_class;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_stream_id(int8_t *value)
{
  *value = ilps22qs_press_model.stream_params.stream_id;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_get_ep_id(int8_t *value)
{
  *value = ilps22qs_press_model.stream_params.usb_ep;
  /* USER Code */
  return 0;
}
uint8_t ilps22qs_press_set_odr(int32_t value)
{
  sys_error_code_t ret = SMSensorSetODR(ilps22qs_press_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ilps22qs_press_model.sensor_status.type.mems.odr = value;
#if (HSD_USE_DUMMY_DATA != 1)
    ilps22qs_press_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ilps22qs_press_set_fs(int32_t value)
{
  sys_error_code_t ret = SMSensorSetFS(ilps22qs_press_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ilps22qs_press_model.sensor_status.type.mems.fs = value;
    /* USER Code */
  }
  return ret;
}
uint8_t ilps22qs_press_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(ilps22qs_press_model.id);
  }
  else
  {
    ret = SMSensorDisable(ilps22qs_press_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    ilps22qs_press_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ilps22qs_press_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 200;
  if (value >= min_v && value <= max_v)
  {
    ilps22qs_press_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
	  ilps22qs_press_model.stream_params.spts = max_v;
  }
  else
  {
	  ilps22qs_press_model.stream_params.spts = min_v;
  }
  return 0;
}
uint8_t ilps22qs_press_set_sensor_annotation(const char *value)
{
  strcpy(ilps22qs_press_model.annotation, value);
  return 0;
}

/* AutoMode PnPL Component ---------------------------------------------------*/
uint8_t automode_comp_init(void)
{
  app_model.automode_model.comp_name = automode_get_key();

  /* USER Component initialization code */
  return 0;
}

char *automode_get_key(void)
{
  return "automode";
}

uint8_t automode_get_enabled(bool *value)
{
  /* USER Code */
  *value = app_model.automode_model.enabled;
  return 0;
}
uint8_t automode_get_nof_acquisitions(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.nof_acquisitions;
  return 0;
}
uint8_t automode_get_start_delay_s(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.start_delay_s;
  return 0;
}
uint8_t automode_get_logging_period_s(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.logging_period_s;
  return 0;
}
uint8_t automode_get_idle_period_s(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.idle_period_s;
  return 0;
}
uint8_t automode_set_enabled(bool value)
{
  /* USER Code */
  app_model.automode_model.enabled = value;
  if (app_model.automode_model.start_delay_s == 0)
  {
    /* To avoid issue while opening SD card or handling files, setup a minimum default value */
    app_model.automode_model.start_delay_s = 3;
  }
  if (app_model.automode_model.idle_period_s == 0)
  {
    /* To avoid issue while opening SD card or handling files, setup a minimum default value */
    app_model.automode_model.idle_period_s = 3;
  }
  if (app_model.automode_model.logging_period_s == 0)
  {
    /* To avoid issue while opening SD card or handling files, setup a minimum default value */
    app_model.automode_model.logging_period_s = 3;
  }
  return 0;
}
uint8_t automode_set_nof_acquisitions(int32_t value)
{
  /* USER Code */
  app_model.automode_model.nof_acquisitions = value;
  return 0;
}
uint8_t automode_set_start_delay_s(int32_t value)
{
  /* USER Code */
  app_model.automode_model.start_delay_s = value;
  return 0;
}
uint8_t automode_set_logging_period_s(int32_t value)
{
  /* USER Code */
  app_model.automode_model.logging_period_s = value;
  return 0;
}
uint8_t automode_set_idle_period_s(int32_t value)
{
  /* USER Code */
  app_model.automode_model.idle_period_s = value;
  return 0;
}

/* Log Controller PnPL Component ---------------------------------------------*/
uint8_t log_controller_comp_init(void)
{
  app_model.log_controller_model.comp_name = log_controller_get_key();

  app_model.log_controller_model.status = false;
  app_model.log_controller_model.interface = -1;
  return 0;
}

char *log_controller_get_key(void)
{
  return "log_controller";
}

uint8_t log_controller_get_log_status(bool *value)
{
  *value = app_model.log_controller_model.status;
  return 0;
}
uint8_t log_controller_get_sd_mounted(bool *value)
{
  app_model.log_controller_model.sd_mounted = SD_IsDetected();
  *value = app_model.log_controller_model.sd_mounted;
  return 0;
}
uint8_t log_controller_get_controller_type(int32_t *value)
{
  *value = 0; /* 0 == HSD log controller, 1 == App classifier controller, 2 = generic log controller */
  return 0;
}
uint8_t log_controller_save_config(ILog_Controller_t *ifn)
{
  ILog_Controller_save_config(ifn);
  return 0;
}
uint8_t log_controller_start_log(ILog_Controller_t *ifn, int32_t interface)
{
  app_model.log_controller_model.interface = interface;

  __stream_control(ifn, TRUE);

  //Reset Tag counter
  TMResetTagListCounter();

  RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdate, RTC_FORMAT_BIN);

  _tm t =
  { .tm_year = sdate.Year + 2000, .tm_mon = sdate.Month - 1, .tm_mday = sdate.Date, .tm_hour = stime.Hours, .tm_min = stime.Minutes, .tm_sec = stime.Seconds };

// WHY THIS -1 (in months) ???
//  struct tm {
//     int tm_sec;         /* seconds,  range 0 to 59          */
//     int tm_min;         /* minutes, range 0 to 59           */
//     int tm_hour;        /* hours, range 0 to 23             */
//     int tm_mday;        /* day of the month, range 1 to 31  */
//     int tm_mon;         /* month, range 0 to 11             */ <------ (-1) months here (0..11), months from RTC (1..12)
//     int tm_year;        /* The number of years since 1900   */
//     int tm_wday;        /* day of the week, range 0 to 6    */
//     int tm_yday;        /* day in the year, range 0 to 365  */
//     int tm_isdst;       /* daylight saving time             */
//  };

  TMSetStartTime(t);
  sprintf(app_model.acquisition_info_model.start_time, "%04d-%02d-%02dT%02d:%02d:%02d", t.tm_year, t.tm_mon + 1,
          t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

  /* last part not done in sprintf to avoid a warning  */
  app_model.acquisition_info_model.start_time[19] = '.';
  app_model.acquisition_info_model.start_time[20] = '0';
  app_model.acquisition_info_model.start_time[21] = '0';
  app_model.acquisition_info_model.start_time[22] = '0';
  app_model.acquisition_info_model.start_time[23] = 'Z';
  app_model.acquisition_info_model.start_time[24] = '\0';

  ILog_Controller_start_log(ifn, interface);
  return 0;
}
uint8_t log_controller_stop_log(ILog_Controller_t *ifn)
{
  PnPLGenerateAcquisitionUUID(app_model.acquisition_info_model.uuid);
  TMCloseAllOpenedTags();
  ILog_Controller_stop_log(ifn);
  TMCalculateEndTime(app_model.acquisition_info_model.end_time);
  return 0;
}
uint8_t log_controller_set_time(ILog_Controller_t *ifn, const char *datetime)
{
  ILog_Controller_set_time(ifn, datetime);
  return 0;
}
uint8_t log_controller_switch_bank(ILog_Controller_t *ifn)
{
  ILog_Controller_switch_bank(ifn);
  return 0;
}

/* Tags Information PnPL Component -------------------------------------------*/
uint8_t tags_info_comp_init(void)
{
  app_model.tags_info_model.comp_name = tags_info_get_key();

  TMInit(HSD_SW_TAG_CLASS_NUM, HSD_HW_TAG_CLASS_NUM);

  tags_info_set_sw_tag0__enabled(true);
  tags_info_set_sw_tag1__enabled(true);
  tags_info_set_sw_tag2__enabled(true);
  tags_info_set_sw_tag3__enabled(true);
  tags_info_set_sw_tag4__enabled(true);
  tags_info_set_sw_tag0__status(false);
  tags_info_set_sw_tag1__status(false);
  tags_info_set_sw_tag2__status(false);
  tags_info_set_sw_tag3__status(false);
  tags_info_set_sw_tag4__status(false);
  return 0;
}

char *tags_info_get_key(void)
{
  return "tags_info";
}

uint8_t tags_info_get_max_tags_num(int32_t *value)
{
  *value = HSD_MAX_TAGS_NUM;
  return 0;
}
uint8_t tags_info_get_sw_tag0__label(char **value)
{
  *value = TMGetSWTagLabel(0);
  return 0;
}
uint8_t tags_info_get_sw_tag0__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(0);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag0__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(0);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag1__label(char **value)
{
  *value = TMGetSWTagLabel(1);
  return 0;
}
uint8_t tags_info_get_sw_tag1__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(1);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag1__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(1);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag2__label(char **value)
{
  *value = TMGetSWTagLabel(2);
  return 0;
}
uint8_t tags_info_get_sw_tag2__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(2);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag2__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(2);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag3__label(char **value)
{
  *value = TMGetSWTagLabel(3);
  return 0;
}
uint8_t tags_info_get_sw_tag3__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(3);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag3__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(3);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag4__label(char **value)
{
  *value = TMGetSWTagLabel(4);
  return 0;
}
uint8_t tags_info_get_sw_tag4__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(4);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag4__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(4);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_set_sw_tag0__label(const char *value)
{
  return TMSetSWTagLabel(value, 0);
}
uint8_t tags_info_set_sw_tag0__enabled(bool value)
{
  return TMEnableSWTag(value, 0);
}
uint8_t tags_info_set_sw_tag0__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 0);
  }
  else
  {
    TMInitSWTagStatus(false, 0);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag1__label(const char *value)
{
  return TMSetSWTagLabel(value, 1);
}
uint8_t tags_info_set_sw_tag1__enabled(bool value)
{
  return TMEnableSWTag(value, 1);
}
uint8_t tags_info_set_sw_tag1__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 1);
  }
  else
  {
    TMInitSWTagStatus(false, 1);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag2__label(const char *value)
{
  return TMSetSWTagLabel(value, 2);
}
uint8_t tags_info_set_sw_tag2__enabled(bool value)
{
  return TMEnableSWTag(value, 2);
}
uint8_t tags_info_set_sw_tag2__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 2);
  }
  else
  {
    TMInitSWTagStatus(false, 2);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag3__label(const char *value)
{
  return TMSetSWTagLabel(value, 3);
}
uint8_t tags_info_set_sw_tag3__enabled(bool value)
{
  return TMEnableSWTag(value, 3);
}
uint8_t tags_info_set_sw_tag3__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 3);
  }
  else
  {
    TMInitSWTagStatus(false, 3);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag4__label(const char *value)
{
  return TMSetSWTagLabel(value, 4);
}
uint8_t tags_info_set_sw_tag4__enabled(bool value)
{
  return TMEnableSWTag(value, 4);
}
uint8_t tags_info_set_sw_tag4__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 4);
  }
  else
  {
    TMInitSWTagStatus(false, 4);
  }
  return 0;
}

/* Acquisition Information PnPL Component ------------------------------------*/
uint8_t acquisition_info_comp_init(void)
{
  app_model.acquisition_info_model.comp_name = acquisition_info_get_key();
  acquisition_info_set_name("STWIN.Box_acquisition");
  acquisition_info_set_description("");
  app_model.acquisition_info_model.interface = -1;
  return 0;
}

char *acquisition_info_get_key(void)
{
  return "acquisition_info";
}

uint8_t acquisition_info_get_name(char **value)
{
  *value = app_model.acquisition_info_model.name;
  return 0;
}
uint8_t acquisition_info_get_description(char **value)
{
  *value = app_model.acquisition_info_model.description;
  return 0;
}
uint8_t acquisition_info_get_uuid(char **value)
{
  *value = app_model.acquisition_info_model.uuid;
  return 0;
}
uint8_t acquisition_info_get_start_time(char **value)
{
  *value = app_model.acquisition_info_model.start_time;
  return 0;
}
uint8_t acquisition_info_get_end_time(char **value)
{
  *value = app_model.acquisition_info_model.end_time;
  return 0;
}
uint8_t acquisition_info_get_data_ext(char **value)
{
  *value = ".dat";
  return 0;
}
uint8_t acquisition_info_get_data_fmt(char **value)
{
  *value = "HSD_2.0.0";
  return 0;
}
uint8_t acquisition_info_get_tags(JSON_Value *value)
{
  JSON_Value *tempJSON1;
  JSON_Object *JSON_Tags;
  JSON_Array *JSON_TagsArray;

  JSON_Tags = json_value_get_object(value);

  json_object_set_value(JSON_Tags, "tags", json_value_init_array());
  JSON_TagsArray = json_object_dotget_array(JSON_Tags, "tags");

  int i;

  HSD_Tag_t *tag_list = TMGetTagList();
  uint8_t tag_list_size = TMGetTagListSize();
  if (tag_list_size > 0)
  {
    for (i = 0; i < tag_list_size; i++)
    {
      tempJSON1 = json_value_init_object();
      JSON_Object *tag_object = json_value_get_object(tempJSON1);
      json_object_set_string(tag_object, "l", tag_list[i].label);
      json_object_set_boolean(tag_object, "e", tag_list[i].status);
      json_object_set_string(tag_object, "ta", tag_list[i].abs_timestamp);
      json_array_append_value(JSON_TagsArray, tempJSON1);
    }
  }
  /* no need to free tempJSON1 as it is part of value */
  return 0;
}
uint8_t acquisition_info_get_interface(char **value)
{
  int8_t acq_interface = app_model.acquisition_info_model.interface;
  switch (acq_interface)
  {
    case 0:
      *value = "SD Card";
      break;
    case 1:
      *value = "USB";
      break;
  }
  return 0;
}
uint8_t acquisition_info_get_schema_version(char **value)
{
  *value = "2.0.0";
  return 0;
}
uint8_t acquisition_info_set_name(const char *value)
{
  if (strlen(value) != 0)
  {
    strcpy(app_model.acquisition_info_model.name, value);
  }
  return 0;
}
uint8_t acquisition_info_set_description(const char *value)
{
  strcpy(app_model.acquisition_info_model.description, value);
  return 0;
}

/* Firmware Information PnPL Component ---------------------------------------*/
uint8_t firmware_info_comp_init(void)
{
  app_model.firmware_info_model.comp_name = firmware_info_get_key();

  char default_alias[DEVICE_ALIAS_LENGTH] = "STWIN_BOX_001";
  firmware_info_set_alias(default_alias);

  char default_mac_address[MAC_ADDRESS_LENGTH] = "00:00:00:00:00:00";
  set_mac_address(default_mac_address);
  return 0;
}

char *firmware_info_get_key(void)
{
  return "firmware_info";
}

uint8_t firmware_info_get_alias(char **value)
{
  *value = app_model.firmware_info_model.alias;
  return 0;
}
uint8_t firmware_info_get_fw_name(char **value)
{
  *value = "FP-SNS-DATALOG2_PDetect";
  return 0;
}
uint8_t firmware_info_get_fw_version(char **value)
{
  *value = FW_VERSION_MAJOR "." FW_VERSION_MINOR "." FW_VERSION_PATCH;
  return 0;
}
uint8_t firmware_info_get_part_number(char **value)
{
  *value = "FP-SNS-DATALOG2";
  return 0;
}
uint8_t firmware_info_get_device_url(char **value)
{
  *value = "https://www.st.com/stwinbox";
  return 0;
}
uint8_t firmware_info_get_fw_url(char **value)
{
  *value = "https://github.com/STMicroelectronics/fp-sns-datalog2";
  return 0;
}
uint8_t firmware_info_get_mac_address(char **value)
{
  *value = app_model.firmware_info_model.mac_address;
  return 0;
}
uint8_t firmware_info_set_alias(const char *value)
{
  strcpy(app_model.firmware_info_model.alias, value);
  return 0;
}

uint8_t set_mac_address(const char *value)
{
  strcpy(app_model.firmware_info_model.mac_address, value);
  return 0;
}

/* Device Information PnPL Component -----------------------------------------*/
uint8_t DeviceInformation_comp_init(void)
{
  return 0;
}

char *DeviceInformation_get_key(void)
{
  return "DeviceInformation";
}

uint8_t DeviceInformation_get_manufacturer(char **value)
{
  *value = "STMicroelectronics";
  return 0;
}
uint8_t DeviceInformation_get_model(char **value)
{
  *value = "STEVAL-STWINBX1";
  return 0;
}
uint8_t DeviceInformation_get_swVersion(char **value)
{
  *value = FW_VERSION_MAJOR "." FW_VERSION_MINOR "." FW_VERSION_PATCH;
  return 0;
}
uint8_t DeviceInformation_get_osName(char **value)
{
  *value = "AzureRTOS";
  return 0;
}
uint8_t DeviceInformation_get_processorArchitecture(char **value)
{
  *value = "ARM Cortex-M33";
  return 0;
}
uint8_t DeviceInformation_get_processorManufacturer(char **value)
{
  *value = "STMicroelectronics";
  return 0;
}
uint8_t DeviceInformation_get_totalStorage(float *value)
{
  *value = 0;
  if (SD_IsDetected())
  {
//    BSP_SD_CardInfo CardInfo;
    HAL_SD_CardInfoTypeDef CardInfo;
    SD_GetCardInfo(&CardInfo);
    *value = round(((float)CardInfo.BlockNbr * (float)CardInfo.BlockSize) / 1000000000.0f);
  }
  return 0;
}
uint8_t DeviceInformation_get_totalMemory(float *value)
{
  *value = (SRAM1_SIZE + SRAM2_SIZE + SRAM3_SIZE + SRAM4_SIZE) / 1024;
  return 0;
}

/* USER Code : --> compute stream ids */
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status)
{
  uint32_t i;

  AppModel_t *p_app_model = getAppModel();

  if (status) //set stream id
  {
    int8_t j, stream_id = 0;
    //sort stream id by bandwidth
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      if (p_app_model->s_models[i] != NULL)
      {
        if (p_app_model->s_models[i]->sensor_status.is_active == true)
        {
          /* Get sensor's bandwidth */
          if (p_app_model->s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_MEMS)
          {
        	p_app_model->s_models[i]->stream_params.bandwidth = p_app_model->s_models[i]->sensor_status.type.mems.odr * SMGetnBytesPerSample(i);
          }
          else if (p_app_model->s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_AUDIO)
          {
        	p_app_model->s_models[i]->stream_params.bandwidth = p_app_model->s_models[i]->sensor_status.type.audio.frequency * SMGetnBytesPerSample(i);
          }
          else if (p_app_model->s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_LIGHT)
          {
        	p_app_model->s_models[i]->stream_params.bandwidth = (1000.0f / (float)p_app_model->s_models[i]->sensor_status.type.light.intermeasurement_time) * SMGetnBytesPerSample(i);
          }
          else if (p_app_model->s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_PRESENCE)
          {
        	p_app_model->s_models[i]->stream_params.bandwidth = p_app_model->s_models[i]->sensor_status.type.presence.data_frequency * SMGetnBytesPerSample(i);
          }
          else if (p_app_model->s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_RANGING)
          {
        	p_app_model->s_models[i]->stream_params.bandwidth = p_app_model->s_models[i]->sensor_status.type.ranging.profile_config.frequency * SMGetnBytesPerSample(i);
          }
          else
          {
            /* TODO: add support for other ISENSOR_CLASS */
          }

          /* This section is executed only after "Start" command when interface != -1 */

          if (p_app_model->log_controller_model.interface == LOG_CTRL_MODE_SD)
          {
            __sc_set_sd_stream_params(&app_model, i);
          }
          else if (p_app_model->log_controller_model.interface == LOG_CTRL_MODE_USB)
          {
            __sc_set_usb_stream_params(&app_model, i);
          }

          p_app_model->s_models[i]->stream_params.stream_id = stream_id;
          stream_id++;

          /*
           * Ensure that elements with higher bandwidth are assigned lower stream_id.
           * Check if the current element has a higher bandwidth than any previous elements (j).
           * If so, the current elements take the place of the previous one that is incremented by one.
           */
          StreamParams_t *p_current = &(p_app_model->s_models[i]->stream_params);
          StreamParams_t *p_other;
          j = i - 1;
          while (j >= 0)
          {
            if (p_app_model->s_models[j] != NULL)
            {
              p_other = &(p_app_model->s_models[j]->stream_params);
              if (p_other->bandwidth <= p_current->bandwidth)
              {
                if (p_other->stream_id >= 0)
                {
                  if (p_other->stream_id < p_current->stream_id)
                  {
                    p_current->stream_id = p_other->stream_id;
                  }
                  p_other->stream_id++;
                }
              }
            }
            j--;
          }
        }
        else
        {
          p_app_model->s_models[i]->stream_params.bandwidth = 0;
          p_app_model->s_models[i]->stream_params.stream_id = -1;
        }
      }
    }
    __sc_set_usb_enpoints(p_app_model);
  }
  else
  {
    __sc_reset_stream_params(p_app_model);
  }
  return 0;
}


/* Maximum time between two consecutive stream packets */
#define SC_SDCARD_WRITE_PERIOD           0.33f

#define SC_DL2_PROTOCOL_COUNTER_SIZE     4U
#define SC_DL2_PROTOCOL_TIMESTAMP_SIZE   8U

static sys_error_code_t __sc_set_sd_stream_params(AppModel_t *p_app_model, uint32_t id)
{
  assert_param(p_app_model != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SensorModel_t **p_s_models = p_app_model->s_models;
  uint16_t fifo_watermark;

  /* SD write every SC_SDCARD_WRITE_PERIOD ms of sensor data. */
  p_s_models[id]->stream_params.sd_dps = (uint32_t)(p_s_models[id]->stream_params.bandwidth * SC_SDCARD_WRITE_PERIOD);
  /* Access to SD is optimized when buffer dimension is multiple of 512 */
  p_s_models[id]->stream_params.sd_dps = p_s_models[id]->stream_params.sd_dps - (p_s_models[id]->stream_params.sd_dps % 512) + 512;

  /* If the sensor is very slow, we force a write every 1 second --> sd_dps = bandwidth
   * It's no longer multiple of 512 */
  if (p_s_models[id]->stream_params.sd_dps > p_s_models[id]->stream_params.bandwidth)
  {
    p_s_models[id]->stream_params.sd_dps = (uint32_t)p_s_models[id]->stream_params.bandwidth
                                           + SC_DL2_PROTOCOL_TIMESTAMP_SIZE + SC_DL2_PROTOCOL_COUNTER_SIZE;
  }

  fifo_watermark = p_s_models[id]->stream_params.sd_dps / SMGetnBytesPerSample(id) / 2;
  if (fifo_watermark == 0)
  {
    fifo_watermark = 1;
  }
  res = SMSensorSetFifoWM(id, fifo_watermark);

  return res;
}


/* Max DPS for USB */
#define SC_USB_DPS_MAX                7000U

/* Under this limit the stream is considered "slow" */
#define SC_USB_SLOW_ODR_LIMIT_HZ      20.0f

/* Maximum time between two consecutive stream packets */
#define SC_USB_MAX_PACKETS_PERIOD     0.05f

static sys_error_code_t __sc_set_usb_stream_params(AppModel_t *p_app_model, uint32_t id)
{
  assert_param(p_app_model != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SensorModel_t **p_s_models = p_app_model->s_models;
  uint16_t fifo_watermark;

  /* in case of slow sensor send 1 sample for each usb packet */
  float low_odr = 0;

  low_odr = SMSensorGetSamplesPerSecond(id);

  if (low_odr <= SC_USB_SLOW_ODR_LIMIT_HZ)
  {
    /* When there's a timestamp, more then one packet will be sent */
    p_s_models[id]->stream_params.usb_dps = SMGetnBytesPerSample(id) + 8; /* 8 = timestamp dimension in bytes */
    fifo_watermark = 1;
  }
  else
  {
    /* 50ms of sensor data; when there's a timestamp packets will be sent fastly */
    p_s_models[id]->stream_params.usb_dps = (uint32_t)(p_s_models[id]->stream_params.bandwidth * SC_USB_MAX_PACKETS_PERIOD);
    if (p_s_models[id]->stream_params.usb_dps > SC_USB_DPS_MAX)
    {
      p_s_models[id]->stream_params.usb_dps = SC_USB_DPS_MAX; // set a limit to avoid buffer to big
    }
    else if (p_s_models[id]->stream_params.usb_dps < SMGetnBytesPerSample(id) + 8)
    {
      /* In case usb_dps is a very low value, verify the setup to send at least 1 sensor data + timestamp */
      p_s_models[id]->stream_params.usb_dps = SMGetnBytesPerSample(id) + 8;
    }

    fifo_watermark = p_s_models[id]->stream_params.usb_dps / SMGetnBytesPerSample(id) / 2;
  }

  res = SMSensorSetFifoWM(id, fifo_watermark);

#ifdef SYS_DEBUG
  SensorDescriptor_t descriptor = SMSensorGetDescription(id);
  float ms = p_s_models[id]->stream_params.usb_dps / p_s_models[id]->stream_params.bandwidth;
  SYS_DEBUGF(
    SYS_DBG_LEVEL_VERBOSE,
    ("**** %s, odr: %f, DPS: %d, ms: %f, FIFO WM: %d \r\n", descriptor.p_name, p_s_models[id]->sensor_status.type.mems.odr,
     p_s_models[id]->stream_params.usb_dps, ms, fifo_watermark));
#endif

  return res;
}

static void __sc_set_usb_enpoints(AppModel_t *p_app_model)
{
  assert_param(p_app_model != NULL);
  SensorModel_t **p_s_models = p_app_model->s_models;
  uint32_t i;
  int8_t stream_id;

  for (i = 0; i < SENSOR_NUMBER; i++)
  {
    if (p_s_models[i] != NULL)
    {
      stream_id = p_s_models[i]->stream_params.stream_id;
      if (stream_id < (int8_t)(SS_N_IN_ENDPOINTS - 1))
      {
        /* Fastest streams on dedicated endpoints */
        p_s_models[i]->stream_params.usb_ep = stream_id;
      }
      else
      {
        /* Slowest streams multiplexed on last endpoint */
        p_s_models[i]->stream_params.usb_ep = SS_N_IN_ENDPOINTS - 1;
      }
    }
  }
}


static void __sc_reset_stream_params(AppModel_t *p_app_model)
{
  assert_param(p_app_model != NULL);
  SensorModel_t **p_s_models = p_app_model->s_models;
  uint32_t i;

  for (i = 0; i < SENSOR_NUMBER; i++)
  {
    if (p_s_models[i] != NULL)
    {
      p_s_models[i]->stream_params.stream_id = -1;
      p_s_models[i]->stream_params.usb_ep = -1;
      p_s_models[i]->stream_params.bandwidth = 0;
    }
  }
}

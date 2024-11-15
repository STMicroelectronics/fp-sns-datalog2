/**
  ******************************************************************************
  * @file    App_model_Vl53l8cx_3_Tof.c
  * @author  SRA
  * @brief   Vl53l8cx_3_Tof PnPL Components APIs
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
  * This file has been auto generated from the following DTDL Component:
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vl53l8cx_3_tof;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include "services/SQuery.h"

/* USER includes -------------------------------------------------------------*/

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* VL53L8CX_2_TOF PnPL Component ---------------------------------------------*/
static SensorModel_t vl53l8cx_3_tof_model;
extern AppModel_t app_model;

uint8_t vl53l8cx_3_tof_comp_init(void)
{
  vl53l8cx_3_tof_model.comp_name = vl53l8cx_3_tof_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "vl53l8cx_3", COM_TYPE_TOF);
  vl53l8cx_3_tof_model.id = id;
  vl53l8cx_3_tof_model.sensor_status = SMSensorGetStatusPointer(id);
  vl53l8cx_3_tof_model.stream_params.stream_id = -1;
  vl53l8cx_3_tof_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &vl53l8cx_3_tof_model);

  vl53l8cx_3_tof_set_sensor_annotation("[EXTERN]\0", NULL);
  vl53l8cx_3_tof_set_resolution(pnpl_vl53l8cx_3_tof_resolution_n8x8, NULL);
  vl53l8cx_3_tof_set_odr(10, NULL);
  vl53l8cx_3_tof_set_ranging_mode(pnpl_vl53l8cx_3_tof_ranging_mode_continuous, NULL);
  vl53l8cx_3_tof_set_integration_time(10, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  vl53l8cx_3_tof_set_samples_per_ts(0, NULL);
#else
  vl53l8cx_3_tof_set_samples_per_ts(10, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *vl53l8cx_3_tof_get_key(void)
{
  return "vl53l8cx_3_tof";
}


uint8_t vl53l8cx_3_tof_get_enable(bool *value)
{
  *value = vl53l8cx_3_tof_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_resolution(pnpl_vl53l8cx_3_tof_resolution_t *enum_id)
{
//  /* vl53l8cx ranging profiles */
//  #define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
//  #define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
//  #define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
//  #define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)
  uint8_t resolution = vl53l8cx_3_tof_model.sensor_status->type.ranging.profile_config.ranging_profile;
  switch (resolution)
  {
    case 1:
    case 2:
      *enum_id = pnpl_vl53l8cx_3_tof_resolution_n4x4;
      break;
    case 3:
    case 4:
      *enum_id = pnpl_vl53l8cx_3_tof_resolution_n8x8;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_odr(int32_t *value)
{
  *value = vl53l8cx_3_tof_model.sensor_status->type.ranging.profile_config.frequency;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_ranging_mode(pnpl_vl53l8cx_3_tof_ranging_mode_t *enum_id)
{
//  /* vl53l8cx ranging profiles */
//  #define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
//  #define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
//  #define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
//  #define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)
  uint8_t ranging_mode = vl53l8cx_3_tof_model.sensor_status->type.ranging.profile_config.ranging_profile;
  switch (ranging_mode)
  {
    case 1:
    case 3:
      *enum_id = pnpl_vl53l8cx_3_tof_ranging_mode_continuous;
      break;
    case 2:
    case 4:
      *enum_id = pnpl_vl53l8cx_3_tof_ranging_mode_autonomous;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_integration_time(int32_t *value)
{
  *value = vl53l8cx_3_tof_model.sensor_status->type.ranging.profile_config.timing_budget;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_samples_per_ts(int32_t *value)
{
  *value = vl53l8cx_3_tof_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_ioffset(float *value)
{
  *value = vl53l8cx_3_tof_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_usb_dps(int32_t *value)
{
  *value = vl53l8cx_3_tof_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_sd_dps(int32_t *value)
{
  *value = vl53l8cx_3_tof_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_data_type(char **value)
{
  *value = "uint32_t";
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_sensor_annotation(char **value)
{
  *value = vl53l8cx_3_tof_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_sensor_category(int32_t *value)
{
  *value = vl53l8cx_3_tof_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_mounted(bool *value)
{
  *value = true;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_dim(int32_t *value)
{
  uint8_t profile = vl53l8cx_3_tof_model.sensor_status->type.ranging.profile_config.ranging_profile;

//  /* vl53l8cx ranging profiles */
//  #define VL53L8CX_PROFILE_4x4_CONTINUOUS        (1U)
//  #define VL53L8CX_PROFILE_4x4_AUTONOMOUS        (2U)
//  #define VL53L8CX_PROFILE_8x8_CONTINUOUS        (3U)
//  #define VL53L8CX_PROFILE_8x8_AUTONOMOUS        (4U)

  switch (profile)
  {
    case 1:
    case 2:
#ifdef TOF_EXTENDED
      *value = 128; /*8 output value x each zone -> 8x(4x4)*/
#else
      *value = 32; /*2 output value x each zone -> 8x(4x4)*/
#endif
      break;
    case 3:
    case 4:
#ifdef TOF_EXTENDED
      *value = 512; /*8 output value x each zone -> 8x(8x8)*/
#else
      *value = 128; /*2 output value x each zone -> 8x(8x8)*/
#endif
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_output_format__target_status__start_id(int32_t *value)
{
  *value = 0;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_output_format__target_distance__start_id(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_output_format__nof_outputs(int32_t *value)
{
  *value = 2;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_stream_id(int8_t *value)
{
  *value = vl53l8cx_3_tof_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l8cx_3_tof_get_ep_id(int8_t *value)
{
  *value = vl53l8cx_3_tof_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t vl53l8cx_3_tof_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(vl53l8cx_3_tof_model.id);
  }
  else
  {
    ret = SMSensorDisable(vl53l8cx_3_tof_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t vl53l8cx_3_tof_set_resolution(pnpl_vl53l8cx_3_tof_resolution_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint8_t value;
  switch (enum_id)
  {
    case pnpl_vl53l8cx_3_tof_resolution_n4x4:
      value = 16;
      break;
    case pnpl_vl53l8cx_3_tof_resolution_n8x8:
      value = 64;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetResolution(vl53l8cx_3_tof_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t vl53l8cx_3_tof_set_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ret = SMSensorSetFrequency(vl53l8cx_3_tof_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    vl53l8cx_3_tof_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
  }
  return ret;
}

uint8_t vl53l8cx_3_tof_set_ranging_mode(pnpl_vl53l8cx_3_tof_ranging_mode_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;

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

  uint32_t value;
  uint8_t mode = vl53l8cx_3_tof_model.sensor_status->type.ranging.profile_config.mode;
  switch (enum_id)
  {
    case pnpl_vl53l8cx_3_tof_ranging_mode_continuous:
      if (mode == 1 || mode == 2)
      {
        value = 1;
      }
      else
      {
        value = 3;
      }
      break;
    case pnpl_vl53l8cx_3_tof_ranging_mode_autonomous:
      if (mode == 1 || mode == 2)
      {
        value = 2;
      }
      else
      {
        value = 4;
      }
      break;
    default:
      return 1;
  }
  ret = SMSensorSetRangingMode(vl53l8cx_3_tof_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vl53l8cx_3_tof_set_integration_time(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 2;
  int32_t max_v = 1000;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetIntegrationTime(vl53l8cx_3_tof_model.id, value);
  }
  return ret;
}

uint8_t vl53l8cx_3_tof_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 60;
  if (value >= min_v && value <= max_v)
  {
    vl53l8cx_3_tof_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    vl53l8cx_3_tof_model.stream_params.spts = max_v;
  }
  else
  {
    vl53l8cx_3_tof_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t vl53l8cx_3_tof_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(vl53l8cx_3_tof_model.annotation, value);
  return ret;
}


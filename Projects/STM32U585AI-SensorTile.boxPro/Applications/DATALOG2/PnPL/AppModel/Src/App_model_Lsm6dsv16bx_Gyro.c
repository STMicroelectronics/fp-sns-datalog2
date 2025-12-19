/**
  ******************************************************************************
  * @file    App_model_Lsm6dsv16bx_Gyro.c
  * @author  SRA
  * @brief   Lsm6dsv16bx_Gyro PnPL Components APIs
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16bx_gyro;2
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

/* LSM6DSV16BX_GYRO PnPL Component -------------------------------------------*/
static SensorModel_t lsm6dsv16bx_gyro_model;
extern AppModel_t app_model;

uint8_t lsm6dsv16bx_gyro_comp_init(void)
{
  lsm6dsv16bx_gyro_model.comp_name = lsm6dsv16bx_gyro_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lsm6dsv16bx", COM_TYPE_GYRO);
  lsm6dsv16bx_gyro_model.id = id;
  lsm6dsv16bx_gyro_set_st_ble_stream__id(id, NULL);

  lsm6dsv16bx_gyro_model.sensor_status = SMSensorGetStatusPointer(id);
  lsm6dsv16bx_gyro_model.stream_params.stream_id = -1;
  lsm6dsv16bx_gyro_model.stream_params.usb_ep = -1;

  lsm6dsv16bx_gyro_set_st_ble_stream__gyro_unit("dps", NULL);
  lsm6dsv16bx_gyro_set_st_ble_stream__gyro_format("int16_t", NULL);

  addSensorToAppModel(id, &lsm6dsv16bx_gyro_model);

  lsm6dsv16bx_gyro_set_sensor_annotation("[EXTERN]\0", NULL);
  lsm6dsv16bx_gyro_set_odr(pnpl_lsm6dsv16bx_gyro_odr_hz7680, NULL);
  lsm6dsv16bx_gyro_set_fs(pnpl_lsm6dsv16bx_gyro_fs_dps4000, NULL);
  lsm6dsv16bx_gyro_set_enable(false, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  lsm6dsv16bx_gyro_set_samples_per_ts(0, NULL);
#else
  lsm6dsv16bx_gyro_set_samples_per_ts(1000, NULL);
#endif

  int32_t value = 0;
  lsm6dsv16bx_gyro_get_dim(&value);
  lsm6dsv16bx_gyro_set_st_ble_stream__gyro_channels(value, NULL);
  float_t sensitivity = 0.0f;
  lsm6dsv16bx_gyro_get_sensitivity(&sensitivity);
  lsm6dsv16bx_gyro_set_st_ble_stream__gyro_multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(lsm6dsv16bx_gyro_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *lsm6dsv16bx_gyro_get_key(void)
{
  return "lsm6dsv16bx_gyro";
}


uint8_t lsm6dsv16bx_gyro_get_odr(pnpl_lsm6dsv16bx_gyro_odr_t *enum_id)
{
  float_t odr = lsm6dsv16bx_gyro_model.sensor_status->type.mems.odr;
  if (odr < 8.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz7_5;
  }
  else if (odr < 16.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz15;
  }
  else if (odr < 31.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz30;
  }
  else if (odr < 61.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz60;
  }
  else if (odr < 121.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz120;
  }
  else if (odr < 241.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz240;
  }
  else if (odr < 481.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz480;
  }
  else if (odr < 961.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz960;
  }
  else if (odr < 1921.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz1920;
  }
  else if (odr < 3841.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz3840;
  }
  else
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_odr_hz7680;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_fs(pnpl_lsm6dsv16bx_gyro_fs_t *enum_id)
{
  float_t fs = lsm6dsv16bx_gyro_model.sensor_status->type.mems.fs;
  if (fs < 126.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_fs_dps125;
  }
  else if (fs < 251.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_fs_dps250;
  }
  else if (fs < 501.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_fs_dps500;
  }
  else if (fs < 1001.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_fs_dps1000;
  }
  else if (fs < 2001.0f)
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_fs_dps2000;
  }
  else
  {
    *enum_id = pnpl_lsm6dsv16bx_gyro_fs_dps4000;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_enable(bool *value)
{
  *value = lsm6dsv16bx_gyro_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_samples_per_ts(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_ioffset(float_t *value)
{
  *value = lsm6dsv16bx_gyro_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_measodr(float_t *value)
{
  *value = lsm6dsv16bx_gyro_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_usb_dps(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_sd_dps(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_sensitivity(float_t *value)
{
  *value = lsm6dsv16bx_gyro_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_sensor_annotation(char **value)
{
  *value = lsm6dsv16bx_gyro_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_sensor_category(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_mounted(bool *value)
{
  *value = true;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__id(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__gyro_enable(bool *value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__gyro_unit(char **value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__gyro_format(char **value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__gyro_elements(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__gyro_channels(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__gyro_multiply_factor(float_t *value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_st_ble_stream__gyro_odr(int32_t *value)
{
  *value = lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_stream_id(int8_t *value)
{
  *value = lsm6dsv16bx_gyro_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_gyro_get_ep_id(int8_t *value)
{
  *value = lsm6dsv16bx_gyro_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t lsm6dsv16bx_gyro_set_odr(pnpl_lsm6dsv16bx_gyro_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_lsm6dsv16bx_gyro_odr_hz7_5:
      value = 7.5f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz15:
      value = 15.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz30:
      value = 30.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz60:
      value = 60.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz120:
      value = 120.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz240:
      value = 240.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz480:
      value = 480.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz960:
      value = 960.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz1920:
      value = 1920.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz3840:
      value = 3840.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_odr_hz7680:
      value = 7680.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(lsm6dsv16bx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.lsm6dsv16bx_mlc_ucf_valid == true)
    {
      app_model.lsm6dsv16bx_mlc_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    lsm6dsv16bx_gyro_set_samples_per_ts((int32_t)value, NULL);
#endif
    if (__stream_control(true) != PNPL_NO_ERROR_CODE)
    {
      if (response_message != NULL)
      {
        *response_message = "PnPL_Warning: Safe bandwidth limit exceeded. Consider disabling sensors or lowering ODRs to avoid possible data corruption";
      }
      ret = PNPL_BASE_ERROR_CODE;
    }
    __sc_set_ble_stream_params(lsm6dsv16bx_gyro_model.id);
  }
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_fs(pnpl_lsm6dsv16bx_gyro_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_lsm6dsv16bx_gyro_fs_dps125:
      value = 125.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_fs_dps250:
      value = 250.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_fs_dps500:
      value = 500.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_fs_dps1000:
      value = 1000.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_fs_dps2000:
      value = 2000.0f;
      break;
    case pnpl_lsm6dsv16bx_gyro_fs_dps4000:
      value = 4000.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetFS(lsm6dsv16bx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.lsm6dsv16bx_mlc_ucf_valid == true)
    {
      app_model.lsm6dsv16bx_mlc_ucf_valid = false;
    }
  }

  float_t sensitivity = 0.0f;
  lsm6dsv16bx_gyro_get_sensitivity(&sensitivity);
  lsm6dsv16bx_gyro_set_st_ble_stream__gyro_multiply_factor(sensitivity, NULL);

  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(lsm6dsv16bx_gyro_model.id);
  }
  else
  {
    ret = SMSensorDisable(lsm6dsv16bx_gyro_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.lsm6dsv16bx_mlc_ucf_valid == true)
    {
      app_model.lsm6dsv16bx_mlc_ucf_valid = false;
    }
    if (__stream_control(true) != PNPL_NO_ERROR_CODE)
    {
      if (response_message != NULL)
      {
        *response_message = "PnPL_Warning: Safe bandwidth limit exceeded. Consider disabling sensors or lowering ODRs to avoid possible data corruption";
      }
      ret = PNPL_BASE_ERROR_CODE;
    }
    __sc_set_ble_stream_params(lsm6dsv16bx_gyro_model.id);
  }
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 1000;
  if (value >= min_v && value <= max_v)
  {
    lsm6dsv16bx_gyro_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    lsm6dsv16bx_gyro_model.stream_params.spts = max_v;
  }
  else
  {
    lsm6dsv16bx_gyro_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lsm6dsv16bx_gyro_model.annotation, value);
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__gyro_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__gyro_unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__gyro_format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__gyro_elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__gyro_channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__gyro_multiply_factor(float_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t lsm6dsv16bx_gyro_set_st_ble_stream__gyro_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_gyro_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}




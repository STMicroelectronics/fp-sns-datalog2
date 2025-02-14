/**
  ******************************************************************************
  * @file    App_model_Ism330dhcx_Gyro.c
  * @author  SRA
  * @brief   Ism330dhcx_Gyro PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinkt1b:fpSnsDatalog2_datalog2:sensors:ism330dhcx_gyro;2
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

/* ISM330DHCX_GYRO PnPL Component --------------------------------------------*/
static SensorModel_t ism330dhcx_gyro_model;
extern AppModel_t app_model;

uint8_t ism330dhcx_gyro_comp_init(void)
{
  ism330dhcx_gyro_model.comp_name = ism330dhcx_gyro_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_GYRO);
  ism330dhcx_gyro_model.id = id;
  ism330dhcx_gyro_set_st_ble_stream__id(id, NULL);

  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatusPointer(id);
  ism330dhcx_gyro_model.stream_params.stream_id = -1;
  ism330dhcx_gyro_model.stream_params.usb_ep = -1;

  ism330dhcx_gyro_set_st_ble_stream__gyro_unit("dps", NULL);
  ism330dhcx_gyro_set_st_ble_stream__gyro_format("int16_t", NULL);

  addSensorToAppModel(id, &ism330dhcx_gyro_model);

  ism330dhcx_gyro_set_sensor_annotation("\0", NULL);
  ism330dhcx_gyro_set_odr(pnpl_ism330dhcx_gyro_odr_hz6667, NULL);
  ism330dhcx_gyro_set_fs(pnpl_ism330dhcx_gyro_fs_dps4000, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  ism330dhcx_gyro_set_samples_per_ts(0, NULL);
#else
  ism330dhcx_gyro_set_samples_per_ts(1000, NULL);
#endif

  int32_t value = 0;
  ism330dhcx_gyro_get_dim(&value);
  ism330dhcx_gyro_set_st_ble_stream__gyro_channels(value, NULL);
  float sensitivity = 0.0f;
  ism330dhcx_gyro_get_sensitivity(&sensitivity);
  ism330dhcx_gyro_set_st_ble_stream__gyro_multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(ism330dhcx_gyro_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *ism330dhcx_gyro_get_key(void)
{
  return "ism330dhcx_gyro";
}


uint8_t ism330dhcx_gyro_get_odr(pnpl_ism330dhcx_gyro_odr_t *enum_id)
{
  float odr = ism330dhcx_gyro_model.sensor_status->type.mems.odr;
  if (odr < 13.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz12_5;
  }
  else if (odr < 27.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz26;
  }
  else if (odr < 53.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz52;
  }
  else if (odr < 105.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz104;
  }
  else if (odr < 209.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz208;
  }
  else if (odr < 417.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz416;
  }
  else if (odr < 834.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz833;
  }
  else if (odr < 1667.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz1666;
  }
  else if (odr < 3333.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz3332;
  }
  else if (odr < 6668.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_odr_hz6667;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_fs(pnpl_ism330dhcx_gyro_fs_t *enum_id)
{
  float fs = ism330dhcx_gyro_model.sensor_status->type.mems.fs;
  if (fs < 126.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_fs_dps125;
  }
  else if (fs < 251.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_fs_dps250;
  }
  else if (fs < 501.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_fs_dps500;
  }
  else if (fs < 1001.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_fs_dps1000;
  }
  else if (fs < 2001.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_fs_dps2000;
  }
  else if (fs < 4001.0f)
  {
    *enum_id = pnpl_ism330dhcx_gyro_fs_dps4000;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_enable(bool *value)
{
  *value = ism330dhcx_gyro_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_samples_per_ts(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_ioffset(float *value)
{
  *value = ism330dhcx_gyro_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_measodr(float *value)
{
  *value = ism330dhcx_gyro_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_usb_dps(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_sd_dps(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_sensitivity(float *value)
{
  *value = ism330dhcx_gyro_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_sensor_annotation(char **value)
{
  *value = ism330dhcx_gyro_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_sensor_category(int32_t *value)
{
  *value = ism330dhcx_gyro_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__id(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__gyro_enable(bool *value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__gyro_unit(char **value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__gyro_format(char **value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__gyro_elements(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__gyro_channels(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__gyro_multiply_factor(float *value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_st_ble_stream__gyro_odr(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_stream_id(int8_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_gyro_get_ep_id(int8_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t ism330dhcx_gyro_set_odr(pnpl_ism330dhcx_gyro_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_ism330dhcx_gyro_odr_hz12_5:
      value = 12.5f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz26:
      value = 26.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz52:
      value = 52.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz104:
      value = 104.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz208:
      value = 208.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz416:
      value = 416.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz833:
      value = 833.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz1666:
      value = 1666.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz3332:
      value = 3332.0f;
      break;
    case pnpl_ism330dhcx_gyro_odr_hz6667:
      value = 6667.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(ism330dhcx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    ism330dhcx_gyro_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    __sc_set_ble_stream_params(ism330dhcx_gyro_model.id);
  }
  return ret;
}

uint8_t ism330dhcx_gyro_set_fs(pnpl_ism330dhcx_gyro_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_ism330dhcx_gyro_fs_dps125:
      value = 125.0f;
      break;
    case pnpl_ism330dhcx_gyro_fs_dps250:
      value = 250.0f;
      break;
    case pnpl_ism330dhcx_gyro_fs_dps500:
      value = 500.0f;
      break;
    case pnpl_ism330dhcx_gyro_fs_dps1000:
      value = 1000.0f;
      break;
    case pnpl_ism330dhcx_gyro_fs_dps2000:
      value = 2000.0f;
      break;
    case pnpl_ism330dhcx_gyro_fs_dps4000:
      value = 4000.0F;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetFS(ism330dhcx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
  }

  float sensitivity = 0.0f;
  ism330dhcx_gyro_get_sensitivity(&sensitivity);
  ism330dhcx_gyro_set_st_ble_stream__gyro_multiply_factor(sensitivity, NULL);

  return ret;
}

uint8_t ism330dhcx_gyro_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
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
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
    __stream_control(true);
    __sc_set_ble_stream_params(ism330dhcx_gyro_model.id);
  }
  return ret;
}

uint8_t ism330dhcx_gyro_set_samples_per_ts(int32_t value, char **response_message)
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
  return ret;
}

uint8_t ism330dhcx_gyro_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330dhcx_gyro_model.annotation, value);
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__gyro_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__gyro_unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__gyro_format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__gyro_elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__gyro_channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__gyro_multiply_factor(float value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t ism330dhcx_gyro_set_st_ble_stream__gyro_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_gyro_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}




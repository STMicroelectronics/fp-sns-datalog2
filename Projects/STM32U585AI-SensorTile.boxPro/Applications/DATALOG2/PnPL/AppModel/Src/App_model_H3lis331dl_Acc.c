/**
  ******************************************************************************
  * @file    App_model_H3lis331dl_Acc.c
  * @author  SRA
  * @brief   H3lis331dl_Acc PnPL Components APIs
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:h3lis331dl_acc;1
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

/* H3LIS331DL_ACC PnPL Component ---------------------------------------------*/
static SensorModel_t h3lis331dl_acc_model;
extern AppModel_t app_model;

uint8_t h3lis331dl_acc_comp_init(void)
{
  h3lis331dl_acc_model.comp_name = h3lis331dl_acc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "h3lis331dl", COM_TYPE_ACC);
  h3lis331dl_acc_model.id = id;
  h3lis331dl_acc_set_st_ble_stream__id(id, NULL);

  h3lis331dl_acc_model.sensor_status = SMSensorGetStatusPointer(id);
  h3lis331dl_acc_model.stream_params.stream_id = -1;
  h3lis331dl_acc_model.stream_params.usb_ep = -1;

  h3lis331dl_acc_set_st_ble_stream__acc_unit("g", NULL);
  h3lis331dl_acc_set_st_ble_stream__acc_format("int16_t", NULL);

  addSensorToAppModel(id, &h3lis331dl_acc_model);

  h3lis331dl_acc_set_sensor_annotation("[EXTERN]\0", NULL);
  h3lis331dl_acc_set_odr(pnpl_h3lis331dl_acc_odr_hz100, NULL);
  h3lis331dl_acc_set_fs(pnpl_h3lis331dl_acc_fs_g100, NULL);
  h3lis331dl_acc_set_enable(true, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  h3lis331dl_acc_set_samples_per_ts(0, NULL);
#else
  h3lis331dl_acc_set_samples_per_ts(100, NULL);
#endif

  int32_t value = 0;
  h3lis331dl_acc_get_dim(&value);
  h3lis331dl_acc_set_st_ble_stream__acc_channels(value, NULL);
  float sensitivity = 0.0f;
  h3lis331dl_acc_get_sensitivity(&sensitivity);
  h3lis331dl_acc_set_st_ble_stream__acc_multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(h3lis331dl_acc_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *h3lis331dl_acc_get_key(void)
{
  return "h3lis331dl_acc";
}


uint8_t h3lis331dl_acc_get_odr(pnpl_h3lis331dl_acc_odr_t *enum_id)
{
  float odr = h3lis331dl_acc_model.sensor_status->type.mems.odr;
  if (odr < 1.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz0_5;
  }
  else if (odr < 2.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz1;
  }
  else if (odr < 3.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz2;
  }
  else if (odr < 6.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz5;
  }
  else if (odr < 11.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz10;
  }
  else if (odr < 51.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz50;
  }
  else if (odr < 101.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz100;
  }
  else if (odr < 401.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz400;
  }
  else
  {
    *enum_id = pnpl_h3lis331dl_acc_odr_hz1000;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_fs(pnpl_h3lis331dl_acc_fs_t *enum_id)
{
  float fs = h3lis331dl_acc_model.sensor_status->type.mems.fs;
  if (fs < 101.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_fs_g100;
  }
  else if (fs < 201.0f)
  {
    *enum_id = pnpl_h3lis331dl_acc_fs_g200;
  }
  else
  {
    *enum_id = pnpl_h3lis331dl_acc_fs_g400;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_enable(bool *value)
{
  *value = h3lis331dl_acc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_samples_per_ts(int32_t *value)
{
  *value = h3lis331dl_acc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_ioffset(float *value)
{
  *value = h3lis331dl_acc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_measodr(float *value)
{
  *value = h3lis331dl_acc_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_usb_dps(int32_t *value)
{
  *value = h3lis331dl_acc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_sd_dps(int32_t *value)
{
  *value = h3lis331dl_acc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_sensitivity(float *value)
{
  *value = h3lis331dl_acc_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_sensor_annotation(char **value)
{
  *value = h3lis331dl_acc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_sensor_category(int32_t *value)
{
  *value = h3lis331dl_acc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_mounted(bool *value)
{
  *value = true;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__id(int32_t *value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__acc_enable(bool *value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__acc_unit(char **value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__acc_format(char **value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__acc_elements(int32_t *value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__acc_channels(int32_t *value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__acc_multiply_factor(float *value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_st_ble_stream__acc_odr(int32_t *value)
{
  *value = h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_stream_id(int8_t *value)
{
  *value = h3lis331dl_acc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t h3lis331dl_acc_get_ep_id(int8_t *value)
{
  *value = h3lis331dl_acc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t h3lis331dl_acc_set_odr(pnpl_h3lis331dl_acc_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_h3lis331dl_acc_odr_hz0_5:
      value = 0.5f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz1:
      value = 1.0f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz2:
      value = 2.0f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz5:
      value = 5.0f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz10:
      value = 10.0f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz50:
      value = 50.0f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz100:
      value = 100.0f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz400:
      value = 400.0f;
      break;
    case pnpl_h3lis331dl_acc_odr_hz1000:
      value = 1000.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(h3lis331dl_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    h3lis331dl_acc_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    __sc_set_ble_stream_params(h3lis331dl_acc_model.id);
  }
  return ret;
}

uint8_t h3lis331dl_acc_set_fs(pnpl_h3lis331dl_acc_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_h3lis331dl_acc_fs_g100:
      value = 100.0f;
      break;
    case pnpl_h3lis331dl_acc_fs_g200:
      value = 200.0f;
      break;
    case pnpl_h3lis331dl_acc_fs_g400:
      value = 400.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetFS(h3lis331dl_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }

  float sensitivity = 0.0f;
  h3lis331dl_acc_get_sensitivity(&sensitivity);
  h3lis331dl_acc_set_st_ble_stream__acc_multiply_factor(sensitivity, NULL);

  return ret;
}

uint8_t h3lis331dl_acc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(h3lis331dl_acc_model.id);
  }
  else
  {
    ret = SMSensorDisable(h3lis331dl_acc_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
    __sc_set_ble_stream_params(h3lis331dl_acc_model.id);
  }
  return ret;
}

uint8_t h3lis331dl_acc_set_samples_per_ts(int32_t value, char **response_message)
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
    h3lis331dl_acc_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    h3lis331dl_acc_model.stream_params.spts = max_v;
  }
  else
  {
    h3lis331dl_acc_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t h3lis331dl_acc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(h3lis331dl_acc_model.annotation, value);
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  h3lis331dl_acc_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__acc_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__acc_unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__acc_format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__acc_elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__acc_channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__acc_multiply_factor(float value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t h3lis331dl_acc_set_st_ble_stream__acc_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  h3lis331dl_acc_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}




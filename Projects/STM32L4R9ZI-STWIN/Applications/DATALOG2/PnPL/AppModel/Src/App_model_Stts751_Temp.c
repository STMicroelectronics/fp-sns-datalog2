/**
  ******************************************************************************
  * @file    App_model_Stts751_Temp.c
  * @author  SRA
  * @brief   Stts751_Temp PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinkt1b:fpSnsDatalog2_datalog2:sensors:stts751_temp;2
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

/* STTS751_TEMP PnPL Component -----------------------------------------------*/
static SensorModel_t stts751_temp_model;
extern AppModel_t app_model;

uint8_t stts751_temp_comp_init(void)
{
  stts751_temp_model.comp_name = stts751_temp_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "stts751", COM_TYPE_TEMP);
  stts751_temp_model.id = id;
  stts751_temp_set_st_ble_stream__id(id, NULL);

  stts751_temp_model.sensor_status = SMSensorGetStatusPointer(id);
  stts751_temp_model.stream_params.stream_id = -1;
  stts751_temp_model.stream_params.usb_ep = -1;

  stts751_temp_set_st_ble_stream__temp__unit("Celsius", NULL);
  stts751_temp_set_st_ble_stream__temp__format("float", NULL);

  addSensorToAppModel(id, &stts751_temp_model);

  stts751_temp_set_sensor_annotation("\0", NULL);
  stts751_temp_set_odr(pnpl_stts751_temp_odr_hz8, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  stts751_temp_set_samples_per_ts(0, NULL);
#else
  stts751_temp_set_samples_per_ts(8, NULL);
#endif

  int32_t value = 0;
  stts751_temp_get_dim(&value);
  stts751_temp_set_st_ble_stream__temp__channels(value, NULL);
  float sensitivity = 0.0f;
  stts751_temp_get_sensitivity(&sensitivity);
  stts751_temp_set_st_ble_stream__temp__multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(stts751_temp_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *stts751_temp_get_key(void)
{
  return "stts751_temp";
}


uint8_t stts751_temp_get_odr(pnpl_stts751_temp_odr_t *enum_id)
{
  float odr = stts751_temp_model.sensor_status->type.mems.odr;
  if (odr < 2.0f)
  {
    *enum_id = pnpl_stts751_temp_odr_hz1;
  }
  else if (odr < 3.0f)
  {
    *enum_id = pnpl_stts751_temp_odr_hz2;
  }
  else if (odr < 5.0f)
  {
    *enum_id = pnpl_stts751_temp_odr_hz4;
  }
  else if (odr < 9.0f)
  {
    *enum_id = pnpl_stts751_temp_odr_hz8;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_fs(pnpl_stts751_temp_fs_t *enum_id)
{
  float fs = stts751_temp_model.sensor_status->type.mems.fs;
  if (fs > 99.0f && fs < 101.0f)
  {
    *enum_id = pnpl_stts751_temp_fs_cdeg100;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_enable(bool *value)
{
  *value = stts751_temp_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_samples_per_ts(int32_t *value)
{
  *value = stts751_temp_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_ioffset(float *value)
{
  *value = stts751_temp_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_measodr(float *value)
{
  *value = stts751_temp_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_usb_dps(int32_t *value)
{
  *value = stts751_temp_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_sd_dps(int32_t *value)
{
  *value = stts751_temp_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_sensitivity(float *value)
{
  *value = stts751_temp_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_data_type(char **value)
{
  *value = "float";
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_sensor_annotation(char **value)
{
  *value = stts751_temp_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_sensor_category(int32_t *value)
{
  *value = stts751_temp_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__id(int32_t *value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__temp__enable(bool *value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__temp__unit(char **value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__temp__format(char **value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__temp__elements(int32_t *value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__temp__channels(int32_t *value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__temp__multiply_factor(float *value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_st_ble_stream__temp__odr(int32_t *value)
{
  /* USER Code */
  *value = stts751_temp_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_stream_id(int8_t *value)
{
  *value = stts751_temp_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts751_temp_get_ep_id(int8_t *value)
{
  *value = stts751_temp_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t stts751_temp_set_odr(pnpl_stts751_temp_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_stts751_temp_odr_hz1:
      value = 1.0f;
      break;
    case pnpl_stts751_temp_odr_hz2:
      value = 2.0f;
      break;
    case pnpl_stts751_temp_odr_hz4:
      value = 4.0f;
      break;
    case pnpl_stts751_temp_odr_hz8:
      value = 8.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(stts751_temp_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    stts751_temp_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    __sc_set_ble_stream_params(stts751_temp_model.id);
  }
  return ret;
}

uint8_t stts751_temp_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(stts751_temp_model.id);
  }
  else
  {
    ret = SMSensorDisable(stts751_temp_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
    __sc_set_ble_stream_params(stts751_temp_model.id);
  }
  return ret;
}

uint8_t stts751_temp_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 8;
  if (value >= min_v && value <= max_v)
  {
    stts751_temp_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    stts751_temp_model.stream_params.spts = max_v;
  }
  else
  {
    stts751_temp_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t stts751_temp_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(stts751_temp_model.annotation, value);
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  stts751_temp_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__temp__enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  stts751_temp_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__temp__unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(stts751_temp_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__temp__format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(stts751_temp_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__temp__elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  stts751_temp_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__temp__channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  stts751_temp_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__temp__multiply_factor(float value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  stts751_temp_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t stts751_temp_set_st_ble_stream__temp__odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  stts751_temp_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}


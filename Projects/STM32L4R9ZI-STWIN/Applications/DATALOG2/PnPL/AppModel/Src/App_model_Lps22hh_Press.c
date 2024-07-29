/**
  ******************************************************************************
  * @file    App_model_Lps22hh_Press.c
  * @author  SRA
  * @brief   Lps22hh_Press PnPL Components APIs
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
  * dtmi:appconfig:steval_stwinkt1b:fpSnsDatalog2_datalog2:sensors:lps22hh_press;1
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

/* LPS22HH_PRESS PnPL Component ----------------------------------------------*/
static SensorModel_t lps22hh_press_model;
extern AppModel_t app_model;

uint8_t lps22hh_press_comp_init(void)
{
  lps22hh_press_model.comp_name = lps22hh_press_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lps22hh", COM_TYPE_PRESS);
  lps22hh_press_model.id = id;
  lps22hh_press_set_st_ble_stream__id(id, NULL);

  lps22hh_press_model.sensor_status = SMSensorGetStatusPointer(id);
  lps22hh_press_model.stream_params.stream_id = -1;
  lps22hh_press_model.stream_params.usb_ep = -1;

  lps22hh_press_set_st_ble_stream__press__unit("hPa", NULL);
  lps22hh_press_set_st_ble_stream__press__format("float", NULL);

  addSensorToAppModel(id, &lps22hh_press_model);

  lps22hh_press_set_sensor_annotation("\0", NULL);
  lps22hh_press_set_odr(pnpl_lps22hh_press_odr_hz200, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  lps22hh_press_set_samples_per_ts(0, NULL);
#else
  lps22hh_press_set_samples_per_ts(200, NULL);
#endif

  int32_t value = 0;
  lps22hh_press_get_dim(&value);
  lps22hh_press_set_st_ble_stream__press__channels(value, NULL);
  float sensitivity = 0.0f;
  lps22hh_press_get_sensitivity(&sensitivity);
  lps22hh_press_set_st_ble_stream__press__multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(lps22hh_press_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *lps22hh_press_get_key(void)
{
  return "lps22hh_press";
}


uint8_t lps22hh_press_get_odr(pnpl_lps22hh_press_odr_t *enum_id)
{
  float odr = lps22hh_press_model.sensor_status->type.mems.odr;
  if (odr < 2.0f)
  {
    *enum_id = pnpl_lps22hh_press_odr_hz1;
  }
  else if (odr < 11.0f)
  {
    *enum_id = pnpl_lps22hh_press_odr_hz10;
  }
  else if (odr < 26.0f)
  {
    *enum_id = pnpl_lps22hh_press_odr_hz25;
  }
  else if (odr < 51.0f)
  {
    *enum_id = pnpl_lps22hh_press_odr_hz50;
  }
  else if (odr < 76.0f)
  {
    *enum_id = pnpl_lps22hh_press_odr_hz75;
  }
  else if (odr < 101.0f)
  {
    *enum_id = pnpl_lps22hh_press_odr_hz100;
  }
  else
  {
    *enum_id = pnpl_lps22hh_press_odr_hz200;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_fs(pnpl_lps22hh_press_fs_t *enum_id)
{
  float fs = lps22hh_press_model.sensor_status->type.mems.fs;
  if (fs > 1259.0f && fs < 1261.0f)
  {
    *enum_id = pnpl_lps22hh_press_fs_hpa1260;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_enable(bool *value)
{
  *value = lps22hh_press_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_samples_per_ts(int32_t *value)
{
  *value = lps22hh_press_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_ioffset(float *value)
{
  *value = lps22hh_press_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_measodr(float *value)
{
  *value = lps22hh_press_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_usb_dps(int32_t *value)
{
  *value = lps22hh_press_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_sd_dps(int32_t *value)
{
  *value = lps22hh_press_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_sensitivity(float *value)
{
  *value = lps22hh_press_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_data_type(char **value)
{
  *value = "float";
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_sensor_annotation(char **value)
{
  *value = lps22hh_press_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_sensor_category(int32_t *value)
{
  *value = lps22hh_press_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__id(int32_t *value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__press__enable(bool *value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__press__unit(char **value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__press__format(char **value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__press__elements(int32_t *value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__press__channels(int32_t *value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__press__multiply_factor(float *value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_st_ble_stream__press__odr(int32_t *value)
{
  /* USER Code */
  *value = lps22hh_press_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_stream_id(int8_t *value)
{
  *value = lps22hh_press_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lps22hh_press_get_ep_id(int8_t *value)
{
  *value = lps22hh_press_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t lps22hh_press_set_odr(pnpl_lps22hh_press_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_lps22hh_press_odr_hz1:
      value = 1.0f;
      break;
    case pnpl_lps22hh_press_odr_hz10:
      value = 10.0f;
      break;
    case pnpl_lps22hh_press_odr_hz25:
      value = 25.0f;
      break;
    case pnpl_lps22hh_press_odr_hz50:
      value = 50.0f;
      break;
    case pnpl_lps22hh_press_odr_hz75:
      value = 75.0f;
      break;
    case pnpl_lps22hh_press_odr_hz100:
      value = 100.0f;
      break;
    case pnpl_lps22hh_press_odr_hz200:
      value = 200.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(lps22hh_press_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    lps22hh_press_set_samples_per_ts((int32_t)value, NULL);
    lps22hh_temp_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    __sc_set_ble_stream_params(lps22hh_press_model.id);
  }
  return ret;
}

uint8_t lps22hh_press_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(lps22hh_press_model.id);
  }
  else
  {
    ret = SMSensorDisable(lps22hh_press_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
    __sc_set_ble_stream_params(lps22hh_press_model.id);
  }
  return ret;
}

uint8_t lps22hh_press_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 200;
  if (value >= min_v && value <= max_v)
  {
    lps22hh_press_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    lps22hh_press_model.stream_params.spts = max_v;
  }
  else
  {
    lps22hh_press_model.stream_params.spts = min_v;
  }
//  lps22hh_temp_get_samples_per_ts(&temp_spts);
//  if (temp_spts != lps22hh_press_model.stream_params.spts)
//  {
//    lps22hh_temp_set_samples_per_ts(temp_spts);
//  }
  return ret;
}

uint8_t lps22hh_press_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lps22hh_press_model.annotation, value);
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lps22hh_press_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__press__enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lps22hh_press_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__press__unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lps22hh_press_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__press__format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lps22hh_press_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__press__elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lps22hh_press_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__press__channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lps22hh_press_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__press__multiply_factor(float value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lps22hh_press_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t lps22hh_press_set_st_ble_stream__press__odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lps22hh_press_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}


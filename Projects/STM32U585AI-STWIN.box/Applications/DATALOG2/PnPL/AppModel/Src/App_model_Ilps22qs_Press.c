/**
  ******************************************************************************
  * @file    App_model_Ilps22qs_Press.c
  * @author  SRA
  * @brief   Ilps22qs_Press PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:ilps22qs_press;4
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

/* ILPS22QS_PRESS PnPL Component ---------------------------------------------*/
static SensorModel_t ilps22qs_press_model;
extern AppModel_t app_model;

uint8_t ilps22qs_press_comp_init(void)
{
  ilps22qs_press_model.comp_name = ilps22qs_press_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ilps22qs", COM_TYPE_PRESS);
  ilps22qs_press_model.id = id;
  ilps22qs_press_set_st_ble_stream__id(id, NULL);

  ilps22qs_press_model.sensor_status = SMSensorGetStatusPointer(id);
  ilps22qs_press_model.stream_params.stream_id = -1;
  ilps22qs_press_model.stream_params.usb_ep = -1;

  ilps22qs_press_set_st_ble_stream__press__unit("hPa", NULL);
  ilps22qs_press_set_st_ble_stream__press__format("float", NULL);

  addSensorToAppModel(id, &ilps22qs_press_model);

  ilps22qs_press_set_sensor_annotation("\0", NULL);
  ilps22qs_press_set_odr(pnpl_ilps22qs_press_odr_hz200, NULL);
  ilps22qs_press_set_fs(pnpl_ilps22qs_press_fs_hpa4060, NULL);
  ilps22qs_press_set_enable(true, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  ilps22qs_press_set_samples_per_ts(0, NULL);
#else
  ilps22qs_press_set_samples_per_ts(200, NULL);
#endif

  int32_t value = 0;
  ilps22qs_press_get_dim(&value);
  ilps22qs_press_set_st_ble_stream__press__channels(value, NULL);
  float sensitivity = 0.0f;
  ilps22qs_press_get_sensitivity(&sensitivity);
  ilps22qs_press_set_st_ble_stream__press__multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(ilps22qs_press_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *ilps22qs_press_get_key(void)
{
  return "ilps22qs_press";
}


uint8_t ilps22qs_press_get_odr(pnpl_ilps22qs_press_odr_t *enum_id)
{
  float odr = ilps22qs_press_model.sensor_status->type.mems.odr;
  if (odr < 2.0f)
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz1;
  }
  else if (odr < 5.0f)
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz4;
  }
  else if (odr < 11.0f)
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz10;
  }
  else if (odr < 26.0f)
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz25;
  }
  else if (odr < 51.0f)
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz50;
  }
  else if (odr < 76.0f)
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz75;
  }
  else if (odr < 101.0f)
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz100;
  }
  else
  {
    *enum_id = pnpl_ilps22qs_press_odr_hz200;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_fs(pnpl_ilps22qs_press_fs_t *enum_id)
{
  float fs = ilps22qs_press_model.sensor_status->type.mems.fs;
  if (fs <= 1261.0f)
  {
    *enum_id = pnpl_ilps22qs_press_fs_hpa1260;
  }
  else if (fs < 4061.0f)
  {
    *enum_id = pnpl_ilps22qs_press_fs_hpa4060;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_enable(bool *value)
{
  *value = ilps22qs_press_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_samples_per_ts(int32_t *value)
{
  *value = ilps22qs_press_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_ioffset(float *value)
{
  *value = ilps22qs_press_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_measodr(float *value)
{
  *value = ilps22qs_press_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_usb_dps(int32_t *value)
{
  *value = ilps22qs_press_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_sd_dps(int32_t *value)
{
  *value = ilps22qs_press_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_sensitivity(float *value)
{
  *value = ilps22qs_press_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_data_type(char **value)
{
  *value = "float";
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_sensor_annotation(char **value)
{
  *value = ilps22qs_press_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_sensor_category(int32_t *value)
{
  *value = ilps22qs_press_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__id(int32_t *value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__press__enable(bool *value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__press__unit(char **value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__press__format(char **value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__press__elements(int32_t *value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__press__channels(int32_t *value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__press__multiply_factor(float *value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_st_ble_stream__press__odr(int32_t *value)
{
  /* USER Code */
  *value = ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_stream_id(int8_t *value)
{
  *value = ilps22qs_press_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_get_ep_id(int8_t *value)
{
  *value = ilps22qs_press_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t ilps22qs_press_set_odr(pnpl_ilps22qs_press_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_ilps22qs_press_odr_hz1:
      value = 1.0f;
      break;
    case pnpl_ilps22qs_press_odr_hz4:
      value = 4.0f;
      break;
    case pnpl_ilps22qs_press_odr_hz10:
      value = 10.0f;
      break;
    case pnpl_ilps22qs_press_odr_hz25:
      value = 25.0f;
      break;
    case pnpl_ilps22qs_press_odr_hz50:
      value = 50.0f;
      break;
    case pnpl_ilps22qs_press_odr_hz75:
      value = 75.0f;
      break;
    case pnpl_ilps22qs_press_odr_hz100:
      value = 100.0f;
      break;
    case pnpl_ilps22qs_press_odr_hz200:
      value = 200.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set ODR";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetODR(ilps22qs_press_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    ilps22qs_press_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    __sc_set_ble_stream_params(ilps22qs_press_model.id);
  }
  return ret;
}

uint8_t ilps22qs_press_set_fs(pnpl_ilps22qs_press_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_ilps22qs_press_fs_hpa1260:
      value = 1260.0f;
      break;
    case pnpl_ilps22qs_press_fs_hpa4060:
      value = 4060.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set FS";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetFS(ilps22qs_press_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }

  float sensitivity = 0.0f;
  ilps22qs_press_get_sensitivity(&sensitivity);
  ilps22qs_press_set_st_ble_stream__press__multiply_factor(sensitivity, NULL);

  return ret;
}

uint8_t ilps22qs_press_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
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
    /* USER Code */
    __stream_control(true);
    __sc_set_ble_stream_params(ilps22qs_press_model.id);
  }
  else
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Failed to enable the sensor";
    }
  }
  return ret;
}

uint8_t ilps22qs_press_set_samples_per_ts(int32_t value, char **response_message)
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
    ilps22qs_press_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    ilps22qs_press_model.stream_params.spts = max_v;
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting above maximum threshold (1000)";
    }
  }
  else
  {
    ilps22qs_press_model.stream_params.spts = min_v;
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting below minimum threshold (0)";
    }
  }
  return ret;
}

uint8_t ilps22qs_press_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ilps22qs_press_model.annotation, value);
  return ret;
}

uint8_t ilps22qs_press_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  ilps22qs_press_model.st_ble_stream.st_ble_stream_id = value;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_set_st_ble_stream__press__enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  /* USER Code */
  ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.status = value;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_set_st_ble_stream__press__unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  /* USER Code */
  strcpy(ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_set_st_ble_stream__press__format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  /* USER Code */
  strcpy(ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.format, value);
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_set_st_ble_stream__press__elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ilps22qs_press_set_st_ble_stream__press__channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t ilps22qs_press_set_st_ble_stream__press__multiply_factor(float value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t ilps22qs_press_set_st_ble_stream__press__odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ilps22qs_press_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}


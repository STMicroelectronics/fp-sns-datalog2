/**
  ******************************************************************************
  * @file    App_model_Stts22h_Temp.c
  * @author  SRA
  * @brief   Stts22h_Temp PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect1:sensors:stts22h_temp;2
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

/* STTS22H_TEMP PnPL Component -----------------------------------------------*/
static SensorModel_t stts22h_temp_model;
extern AppModel_t app_model;

uint8_t stts22h_temp_comp_init(void)
{
  stts22h_temp_model.comp_name = stts22h_temp_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "stts22h", COM_TYPE_TEMP);
  stts22h_temp_model.id = id;
  stts22h_temp_model.sensor_status = SMSensorGetStatusPointer(id);
  stts22h_temp_model.stream_params.stream_id = -1;
  stts22h_temp_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &stts22h_temp_model);

  stts22h_temp_set_sensor_annotation("\0", NULL);
  stts22h_temp_set_odr(pnpl_stts22h_temp_odr_hz1, NULL);
  stts22h_temp_set_enable(false, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  stts22h_temp_set_samples_per_ts(0, NULL);
#else
  stts22h_temp_set_samples_per_ts(1, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *stts22h_temp_get_key(void)
{
  return "stts22h_temp";
}


uint8_t stts22h_temp_get_odr(pnpl_stts22h_temp_odr_t *enum_id)
{
  float odr = stts22h_temp_model.sensor_status->type.mems.odr;
  if (odr < 2.0f)
  {
    *enum_id = pnpl_stts22h_temp_odr_hz1;
  }
  else if (odr < 26.0f)
  {
    *enum_id = pnpl_stts22h_temp_odr_hz25;
  }
  else if (odr < 51.0f)
  {
    *enum_id = pnpl_stts22h_temp_odr_hz50;
  }
  else if (odr < 101.0f)
  {
    *enum_id = pnpl_stts22h_temp_odr_hz100;
  }
  else if (odr < 201.0f)
  {
    *enum_id = pnpl_stts22h_temp_odr_hz200;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_fs(pnpl_stts22h_temp_fs_t *enum_id)
{
  float fs = stts22h_temp_model.sensor_status->type.mems.fs;
  if (fs > 99.0f && fs < 101.0f)
  {
    *enum_id = pnpl_stts22h_temp_fs_cdeg100;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_enable(bool *value)
{
  *value = stts22h_temp_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_samples_per_ts(int32_t *value)
{
  *value = stts22h_temp_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_ioffset(float *value)
{
  *value = stts22h_temp_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_measodr(float *value)
{
  *value = stts22h_temp_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_usb_dps(int32_t *value)
{
  *value = stts22h_temp_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_sd_dps(int32_t *value)
{
  *value = stts22h_temp_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_sensitivity(float *value)
{
  *value = stts22h_temp_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_data_type(char **value)
{
  *value = "float";
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_sensor_annotation(char **value)
{
  *value = stts22h_temp_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_sensor_category(int32_t *value)
{
  *value = stts22h_temp_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_stream_id(int8_t *value)
{
  *value = stts22h_temp_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t stts22h_temp_get_ep_id(int8_t *value)
{
  *value = stts22h_temp_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t stts22h_temp_set_odr(pnpl_stts22h_temp_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_stts22h_temp_odr_hz1:
      value = 1.0f;
      break;
    case pnpl_stts22h_temp_odr_hz25:
      value = 25.0f;
      break;
    case pnpl_stts22h_temp_odr_hz50:
      value = 50.0f;
      break;
    case pnpl_stts22h_temp_odr_hz100:
      value = 100.0f;
      break;
    case pnpl_stts22h_temp_odr_hz200:
      value = 200.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(stts22h_temp_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    stts22h_temp_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
  }
  return ret;
}

uint8_t stts22h_temp_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
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
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t stts22h_temp_set_samples_per_ts(int32_t value, char **response_message)
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
  return ret;
}

uint8_t stts22h_temp_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(stts22h_temp_model.annotation, value);
  return ret;
}




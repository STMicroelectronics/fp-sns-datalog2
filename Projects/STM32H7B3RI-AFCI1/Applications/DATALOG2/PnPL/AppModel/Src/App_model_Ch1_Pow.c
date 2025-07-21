/**
  ******************************************************************************
  * @file    App_model_Ch1_Pow.c
  * @author  SRA
  * @brief   Ch1_Pow PnPL Components APIs
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
  * dtmi:vespucci:afci_h7:fpSnsDatalog2_datalog2:sensors:ch1_pow;1
  *
  * Created by: DTDL2PnPL_cGen version 2.3.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include "services/SQuery.h"

/* USER includes -------------------------------------------------------------*/

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* CH1_POW PnPL Component ----------------------------------------------------*/
static SensorModel_t ch1_pow_model;
extern AppModel_t app_model;

uint8_t ch1_pow_comp_init(void)
{
  ch1_pow_model.comp_name = ch1_pow_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ch1", COM_TYPE_POW);
  ch1_pow_model.id = id;
  ch1_pow_model.sensor_status = SMSensorGetStatusPointer(id);
  ch1_pow_model.stream_params.stream_id = -1;
  ch1_pow_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &ch1_pow_model);

  ch1_pow_set_sensor_annotation("\0", NULL);
  ch1_pow_set_odr(pnpl_ch1_pow_odr_hz250000, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  ch1_pow_set_samples_per_ts(0, NULL);
#else
  ch1_pow_set_samples_per_ts(1000, NULL);
#endif
  ch1_pow_set_sensitivity(1, NULL);
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *ch1_pow_get_key(void)
{
  return "ch1_pow";
}


uint8_t ch1_pow_get_odr(pnpl_ch1_pow_odr_t *enum_id)
{
  float_t odr = ch1_pow_model.sensor_status->type.mems.odr;
  if (odr < 100001.0f)
  {
    *enum_id = pnpl_ch1_pow_odr_hz100000;
  }
  else if (odr < 150001.0f)
  {
    *enum_id = pnpl_ch1_pow_odr_hz150000;
  }
  else if (odr < 200001.0f)
  {
    *enum_id = pnpl_ch1_pow_odr_hz200000;
  }
  else if (odr < 250001.0f)
  {
    *enum_id = pnpl_ch1_pow_odr_hz250000;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_fs(pnpl_ch1_pow_fs_t *enum_id)
{
  *enum_id = pnpl_ch1_pow_fs_ua1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_enable(bool *value)
{
  *value = ch1_pow_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_samples_per_ts(int32_t *value)
{
  *value = ch1_pow_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_ioffset(float *value)
{
  *value = ch1_pow_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_measodr(float *value)
{
  *value = ch1_pow_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_usb_dps(int32_t *value)
{
  *value = ch1_pow_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_sd_dps(int32_t *value)
{
  *value = ch1_pow_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_sensitivity(float *value)
{
  *value = ch1_pow_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_data_type(char **value)
{
  *value = "int16_t";
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_sensor_annotation(char **value)
{
  *value = ch1_pow_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_sensor_category(int32_t *value)
{
  *value = ch1_pow_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_stream_id(int8_t *value)
{
  *value = ch1_pow_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ch1_pow_get_ep_id(int8_t *value)
{
  *value = ch1_pow_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t ch1_pow_set_odr(pnpl_ch1_pow_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_ch1_pow_odr_hz100000:
      value = 100000.0f;
      break;
    case pnpl_ch1_pow_odr_hz150000:
      value = 150000.0f;
      break;
    case pnpl_ch1_pow_odr_hz200000:
      value = 200000.0f;
      break;
    case pnpl_ch1_pow_odr_hz250000:
      value = 250000.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(ch1_pow_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t ch1_pow_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(ch1_pow_model.id);
  }
  else
  {
    ret = SMSensorDisable(ch1_pow_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t ch1_pow_set_samples_per_ts(int32_t value, char **response_message)
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
    ch1_pow_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    ch1_pow_model.stream_params.spts = max_v;
  }
  else
  {
    ch1_pow_model.stream_params.spts = min_v;
  }

  return ret;
}

uint8_t ch1_pow_set_sensitivity(float value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ch1_pow_model.sensor_status->type.mems.sensitivity = value;
  return ret;
}

uint8_t ch1_pow_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ch1_pow_model.annotation, value);
  return ret;
}




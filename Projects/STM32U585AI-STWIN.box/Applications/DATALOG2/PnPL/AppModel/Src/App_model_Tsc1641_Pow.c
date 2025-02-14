/**
  ******************************************************************************
  * @file    App_model_Tsc1641_Pow.c
  * @author  SRA
  * @brief   Tsc1641_Pow PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:tsc1641_pow;2
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

/* TSC1641_POW PnPL Component ------------------------------------------------*/
static SensorModel_t tsc1641_pow_model;
extern AppModel_t app_model;

uint8_t tsc1641_pow_comp_init(void)
{
  tsc1641_pow_model.comp_name = tsc1641_pow_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "tsc1641", COM_TYPE_POW);
  tsc1641_pow_model.id = id;
  tsc1641_pow_model.sensor_status = SMSensorGetStatusPointer(id);
  tsc1641_pow_model.stream_params.stream_id = -1;
  tsc1641_pow_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &tsc1641_pow_model);

  tsc1641_pow_set_sensor_annotation("[EXTERN]\0", NULL);
  tsc1641_pow_set_adc_conversion_time(pnpl_tsc1641_pow_adc_conversion_time_n8192, NULL);
  tsc1641_pow_set_enable(false, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  tsc1641_pow_model.stream_params.spts = 0;
#else
  tsc1641_pow_model.stream_params.spts = 1;
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *tsc1641_pow_get_key(void)
{
  return "tsc1641_pow";
}


uint8_t tsc1641_pow_get_adc_conversion_time(pnpl_tsc1641_pow_adc_conversion_time_t *enum_id)
{
  uint32_t adc = tsc1641_pow_model.sensor_status->type.power_meter.adc_conversion_time;
  if (adc < 129)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n128;
  }
  else if (adc < 257)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n256;
  }
  else if (adc < 513)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n512;
  }
  else if (adc < 1025)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n1024;
  }
  else if (adc < 2049)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n2048;
  }
  else if (adc < 4097)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n4096;
  }
  else if (adc < 8193)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n8192;
  }
  else if (adc < 16385)
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n16384;
  }
  else
  {
    *enum_id = pnpl_tsc1641_pow_adc_conversion_time_n32768;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_r_shunt(int32_t *value)
{
  *value = tsc1641_pow_model.sensor_status->type.power_meter.r_shunt;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_enable(bool *value)
{
  *value = tsc1641_pow_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_samples_per_ts(int32_t *value)
{
  *value = tsc1641_pow_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_dim(int32_t *value)
{
  *value = 4;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_ioffset(float *value)
{
  *value = tsc1641_pow_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_data_type(char **value)
{
  *value = "float";
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_usb_dps(int32_t *value)
{
  *value = tsc1641_pow_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_sd_dps(int32_t *value)
{
  *value = tsc1641_pow_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_sensor_annotation(char **value)
{
  *value = tsc1641_pow_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_sensor_category(int32_t *value)
{
  *value = tsc1641_pow_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_mounted(bool *value)
{
  *value = true;
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_stream_id(int8_t *value)
{
  *value = tsc1641_pow_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t tsc1641_pow_get_ep_id(int8_t *value)
{
  *value = tsc1641_pow_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t tsc1641_pow_set_adc_conversion_time(pnpl_tsc1641_pow_adc_conversion_time_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint32_t value;
  switch (enum_id)
  {
    case pnpl_tsc1641_pow_adc_conversion_time_n128:
      value = 128;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n256:
      value = 256;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n512:
      value = 512;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n1024:
      value = 1024;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n2048:
      value = 2048;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n4096:
      value = 4096;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n8192:
      value = 8192;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n16384:
      value = 16384;
      break;
    case pnpl_tsc1641_pow_adc_conversion_time_n32768:
      value = 32768;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetADCConversionTime(tsc1641_pow_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    __stream_control(true);
  }
  return ret;
}

uint8_t tsc1641_pow_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(tsc1641_pow_model.id);
  }
  else
  {
    ret = SMSensorDisable(tsc1641_pow_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t tsc1641_pow_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(tsc1641_pow_model.annotation, value);
  return ret;
}




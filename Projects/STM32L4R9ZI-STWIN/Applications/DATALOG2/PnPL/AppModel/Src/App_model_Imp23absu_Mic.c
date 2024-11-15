/**
  ******************************************************************************
  * @file    App_model_Imp23absu_Mic.c
  * @author  SRA
  * @brief   Imp23absu_Mic PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinkt1b:fpSnsDatalog2_datalog2:sensors:imp23absu_mic;2
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

/* IMP23ABSU_MIC PnPL Component ----------------------------------------------*/
static SensorModel_t imp23absu_mic_model;
extern AppModel_t app_model;

uint8_t imp23absu_mic_comp_init(void)
{
  imp23absu_mic_model.comp_name = imp23absu_mic_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "imp23absu", COM_TYPE_MIC);
  imp23absu_mic_model.id = id;
  imp23absu_mic_model.sensor_status = SMSensorGetStatusPointer(id);
  imp23absu_mic_model.stream_params.stream_id = -1;
  imp23absu_mic_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &imp23absu_mic_model);

  imp23absu_mic_set_sensor_annotation("\0", NULL);
  imp23absu_mic_set_odr(pnpl_imp23absu_mic_odr_hz192000, NULL);
  imp23absu_mic_set_volume(100, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  imp23absu_mic_set_samples_per_ts(0, NULL);
#else
  imp23absu_mic_set_samples_per_ts(1000, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *imp23absu_mic_get_key(void)
{
  return "imp23absu_mic";
}


uint8_t imp23absu_mic_get_odr(pnpl_imp23absu_mic_odr_t *enum_id)
{
  uint32_t odr = imp23absu_mic_model.sensor_status->type.audio.frequency;
  if (odr <= 16000)
  {
    *enum_id = pnpl_imp23absu_mic_odr_hz16000;
  }
  else if (odr <= 32000)
  {
    *enum_id = pnpl_imp23absu_mic_odr_hz32000;
  }
  else if (odr <= 48000)
  {
    *enum_id = pnpl_imp23absu_mic_odr_hz48000;
  }
  else if (odr <= 96000)
  {
    *enum_id = pnpl_imp23absu_mic_odr_hz96000;
  }
  else
  {
    *enum_id = pnpl_imp23absu_mic_odr_hz192000;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_aop(pnpl_imp23absu_mic_aop_t *enum_id)
{
  *enum_id = pnpl_imp23absu_mic_aop_dbspl130;
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_enable(bool *value)
{
  *value = imp23absu_mic_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_volume(int32_t *value)
{
  *value = imp23absu_mic_model.sensor_status->type.audio.volume;
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_resolution(pnpl_imp23absu_mic_resolution_t *enum_id)
{
  uint8_t resolution = imp23absu_mic_model.sensor_status->type.audio.resolution;
  if (resolution == 16)
  {
    *enum_id = pnpl_imp23absu_mic_resolution_bit16;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_samples_per_ts(int32_t *value)
{
  *value = imp23absu_mic_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_ioffset(float *value)
{
  *value = imp23absu_mic_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_usb_dps(int32_t *value)
{
  *value = imp23absu_mic_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_sd_dps(int32_t *value)
{
  *value = imp23absu_mic_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_sensitivity(float *value)
{
  *value = 0.000030517578125; // 2/(2^imp23absu_mic_model.sensor_status.type.audio.resolution);
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_sensor_annotation(char **value)
{
  *value = imp23absu_mic_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_sensor_category(int32_t *value)
{
  *value = imp23absu_mic_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_stream_id(int8_t *value)
{
  *value = imp23absu_mic_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t imp23absu_mic_get_ep_id(int8_t *value)
{
  *value = imp23absu_mic_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t imp23absu_mic_set_odr(pnpl_imp23absu_mic_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint32_t value;
  switch (enum_id)
  {
    case pnpl_imp23absu_mic_odr_hz16000:
      value = 16000;
      break;
    case pnpl_imp23absu_mic_odr_hz32000:
      value = 32000;
      break;
    case pnpl_imp23absu_mic_odr_hz48000:
      value = 48000;
      break;
    case pnpl_imp23absu_mic_odr_hz96000:
      value = 96000;
      break;
    case pnpl_imp23absu_mic_odr_hz192000:
      value = 192000;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetFrequency(imp23absu_mic_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    imp23absu_mic_set_samples_per_ts(value, NULL);
#endif
    __stream_control(true);
  }
  return ret;
}

uint8_t imp23absu_mic_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(imp23absu_mic_model.id);
  }
  else
  {
    ret = SMSensorDisable(imp23absu_mic_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t imp23absu_mic_set_volume(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ret = SMSensorSetVolume(imp23absu_mic_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t imp23absu_mic_set_samples_per_ts(int32_t value, char **response_message)
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
    imp23absu_mic_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    imp23absu_mic_model.stream_params.spts = max_v;
  }
  else
  {
    imp23absu_mic_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t imp23absu_mic_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(imp23absu_mic_model.annotation, value);
  return ret;
}


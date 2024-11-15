/**
  ******************************************************************************
  * @file    App_model_Sht40_Hum.c
  * @author  SRA
  * @brief   Sht40_Hum PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:FP_SNS_DATALOG2_PDetect:sensors:sht40_hum;1
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

/* SHT40_HUM PnPL Component --------------------------------------------------*/
static SensorModel_t sht40_hum_model;
extern AppModel_t app_model;

uint8_t sht40_hum_comp_init(void)
{
  sht40_hum_model.comp_name = sht40_hum_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "sht40", COM_TYPE_HUM);
  sht40_hum_model.id = id;
  sht40_hum_model.sensor_status = SMSensorGetStatusPointer(id);
  sht40_hum_model.stream_params.stream_id = -1;
  sht40_hum_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &sht40_hum_model);

  sht40_hum_set_sensor_annotation("[EXTERN]\0", NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  sht40_hum_set_samples_per_ts(0, NULL);
#else
  sht40_hum_set_samples_per_ts(1, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *sht40_hum_get_key(void)
{
  return "sht40_hum";
}


uint8_t sht40_hum_get_odr(pnpl_sht40_hum_odr_t *enum_id)
{
  *enum_id = pnpl_sht40_hum_odr_hz1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_fs(pnpl_sht40_hum_fs_t *enum_id)
{
  *enum_id = pnpl_sht40_hum_fs_rh100;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_enable(bool *value)
{
  *value = sht40_hum_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_samples_per_ts(int32_t *value)
{
  *value = sht40_hum_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_ioffset(float *value)
{
  *value = sht40_hum_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_measodr(float *value)
{
  *value = sht40_hum_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_usb_dps(int32_t *value)
{
  *value = sht40_hum_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_sd_dps(int32_t *value)
{
  *value = sht40_hum_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_sensitivity(float *value)
{
  *value = sht40_hum_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_data_type(char **value)
{
  *value = "float";
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_sensor_annotation(char **value)
{
  *value = sht40_hum_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_sensor_category(int32_t *value)
{
  *value = sht40_hum_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_mounted(bool *value)
{
  *value = true;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_stream_id(int8_t *value)
{
  *value = sht40_hum_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sht40_hum_get_ep_id(int8_t *value)
{
  *value = sht40_hum_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t sht40_hum_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(sht40_hum_model.id);
  }
  else
  {
    ret = SMSensorDisable(sht40_hum_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t sht40_hum_set_samples_per_ts(int32_t value, char **response_message)
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
    sht40_hum_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    sht40_hum_model.stream_params.spts = max_v;
  }
  else
  {
    sht40_hum_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t sht40_hum_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(sht40_hum_model.annotation, value);
  return ret;
}




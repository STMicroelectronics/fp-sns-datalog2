/**
  ******************************************************************************
  * @file    App_model_Sgp40_Temp.c
  * @author  SRA
  * @brief   Sgp40_Temp PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:FP_SNS_DATALOG2_PDetect:sensors:sgp40_temp;1
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

/* SGP40 PnPL Component ------------------------------------------------------*/
static SensorModel_t sgp40_temp_model;
extern AppModel_t app_model;

uint8_t sgp40_temp_comp_init(void)
{
  sgp40_temp_model.comp_name = sgp40_temp_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "sgp40", COM_TYPE_TEMP);
  sgp40_temp_model.id = id;
  sgp40_temp_model.sensor_status = SMSensorGetStatusPointer(id);
  sgp40_temp_model.stream_params.stream_id = -1;
  sgp40_temp_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &sgp40_temp_model);

  sgp40_temp_set_sensor_annotation("[EXTERN]\0", NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  sgp40_temp_set_samples_per_ts(0, NULL);
#else
  sgp40_temp_set_samples_per_ts(1, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *sgp40_temp_get_key(void)
{
  return "sgp40_temp";
}


uint8_t sgp40_temp_get_odr(pnpl_sgp40_temp_odr_t *enum_id)
{
  *enum_id = pnpl_sgp40_temp_odr_hz1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_fs(pnpl_sgp40_temp_fs_t *enum_id)
{
  *enum_id = pnpl_sgp40_temp_fs_voc100;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_enable(bool *value)
{
  *value = sgp40_temp_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_samples_per_ts(int32_t *value)
{
  *value = sgp40_temp_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_ioffset(float *value)
{
  *value = sgp40_temp_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_measodr(float *value)
{
  *value = sgp40_temp_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_usb_dps(int32_t *value)
{
  *value = sgp40_temp_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_sd_dps(int32_t *value)
{
  *value = sgp40_temp_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_sensitivity(float *value)
{
  *value = sgp40_temp_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_data_type(char **value)
{
  *value = "uint16_t";
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_sensor_annotation(char **value)
{
  *value = sgp40_temp_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_sensor_category(int32_t *value)
{
  *value = sgp40_temp_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_stream_id(int8_t *value)
{
  *value = sgp40_temp_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_temp_get_ep_id(int8_t *value)
{
  *value = sgp40_temp_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t sgp40_temp_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(sgp40_temp_model.id);
  }
  else
  {
    ret = SMSensorDisable(sgp40_temp_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t sgp40_temp_set_samples_per_ts(int32_t value, char **response_message)
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
    sgp40_temp_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    sgp40_temp_model.stream_params.spts = max_v;
  }
  else
  {
    sgp40_temp_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t sgp40_temp_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(sgp40_temp_model.annotation, value);
  return ret;
}




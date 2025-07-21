/**
  ******************************************************************************
  * @file    App_model_Sgp40_Voc.c
  * @author  SRA
  * @brief   Sgp40_Voc PnPL Components APIs
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
  * dtmi:vespucci:steval_stwinbx1:FP_SNS_DATALOG2_PDetect:sensors:sgp40_voc;1
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

/* SGP40_VOC PnPL Component --------------------------------------------------*/
static SensorModel_t sgp40_voc_model;
extern AppModel_t app_model;

uint8_t sgp40_voc_comp_init(void)
{
  sgp40_voc_model.comp_name = sgp40_voc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "sgp40", COM_TYPE_TEMP);
  sgp40_voc_model.id = id;
  sgp40_voc_model.sensor_status = SMSensorGetStatusPointer(id);
  sgp40_voc_model.stream_params.stream_id = -1;
  sgp40_voc_model.stream_params.usb_ep = -1;
  sgp40_voc_model.stream_params.spts = 1;

  addSensorToAppModel(id, &sgp40_voc_model);

  sgp40_voc_set_sensor_annotation("[EXTERN]\0", NULL);
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *sgp40_voc_get_key(void)
{
  return "sgp40_voc";
}


uint8_t sgp40_voc_get_odr(pnpl_sgp40_voc_odr_t *enum_id)
{
  *enum_id = pnpl_sgp40_voc_odr_hz1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_fs(pnpl_sgp40_voc_fs_t *enum_id)
{
  *enum_id = pnpl_sgp40_voc_fs_voc100;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_enable(bool *value)
{
  *value = sgp40_voc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_samples_per_ts(int32_t *value)
{
  *value = sgp40_voc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_dim(int32_t *value)
{
  *value = 1;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_ioffset(float_t *value)
{
  *value = sgp40_voc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_measodr(float_t *value)
{
  *value = sgp40_voc_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_usb_dps(int32_t *value)
{
  *value = sgp40_voc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_sd_dps(int32_t *value)
{
  *value = sgp40_voc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_sensitivity(float_t *value)
{
  *value = sgp40_voc_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_data_type(char **value)
{
  *value = "uint16_t";
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_sensor_annotation(char **value)
{
  *value = sgp40_voc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_sensor_category(int32_t *value)
{
  *value = sgp40_voc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_mounted(bool *value)
{
  *value = true;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_stream_id(int8_t *value)
{
  *value = sgp40_voc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sgp40_voc_get_ep_id(int8_t *value)
{
  *value = sgp40_voc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t sgp40_voc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(sgp40_voc_model.id);
  }
  else
  {
    ret = SMSensorDisable(sgp40_voc_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t sgp40_voc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(sgp40_voc_model.annotation, value);
  return ret;
}




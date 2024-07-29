/**
  ******************************************************************************
  * @file    App_model_Ism330is_Ispu.c
  * @author  SRA
  * @brief   Ism330is_Ispu PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:ism330is_ispu;2
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include "services/SQuery.h"

/* USER includes -------------------------------------------------------------*/
#include "DatalogAppTask.h"

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* ISM330IS_ISPU PnPL Component ----------------------------------------------*/
static SensorModel_t ism330is_ispu_model;
extern AppModel_t app_model;

uint8_t ism330is_ispu_comp_init(void)
{
  ism330is_ispu_model.comp_name = ism330is_ispu_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330is", COM_TYPE_ISPU);
  ism330is_ispu_model.id = id;
  ism330is_ispu_model.sensor_status = SMSensorGetStatusPointer(id);
  ism330is_ispu_model.stream_params.stream_id = -1;
  ism330is_ispu_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &ism330is_ispu_model);

  ism330is_ispu_set_sensor_annotation("[EXTERN]\0", NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  ism330is_ispu_model.stream_params.spts = 0;
#else
  ism330is_ispu_model.stream_params.spts = 1;
#endif
  __stream_control(true);
  /* USER Component initialization code */
  app_model.ispu_ucf_valid = false;
  return PNPL_NO_ERROR_CODE;
}

char *ism330is_ispu_get_key(void)
{
  return "ism330is_ispu";
}


uint8_t ism330is_ispu_get_enable(bool *value)
{
  *value = ism330is_ispu_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_samples_per_ts(int32_t *value)
{
  *value = ism330is_ispu_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_dim(int32_t *value)
{
  *value = 32;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_ioffset(float *value)
{
  *value = ism330is_ispu_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_ucf_status(bool *value)
{
  *value = app_model.ispu_ucf_valid;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_usb_dps(float *value)
{
  *value = ism330is_ispu_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_sd_dps(float *value)
{
  *value = ism330is_ispu_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_data_type(char **value)
{
  *value = "int16";//TODO ?????????? isn't it int8_t????
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_sensor_annotation(char **value)
{
  *value = ism330is_ispu_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_sensor_category(int32_t *value)
{
  *value = ism330is_ispu_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_mounted(bool *value)
{
  *value = true;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_stream_id(int8_t *value)
{
  *value = ism330is_ispu_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_ispu_get_ep_id(int8_t *value)
{
  *value = ism330is_ispu_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t ism330is_ispu_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (app_model.ispu_ucf_valid == true)
  {
    if (value)
    {
      ret = SMSensorEnable(ism330is_ispu_model.id);
    }
    else
    {
      ret = SMSensorDisable(ism330is_ispu_model.id);
    }
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
      __stream_control(true);
    }
    else
    {
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to enable the sensor";
      }
    }
  }
  return ret;
}

uint8_t ism330is_ispu_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330is_ispu_model.annotation, value);
  return ret;
}


uint8_t ism330is_ispu_load_file(const char *ucf_data, int32_t ucf_size, const char *output_data, int32_t output_size)
{
  DatalogAppTask_load_ism330is_ucf_vtbl(ucf_data, ucf_size, output_data, output_size);
  app_model.ispu_ucf_valid = true;
  __stream_control(true);
  return PNPL_NO_ERROR_CODE;
}


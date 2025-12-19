/**
  ******************************************************************************
  * @file    App_model_Ism6hg256x_Mlc.c
  * @author  SRA
  * @brief   Ism6hg256x_Mlc PnPL Components APIs
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:ism6hg256x_mlc;1
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

/* LSM6DSV320X_MLC PnPL Component ---------------------------------------------*/
static SensorModel_t ism6hg256x_mlc_model;
extern AppModel_t app_model;

uint8_t ism6hg256x_mlc_comp_init(void)
{
  ism6hg256x_mlc_model.comp_name = ism6hg256x_mlc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism6hg256x", COM_TYPE_MLC);
  ism6hg256x_mlc_model.id = id;
  ism6hg256x_mlc_set_st_ble_stream__id(id, NULL);

  ism6hg256x_mlc_model.sensor_status = SMSensorGetStatusPointer(id);
  ism6hg256x_mlc_model.stream_params.stream_id = -1;
  ism6hg256x_mlc_model.stream_params.usb_ep = -1;

  ism6hg256x_mlc_set_st_ble_stream__mlc_unit("", NULL);
  ism6hg256x_mlc_set_st_ble_stream__mlc_format("int8_t", NULL);

  addSensorToAppModel(id, &ism6hg256x_mlc_model);

  ism6hg256x_mlc_set_sensor_annotation("[EXTERN]\0", NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  ism6hg256x_mlc_model.stream_params.spts = 0;
#else
  ism6hg256x_mlc_model.stream_params.spts = 1;
#endif

  int32_t value = 0;
  ism6hg256x_mlc_get_dim(&value);
  ism6hg256x_mlc_set_st_ble_stream__mlc_channels(value, NULL);
  float_t sensitivity = 1.0f;
  ism6hg256x_mlc_set_st_ble_stream__mlc_multiply_factor(sensitivity, NULL);

  app_model.ism6hg256x_mlc_ucf_valid = false;
  __stream_control(true);
  __sc_set_ble_stream_params(ism6hg256x_mlc_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE ;
}

char *ism6hg256x_mlc_get_key(void)
{
  return "ism6hg256x_mlc";
}

uint8_t ism6hg256x_mlc_get_enable(bool *value)
{
  *value = ism6hg256x_mlc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_samples_per_ts(int32_t *value)
{
  *value = ism6hg256x_mlc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_ucf_status(bool *value)
{
  *value = app_model.ism6hg256x_mlc_ucf_valid;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_dim(int32_t *value)
{
  *value = 5;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_ioffset(float_t *value)
{
  *value = ism6hg256x_mlc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_data_type(char **value)
{
  *value = "int8";
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_usb_dps(int32_t *value)
{
  *value = ism6hg256x_mlc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_sd_dps(int32_t *value)
{
  *value = ism6hg256x_mlc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_sensor_annotation(char **value)
{
  *value = ism6hg256x_mlc_model.annotation;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_sensor_category(int32_t *value)
{
  *value = ism6hg256x_mlc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_mounted(bool *value)
{
  *value = true;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__id(int32_t *value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__mlc_enable(bool *value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__mlc_unit(char **value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__mlc_format(char **value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__mlc_elements(int32_t *value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__mlc_channels(int32_t *value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__mlc_multiply_factor(float_t *value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_st_ble_stream__mlc_odr(int32_t *value)
{
  *value = ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_stream_id(int8_t *value)
{
  *value = ism6hg256x_mlc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_get_ep_id(int8_t *value)
{
  *value = ism6hg256x_mlc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE ;
}

uint8_t ism6hg256x_mlc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (app_model.ism6hg256x_mlc_ucf_valid == true)
  {
    if (value)
    {
      ret = SMSensorEnable(ism6hg256x_mlc_model.id);
    }
    else
    {
      ret = SMSensorDisable(ism6hg256x_mlc_model.id);
    }
    if (ret == SYS_NO_ERROR_CODE)
    {
    }
    __stream_control(true);
    __sc_set_ble_stream_params(ism6hg256x_mlc_model.id);
  }
  else
  {
    ret = PNPL_BASE_ERROR_CODE;
    if (response_message != NULL)
    {
      *response_message = "UCF not loaded";
    }
  }
  return ret;
}

uint8_t ism6hg256x_mlc_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  return ret;
}

uint8_t ism6hg256x_mlc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism6hg256x_mlc_model.annotation, value);
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__mlc_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__mlc_unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__mlc_format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__mlc_elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__mlc_channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__mlc_multiply_factor(float_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t ism6hg256x_mlc_set_st_ble_stream__mlc_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism6hg256x_mlc_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}

uint8_t ism6hg256x_mlc_load_file(const char *data, int32_t size)
{
  uint8_t ret = PNPL_NO_ERROR_CODE;
  DatalogAppTask_load_ism6hg256x_ucf_vtbl(data, size);
  app_model.ism6hg256x_mlc_ucf_valid = true;
  __stream_control(true);
  __sc_set_ble_stream_params(ism6hg256x_mlc_model.id);
  /* UCF modifies also acc and gyro parameters.
   * Get status from SM and update ble_stream_params also */
  SQuery_t query;
  uint16_t id;
  SQInit(&query, SMGetSensorManager());
  id = SQNextByNameAndType(&query, "ism6hg256x", COM_TYPE_HG_ACC);
  __sc_set_ble_stream_params(id);
  SQInit(&query, SMGetSensorManager());
  id = SQNextByNameAndType(&query, "ism6hg256x", COM_TYPE_ACC);
  __sc_set_ble_stream_params(id);
  SQInit(&query, SMGetSensorManager());
  id = SQNextByNameAndType(&query, "ism6hg256x", COM_TYPE_GYRO);
  __sc_set_ble_stream_params(id);
  return ret;
}


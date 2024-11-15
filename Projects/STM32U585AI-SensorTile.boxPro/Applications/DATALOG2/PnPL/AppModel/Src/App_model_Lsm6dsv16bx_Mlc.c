/**
  ******************************************************************************
  * @file    App_model_Lsm6dsv16bx_Mlc.c
  * @author  SRA
  * @brief   Lsm6dsv16bx_Mlc PnPL Components APIs
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:lsm6dsv16bx_mlc;1
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

/* LSM6DSV16BX_MLC PnPL Component --------------------------------------------*/
static SensorModel_t lsm6dsv16bx_mlc_model;
extern AppModel_t app_model;

uint8_t lsm6dsv16bx_mlc_comp_init(void)
{
  lsm6dsv16bx_mlc_model.comp_name = lsm6dsv16bx_mlc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lsm6dsv16bx", COM_TYPE_MLC);
  lsm6dsv16bx_mlc_model.id = id;
  lsm6dsv16bx_mlc_set_st_ble_stream__id(id, NULL);

  lsm6dsv16bx_mlc_model.sensor_status = SMSensorGetStatusPointer(id);
  lsm6dsv16bx_mlc_model.stream_params.stream_id = -1;
  lsm6dsv16bx_mlc_model.stream_params.usb_ep = -1;

  lsm6dsv16bx_mlc_set_st_ble_stream__mlc__unit("", NULL);
  lsm6dsv16bx_mlc_set_st_ble_stream__mlc__format("int8_t", NULL);

  addSensorToAppModel(id, &lsm6dsv16bx_mlc_model);

  lsm6dsv16bx_mlc_set_sensor_annotation("[EXTERN]\0", NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  lsm6dsv16bx_mlc_model.stream_params.spts = 0;
#else
  lsm6dsv16bx_mlc_model.stream_params.spts = 1;
#endif

  int32_t value = 0;
  lsm6dsv16bx_mlc_get_dim(&value);
  lsm6dsv16bx_mlc_set_st_ble_stream__mlc__channels(value, NULL);
  float sensitivity = 1.0f;
  lsm6dsv16bx_mlc_set_st_ble_stream__mlc__multiply_factor(sensitivity, NULL);

  app_model.mlc_ucf_valid = false;
  __stream_control(true);
  __sc_set_ble_stream_params(lsm6dsv16bx_mlc_model.id);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *lsm6dsv16bx_mlc_get_key(void)
{
  return "lsm6dsv16bx_mlc";
}


uint8_t lsm6dsv16bx_mlc_get_enable(bool *value)
{
  *value = lsm6dsv16bx_mlc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_samples_per_ts(int32_t *value)
{
  *value = lsm6dsv16bx_mlc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_ucf_status(bool *value)
{
  *value = app_model.mlc_ucf_valid;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_dim(int32_t *value)
{
  *value = 5;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_ioffset(float *value)
{
  *value = lsm6dsv16bx_mlc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_data_type(char **value)
{
  *value = "int8";
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_usb_dps(int32_t *value)
{
  *value = lsm6dsv16bx_mlc_model.stream_params.usb_dps;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_sd_dps(int32_t *value)
{
  *value = lsm6dsv16bx_mlc_model.stream_params.sd_dps;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_sensor_annotation(char **value)
{
  *value = lsm6dsv16bx_mlc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_sensor_category(int32_t *value)
{
  *value = lsm6dsv16bx_mlc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_mounted(bool *value)
{
  *value = true;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__id(int32_t *value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__mlc__enable(bool *value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__mlc__unit(char **value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__mlc__format(char **value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__mlc__elements(int32_t *value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__mlc__channels(int32_t *value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__mlc__multiply_factor(float *value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_st_ble_stream__mlc__odr(int32_t *value)
{
  /* USER Code */
  *value = lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_stream_id(int8_t *value)
{
  *value = lsm6dsv16bx_mlc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dsv16bx_mlc_get_ep_id(int8_t *value)
{
  *value = lsm6dsv16bx_mlc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t lsm6dsv16bx_mlc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (app_model.mlc_ucf_valid == true)
  {
    if (value)
    {
      ret = SMSensorEnable(lsm6dsv16bx_mlc_model.id);
    }
    else
    {
      ret = SMSensorDisable(lsm6dsv16bx_mlc_model.id);
    }
    if (ret == SYS_NO_ERROR_CODE)
    {
      __stream_control(true);
      __sc_set_ble_stream_params(lsm6dsv16bx_mlc_model.id);
    }
  }
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lsm6dsv16bx_mlc_model.annotation, value);
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__mlc__enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__mlc__unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__mlc__format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__mlc__elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__mlc__channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__mlc__multiply_factor(float value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t lsm6dsv16bx_mlc_set_st_ble_stream__mlc__odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  lsm6dsv16bx_mlc_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}


uint8_t lsm6dsv16bx_mlc_load_file(const char *data, int32_t size)
{
  DatalogAppTask_load_lsm6dsv16bx_ucf_vtbl(data, size);
  app_model.mlc_ucf_valid = true;
  __stream_control(true);
  __sc_set_ble_stream_params(lsm6dsv16bx_mlc_model.id);
  /* UCF modifies also acc and gyro parameters.
   * Get status from SM and update ble_stream_params also */
  SQuery_t query;
  uint16_t id;
  SQInit(&query, SMGetSensorManager());
  id = SQNextByNameAndType(&query, "lsm6dsv16bx", COM_TYPE_ACC);
  __sc_set_ble_stream_params(id);
  SQInit(&query, SMGetSensorManager());
  id = SQNextByNameAndType(&query, "lsm6dsv16bx", COM_TYPE_GYRO);
  __sc_set_ble_stream_params(id);
  return PNPL_NO_ERROR_CODE;
}



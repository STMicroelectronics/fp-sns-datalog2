/**
  ******************************************************************************
  * @file    App_model_Ism330dhcx_Mlc.c
  * @author  SRA
  * @brief   Ism330dhcx_Mlc PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinkt1b:fpSnsDatalog2_datalog2:sensors:ism330dhcx_mlc;1
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

/* ISM330DHCX_MLC PnPL Component ---------------------------------------------*/
static SensorModel_t ism330dhcx_mlc_model;
extern AppModel_t app_model;

uint8_t ism330dhcx_mlc_comp_init(void)
{
  ism330dhcx_mlc_model.comp_name = ism330dhcx_mlc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_MLC);
  ism330dhcx_mlc_model.id = id;
  ism330dhcx_mlc_set_st_ble_stream__id(id, NULL);

  ism330dhcx_mlc_model.sensor_status = SMSensorGetStatusPointer(id);
  ism330dhcx_mlc_model.stream_params.stream_id = -1;
  ism330dhcx_mlc_model.stream_params.usb_ep = -1;

  ism330dhcx_mlc_set_st_ble_stream__mlc_unit("", NULL);
  ism330dhcx_mlc_set_st_ble_stream__mlc_format("int8_t", NULL);

  addSensorToAppModel(id, &ism330dhcx_mlc_model);

  ism330dhcx_mlc_set_sensor_annotation("\0", NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  ism330dhcx_mlc_model.stream_params.spts = 0;
#else
  ism330dhcx_mlc_model.stream_params.spts = 1;
#endif

  int32_t value = 0;
  ism330dhcx_mlc_get_dim(&value);
  ism330dhcx_mlc_set_st_ble_stream__mlc_channels(value, NULL);
  int32_t sensitivity = 1;
  ism330dhcx_mlc_set_st_ble_stream__mlc_multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(ism330dhcx_mlc_model.id);
  app_model.mlc_ucf_valid = false;
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *ism330dhcx_mlc_get_key(void)
{
  return "ism330dhcx_mlc";
}


uint8_t ism330dhcx_mlc_get_enable(bool *value)
{
  *value = ism330dhcx_mlc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_samples_per_ts(int32_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_ucf_status(bool *value)
{
  *value = app_model.mlc_ucf_valid;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_dim(int32_t *value)
{
  *value = 9;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_ioffset(float *value)
{
  *value = ism330dhcx_mlc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_data_type(char **value)
{
  *value = "int8";
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_usb_dps(int32_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_sd_dps(int32_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_sensor_annotation(char **value)
{
  *value = ism330dhcx_mlc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_sensor_category(int32_t *value)
{
  *value = ism330dhcx_mlc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__id(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__mlc_enable(bool *value)
{
  /* USER Code */
  *value = ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__mlc_unit(char **value)
{
  /* USER Code */
  *value = ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__mlc_format(char **value)
{
  /* USER Code */
  *value = ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__mlc_elements(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__mlc_channels(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__mlc_multiply_factor(int32_t *value)
{
  /* USER Code */
  *value = (int32_t)ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_st_ble_stream__mlc_odr(int32_t *value)
{
  /* USER Code */
  *value = ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_stream_id(int8_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330dhcx_mlc_get_ep_id(int8_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t ism330dhcx_mlc_set_enable(bool value, char **response_message)
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
      ret = SMSensorEnable(ism330dhcx_mlc_model.id);
    }
    else
    {
      ret = SMSensorDisable(ism330dhcx_mlc_model.id);
    }
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
      __stream_control(true);
      __sc_set_ble_stream_params(ism330dhcx_mlc_model.id);
    }
  }
  return ret;
}

uint8_t ism330dhcx_mlc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330dhcx_mlc_model.annotation, value);
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__mlc_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__mlc_unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__mlc_format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__mlc_elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__mlc_channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__mlc_multiply_factor(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t ism330dhcx_mlc_set_st_ble_stream__mlc_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330dhcx_mlc_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}


uint8_t ism330dhcx_mlc_load_file(const char *data, int32_t size)
{
  DatalogAppTask_load_ism330dhcx_ucf_vtbl(data, size);
  app_model.mlc_ucf_valid = true;
  __stream_control(true);
  __sc_set_ble_stream_params(ism330dhcx_mlc_model.id);
  /* UCF modifies also acc and gyro parameters.
   * Get status from SM and update ble_stream_params also */
  SQuery_t query;
  uint16_t id;
  SQInit(&query, SMGetSensorManager());
  id = SQNextByNameAndType(&query, "ism330dhcx", COM_TYPE_ACC);
  __sc_set_ble_stream_params(id);
  SQInit(&query, SMGetSensorManager());
  id = SQNextByNameAndType(&query, "ism330dhcx", COM_TYPE_GYRO);
  __sc_set_ble_stream_params(id);
  return PNPL_NO_ERROR_CODE;
}



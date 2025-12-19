/**
  ******************************************************************************
  * @file    App_model_Iis3dwb10is_Ext_Acc.c
  * @author  SRA
  * @brief   Iis3dwb10is_Ext_Acc PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_datalog2:sensors:iis3dwb10is_ext_acc;4
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

/* IIS3DWB10IS_ACC_EXT PnPL Component --------------------------------------------*/
static SensorModel_t iis3dwb10is_ext_acc_model;
extern AppModel_t app_model;

uint8_t iis3dwb10is_ext_acc_comp_init(void)
{
  iis3dwb10is_ext_acc_model.comp_name = iis3dwb10is_ext_acc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "iis3dwb10is", COM_TYPE_ACC);
  iis3dwb10is_ext_acc_model.id = id;

  iis3dwb10is_ext_acc_model.sensor_status = SMSensorGetStatusPointer(id);
  iis3dwb10is_ext_acc_model.stream_params.stream_id = -1;
  iis3dwb10is_ext_acc_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &iis3dwb10is_ext_acc_model);

  iis3dwb10is_ext_acc_set_sensor_annotation("[EXTERN]\0", NULL);
  iis3dwb10is_ext_acc_set_odr(pnpl_iis3dwb10is_ext_acc_odr_hz80000, NULL);
  iis3dwb10is_ext_acc_set_fs(pnpl_iis3dwb10is_ext_acc_fs_g50, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  iis3dwb10is_ext_acc_set_samples_per_ts(0, NULL);
#else
  iis3dwb10is_ext_acc_set_samples_per_ts(1000, NULL);
#endif

  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *iis3dwb10is_ext_acc_get_key(void)
{
  return "iis3dwb10is_ext_acc";
}


uint8_t iis3dwb10is_ext_acc_get_odr(pnpl_iis3dwb10is_ext_acc_odr_t *enum_id)
{
  float_t odr = iis3dwb10is_ext_acc_model.sensor_status->type.mems.odr;
  if (odr < 2501.0f)
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_odr_hz2500;
  }
  else if (odr < 5001.0f)
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_odr_hz5000;
  }
  else if (odr < 10001.0f)
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_odr_hz10000;
  }
  else if (odr < 20001.0f)
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_odr_hz20000;
  }
  else if (odr < 40001.0f)
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_odr_hz40000;
  }
  else
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_odr_hz80000;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_fs(pnpl_iis3dwb10is_ext_acc_fs_t *enum_id)
{
  float_t fs = iis3dwb10is_ext_acc_model.sensor_status->type.mems.fs;
  if (fs < 51.0f)
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_fs_g50;
  }
  else if (fs < 101.0f)
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_fs_g100;
  }
  else
  {
    *enum_id = pnpl_iis3dwb10is_ext_acc_fs_g200;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_enable(bool *value)
{
  *value = iis3dwb10is_ext_acc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_samples_per_ts(int32_t *value)
{
  *value = iis3dwb10is_ext_acc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_ioffset(float_t *value)
{
  *value = iis3dwb10is_ext_acc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_measodr(float_t *value)
{
  *value = iis3dwb10is_ext_acc_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_usb_dps(int32_t *value)
{
  *value = iis3dwb10is_ext_acc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_sd_dps(int32_t *value)
{
  *value = iis3dwb10is_ext_acc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_sensitivity(float_t *value)
{
  *value = iis3dwb10is_ext_acc_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_data_type(char **value)
{
  *value = "int24_t";
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_sensor_annotation(char **value)
{
  *value = iis3dwb10is_ext_acc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_sensor_category(int32_t *value)
{
  *value = iis3dwb10is_ext_acc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_mounted(bool *value)
{
  *value = true;
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_stream_id(int8_t *value)
{
  *value = iis3dwb10is_ext_acc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t iis3dwb10is_ext_acc_get_ep_id(int8_t *value)
{
  *value = iis3dwb10is_ext_acc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t iis3dwb10is_ext_acc_set_odr(pnpl_iis3dwb10is_ext_acc_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_iis3dwb10is_ext_acc_odr_hz2500:
      value = 2500.0f;
      break;
    case pnpl_iis3dwb10is_ext_acc_odr_hz5000:
      value = 5000.0f;
      break;
    case pnpl_iis3dwb10is_ext_acc_odr_hz10000:
      value = 10000.0f;
      break;
    case pnpl_iis3dwb10is_ext_acc_odr_hz20000:
      value = 20000.0f;
      break;
    case pnpl_iis3dwb10is_ext_acc_odr_hz40000:
      value = 40000.0f;
      break;
    case pnpl_iis3dwb10is_ext_acc_odr_hz80000:
      value = 80000.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set ODR";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetODR(iis3dwb10is_ext_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    iis3dwb10is_ext_acc_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
  }
  return ret;
}


uint8_t iis3dwb10is_ext_acc_set_fs(pnpl_iis3dwb10is_ext_acc_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_iis3dwb10is_ext_acc_fs_g50:
      value = 50.0f;
      break;
    case pnpl_iis3dwb10is_ext_acc_fs_g100:
      value = 100.0f;
      break;
    case pnpl_iis3dwb10is_ext_acc_fs_g200:
      value = 200.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set FS";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetFS(iis3dwb10is_ext_acc_model.id, value);

  return ret;
}

uint8_t iis3dwb10is_ext_acc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(iis3dwb10is_ext_acc_model.id);
  }
  else
  {
    ret = SMSensorDisable(iis3dwb10is_ext_acc_model.id);
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
  return ret;
}

uint8_t iis3dwb10is_ext_acc_set_samples_per_ts(int32_t value, char **response_message)
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
    iis3dwb10is_ext_acc_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    iis3dwb10is_ext_acc_model.stream_params.spts = max_v;
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting above maximum threshold (1000)";
    }
  }
  else
  {
    iis3dwb10is_ext_acc_model.stream_params.spts = min_v;
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting below minimum threshold (0)";
    }
  }
  return ret;
}

uint8_t iis3dwb10is_ext_acc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(iis3dwb10is_ext_acc_model.annotation, value);
  return ret;
}



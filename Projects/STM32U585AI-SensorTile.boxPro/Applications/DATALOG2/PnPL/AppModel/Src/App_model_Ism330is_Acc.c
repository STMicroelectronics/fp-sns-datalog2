/**
  ******************************************************************************
  * @file    App_model_Ism330is_Acc.c
  * @author  SRA
  * @brief   Ism330is_Acc PnPL Components APIs
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
  * dtmi:vespucci:steval_mkboxpro:fpSnsDatalog2_datalog2:sensors:ism330is_acc;3
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

/* ISM330IS_ACC PnPL Component -----------------------------------------------*/
static SensorModel_t ism330is_acc_model;
extern AppModel_t app_model;

uint8_t ism330is_acc_comp_init(void)
{
  ism330is_acc_model.comp_name = ism330is_acc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330is", COM_TYPE_ACC);
  ism330is_acc_model.id = id;
  ism330is_acc_set_st_ble_stream__id(id, NULL);

  ism330is_acc_model.sensor_status = SMSensorGetStatusPointer(id);
  ism330is_acc_model.stream_params.stream_id = -1;
  ism330is_acc_model.stream_params.usb_ep = -1;

  ism330is_acc_set_st_ble_stream__acc_unit("g", NULL);
  ism330is_acc_set_st_ble_stream__acc_format("int16_t", NULL);

  addSensorToAppModel(id, &ism330is_acc_model);

  ism330is_acc_set_sensor_annotation("[EXTERN]\0", NULL);
  ism330is_acc_set_odr(pnpl_ism330is_acc_odr_hz104, NULL);
  ism330is_acc_set_fs(pnpl_ism330is_acc_fs_g16, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  ism330is_acc_set_samples_per_ts(0, NULL);
#else
  ism330is_acc_set_samples_per_ts(100, NULL);
#endif

  int32_t value = 0;
  ism330is_acc_get_dim(&value);
  ism330is_acc_set_st_ble_stream__acc_channels(value, NULL);
  float_t sensitivity = 0.0f;
  ism330is_acc_get_sensitivity(&sensitivity);
  ism330is_acc_set_st_ble_stream__acc_multiply_factor(sensitivity, NULL);

  __stream_control(true);
  __sc_set_ble_stream_params(ism330is_acc_model.id);

  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *ism330is_acc_get_key(void)
{
  return "ism330is_acc";
}


uint8_t ism330is_acc_get_odr(pnpl_ism330is_acc_odr_t *enum_id)
{
  float_t odr = ism330is_acc_model.sensor_status->type.mems.odr;
  if (odr < 13.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz12_5;
  }
  else if (odr < 27.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz26;
  }
  else if (odr < 53.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz52;
  }
  else if (odr < 105.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz104;
  }
  else if (odr < 209.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz208;
  }
  else if (odr < 417.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz416;
  }
  else if (odr < 834.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz833;
  }
  else if (odr < 1668.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz1667;
  }
  else if (odr < 3334.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz3333;
  }
  else if (odr < 6668.0f)
  {
    *enum_id = pnpl_ism330is_acc_odr_hz6667;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_fs(pnpl_ism330is_acc_fs_t *enum_id)
{
  float_t fs = ism330is_acc_model.sensor_status->type.mems.fs;
  if (fs < 3.0f)
  {
    *enum_id = pnpl_ism330is_acc_fs_g2;
  }
  else if (fs < 5.0f)
  {
    *enum_id = pnpl_ism330is_acc_fs_g4;
  }
  else if (fs < 9.0f)
  {
    *enum_id = pnpl_ism330is_acc_fs_g8;
  }
  else if (fs < 17.0f)
  {
    *enum_id = pnpl_ism330is_acc_fs_g16;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_enable(bool *value)
{
  *value = ism330is_acc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_samples_per_ts(int32_t *value)
{
  *value = ism330is_acc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_ioffset(float_t *value)
{
  *value = ism330is_acc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_measodr(float_t *value)
{
  *value = ism330is_acc_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_usb_dps(int32_t *value)
{
  *value = ism330is_acc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_sd_dps(int32_t *value)
{
  *value = ism330is_acc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_sensitivity(float_t *value)
{
  *value = ism330is_acc_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_sensor_annotation(char **value)
{
  *value = ism330is_acc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_sensor_category(int32_t *value)
{
  *value = ism330is_acc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_mounted(bool *value)
{
  *value = true;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__id(int32_t *value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__acc_enable(bool *value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_objects.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__acc_unit(char **value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_objects.unit;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__acc_format(char **value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_objects.format;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__acc_elements(int32_t *value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_objects.elements;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__acc_channels(int32_t *value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_objects.channel;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__acc_multiply_factor(float_t *value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_objects.multiply_factor;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_st_ble_stream__acc_odr(int32_t *value)
{
  *value = ism330is_acc_model.st_ble_stream.st_ble_stream_objects.odr;
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_stream_id(int8_t *value)
{
  *value = ism330is_acc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t ism330is_acc_get_ep_id(int8_t *value)
{
  *value = ism330is_acc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t ism330is_acc_set_odr(pnpl_ism330is_acc_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_ism330is_acc_odr_hz12_5:
      value = 12.5f;
      break;
    case pnpl_ism330is_acc_odr_hz26:
      value = 26.0f;
      break;
    case pnpl_ism330is_acc_odr_hz52:
      value = 52.0f;
      break;
    case pnpl_ism330is_acc_odr_hz104:
      value = 104.0f;
      break;
    case pnpl_ism330is_acc_odr_hz208:
      value = 208.0f;
      break;
    case pnpl_ism330is_acc_odr_hz416:
      value = 416.0f;
      break;
    case pnpl_ism330is_acc_odr_hz833:
      value = 833.0f;
      break;
    case pnpl_ism330is_acc_odr_hz1667:
      value = 1667.0f;
      break;
    case pnpl_ism330is_acc_odr_hz3333:
      value = 3333.0f;
      break;
    case pnpl_ism330is_acc_odr_hz6667:
      value = 6667.0f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(ism330is_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    app_model.ispu_ucf_valid = false;
#if (HSD_USE_DUMMY_DATA != 1)
    ism330is_acc_set_samples_per_ts((int32_t)value, NULL);
#endif
    if (__stream_control(true) != PNPL_NO_ERROR_CODE)
    {
      if (response_message != NULL)
      {
        *response_message = "PnPL_Warning: Safe bandwidth limit exceeded. Consider disabling sensors or lowering ODRs to avoid possible data corruption";
      }
      ret = PNPL_BASE_ERROR_CODE;
    }
    __sc_set_ble_stream_params(ism330is_acc_model.id);
  }
  return ret;
}

uint8_t ism330is_acc_set_fs(pnpl_ism330is_acc_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_ism330is_acc_fs_g2:
      value = 2.0;
      break;
    case pnpl_ism330is_acc_fs_g4:
      value = 4.0;
      break;
    case pnpl_ism330is_acc_fs_g8:
      value = 8.0;
      break;
    case pnpl_ism330is_acc_fs_g16:
      value = 16.0;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetFS(ism330is_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    app_model.ispu_ucf_valid = false;
  }

  float_t sensitivity = 0.0f;
  ism330is_acc_get_sensitivity(&sensitivity);
  ism330is_acc_set_st_ble_stream__acc_multiply_factor(sensitivity, NULL);

  return ret;
}

uint8_t ism330is_acc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(ism330is_acc_model.id);
  }
  else
  {
    ret = SMSensorDisable(ism330is_acc_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    app_model.ispu_ucf_valid = false;
    if (__stream_control(true) != PNPL_NO_ERROR_CODE)
    {
      if (response_message != NULL)
      {
        *response_message = "PnPL_Warning: Safe bandwidth limit exceeded. Consider disabling sensors or lowering ODRs to avoid possible data corruption";
      }
      ret = PNPL_BASE_ERROR_CODE;
    }
    __sc_set_ble_stream_params(ism330is_acc_model.id);
  }
  return ret;
}

uint8_t ism330is_acc_set_samples_per_ts(int32_t value, char **response_message)
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
    ism330is_acc_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    ism330is_acc_model.stream_params.spts = max_v;
  }
  else
  {
    ism330is_acc_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t ism330is_acc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330is_acc_model.annotation, value);
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__id(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330is_acc_model.st_ble_stream.st_ble_stream_id = value;
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__acc_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330is_acc_model.st_ble_stream.st_ble_stream_objects.status = value;
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__acc_unit(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330is_acc_model.st_ble_stream.st_ble_stream_objects.unit, value);
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__acc_format(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(ism330is_acc_model.st_ble_stream.st_ble_stream_objects.format, value);
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__acc_elements(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330is_acc_model.st_ble_stream.st_ble_stream_objects.elements = value;
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__acc_channels(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330is_acc_model.st_ble_stream.st_ble_stream_objects.channel = value;
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__acc_multiply_factor(float_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330is_acc_model.st_ble_stream.st_ble_stream_objects.multiply_factor = value;
  return ret;
}

uint8_t ism330is_acc_set_st_ble_stream__acc_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ism330is_acc_model.st_ble_stream.st_ble_stream_objects.odr = value;
  return ret;
}




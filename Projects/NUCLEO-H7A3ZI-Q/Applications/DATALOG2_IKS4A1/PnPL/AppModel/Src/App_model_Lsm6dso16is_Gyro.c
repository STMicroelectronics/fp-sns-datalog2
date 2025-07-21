/**
  ******************************************************************************
  * @file    App_model_Lsm6dso16is_Gyro.c
  * @author  SRA
  * @brief   Lsm6dso16is_Gyro PnPL Components APIs
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lsm6dso16is_gyro;1
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

/* LSM6DSO16IS_GYRO PnPL Component -------------------------------------------*/
static SensorModel_t lsm6dso16is_gyro_model;
extern AppModel_t app_model;

uint8_t lsm6dso16is_gyro_comp_init(void)
{
  lsm6dso16is_gyro_model.comp_name = lsm6dso16is_gyro_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lsm6dso16is", COM_TYPE_GYRO);
  lsm6dso16is_gyro_model.id = id;
  lsm6dso16is_gyro_model.sensor_status = SMSensorGetStatusPointer(id);
  lsm6dso16is_gyro_model.stream_params.stream_id = -1;
  lsm6dso16is_gyro_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &lsm6dso16is_gyro_model);

  lsm6dso16is_gyro_set_sensor_annotation("\0", NULL);
  lsm6dso16is_gyro_set_odr(pnpl_lsm6dso16is_gyro_odr_hz104, NULL);
  lsm6dso16is_gyro_set_fs(pnpl_lsm6dso16is_gyro_fs_dps2000, NULL);
  lsm6dso16is_gyro_set_enable(true, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  lsm6dso16is_gyro_set_samples_per_ts(0, NULL);
#else
  lsm6dso16is_gyro_set_samples_per_ts(100, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *lsm6dso16is_gyro_get_key(void)
{
  return "lsm6dso16is_gyro";
}


uint8_t lsm6dso16is_gyro_get_odr(pnpl_lsm6dso16is_gyro_odr_t *enum_id)
{
  float_t odr = lsm6dso16is_gyro_model.sensor_status->type.mems.odr;
  if (odr < 13.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz12_5;
  }
  else if (odr < 27.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz26;
  }
  else if (odr < 53.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz52;
  }
  else if (odr < 105.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz104;
  }
  else if (odr < 209.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz208;
  }
  else if (odr < 417.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz416;
  }
  else if (odr < 834.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz833;
  }
  else if (odr < 1668.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz1667;
  }
  else if (odr < 3334.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz3333;
  }
  else
  {
    *enum_id = pnpl_lsm6dso16is_gyro_odr_hz6667;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_fs(pnpl_lsm6dso16is_gyro_fs_t *enum_id)
{
  float_t fs = lsm6dso16is_gyro_model.sensor_status->type.mems.fs;
  if (fs < 126.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_fs_dps125;
  }
  else if (fs < 251.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_fs_dps250;
  }
  else if (fs < 501.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_fs_dps500;
  }
  else if (fs < 1001.0f)
  {
    *enum_id = pnpl_lsm6dso16is_gyro_fs_dps1000;
  }
  else
  {
    *enum_id = pnpl_lsm6dso16is_gyro_fs_dps2000;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_enable(bool *value)
{
  *value = lsm6dso16is_gyro_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_samples_per_ts(int32_t *value)
{
  *value = lsm6dso16is_gyro_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_ioffset(float_t *value)
{
  *value = lsm6dso16is_gyro_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_measodr(float_t *value)
{
  *value = lsm6dso16is_gyro_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_usb_dps(int32_t *value)
{
  *value = lsm6dso16is_gyro_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_sd_dps(int32_t *value)
{
  *value = lsm6dso16is_gyro_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_sensitivity(float_t *value)
{
  *value = lsm6dso16is_gyro_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_sensor_category(int32_t *value)
{
  *value = lsm6dso16is_gyro_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_sensor_annotation(char **value)
{
  *value = lsm6dso16is_gyro_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_stream_id(int8_t *value)
{
  *value = lsm6dso16is_gyro_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lsm6dso16is_gyro_get_ep_id(int8_t *value)
{
  *value = lsm6dso16is_gyro_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t lsm6dso16is_gyro_set_odr(pnpl_lsm6dso16is_gyro_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_lsm6dso16is_gyro_odr_hz12_5:
      value = 12.5f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz26:
      value = 26.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz52:
      value = 52.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz104:
      value = 104.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz208:
      value = 208.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz416:
      value = 416.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz833:
      value = 833.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz1667:
      value = 1667.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz3333:
      value = 3333.0f;
      break;
    case pnpl_lsm6dso16is_gyro_odr_hz6667:
      value = 6667.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set ODR";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetODR(lsm6dso16is_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.ispu_ucf_valid == true)
    {
      app_model.ispu_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    lsm6dso16is_gyro_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t lsm6dso16is_gyro_set_fs(pnpl_lsm6dso16is_gyro_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_lsm6dso16is_gyro_fs_dps125:
      value = 125.0f;
      break;
    case pnpl_lsm6dso16is_gyro_fs_dps250:
      value = 250.0f;
      break;
    case pnpl_lsm6dso16is_gyro_fs_dps500:
      value = 500.0f;
      break;
    case pnpl_lsm6dso16is_gyro_fs_dps1000:
      value = 1000.0f;
      break;
    case pnpl_lsm6dso16is_gyro_fs_dps2000:
      value = 2000.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set FS";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetFS(lsm6dso16is_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    app_model.ispu_ucf_valid = false;
  }
  return ret;
}

uint8_t lsm6dso16is_gyro_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(lsm6dso16is_gyro_model.id);
  }
  else
  {
    ret = SMSensorDisable(lsm6dso16is_gyro_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    app_model.ispu_ucf_valid = false;
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t lsm6dso16is_gyro_set_samples_per_ts(int32_t value, char **response_message)
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
    lsm6dso16is_gyro_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    lsm6dso16is_gyro_model.stream_params.spts = max_v;
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting above maximum threshold (1000)";
    }
  }
  else
  {
    lsm6dso16is_gyro_model.stream_params.spts = min_v;
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting below minimum threshold (0)";
    }
  }
  return ret;
}

uint8_t lsm6dso16is_gyro_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lsm6dso16is_gyro_model.annotation, value);
  return ret;
}


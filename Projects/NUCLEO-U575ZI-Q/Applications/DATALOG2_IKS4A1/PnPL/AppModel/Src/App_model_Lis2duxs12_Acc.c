/**
  ******************************************************************************
  * @file    App_model_Lis2duxs12_Acc.c
  * @author  SRA
  * @brief   Lis2duxs12_Acc PnPL Components APIs
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lis2duxs12_acc;1
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

/* LIS2DUXS12_ACC PnPL Component ---------------------------------------------*/
static SensorModel_t lis2duxs12_acc_model;
extern AppModel_t app_model;

uint8_t lis2duxs12_acc_comp_init(void)
{
  lis2duxs12_acc_model.comp_name = lis2duxs12_acc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lis2duxs12", COM_TYPE_ACC);
  lis2duxs12_acc_model.id = id;
  lis2duxs12_acc_model.sensor_status = SMSensorGetStatusPointer(id);
  lis2duxs12_acc_model.stream_params.stream_id = -1;
  lis2duxs12_acc_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &lis2duxs12_acc_model);

  lis2duxs12_acc_set_sensor_annotation("\0", NULL);
  lis2duxs12_acc_set_odr(pnpl_lis2duxs12_acc_odr_hz100, NULL);
  lis2duxs12_acc_set_fs(pnpl_lis2duxs12_acc_fs_g16, NULL);
  lis2duxs12_acc_set_enable(true, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  lis2duxs12_acc_set_samples_per_ts(0, NULL);
#else
  lis2duxs12_acc_set_samples_per_ts(100, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *lis2duxs12_acc_get_key(void)
{
  return "lis2duxs12_acc";
}


uint8_t lis2duxs12_acc_get_odr(pnpl_lis2duxs12_acc_odr_t *enum_id)
{
  float_t odr = lis2duxs12_acc_model.sensor_status->type.mems.odr;
  if (odr < 7.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz6;
  }
  else if (odr < 13.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz12_5;
  }
  else if (odr < 26.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz25;
  }
  else if (odr < 51.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz50;
  }
  else if (odr < 101.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz100;
  }
  else if (odr < 201.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz200;
  }
  else if (odr < 401.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz400;
  }
  else
  {
    *enum_id = pnpl_lis2duxs12_acc_odr_hz800;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_fs(pnpl_lis2duxs12_acc_fs_t *enum_id)
{
  float_t fs = lis2duxs12_acc_model.sensor_status->type.mems.fs;
  if (fs < 3.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_fs_g2;
  }
  else if (fs < 5.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_fs_g4;
  }
  else if (fs < 9.0f)
  {
    *enum_id = pnpl_lis2duxs12_acc_fs_g8;
  }
  else
  {
    *enum_id = pnpl_lis2duxs12_acc_fs_g16;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_enable(bool *value)
{
  *value = lis2duxs12_acc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_samples_per_ts(int32_t *value)
{
  *value = lis2duxs12_acc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_ioffset(float_t *value)
{
  *value = lis2duxs12_acc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_measodr(float_t *value)
{
  *value = lis2duxs12_acc_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_usb_dps(int32_t *value)
{
  *value = lis2duxs12_acc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_sd_dps(int32_t *value)
{
  *value = lis2duxs12_acc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_sensitivity(float_t *value)
{
  *value = lis2duxs12_acc_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_sensor_annotation(char **value)
{
  *value = lis2duxs12_acc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_sensor_category(int32_t *value)
{
  *value = lis2duxs12_acc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_stream_id(int8_t *value)
{
  *value = lis2duxs12_acc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_acc_get_ep_id(int8_t *value)
{
  *value = lis2duxs12_acc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t lis2duxs12_acc_set_odr(pnpl_lis2duxs12_acc_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_lis2duxs12_acc_odr_hz6:
      value = 6.0f;
      break;
    case pnpl_lis2duxs12_acc_odr_hz12_5:
      value = 12.5f;
      break;
    case pnpl_lis2duxs12_acc_odr_hz25:
      value = 25.0f;
      break;
    case pnpl_lis2duxs12_acc_odr_hz50:
      value = 50.0f;
      break;
    case pnpl_lis2duxs12_acc_odr_hz100:
      value = 100.0f;
      break;
    case pnpl_lis2duxs12_acc_odr_hz200:
      value = 200.0f;
      break;
    case pnpl_lis2duxs12_acc_odr_hz400:
      value = 400.0f;
      break;
    case pnpl_lis2duxs12_acc_odr_hz800:
      value = 800.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set ODR";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetODR(lis2duxs12_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.lis2duxs12_mlc_ucf_valid == true)
    {
      app_model.lis2duxs12_mlc_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    lis2duxs12_acc_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t lis2duxs12_acc_set_fs(pnpl_lis2duxs12_acc_fs_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_lis2duxs12_acc_fs_g2:
      value = 2.0f;
      break;
    case pnpl_lis2duxs12_acc_fs_g4:
      value = 4.0f;
      break;
    case pnpl_lis2duxs12_acc_fs_g8:
      value = 8.0f;
      break;
    case pnpl_lis2duxs12_acc_fs_g16:
      value = 16.0f;
      break;
    default:
      if (response_message != NULL)
      {
        *response_message = "Error: Failed to set FS";
      }
      return PNPL_BASE_ERROR_CODE;
  }
  ret = SMSensorSetFS(lis2duxs12_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.lis2duxs12_mlc_ucf_valid == true)
    {
      app_model.lis2duxs12_mlc_ucf_valid = false;
    }
  }
  return ret;
}

uint8_t lis2duxs12_acc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(lis2duxs12_acc_model.id);
  }
  else
  {
    ret = SMSensorDisable(lis2duxs12_acc_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    if (app_model.lis2duxs12_mlc_ucf_valid == true)
    {
      app_model.lis2duxs12_mlc_ucf_valid = false;
    }
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t lis2duxs12_acc_set_samples_per_ts(int32_t value, char **response_message)
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
    lis2duxs12_acc_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    lis2duxs12_acc_model.stream_params.spts = max_v;
  }
  else
  {
    lis2duxs12_acc_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t lis2duxs12_acc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lis2duxs12_acc_model.annotation, value);
  return ret;
}




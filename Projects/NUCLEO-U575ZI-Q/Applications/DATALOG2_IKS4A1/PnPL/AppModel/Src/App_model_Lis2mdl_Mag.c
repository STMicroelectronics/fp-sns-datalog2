/**
  ******************************************************************************
  * @file    App_model_Lis2mdl_Mag.c
  * @author  SRA
  * @brief   Lis2mdl_Mag PnPL Components APIs
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lis2mdl_mag;1
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

/* LIS2MDL_MAG PnPL Component ------------------------------------------------*/
static SensorModel_t lis2mdl_mag_model;
extern AppModel_t app_model;

uint8_t lis2mdl_mag_comp_init(void)
{
  lis2mdl_mag_model.comp_name = lis2mdl_mag_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lis2mdl", COM_TYPE_MAG);
  lis2mdl_mag_model.id = id;
  lis2mdl_mag_model.sensor_status = SMSensorGetStatusPointer(id);
  lis2mdl_mag_model.stream_params.stream_id = -1;
  lis2mdl_mag_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &lis2mdl_mag_model);

  lis2mdl_mag_set_sensor_annotation("\0", NULL);
  lis2mdl_mag_set_odr(pnpl_lis2mdl_mag_odr_hz100, NULL);
  lis2mdl_mag_set_enable(true, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  lis2mdl_mag_set_samples_per_ts(0, NULL);
#else
  lis2mdl_mag_set_samples_per_ts(100, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *lis2mdl_mag_get_key(void)
{
  return "lis2mdl_mag";
}


uint8_t lis2mdl_mag_get_odr(pnpl_lis2mdl_mag_odr_t *enum_id)
{
  float_t odr = lis2mdl_mag_model.sensor_status->type.mems.odr;
  if (odr < 11.0f)
  {
    *enum_id = pnpl_lis2mdl_mag_odr_hz10;
  }
  else if (odr < 21.0f)
  {
    *enum_id = pnpl_lis2mdl_mag_odr_hz20;
  }
  else if (odr < 51.0f)
  {
    *enum_id = pnpl_lis2mdl_mag_odr_hz50;
  }
  else
  {
    *enum_id = pnpl_lis2mdl_mag_odr_hz100;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_fs(pnpl_lis2mdl_mag_fs_t *enum_id)
{
  float_t fs = lis2mdl_mag_model.sensor_status->type.mems.fs;
  if (fs > 49.0f && fs < 51.0f)
  {
    *enum_id = pnpl_lis2mdl_mag_fs_g50;
  }
  else
  {
    return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_enable(bool *value)
{
  *value = lis2mdl_mag_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_samples_per_ts(int32_t *value)
{
  *value = lis2mdl_mag_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_dim(int32_t *value)
{
  *value = 3;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_ioffset(float_t *value)
{
  *value = lis2mdl_mag_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_measodr(float_t *value)
{
  *value = lis2mdl_mag_model.sensor_status->type.mems.measured_odr;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_usb_dps(int32_t *value)
{
  *value = lis2mdl_mag_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_sd_dps(int32_t *value)
{
  *value = lis2mdl_mag_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_sensitivity(float_t *value)
{
  *value = lis2mdl_mag_model.sensor_status->type.mems.sensitivity;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_data_type(char **value)
{
  *value = "int16";
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_sensor_annotation(char **value)
{
  *value = lis2mdl_mag_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_sensor_category(int32_t *value)
{
  *value = lis2mdl_mag_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_stream_id(int8_t *value)
{
  *value = lis2mdl_mag_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2mdl_mag_get_ep_id(int8_t *value)
{
  *value = lis2mdl_mag_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t lis2mdl_mag_set_odr(pnpl_lis2mdl_mag_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float_t value;
  switch (enum_id)
  {
    case pnpl_lis2mdl_mag_odr_hz10:
      value = 10;
      break;
    case pnpl_lis2mdl_mag_odr_hz20:
      value = 20;
      break;
    case pnpl_lis2mdl_mag_odr_hz50:
      value = 50;
      break;
    case pnpl_lis2mdl_mag_odr_hz100:
      value = 100;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetODR(lis2mdl_mag_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    lis2mdl_mag_set_samples_per_ts((int32_t)value, NULL);
#endif
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t lis2mdl_mag_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(lis2mdl_mag_model.id);
  }
  else
  {
    ret = SMSensorDisable(lis2mdl_mag_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
    /* USER Code */
  }
  return ret;
}

uint8_t lis2mdl_mag_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 100;
  if (value >= min_v && value <= max_v)
  {
    lis2mdl_mag_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    lis2mdl_mag_model.stream_params.spts = max_v;
  }
  else
  {
    lis2mdl_mag_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t lis2mdl_mag_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lis2mdl_mag_model.annotation, value);
  return ret;
}




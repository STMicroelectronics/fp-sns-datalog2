/**
  ******************************************************************************
  * @file    App_Model.c
  * @author  SRA
  * @brief   App Application Model and PnPL Components APIs
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
  * This file has been auto generated from the following Device Template Model:
  * dtmi:appconfig:b_u585i_iot02a:fpSnsDatalog2_datalog2;1
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include <string.h>
#include <stdio.h>
/* USER includes -------------------------------------------------------------*/
#include "services/SQuery.h"
#include "services/sysdebug.h"
#include "rtc.h"

/* USER defines --------------------------------------------------------------*/
#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DT, level, message)


/* USER private function prototypes ------------------------------------------*/
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status);


AppModel_t app_model;

AppModel_t *getAppModel(void)
{
  return &app_model;
}

/* Device Components APIs ----------------------------------------------------*/

/* IIS2MDC_MAG PnPL Component ------------------------------------------------*/
static SensorModel_t iis2mdc_mag_model;

uint8_t iis2mdc_mag_comp_init(void)
{
  iis2mdc_mag_model.comp_name = iis2mdc_mag_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "iis2mdc", COM_TYPE_MAG);
  iis2mdc_mag_model.id = id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  iis2mdc_mag_model.stream_params.stream_id = -1;
  iis2mdc_mag_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  iis2mdc_mag_set_sensor_annotation(default_notes);
  /* USER Code */
#if (HSD_USE_DUMMY_DATA == 1)
  iis2mdc_mag_set_samples_per_ts(0);
#else
  iis2mdc_mag_set_samples_per_ts(100);
#endif
  app_model.s_models[id] = &iis2mdc_mag_model;
  __stream_control(NULL, true);

  /* USER Component initialization code */
  return 0;
}

char *iis2mdc_mag_get_key(void)
{
  return "iis2mdc_mag";
}

uint8_t iis2mdc_mag_get_odr(float *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_fs(float *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_enable(bool *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_samples_per_ts(int32_t *value)
{
  *value = iis2mdc_mag_model.stream_params.spts;
  return 0;
}
uint8_t iis2mdc_mag_get_dim(int32_t *value)
{
  *value = 3;
  return 0;
}
uint8_t iis2mdc_mag_get_ioffset(float *value)
{
  *value = iis2mdc_mag_model.stream_params.ioffset;
  return 0;
}
uint8_t iis2mdc_mag_get_measodr(float *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_usb_dps(int32_t *value)
{
  *value = iis2mdc_mag_model.stream_params.usb_dps;
  return 0;
}
uint8_t iis2mdc_mag_get_sd_dps(int32_t *value)
{
  *value = iis2mdc_mag_model.stream_params.sd_dps;
  return 0;
}
uint8_t iis2mdc_mag_get_sensitivity(float *value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t iis2mdc_mag_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t iis2mdc_mag_get_sensor_annotation(char **value)
{
  uint16_t id = iis2mdc_mag_model.id;
  iis2mdc_mag_model.sensor_status = SMSensorGetStatus(id);
  *value = iis2mdc_mag_model.annotation;
  return 0;
}
uint8_t iis2mdc_mag_get_sensor_category(int32_t *value)
{
    *value = iis2mdc_mag_model.sensor_status.isensor_class;
  return 0;
}
uint8_t iis2mdc_mag_get_stream_id(int8_t *value)
{
  *value = iis2mdc_mag_model.stream_params.stream_id;
  return 0;
}
uint8_t iis2mdc_mag_get_ep_id(int8_t *value)
{
  *value = iis2mdc_mag_model.stream_params.usb_ep;
  return 0;
}
uint8_t iis2mdc_mag_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(iis2mdc_mag_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    iis2mdc_mag_model.sensor_status.type.mems.odr = value;
    /* USER Code */
#if (HSD_USE_DUMMY_DATA != 1)
    iis2mdc_mag_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t iis2mdc_mag_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(iis2mdc_mag_model.id);
  }
  else
  {
    ret = SMSensorDisable(iis2mdc_mag_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    iis2mdc_mag_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t iis2mdc_mag_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 100;
  if (value >= min_v && value <= max_v)
  {
    iis2mdc_mag_model.stream_params.spts = value;
  }
  return 0;
}
uint8_t iis2mdc_mag_set_sensor_annotation(const char *value)
{
  strcpy(iis2mdc_mag_model.annotation, value);
  return 0;
}


/* ISM330DHCX_GYRO PnPL Component --------------------------------------------*/
static SensorModel_t ism330dhcx_gyro_model;

uint8_t ism330dhcx_gyro_comp_init(void)
{
  ism330dhcx_gyro_model.comp_name = ism330dhcx_gyro_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_GYRO);
  ism330dhcx_gyro_model.id = id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  ism330dhcx_gyro_model.stream_params.stream_id = -1;
  ism330dhcx_gyro_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  ism330dhcx_gyro_set_sensor_annotation(default_notes);
  /* USER Code */
#if (HSD_USE_DUMMY_DATA == 1)
  ism330dhcx_gyro_set_samples_per_ts(0);
#else
  ism330dhcx_gyro_set_samples_per_ts(1000);
#endif
  ism330dhcx_gyro_set_odr(833);
  app_model.s_models[id] = &ism330dhcx_gyro_model;
  __stream_control(NULL, true);
  return 0;
}
char *ism330dhcx_gyro_get_key(void)
{
  return "ism330dhcx_gyro";
}

uint8_t ism330dhcx_gyro_get_odr(float *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_fs(float *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_enable(bool *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_samples_per_ts(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.spts;
  return 0;
}
uint8_t ism330dhcx_gyro_get_dim(int32_t *value)
{
  *value = 3;
  return 0;
}
uint8_t ism330dhcx_gyro_get_ioffset(float *value)
{
  *value = ism330dhcx_gyro_model.stream_params.ioffset;
  return 0;
}
uint8_t ism330dhcx_gyro_get_measodr(float *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_usb_dps(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.usb_dps;
  return 0;
}
uint8_t ism330dhcx_gyro_get_sd_dps(int32_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.sd_dps;
  return 0;
}
uint8_t ism330dhcx_gyro_get_sensitivity(float *value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_gyro_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t ism330dhcx_gyro_get_sensor_annotation(char **value)
{
  uint16_t id = ism330dhcx_gyro_model.id;
  ism330dhcx_gyro_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_gyro_model.annotation;
  return 0;
}
uint8_t ism330dhcx_gyro_get_sensor_category(int32_t *value)
{
    *value = ism330dhcx_gyro_model.sensor_status.isensor_class;
  return 0;
}
uint8_t ism330dhcx_gyro_get_stream_id(int8_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.stream_id;
  return 0;
}
uint8_t ism330dhcx_gyro_get_ep_id(int8_t *value)
{
  *value = ism330dhcx_gyro_model.stream_params.usb_ep;
  return 0;
}
uint8_t ism330dhcx_gyro_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(ism330dhcx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_gyro_model.sensor_status.type.mems.odr = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    ism330dhcx_gyro_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_gyro_set_fs(float value)
{
  sys_error_code_t ret = SMSensorSetFS(ism330dhcx_gyro_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_gyro_model.sensor_status.type.mems.fs = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
  }
  return ret;
}
uint8_t ism330dhcx_gyro_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(ism330dhcx_gyro_model.id);
  }
  else
  {
    ret = SMSensorDisable(ism330dhcx_gyro_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_gyro_model.sensor_status.is_active = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_gyro_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 1000;
  if (value >= min_v && value <= max_v)
  {
    ism330dhcx_gyro_model.stream_params.spts = value;
  }
  return 0;
}
uint8_t ism330dhcx_gyro_set_sensor_annotation(const char *value)
{
  strcpy(ism330dhcx_gyro_model.annotation, value);
  return 0;
}

/* ISM330DHCX_ACC PnPL Component ---------------------------------------------*/
static SensorModel_t ism330dhcx_acc_model;

uint8_t ism330dhcx_acc_comp_init(void)
{
  ism330dhcx_acc_model.comp_name = ism330dhcx_acc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_ACC);
  ism330dhcx_acc_model.id = id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  ism330dhcx_acc_model.stream_params.stream_id = -1;
  ism330dhcx_acc_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  ism330dhcx_acc_set_sensor_annotation(default_notes);
  /* USER Code */
#if (HSD_USE_DUMMY_DATA == 1)
  ism330dhcx_acc_set_samples_per_ts(0);
#else
  ism330dhcx_acc_set_samples_per_ts(1000);
#endif
  ism330dhcx_acc_set_odr(833);
  app_model.s_models[id] = &ism330dhcx_acc_model;
  __stream_control(NULL, true);
  return 0;
}
char *ism330dhcx_acc_get_key(void)
{
  return "ism330dhcx_acc";
}

uint8_t ism330dhcx_acc_get_odr(float *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_fs(float *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_enable(bool *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_samples_per_ts(int32_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.spts;
  return 0;
}
uint8_t ism330dhcx_acc_get_dim(int32_t *value)
{
  *value = 3;
  return 0;
}
uint8_t ism330dhcx_acc_get_ioffset(float *value)
{
  *value = ism330dhcx_acc_model.stream_params.ioffset;
  return 0;
}
uint8_t ism330dhcx_acc_get_measodr(float *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_usb_dps(int32_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.usb_dps;
  return 0;
}
uint8_t ism330dhcx_acc_get_sd_dps(int32_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.sd_dps;
  return 0;
}
uint8_t ism330dhcx_acc_get_sensitivity(float *value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_acc_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t ism330dhcx_acc_get_sensor_annotation(char **value)
{
  uint16_t id = ism330dhcx_acc_model.id;
  ism330dhcx_acc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_acc_model.annotation;
  return 0;
}
uint8_t ism330dhcx_acc_get_sensor_category(int32_t *value)
{
    *value = ism330dhcx_acc_model.sensor_status.isensor_class;
  return 0;
}
uint8_t ism330dhcx_acc_get_stream_id(int8_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.stream_id;
  return 0;
}
uint8_t ism330dhcx_acc_get_ep_id(int8_t *value)
{
  *value = ism330dhcx_acc_model.stream_params.usb_ep;
  return 0;
}
uint8_t ism330dhcx_acc_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(ism330dhcx_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_acc_model.sensor_status.type.mems.odr = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
#if (HSD_USE_DUMMY_DATA != 1)
    ism330dhcx_acc_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_acc_set_fs(float value)
{
  sys_error_code_t ret = SMSensorSetFS(ism330dhcx_acc_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_acc_model.sensor_status.type.mems.fs = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
  }
  return ret;
}
uint8_t ism330dhcx_acc_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(ism330dhcx_acc_model.id);
  }
  else
  {
    ret = SMSensorDisable(ism330dhcx_acc_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    ism330dhcx_acc_model.sensor_status.is_active = value;
    if (app_model.mlc_ucf_valid == true)
    {
      app_model.mlc_ucf_valid = false;
    }
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_acc_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 1000;
  if (value >= min_v && value <= max_v)
  {
    ism330dhcx_acc_model.stream_params.spts = value;
    //update spts in sensor manager
  }
  return 0;
}
uint8_t ism330dhcx_acc_set_sensor_annotation(const char *value)
{
  strcpy(ism330dhcx_acc_model.annotation, value);
  return 0;
}

/* ISM330DHCX_MLC PnPL Component ---------------------------------------------*/
static SensorModel_t ism330dhcx_mlc_model;

uint8_t ism330dhcx_mlc_comp_init(void)
{
  ism330dhcx_mlc_model.comp_name = ism330dhcx_mlc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "ism330dhcx", COM_TYPE_MLC);
  ism330dhcx_mlc_model.id = id;
  ism330dhcx_mlc_model.sensor_status = SMSensorGetStatus(id);
  ism330dhcx_mlc_model.stream_params.stream_id = -1;
  ism330dhcx_mlc_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  ism330dhcx_mlc_set_sensor_annotation(default_notes);
  /* USER Code */
#if (HSD_USE_DUMMY_DATA == 1)
  ism330dhcx_mlc_model.stream_params.spts = 0;
#else
  ism330dhcx_mlc_model.stream_params.spts = 1;
#endif
  app_model.s_models[id] = &ism330dhcx_mlc_model;
  app_model.mlc_ucf_valid = false;
  __stream_control(NULL, true);
  return 0;
}
char *ism330dhcx_mlc_get_key(void)
{
  return "ism330dhcx_mlc";
}

uint8_t ism330dhcx_mlc_get_enable(bool *value)
{
  uint16_t id = ism330dhcx_mlc_model.id;
  ism330dhcx_mlc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_mlc_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_mlc_get_samples_per_ts(int32_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.spts;
  return 0;
}
uint8_t ism330dhcx_mlc_get_ucf_status(bool *value)
{
  *value = app_model.mlc_ucf_valid;
  return 0;
}
uint8_t ism330dhcx_mlc_get_dim(int32_t *value)
{
  *value = 9;
  return 0;
}
uint8_t ism330dhcx_mlc_get_ioffset(float *value)
{
  *value = ism330dhcx_mlc_model.stream_params.ioffset;
  return 0;
}
uint8_t ism330dhcx_mlc_get_data_type(char **value)
{
  *value = "int8"; //TODO check this
  return 0;
}
uint8_t ism330dhcx_mlc_get_usb_dps(int32_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.usb_dps;
  return 0;
}
uint8_t ism330dhcx_mlc_get_sd_dps(int32_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.sd_dps;
  return 0;
}
uint8_t ism330dhcx_mlc_get_sensor_annotation(char **value)
{
  uint16_t id = ism330dhcx_mlc_model.id;
  ism330dhcx_mlc_model.sensor_status = SMSensorGetStatus(id);
  *value = ism330dhcx_mlc_model.annotation;
  return 0;
}
uint8_t ism330dhcx_mlc_get_sensor_category(int32_t *value)
{
    *value = ism330dhcx_mlc_model.sensor_status.isensor_class;
  return 0;
}
uint8_t ism330dhcx_mlc_get_stream_id(int8_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.stream_id;
  return 0;
}
uint8_t ism330dhcx_mlc_get_ep_id(int8_t *value)
{
  *value = ism330dhcx_mlc_model.stream_params.usb_ep;
  return 0;
}
uint8_t ism330dhcx_mlc_set_enable(bool value)
{
  sys_error_code_t ret = 1;
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
    ism330dhcx_mlc_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t ism330dhcx_mlc_set_sensor_annotation(const char *value)
{
  /* USER Code */
  return 0;
}
uint8_t ism330dhcx_mlc_load_file(IIsm330dhcx_Mlc_t *ifn, const char *data, int32_t size)
{
  IIsm330dhcx_Mlc_load_file(ifn, data, size);
  app_model.mlc_ucf_valid = true;
  ism330dhcx_mlc_model.sensor_status.is_active = true;
  __stream_control(NULL, true);
  return 0;
}

/* LPS22HH_TEMP PnPL Component -----------------------------------------------*/
static SensorModel_t lps22hh_temp_model;
static SensorModel_t lps22hh_press_model;

uint8_t lps22hh_temp_comp_init(void)
{
  lps22hh_temp_model.comp_name = lps22hh_temp_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lps22hh", COM_TYPE_TEMP);
  lps22hh_temp_model.id = id;
  lps22hh_temp_model.sensor_status = SMSensorGetStatus(id);
  lps22hh_temp_model.stream_params.stream_id = -1;
  lps22hh_temp_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  lps22hh_temp_set_sensor_annotation(default_notes);
  /* USER code */
#if (HSD_USE_DUMMY_DATA != 0)
  lps22hh_temp_set_samples_per_ts(0);
#else
  lps22hh_temp_set_samples_per_ts(200);
#endif
  app_model.s_models[id] = &lps22hh_temp_model;
  __stream_control(NULL, true);

  /* USER Component initialization code */
  return 0;
}
char *lps22hh_temp_get_key(void)
{
  return "lps22hh_temp";
}

uint8_t lps22hh_temp_get_odr(float *value)
{
  uint16_t id = lps22hh_temp_model.id;
  lps22hh_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_temp_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}
uint8_t lps22hh_temp_get_fs(float *value)
{
  uint16_t id = lps22hh_temp_model.id;
  lps22hh_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_temp_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}
uint8_t lps22hh_temp_get_enable(bool *value)
{
  uint16_t id = lps22hh_temp_model.id;
  lps22hh_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_temp_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t lps22hh_temp_get_samples_per_ts(int32_t *value)
{
  *value = lps22hh_temp_model.stream_params.spts;
  return 0;
}
uint8_t lps22hh_temp_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}
uint8_t lps22hh_temp_get_ioffset(float *value)
{
  *value = lps22hh_temp_model.stream_params.ioffset;
  return 0;
}
uint8_t lps22hh_temp_get_measodr(float *value)
{
  uint16_t id = lps22hh_temp_model.id;
  lps22hh_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_temp_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}
uint8_t lps22hh_temp_get_usb_dps(int32_t *value)
{
  *value = lps22hh_temp_model.stream_params.usb_dps;
  return 0;
}
uint8_t lps22hh_temp_get_sd_dps(int32_t *value)
{
  *value = lps22hh_temp_model.stream_params.sd_dps;
  return 0;
}
uint8_t lps22hh_temp_get_sensitivity(float *value)
{
  uint16_t id = lps22hh_temp_model.id;
  lps22hh_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_temp_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}
uint8_t lps22hh_temp_get_data_type(char **value)
{
  *value = "float";
  return 0;
}
uint8_t lps22hh_temp_get_sensor_annotation(char **value)
{
  uint16_t id = lps22hh_temp_model.id;
  lps22hh_temp_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_temp_model.annotation;
  return 0;
}
uint8_t lps22hh_temp_get_sensor_category(int32_t *value)
{
    *value = lps22hh_temp_model.sensor_status.isensor_class;
  return 0;
}
uint8_t lps22hh_temp_get_stream_id(int8_t *value)
{
  *value = lps22hh_temp_model.stream_params.stream_id;
  return 0;
}
uint8_t lps22hh_temp_get_ep_id(int8_t *value)
{
  *value = lps22hh_temp_model.stream_params.usb_ep;
  return 0;
}
uint8_t lps22hh_temp_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(lps22hh_temp_model.id, value);
  if(ret == SYS_NO_ERROR_CODE)
  {
    lps22hh_temp_model.sensor_status.type.mems.odr = value;
    lps22hh_press_model.sensor_status.type.mems.odr = value;
    /* USER Code */
#if (HSD_USE_DUMMY_DATA != 1)
    lps22hh_temp_set_samples_per_ts((int32_t)value);
    lps22hh_press_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t lps22hh_temp_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(lps22hh_temp_model.id);
  }
  else
  {
    ret = SMSensorDisable(lps22hh_temp_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    lps22hh_temp_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t lps22hh_temp_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 200;
  if (value >= min_v && value <= max_v)
  {
    lps22hh_temp_model.stream_params.spts = value;
    lps22hh_press_model.stream_params.spts = value;
  }
  return 0;
}
uint8_t lps22hh_temp_set_sensor_annotation(const char *value)
{
  strcpy(lps22hh_temp_model.annotation, value);
  return 0;
}

/* LPS22HH_PRESS PnPL Component ----------------------------------------------*/
uint8_t lps22hh_press_comp_init(void)
{
  lps22hh_press_model.comp_name = lps22hh_press_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lps22hh", COM_TYPE_PRESS);
  lps22hh_press_model.id = id;
  lps22hh_press_model.sensor_status = SMSensorGetStatus(id);
  lps22hh_press_model.stream_params.stream_id = -1;
  lps22hh_press_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  lps22hh_press_set_sensor_annotation(default_notes);
  /* USER code */
#if (HSD_USE_DUMMY_DATA != 0)
  lps22hh_press_set_samples_per_ts(0);
#else
  lps22hh_press_set_samples_per_ts(200);
#endif
  app_model.s_models[id] = &lps22hh_press_model;
  __stream_control(NULL, true);

  return 0;
}

char *lps22hh_press_get_key(void)
{
  return "lps22hh_press";
}

uint8_t lps22hh_press_get_odr(float *value)
{
  uint16_t id = lps22hh_press_model.id;
  lps22hh_press_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_press_model.sensor_status.type.mems.odr;
  /* USER Code */
  return 0;
}

uint8_t lps22hh_press_get_fs(float *value)
{
  uint16_t id = lps22hh_press_model.id;
  lps22hh_press_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_press_model.sensor_status.type.mems.fs;
  /* USER Code */
  return 0;
}

uint8_t lps22hh_press_get_enable(bool *value)
{
  uint16_t id = lps22hh_press_model.id;
  lps22hh_press_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_press_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}

uint8_t lps22hh_press_get_samples_per_ts(int32_t *value)
{
  *value = lps22hh_press_model.stream_params.spts;
  return 0;
}

uint8_t lps22hh_press_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}

uint8_t lps22hh_press_get_ioffset(float *value)
{
  *value = lps22hh_press_model.stream_params.ioffset;
  return 0;
}

uint8_t lps22hh_press_get_measodr(float *value)
{
  uint16_t id = lps22hh_press_model.id;
  lps22hh_press_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_press_model.sensor_status.type.mems.measured_odr;
  /* USER Code */
  return 0;
}

uint8_t lps22hh_press_get_usb_dps(int32_t *value)
{
  *value = lps22hh_press_model.stream_params.usb_dps;
  return 0;
}

uint8_t lps22hh_press_get_sd_dps(int32_t *value)
{
  *value = lps22hh_press_model.stream_params.sd_dps;
  return 0;
}

uint8_t lps22hh_press_get_sensitivity(float *value)
{
  uint16_t id = lps22hh_press_model.id;
  lps22hh_press_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_press_model.sensor_status.type.mems.sensitivity;
  /* USER Code */
  return 0;
}

uint8_t lps22hh_press_get_data_type(char **value)
{
  *value = "float";
  return 0;
}
uint8_t lps22hh_press_get_sensor_annotation(char **value)
{
  uint16_t id = lps22hh_press_model.id;
  lps22hh_press_model.sensor_status = SMSensorGetStatus(id);
  *value = lps22hh_press_model.annotation;
  return 0;
}
uint8_t lps22hh_press_get_sensor_category(int32_t *value)
{
    *value = lps22hh_press_model.sensor_status.isensor_class;
  return 0;
}

uint8_t lps22hh_press_get_stream_id(int8_t *value)
{
  *value = lps22hh_press_model.stream_params.stream_id;
  return 0;
}

uint8_t lps22hh_press_get_ep_id(int8_t *value)
{
  *value = lps22hh_press_model.stream_params.usb_ep;
  return 0;
}

uint8_t lps22hh_press_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(lps22hh_press_model.id, value);
  if(ret == SYS_NO_ERROR_CODE)
  {
    lps22hh_press_model.sensor_status.type.mems.odr = value;
    lps22hh_temp_model.sensor_status.type.mems.odr = value;
    /* USER Code */
#if (HSD_USE_DUMMY_DATA != 1)
    lps22hh_press_set_samples_per_ts((int32_t)value);
    lps22hh_temp_set_samples_per_ts((int32_t)value);
#endif
    __stream_control(NULL, true);
  }
  return ret;
}

uint8_t lps22hh_press_set_fs(float value)
{
  sys_error_code_t ret = SMSensorSetFS(lps22hh_press_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    lps22hh_press_model.sensor_status.type.mems.fs = value;
    /* USER Code */
  }
  return ret;
}

uint8_t lps22hh_press_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(lps22hh_press_model.id);
  }
  else
  {
    ret = SMSensorDisable(lps22hh_press_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    lps22hh_press_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}

uint8_t lps22hh_press_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 200;
  if (value >= min_v && value <= max_v)
  {
    lps22hh_press_model.stream_params.spts = value;
    lps22hh_temp_model.stream_params.spts = value;
  }
  return 0;
}
uint8_t lps22hh_press_set_sensor_annotation(const char *value)
{
  strcpy(lps22hh_press_model.annotation, value);
  return 0;
}

/* MP23DB01HP_1_MIC PnPL Component -------------------------------------------*/
static SensorModel_t mp23db01hp_1_mic_model;

uint8_t mp23db01hp_1_mic_comp_init(void)
{
  mp23db01hp_1_mic_model.comp_name = mp23db01hp_1_mic_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "mp23db01hp", COM_TYPE_MIC);
  mp23db01hp_1_mic_model.id = id;
  mp23db01hp_1_mic_model.sensor_status = SMSensorGetStatus(id);
  mp23db01hp_1_mic_model.stream_params.stream_id = -1;
  mp23db01hp_1_mic_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  mp23db01hp_1_mic_set_sensor_annotation(default_notes);
  /* USER code */
#if (HSD_USE_DUMMY_DATA != 0)
  mp23db01hp_1_mic_set_samples_per_ts(0);
#else
  mp23db01hp_1_mic_set_samples_per_ts(1000);
#endif
  app_model.s_models[id] = &mp23db01hp_1_mic_model;
  __stream_control(NULL, true);

  /* USER Component initialization code */
  return 0;
}
char *mp23db01hp_1_mic_get_key(void)
{
  return "mp23db01hp_1_mic";
}

uint8_t mp23db01hp_1_mic_get_odr(float *value)
{
  uint16_t id = mp23db01hp_1_mic_model.id;
  mp23db01hp_1_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_1_mic_model.sensor_status.type.audio.frequency;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_1_mic_get_aop(float *value)
{
  uint16_t id = mp23db01hp_1_mic_model.id;
  mp23db01hp_1_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_1_mic_model.sensor_status.type.audio.volume;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_1_mic_get_enable(bool *value)
{
  uint16_t id = mp23db01hp_1_mic_model.id;
  mp23db01hp_1_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_1_mic_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_1_mic_get_samples_per_ts(int32_t *value)
{
  *value = mp23db01hp_1_mic_model.stream_params.spts;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_ioffset(float *value)
{
  *value = mp23db01hp_1_mic_model.stream_params.ioffset;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_measodr(float *value)
{
  uint16_t id = mp23db01hp_1_mic_model.id;
  mp23db01hp_1_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_1_mic_model.sensor_status.type.audio.frequency;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_1_mic_get_usb_dps(int32_t *value)
{
  *value = mp23db01hp_1_mic_model.stream_params.usb_dps;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_sd_dps(int32_t *value)
{
  *value = mp23db01hp_1_mic_model.stream_params.sd_dps;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_sensitivity(float *value)
{
  *value = 0.000030517578125; //2.0f/((float)(2^mp23db01hp_1_mic_model.sensor_status.type.audio.resolution));
  return 0;
}
uint8_t mp23db01hp_1_mic_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t mp23db01hp_1_mic_get_sensor_annotation(char **value)
{
  uint16_t id = mp23db01hp_1_mic_model.id;
  mp23db01hp_1_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_1_mic_model.annotation;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_sensor_category(int32_t *value)
{
    *value = mp23db01hp_1_mic_model.sensor_status.isensor_class;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_stream_id(int8_t *value)
{
  *value = mp23db01hp_1_mic_model.stream_params.stream_id;
  return 0;
}
uint8_t mp23db01hp_1_mic_get_ep_id(int8_t *value)
{
  *value = mp23db01hp_1_mic_model.stream_params.usb_ep;
  return 0;
}
uint8_t mp23db01hp_1_mic_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetFrequency(mp23db01hp_1_mic_model.id, (uint32_t)value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    mp23db01hp_1_mic_model.sensor_status.type.audio.frequency = (uint32_t)value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t mp23db01hp_1_mic_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(mp23db01hp_1_mic_model.id);
  }
  else
  {
    ret = SMSensorDisable(mp23db01hp_1_mic_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    mp23db01hp_1_mic_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t mp23db01hp_1_mic_set_samples_per_ts(int32_t value)
{
  int32_t min_v = 0;
  int32_t max_v = 1000;
  if (value >= min_v && value <= max_v)
  {
    mp23db01hp_1_mic_model.stream_params.spts = value;
  }
  return 0;
}
uint8_t mp23db01hp_1_mic_set_sensor_annotation(const char *value)
{
  /* USER Code */
  return 0;
}

/* MP23DB01HP_2_MIC PnPL Component -------------------------------------------*/
static SensorModel_t mp23db01hp_2_mic_model;

uint8_t mp23db01hp_2_mic_comp_init(void)
{
  mp23db01hp_2_mic_model.comp_name = mp23db01hp_2_mic_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "mp23db01hp_2", COM_TYPE_MIC);
  mp23db01hp_2_mic_model.id = id;
  mp23db01hp_2_mic_model.sensor_status = SMSensorGetStatus(id);
  mp23db01hp_2_mic_model.stream_params.stream_id = -1;
  mp23db01hp_2_mic_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  mp23db01hp_2_mic_set_sensor_annotation(default_notes);
  /* USER code */
  app_model.s_models[id] = &mp23db01hp_2_mic_model;
  __stream_control(NULL, true);

  /* USER Component initialization code */
  return 0;
}
char *mp23db01hp_2_mic_get_key(void)
{
  return "mp23db01hp_2_mic";
}

uint8_t mp23db01hp_2_mic_get_odr(float *value)
{
  uint16_t id = mp23db01hp_2_mic_model.id;
  mp23db01hp_2_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_2_mic_model.sensor_status.type.audio.frequency;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_aop(float *value)
{
  uint16_t id = mp23db01hp_2_mic_model.id;
  mp23db01hp_2_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_2_mic_model.sensor_status.type.audio.volume;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_enable(bool *value)
{
  uint16_t id = mp23db01hp_2_mic_model.id;
  mp23db01hp_2_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_2_mic_model.sensor_status.is_active;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_samples_per_ts(int32_t *value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_dim(int32_t *value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_ioffset(float *value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_measodr(float *value)
{
  uint16_t id = mp23db01hp_2_mic_model.id;
  mp23db01hp_2_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_2_mic_model.sensor_status.type.audio.frequency;
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_usb_dps(int32_t *value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_sd_dps(int32_t *value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_sensitivity(float *value)
{
  *value = 0.000030517578125; //2/(2^mp23db01hp_2_mic_model.sensor_status.type.audio.resolution);
  return 0;
}
uint8_t mp23db01hp_2_mic_get_data_type(char **value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_sensor_annotation(char **value)
{
  uint16_t id = mp23db01hp_2_mic_model.id;
  mp23db01hp_2_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = mp23db01hp_2_mic_model.annotation;
  return 0;
}
uint8_t mp23db01hp_2_mic_get_sensor_category(int32_t *value)
{
    *value = mp23db01hp_2_mic_model.sensor_status.isensor_class;
  return 0;
}
uint8_t mp23db01hp_2_mic_get_stream_id(int8_t *value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_get_ep_id(int8_t *value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetFrequency(mp23db01hp_2_mic_model.id, (uint32_t)value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    mp23db01hp_2_mic_model.sensor_status.type.audio.frequency = (uint32_t)value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t mp23db01hp_2_mic_set_enable(bool value)
{
  sys_error_code_t ret = 1;
  if (value)
  {
    ret = SMSensorEnable(mp23db01hp_2_mic_model.id);
  }
  else
  {
    ret = SMSensorDisable(mp23db01hp_2_mic_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    mp23db01hp_2_mic_model.sensor_status.is_active = value;
    /* USER Code */
    __stream_control(NULL, true);
  }
  return ret;
}
uint8_t mp23db01hp_2_mic_set_samples_per_ts(int32_t value)
{
  /* USER Code */
  return 0;
}
uint8_t mp23db01hp_2_mic_set_sensor_annotation(const char *value)
{
  /* USER Code */
  return 0;
}

/* AutoMode PnPL Component ---------------------------------------------------*/
uint8_t automode_comp_init(void)
{
  app_model.automode_model.comp_name = automode_get_key();

  /* USER Component initialization code */
  return 0;
}
char *automode_get_key(void)
{
  return "automode";
}

uint8_t automode_get_enabled(bool *value)
{
  /* USER Code */
  *value = app_model.automode_model.enabled;
  return 0;
}
uint8_t automode_get_nof_acquisitions(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.nof_acquisitions;
  return 0;
}
uint8_t automode_get_start_delay_s(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.start_delay_s;
  return 0;
}
uint8_t automode_get_logging_period_s(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.logging_period_s;
  return 0;
}
uint8_t automode_get_idle_period_s(int32_t *value)
{
  /* USER Code */
  *value = app_model.automode_model.idle_period_s;
  return 0;
}
uint8_t automode_set_enabled(bool value)
{
  /* USER Code */
  app_model.automode_model.enabled = value;
  return 0;
}
uint8_t automode_set_nof_acquisitions(int32_t value)
{
  /* USER Code */
  app_model.automode_model.nof_acquisitions = value;
  return 0;
}
uint8_t automode_set_start_delay_s(int32_t value)
{
  /* USER Code */
  app_model.automode_model.start_delay_s = value;
  return 0;
}
uint8_t automode_set_logging_period_s(int32_t value)
{
  /* USER Code */
  app_model.automode_model.logging_period_s = value;
  return 0;
}
uint8_t automode_set_idle_period_s(int32_t value)
{
  /* USER Code */
  app_model.automode_model.idle_period_s = value;
  return 0;
}

/* Log Controller PnPL Component ---------------------------------------------*/
uint8_t log_controller_comp_init(void)
{
  app_model.log_controller_model.comp_name = log_controller_get_key();

  app_model.log_controller_model.status = false;
  app_model.log_controller_model.interface = -1;
  return 0;
}

char *log_controller_get_key(void)
{
  return "log_controller";
}

uint8_t log_controller_get_log_status(bool *value)
{
  *value = app_model.log_controller_model.status;
  return 0;
}
uint8_t log_controller_get_sd_mounted(bool *value)
{
  /* USER Code */
  *value = false;
  return 0;
}
uint8_t log_controller_get_controller_type(int32_t *value)
{
  *value = 0; /* 0 == HSD log controller, 1 == App classifier controller, 2 = generic log controller */
  return 0;
}
uint8_t log_controller_save_config(ILog_Controller_t *ifn)
{
  ILog_Controller_save_config(ifn);
  return 0;
}
uint8_t log_controller_start_log(ILog_Controller_t *ifn, int32_t interface)
{
  app_model.log_controller_model.interface = interface;

  __stream_control(ifn, TRUE);

  //Reset Tag counter
  TMResetTagListCounter();
  ILog_Controller_start_log(ifn, interface);

  RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdate, RTC_FORMAT_BIN);

  _tm t =
  { .tm_year = sdate.Year + 2000, .tm_mon = sdate.Month - 1, .tm_mday = sdate.Date, .tm_hour = stime.Hours, .tm_min = stime.Minutes, .tm_sec = stime.Seconds };

// WHY THIS -1 (in months) ???
//  struct tm {
//     int tm_sec;         /* seconds,  range 0 to 59          */
//     int tm_min;         /* minutes, range 0 to 59           */
//     int tm_hour;        /* hours, range 0 to 23             */
//     int tm_mday;        /* day of the month, range 1 to 31  */
//     int tm_mon;         /* month, range 0 to 11             */ <------ (-1) months here (0..11), months from RTC (1..12)
//     int tm_year;        /* The number of years since 1900   */
//     int tm_wday;        /* day of the week, range 0 to 6    */
//     int tm_yday;        /* day in the year, range 0 to 365  */
//     int tm_isdst;       /* daylight saving time             */
//  };

  TMSetStartTime(t);
  sprintf(app_model.acquisition_info_model.start_time, "%04d-%02d-%02dT%02d:%02d:%02d", t.tm_year, t.tm_mon + 1,
          t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

  /* last part not done in sprintf to avoid a warning  */
  app_model.acquisition_info_model.start_time[19] = '.';
  app_model.acquisition_info_model.start_time[20] = '0';
  app_model.acquisition_info_model.start_time[21] = '0';
  app_model.acquisition_info_model.start_time[22] = '0';
  app_model.acquisition_info_model.start_time[23] = 'Z';
  app_model.acquisition_info_model.start_time[24] = '\0';

  return 0;
}
uint8_t log_controller_stop_log(ILog_Controller_t *ifn)
{
  PnPLGenerateAcquisitionUUID(app_model.acquisition_info_model.uuid);
  ILog_Controller_stop_log(ifn);
  TMCalculateEndTime(app_model.acquisition_info_model.end_time);
  return 0;
}
uint8_t log_controller_set_time(ILog_Controller_t *ifn, const char *datetime)
{
  char datetimeStr[3];

  //internal input format: yyyyMMdd_hh_mm_ss

  RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;

  /** extract year string (only the last two digit). It will be necessary to add 2000*/
  datetimeStr[0] = datetime[2];
  datetimeStr[1] = datetime[3];
  datetimeStr[2] = '\0';
  sdate.Year = atoi(datetimeStr);

  /** extract month string */
  datetimeStr[0] = datetime[4];
  datetimeStr[1] = datetime[5];
  sdate.Month = atoi(datetimeStr);

  /** extract day string */
  datetimeStr[0] = datetime[6];
  datetimeStr[1] = datetime[7];
  sdate.Date = atoi(datetimeStr);

  /** Week day initialization (not used)*/
  sdate.WeekDay = RTC_WEEKDAY_MONDAY; //Not used

  /** extract hour string */
  datetimeStr[0] = datetime[9];
  datetimeStr[1] = datetime[10];
  stime.Hours = atoi(datetimeStr);

  /** extract minute string */
  datetimeStr[0] = datetime[12];
  datetimeStr[1] = datetime[13];
  stime.Minutes = atoi(datetimeStr);

  /** extract second string */
  datetimeStr[0] = datetime[15];
  datetimeStr[1] = datetime[16];
  stime.Seconds = atoi(datetimeStr);

  /** not used */
  //stime.TimeFormat = RTC_HOURFORMAT12_AM;
  stime.SecondFraction = 0;
  stime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stime.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    while (1);
  }
  if (HAL_RTC_SetDate(&hrtc, &sdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    while (1);
  }
  return 0;
}
uint8_t log_controller_switch_bank(ILog_Controller_t *ifn)
{
  return 0;
}

/* Tags Information PnPL Component -------------------------------------------*/
uint8_t tags_info_comp_init(void)
{
  app_model.tags_info_model.comp_name = tags_info_get_key();

  TMInit(HSD_SW_TAG_CLASS_NUM, HSD_HW_TAG_CLASS_NUM);

  tags_info_set_sw_tag0__enabled(true);
  tags_info_set_sw_tag1__enabled(true);
  tags_info_set_sw_tag2__enabled(true);
  tags_info_set_sw_tag3__enabled(true);
  tags_info_set_sw_tag4__enabled(true);
  tags_info_set_sw_tag0__status(false);
  tags_info_set_sw_tag1__status(false);
  tags_info_set_sw_tag2__status(false);
  tags_info_set_sw_tag3__status(false);
  tags_info_set_sw_tag4__status(false);
  return 0;
}

char *tags_info_get_key(void)
{
  return "tags_info";
}

uint8_t tags_info_get_max_tags_num(int32_t *value)
{
  *value = HSD_MAX_TAGS_NUM;
  return 0;
}
uint8_t tags_info_get_sw_tag0__label(char **value)
{
  *value = TMGetSWTagLabel(0);
  return 0;
}
uint8_t tags_info_get_sw_tag0__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(0);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag0__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(0);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag1__label(char **value)
{
  *value = TMGetSWTagLabel(1);
  return 0;
}
uint8_t tags_info_get_sw_tag1__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(1);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag1__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(1);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag2__label(char **value)
{
  *value = TMGetSWTagLabel(2);
  return 0;
}
uint8_t tags_info_get_sw_tag2__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(2);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag2__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(2);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag3__label(char **value)
{
  *value = TMGetSWTagLabel(3);
  return 0;
}
uint8_t tags_info_get_sw_tag3__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(3);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag3__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(3);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_get_sw_tag4__label(char **value)
{
  *value = TMGetSWTagLabel(4);
  return 0;
}
uint8_t tags_info_get_sw_tag4__enabled(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(4);
  *value = sw_tag_class->enabled;
  return 0;
}
uint8_t tags_info_get_sw_tag4__status(bool *value)
{
  HSD_SW_Tag_Class_t *sw_tag_class;
  sw_tag_class = TMGetSWTag(4);
  *value = sw_tag_class->status;
  return 0;
}
uint8_t tags_info_set_sw_tag0__label(const char *value)
{
  return TMSetSWTagLabel(value, 0);
}
uint8_t tags_info_set_sw_tag0__enabled(bool value)
{
  return TMEnableSWTag(value, 0);
}
uint8_t tags_info_set_sw_tag0__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 0);
  }
  else
  {
    TMInitSWTagStatus(false, 0);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag1__label(const char *value)
{
  return TMSetSWTagLabel(value, 1);
}
uint8_t tags_info_set_sw_tag1__enabled(bool value)
{
  return TMEnableSWTag(value, 1);
}
uint8_t tags_info_set_sw_tag1__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 1);
  }
  else
  {
    TMInitSWTagStatus(false, 1);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag2__label(const char *value)
{
  return TMSetSWTagLabel(value, 2);
}
uint8_t tags_info_set_sw_tag2__enabled(bool value)
{
  return TMEnableSWTag(value, 2);
}
uint8_t tags_info_set_sw_tag2__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 2);
  }
  else
  {
    TMInitSWTagStatus(false, 2);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag3__label(const char *value)
{
  return TMSetSWTagLabel(value, 3);
}
uint8_t tags_info_set_sw_tag3__enabled(bool value)
{
  return TMEnableSWTag(value, 3);
}
uint8_t tags_info_set_sw_tag3__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 3);
  }
  else
  {
    TMInitSWTagStatus(false, 3);
  }
  return 0;
}
uint8_t tags_info_set_sw_tag4__label(const char *value)
{
  return TMSetSWTagLabel(value, 4);
}
uint8_t tags_info_set_sw_tag4__enabled(bool value)
{
  return TMEnableSWTag(value, 4);
}
uint8_t tags_info_set_sw_tag4__status(bool value)
{
  bool status;
  log_controller_get_log_status(&status);

  if (status)
  {
    TMSetSWTag(value, 4);
  }
  else
  {
    TMInitSWTagStatus(false, 4);
  }
  return 0;
}

/* Acquisition Information PnPL Component ------------------------------------*/
uint8_t acquisition_info_comp_init(void)
{
  app_model.acquisition_info_model.comp_name = acquisition_info_get_key();

  acquisition_info_set_name("B-U585I-IOT02A_acquisition");
  acquisition_info_set_description("");
  app_model.acquisition_info_model.interface = -1;
  return 0;
}

char *acquisition_info_get_key(void)
{
  return "acquisition_info";
}

uint8_t acquisition_info_get_name(char **value)
{
  *value = app_model.acquisition_info_model.name;
  return 0;
}
uint8_t acquisition_info_get_description(char **value)
{
  *value = app_model.acquisition_info_model.description;
  return 0;
}
uint8_t acquisition_info_get_uuid(char **value)
{
  *value = app_model.acquisition_info_model.uuid;
  return 0;
}
uint8_t acquisition_info_get_start_time(char **value)
{
  *value = app_model.acquisition_info_model.start_time;
  return 0;
}
uint8_t acquisition_info_get_end_time(char **value)
{
  *value = app_model.acquisition_info_model.end_time;
  return 0;
}
uint8_t acquisition_info_get_data_ext(char **value)
{
  *value = ".dat";
  return 0;
}
uint8_t acquisition_info_get_data_fmt(char **value)
{
  *value = "HSD_2.0.0";
  return 0;
}
uint8_t acquisition_info_get_tags(JSON_Value *value)
{
  JSON_Value *tempJSON1;
  JSON_Object *JSON_Tags;
  JSON_Array *JSON_TagsArray;

  JSON_Tags = json_value_get_object(value);

  json_object_set_value(JSON_Tags, "tags", json_value_init_array());
  JSON_TagsArray = json_object_dotget_array(JSON_Tags, "tags");

  int i;

  HSD_Tag_t *tag_list = TMGetTagList();
  uint8_t tag_list_size = TMGetTagListSize();
  if (tag_list_size > 0)
  {
    for (i = 0; i < tag_list_size; i++)
    {
      tempJSON1 = json_value_init_object();
      JSON_Object *tag_object = json_value_get_object(tempJSON1);
      json_object_set_string(tag_object, "l", tag_list[i].label);
      json_object_set_boolean(tag_object, "e", tag_list[i].status);
      json_object_set_string(tag_object, "ta", tag_list[i].abs_timestamp);
      json_array_append_value(JSON_TagsArray, tempJSON1);
    }
  }
  /* no need to free tempJSON1 as it is part of value */
  return 0;
}
uint8_t acquisition_info_get_interface(char **value)
{
  int8_t acq_interface = app_model.acquisition_info_model.interface;
  switch (acq_interface)
  {
//    case 0:
//      *value = "SD Card";
//      break;
    case 1:
      *value = "USB";
      break;
  }
  return 0;
}
uint8_t acquisition_info_get_schema_version(char **value)
{
  *value = "2.0.0";
  return 0;
}
uint8_t acquisition_info_set_name(const char *value)
{
  if (strlen(value) != 0)
  {
    strcpy(app_model.acquisition_info_model.name, value);
  }
  return 0;
}
uint8_t acquisition_info_set_description(const char *value)
{
  strcpy(app_model.acquisition_info_model.description, value);
  return 0;
}

/* Firmware Information PnPL Component ---------------------------------------*/
uint8_t firmware_info_comp_init(void)
{
  app_model.firmware_info_model.comp_name = firmware_info_get_key();

  char default_alias[DEVICE_ALIAS_LENGTH] = "B-U585I-IOT02A";
  firmware_info_set_alias(default_alias);
  return 0;
}

char *firmware_info_get_key(void)
{
  return "firmware_info";
}

uint8_t firmware_info_get_alias(char **value)
{
  *value = app_model.firmware_info_model.alias;
  return 0;
}
uint8_t firmware_info_get_fw_name(char **value)
{
  *value = "FP-SNS-DATALOG2_Datalog2";
  return 0;
}
uint8_t firmware_info_get_fw_version(char **value)
{
  *value = FW_VERSION_MAJOR "." FW_VERSION_MINOR "." FW_VERSION_PATCH;
  return 0;
}
uint8_t firmware_info_get_part_number(char **value)
{
  *value = "FP-SNS-DATALOG2";
  return 0;
}
uint8_t firmware_info_get_device_url(char **value)
{
  *value = "https://www.st.com/en/evaluation-tools/b-u585i-iot02a.html";
  return 0;
}
uint8_t firmware_info_get_fw_url(char **value)
{
  *value = "https://github.com/STMicroelectronics/fp-sns-datalog2";
  return 0;
}
uint8_t firmware_info_get_mac_address(char **value)
{
  /* USER Code */
  return 0;
}
uint8_t firmware_info_set_alias(const char *value)
{
  strcpy(app_model.firmware_info_model.alias, value);
  return 0;
}

/* Device Information PnPL Component -----------------------------------------*/
uint8_t DeviceInformation_comp_init(void)
{

  /* USER Component initialization code */
  return 0;
}

char *DeviceInformation_get_key(void)
{
  return "DeviceInformation";
}

uint8_t DeviceInformation_get_manufacturer(char **value)
{
  *value = "STMicroelectronics";
  return 0;
}
uint8_t DeviceInformation_get_model(char **value)
{
  *value = "B-U585I-IOT02A";
  return 0;
}
uint8_t DeviceInformation_get_swVersion(char **value)
{
  *value = FW_VERSION_MAJOR "." FW_VERSION_MINOR "." FW_VERSION_PATCH;
  return 0;
}
uint8_t DeviceInformation_get_osName(char **value)
{
  *value = "AzureRTOS";
  return 0;
}
uint8_t DeviceInformation_get_processorArchitecture(char **value)
{
  *value = "ARM Cortex-M33";
  return 0;
}
uint8_t DeviceInformation_get_processorManufacturer(char **value)
{
  *value = "STMicroelectronics";
  return 0;
}
uint8_t DeviceInformation_get_totalStorage(float *value)
{
  *value = 0;
  return 0;
}
uint8_t DeviceInformation_get_totalMemory(float *value)
{
  *value = 0;
  return 0;
}

/* USER Code : --> compute stream ids */
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status)
{
  int8_t i;
  if (status) //set stream ids
  {
    int8_t j, st_i = 0;
    uint16_t proposed_fifoWM = 1;
    //sort stream id by bandwidth
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      if (app_model.s_models[i] != NULL)
      {
        if (app_model.s_models[i]->sensor_status.is_active == true)
        {
          if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_MEMS)
          {
            app_model.s_models[i]->stream_params.bandwidth = app_model.s_models[i]->sensor_status.type.mems.odr * SMGetnBytesPerSample(i);
          }
          else if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_AUDIO)
          {
            app_model.s_models[i]->stream_params.bandwidth = app_model.s_models[i]->sensor_status.type.audio.frequency * SMGetnBytesPerSample(i);
          }
          else
          {
            /* TODO: add support for other ISENSOR_CLASS */
          }

          if (app_model.log_controller_model.interface == LOG_CTRL_MODE_USB)
          {

            /* in case of slow sensor send 1 sample for each usb packet */
            float low_odr = 0;
            if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_MEMS)
            {
              low_odr = app_model.s_models[i]->sensor_status.type.mems.odr;
            }
            else if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_AUDIO)
            {
              low_odr = app_model.s_models[i]->sensor_status.type.audio.frequency;
            }
            else
            {
              /* TODO: add support for other ISENSOR_CLASS */
            }

            if (low_odr <= 20)
            {
              /* When there's a timestamp, more then one packet will be sent */
              app_model.s_models[i]->stream_params.usb_dps = SMGetnBytesPerSample(i) + 8;  /* 8 = timestamp dimension in bytes */
              proposed_fifoWM = 1;
              sys_error_code_t ret = SMSensorSetFifoWM(i, proposed_fifoWM);
              if (ret != SYS_NO_ERROR_CODE)
              {

              }
            }
            else
            {
              /* 50ms of sensor data; when there's a timestamp packets will be sent fastly */
              app_model.s_models[i]->stream_params.usb_dps = (uint32_t)(app_model.s_models[i]->stream_params.bandwidth * 0.05f);
              if (app_model.s_models[i]->stream_params.usb_dps > 7000)
              {
                app_model.s_models[i]->stream_params.usb_dps = 7000; // set a limit to avoid buffer to big
              }
              else if (app_model.s_models[i]->stream_params.usb_dps < SMGetnBytesPerSample(i) + 8)
              {
                /* In case usb_dps is a very low value, verify the setup to send at least 1 sensor data + timestamp */
                app_model.s_models[i]->stream_params.usb_dps = SMGetnBytesPerSample(i) + 8;
              }

              proposed_fifoWM = app_model.s_models[i]->stream_params.usb_dps / SMGetnBytesPerSample(i) / 2;
              if (proposed_fifoWM == 0)
              {
                proposed_fifoWM = 1;
              }
              sys_error_code_t ret = SMSensorSetFifoWM(i, proposed_fifoWM);
              if (ret != SYS_NO_ERROR_CODE)
              {

              }
            }
#ifdef SYS_DEBUG
            SensorDescriptor_t descriptor = SMSensorGetDescription(i);
            float ms = app_model.s_models[i]->stream_params.usb_dps / app_model.s_models[i]->stream_params.bandwidth;
            SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("**** %s, odr: %f, DPS: %d, ms: %f, proposed FIFO WM: %d \r\n", descriptor.p_name,
                                               app_model.s_models[i]->sensor_status.type.mems.odr, app_model.s_models[i]->stream_params.usb_dps, ms, proposed_fifoWM));
#endif
          }

          app_model.s_models[i]->stream_params.stream_id = st_i;
          st_i++;

          if (i > 0)
          {
            j = i - 1;
            while (j >= 0)
            {
              if (app_model.s_models[j] != NULL)
              {
                if (app_model.s_models[j]->stream_params.bandwidth <= app_model.s_models[i]->stream_params.bandwidth)
                {
                  if (app_model.s_models[j]->stream_params.stream_id >= 0)
                  {
                    if (app_model.s_models[j]->stream_params.stream_id < app_model.s_models[i]->stream_params.stream_id)
                    {
                      app_model.s_models[i]->stream_params.stream_id = app_model.s_models[j]->stream_params.stream_id;
                    }
                    app_model.s_models[j]->stream_params.stream_id++;
                  }
                }
              }
              j--;
            }
          }
        }
        else
        {
          app_model.s_models[i]->stream_params.bandwidth = 0;
          app_model.s_models[i]->stream_params.stream_id = -1;
        }
      }
    }
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      if (app_model.s_models[i] != NULL)
      {
        int8_t stream = app_model.s_models[i]->stream_params.stream_id;
        if (stream < N_MAX_EP - 1)
        {
          app_model.s_models[i]->stream_params.usb_ep = stream;
        }
        else
        {
          app_model.s_models[i]->stream_params.usb_ep = N_MAX_EP - 1;
        }
      }
    }
  }
  else
  {
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      if (app_model.s_models[i] != NULL)
      {
        app_model.s_models[i]->stream_params.stream_id = -1;
        app_model.s_models[i]->stream_params.usb_ep = -1;
        app_model.s_models[i]->stream_params.bandwidth = 0;
      }
    }
  }
  return 0;
}

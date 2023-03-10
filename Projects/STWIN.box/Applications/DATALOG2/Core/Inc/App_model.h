/**
  ******************************************************************************
  * @file    App_model.h
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
  * dtmi:appconfig:steval_stwinbx1:fp_sns_datalog2;1
  * dtmi:appconfig:steval_stwinbx1:fp_sns_datalog2_ispu;1
  *
  * Created by: DTDL2PnPL_cGen version 1.0.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

/**
  ******************************************************************************
  * Component APIs *************************************************************
  * - Component init function
  *    <comp_name>_comp_init(void)
  * - Component get_key function
  *    <comp_name>_get_key(void)
  * - Component GET/SET Properties APIs ****************************************
  *  - GET Functions
  *    uint8_t <comp_name>_get_<prop_name>(prop_type *value)
  *      if prop_type == char --> (char **value)
  *  - SET Functions
  *    uint8_t <comp_name>_set_<prop_name>(prop_type value)
  *      if prop_type == char --> (char *value)
  *  - Component COMMAND Reaction Functions
  *      uint8_t <comp_name>_<command_name>(I<Compname>_t * ifn,
  *                     field1_type field1_name, field2_type field2_name, ...,
  *                     fieldN_type fieldN_name); //ifn: Interface Functions
  *  - Component TELEMETRY Send Functions
  *      uint8_t <comp_name>_create_telemetry(tel1_type tel1_name,
  *                     tel2_type tel2_name, ..., telN_type telN_name,
  *                     char **telemetry, uint32_t *size)
  ******************************************************************************
  */

#ifndef APP_MODEL_H_
#define APP_MODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "PnPLCompManager.h"
#include "SensorManager.h"
#include "IIsm330is_Ispu.h"
#include "IIsm330is_Ispu_vtbl.h"
#include "IIsm330dhcx_Mlc.h"
#include "IIsm330dhcx_Mlc_vtbl.h"
#include "ILog_Controller.h"
#include "ILog_Controller_vtbl.h"
#include "parson.h"
/* USER includes -------------------------------------------------------------*/
#include "TagManager.h"

#define COMP_TYPE_SENSOR          0x00
#define COMP_TYPE_ALGORITHM       0x01
#define COMP_TYPE_OTHER           0x02

#define LOG_CTRL_MODE_SD          0x00
#define LOG_CTRL_MODE_USB         0x01
#define LOG_CTRL_MODE_BLE         0x02

#define SENSOR_NUMBER             14 //TODO create a define SM_MAX_SENSORS = 11
#define ALGORITHM_NUMBER	        0
#define OTHER_COMP_NUMBER	        5

#define DEVICE_ALIAS_LENGTH       16U

#define HSD_ACQ_NAME_LENGTH                 64U
#define HSD_ACQ_DESC_LENGTH                100U
#define HSD_ACQ_INTERNAL_TIMESTAMP_LENGTH   18U
#define HSD_ACQ_TIMESTAMP_LENGTH            25U

#define N_MAX_EP 5

#define FW_VERSION_MAJOR  '1'
#define FW_VERSION_MINOR  '0'
#define FW_VERSION_PATCH  '1'

typedef struct _StreamParams_t
{
  int8_t stream_id;
  int8_t usb_ep;
  uint16_t spts;
  uint32_t usb_dps;
  uint32_t sd_dps;
  float ioffset;
  float bandwidth;
  /* Stream Parameters Model USER code */
} StreamParams_t;

typedef struct _SensorModel_t
{
  /* E.g. IIS3DWB Component is a sensor (look @ schema field)
     so, its model has the following structure */
  uint8_t id;
  char *comp_name;
  StreamParams_t streamParams;
  SensorStatus_t sensorStatus;
  /* Sensor Components Model USER code */
} SensorModel_t;

typedef struct _AutomodeModel_t
{
  char *comp_name;
  /* Automode Componnent Model USER code */
  bool enabled;
  int32_t nof_acquisitions;
  int32_t start_delay_ms;
  int32_t datalog_time_length;
  int32_t idle_time_length;
} AutomodeModel_t;

typedef struct _LogControllerModel_t
{
  char *comp_name;
  /* LogController Componnent Model USER code */
  bool status;
  bool sd_mounted;
  int8_t interface;
} LogControllerModel_t;

typedef struct _TagsInfoModel_t
{
  char *comp_name;
  /* TagsInfo Componnent Model USER code */
} TagsInfoModel_t;

typedef struct _AcquisitionInfoModel_t
{
  char *comp_name;
  /* AcquisitionInfo Componnent Model USER code */
  char name[HSD_ACQ_NAME_LENGTH];
  char description[HSD_ACQ_DESC_LENGTH];
  int8_t interface;
  char uuid[37]; // UUID: 8-4-4-4-12 = 36char + \0
  char start_time[HSD_ACQ_TIMESTAMP_LENGTH];// "YYYY-MM-DDTHH:MM:SS.mmmZ"
  char end_time[HSD_ACQ_TIMESTAMP_LENGTH];// "YYYY-MM-DDTHH:MM:SS.mmmZ"
} AcquisitionInfoModel_t;

typedef struct _FirmwareInfoModel_t
{
  char *comp_name;
  /* FirmwareInfo Componnent Model USER code */
  char alias[DEVICE_ALIAS_LENGTH];
} FirmwareInfoModel_t;

typedef struct _AppModel_t
{
  SensorModel_t *s_models[SENSOR_NUMBER];
  AutomodeModel_t automode_model;
  LogControllerModel_t log_controller_model;
  TagsInfoModel_t tags_info_model;
  AcquisitionInfoModel_t acquisition_info_model;
  FirmwareInfoModel_t firmware_info_model;
  /* Insert here your custom App Model code */
  bool mlc_ucf_valid;
  bool ispu_ucf_valid;
} AppModel_t;

AppModel_t *getAppModel(void);

/* Device Components APIs ----------------------------------------------------*/

/* ISM330IS_ACC PnPL Component -----------------------------------------------*/
uint8_t ism330is_acc_comp_init(void);
char* ism330is_acc_get_key(void);
uint8_t ism330is_acc_get_odr(float *value);
uint8_t ism330is_acc_get_fs(float *value);
uint8_t ism330is_acc_get_enable(bool *value);
uint8_t ism330is_acc_get_samples_per_ts__val(int32_t *value);
uint8_t ism330is_acc_get_samples_per_ts__min(int32_t *value);
uint8_t ism330is_acc_get_samples_per_ts__max(int32_t *value);
uint8_t ism330is_acc_get_dim(int32_t *value);
uint8_t ism330is_acc_get_ioffset(float *value);
uint8_t ism330is_acc_get_measodr(float *value);
uint8_t ism330is_acc_get_usb_dps(int32_t *value);
uint8_t ism330is_acc_get_sd_dps(int32_t *value);
uint8_t ism330is_acc_get_sensitivity(float *value);
uint8_t ism330is_acc_get_data_type(char **value);
uint8_t ism330is_acc_get_stream_id(int8_t *value);
uint8_t ism330is_acc_get_ep_id(int8_t *value);
uint8_t ism330is_acc_set_odr(float value);
uint8_t ism330is_acc_set_fs(float value);
uint8_t ism330is_acc_set_enable(bool value);
uint8_t ism330is_acc_set_samples_per_ts__val(int32_t value);

/* ISM330IS_GYRO PnPL Component ----------------------------------------------*/
uint8_t ism330is_gyro_comp_init(void);
char* ism330is_gyro_get_key(void);
uint8_t ism330is_gyro_get_odr(float *value);
uint8_t ism330is_gyro_get_fs(float *value);
uint8_t ism330is_gyro_get_enable(bool *value);
uint8_t ism330is_gyro_get_samples_per_ts__val(int32_t *value);
uint8_t ism330is_gyro_get_samples_per_ts__min(int32_t *value);
uint8_t ism330is_gyro_get_samples_per_ts__max(int32_t *value);
uint8_t ism330is_gyro_get_dim(int32_t *value);
uint8_t ism330is_gyro_get_ioffset(float *value);
uint8_t ism330is_gyro_get_measodr(float *value);
uint8_t ism330is_gyro_get_usb_dps(int32_t *value);
uint8_t ism330is_gyro_get_sd_dps(int32_t *value);
uint8_t ism330is_gyro_get_sensitivity(float *value);
uint8_t ism330is_gyro_get_data_type(char **value);
uint8_t ism330is_gyro_get_stream_id(int8_t *value);
uint8_t ism330is_gyro_get_ep_id(int8_t *value);
uint8_t ism330is_gyro_set_odr(float value);
uint8_t ism330is_gyro_set_fs(float value);
uint8_t ism330is_gyro_set_enable(bool value);
uint8_t ism330is_gyro_set_samples_per_ts__val(int32_t value);

/* ISM330IS_ISPU PnPL Component ----------------------------------------------*/
uint8_t ism330is_ispu_comp_init(void);
char* ism330is_ispu_get_key(void);
uint8_t ism330is_ispu_get_enable(bool *value);
uint8_t ism330is_ispu_get_samples_per_ts__val(int32_t *value);
uint8_t ism330is_ispu_get_samples_per_ts__min(int32_t *value);
uint8_t ism330is_ispu_get_samples_per_ts__max(int32_t *value);
uint8_t ism330is_ispu_get_ucf_status(bool *value);
uint8_t ism330is_ispu_get_usb_dps(float *value);
uint8_t ism330is_ispu_get_sd_dps(float *value);
uint8_t ism330is_ispu_get_data_type(char **value);
uint8_t ism330is_ispu_get_dim(int32_t *value);
uint8_t ism330is_ispu_get_ioffset(float *value);
uint8_t ism330is_ispu_get_stream_id(int8_t *value);
uint8_t ism330is_ispu_get_ep_id(int8_t *value);
uint8_t ism330is_ispu_set_enable(bool value);
uint8_t ism330is_ispu_set_samples_per_ts__val(int32_t value);
uint8_t ism330is_ispu_load_file(IIsm330is_Ispu_t *ifn, const char *ucf_data, int32_t ucf_size, const char *output_data, int32_t output_size);

/* IIS3DWB_ACC PnPL Component ------------------------------------------------*/
uint8_t iis3dwb_acc_comp_init(void);
char* iis3dwb_acc_get_key(void);
uint8_t iis3dwb_acc_get_odr(float *value);
uint8_t iis3dwb_acc_get_fs(float *value);
uint8_t iis3dwb_acc_get_enable(bool *value);
uint8_t iis3dwb_acc_get_samples_per_ts__val(int32_t *value);
uint8_t iis3dwb_acc_get_samples_per_ts__min(int32_t *value);
uint8_t iis3dwb_acc_get_samples_per_ts__max(int32_t *value);
uint8_t iis3dwb_acc_get_dim(int32_t *value);
uint8_t iis3dwb_acc_get_ioffset(float *value);
uint8_t iis3dwb_acc_get_measodr(float *value);
uint8_t iis3dwb_acc_get_usb_dps(int32_t *value);
uint8_t iis3dwb_acc_get_sd_dps(int32_t *value);
uint8_t iis3dwb_acc_get_sensitivity(float *value);
uint8_t iis3dwb_acc_get_data_type(char **value);
uint8_t iis3dwb_acc_get_stream_id(int8_t *value);
uint8_t iis3dwb_acc_get_ep_id(int8_t *value);
uint8_t iis3dwb_acc_set_fs(float value);
uint8_t iis3dwb_acc_set_enable(bool value);
uint8_t iis3dwb_acc_set_samples_per_ts__val(int32_t value);

/* IIS2MDC_MAG PnPL Component ------------------------------------------------*/
uint8_t iis2mdc_mag_comp_init(void);
char* iis2mdc_mag_get_key(void);
uint8_t iis2mdc_mag_get_odr(float *value);
uint8_t iis2mdc_mag_get_fs(float *value);
uint8_t iis2mdc_mag_get_enable(bool *value);
uint8_t iis2mdc_mag_get_samples_per_ts__val(int32_t *value);
uint8_t iis2mdc_mag_get_samples_per_ts__min(int32_t *value);
uint8_t iis2mdc_mag_get_samples_per_ts__max(int32_t *value);
uint8_t iis2mdc_mag_get_dim(int32_t *value);
uint8_t iis2mdc_mag_get_ioffset(float *value);
uint8_t iis2mdc_mag_get_measodr(float *value);
uint8_t iis2mdc_mag_get_usb_dps(int32_t *value);
uint8_t iis2mdc_mag_get_sd_dps(int32_t *value);
uint8_t iis2mdc_mag_get_sensitivity(float *value);
uint8_t iis2mdc_mag_get_data_type(char **value);
uint8_t iis2mdc_mag_get_stream_id(int8_t *value);
uint8_t iis2mdc_mag_get_ep_id(int8_t *value);
uint8_t iis2mdc_mag_set_odr(float value);
uint8_t iis2mdc_mag_set_enable(bool value);
uint8_t iis2mdc_mag_set_samples_per_ts__val(int32_t value);

/* IMP23ABSU_MIC PnPL Component ----------------------------------------------*/
uint8_t imp23absu_mic_comp_init(void);
char* imp23absu_mic_get_key(void);
uint8_t imp23absu_mic_get_odr(float *value);
uint8_t imp23absu_mic_get_aop(float *value);
uint8_t imp23absu_mic_get_enable(bool *value);
uint8_t imp23absu_mic_get_samples_per_ts__val(int32_t *value);
uint8_t imp23absu_mic_get_samples_per_ts__min(int32_t *value);
uint8_t imp23absu_mic_get_samples_per_ts__max(int32_t *value);
uint8_t imp23absu_mic_get_dim(int32_t *value);
uint8_t imp23absu_mic_get_ioffset(float *value);
uint8_t imp23absu_mic_get_measodr(float *value);
uint8_t imp23absu_mic_get_usb_dps(int32_t *value);
uint8_t imp23absu_mic_get_sd_dps(int32_t *value);
uint8_t imp23absu_mic_get_sensitivity(float *value);
uint8_t imp23absu_mic_get_data_type(char **value);
uint8_t imp23absu_mic_get_stream_id(int8_t *value);
uint8_t imp23absu_mic_get_ep_id(int8_t *value);
uint8_t imp23absu_mic_set_odr(float value);
uint8_t imp23absu_mic_set_enable(bool value);
uint8_t imp23absu_mic_set_samples_per_ts__val(int32_t value);

/* ISM330DHCX_ACC PnPL Component ---------------------------------------------*/
uint8_t ism330dhcx_acc_comp_init(void);
char* ism330dhcx_acc_get_key(void);
uint8_t ism330dhcx_acc_get_odr(float *value);
uint8_t ism330dhcx_acc_get_fs(float *value);
uint8_t ism330dhcx_acc_get_enable(bool *value);
uint8_t ism330dhcx_acc_get_samples_per_ts__val(int32_t *value);
uint8_t ism330dhcx_acc_get_samples_per_ts__min(int32_t *value);
uint8_t ism330dhcx_acc_get_samples_per_ts__max(int32_t *value);
uint8_t ism330dhcx_acc_get_dim(int32_t *value);
uint8_t ism330dhcx_acc_get_ioffset(float *value);
uint8_t ism330dhcx_acc_get_measodr(float *value);
uint8_t ism330dhcx_acc_get_usb_dps(int32_t *value);
uint8_t ism330dhcx_acc_get_sd_dps(int32_t *value);
uint8_t ism330dhcx_acc_get_sensitivity(float *value);
uint8_t ism330dhcx_acc_get_data_type(char **value);
uint8_t ism330dhcx_acc_get_stream_id(int8_t *value);
uint8_t ism330dhcx_acc_get_ep_id(int8_t *value);
uint8_t ism330dhcx_acc_set_odr(float value);
uint8_t ism330dhcx_acc_set_fs(float value);
uint8_t ism330dhcx_acc_set_enable(bool value);
uint8_t ism330dhcx_acc_set_samples_per_ts__val(int32_t value);

/* ISM330DHCX_GYRO PnPL Component --------------------------------------------*/
uint8_t ism330dhcx_gyro_comp_init(void);
char* ism330dhcx_gyro_get_key(void);
uint8_t ism330dhcx_gyro_get_odr(float *value);
uint8_t ism330dhcx_gyro_get_fs(float *value);
uint8_t ism330dhcx_gyro_get_enable(bool *value);
uint8_t ism330dhcx_gyro_get_samples_per_ts__val(int32_t *value);
uint8_t ism330dhcx_gyro_get_samples_per_ts__min(int32_t *value);
uint8_t ism330dhcx_gyro_get_samples_per_ts__max(int32_t *value);
uint8_t ism330dhcx_gyro_get_dim(int32_t *value);
uint8_t ism330dhcx_gyro_get_ioffset(float *value);
uint8_t ism330dhcx_gyro_get_measodr(float *value);
uint8_t ism330dhcx_gyro_get_usb_dps(int32_t *value);
uint8_t ism330dhcx_gyro_get_sd_dps(int32_t *value);
uint8_t ism330dhcx_gyro_get_sensitivity(float *value);
uint8_t ism330dhcx_gyro_get_data_type(char **value);
uint8_t ism330dhcx_gyro_get_stream_id(int8_t *value);
uint8_t ism330dhcx_gyro_get_ep_id(int8_t *value);
uint8_t ism330dhcx_gyro_set_odr(float value);
uint8_t ism330dhcx_gyro_set_fs(float value);
uint8_t ism330dhcx_gyro_set_enable(bool value);
uint8_t ism330dhcx_gyro_set_samples_per_ts__val(int32_t value);

/* ISM330DHCX_MLC PnPL Component ---------------------------------------------*/
uint8_t ism330dhcx_mlc_comp_init(void);
char* ism330dhcx_mlc_get_key(void);
uint8_t ism330dhcx_mlc_get_enable(bool *value);
uint8_t ism330dhcx_mlc_get_samples_per_ts__val(int32_t *value);
uint8_t ism330dhcx_mlc_get_samples_per_ts__min(int32_t *value);
uint8_t ism330dhcx_mlc_get_samples_per_ts__max(int32_t *value);
uint8_t ism330dhcx_mlc_get_ucf_status(bool *value);
uint8_t ism330dhcx_mlc_get_dim(int32_t *value);
uint8_t ism330dhcx_mlc_get_ioffset(float *value);
uint8_t ism330dhcx_mlc_get_data_type(char **value);
uint8_t ism330dhcx_mlc_get_usb_dps(int32_t *value);
uint8_t ism330dhcx_mlc_get_sd_dps(int32_t *value);
uint8_t ism330dhcx_mlc_get_stream_id(int8_t *value);
uint8_t ism330dhcx_mlc_get_ep_id(int8_t *value);
uint8_t ism330dhcx_mlc_set_enable(bool value);
uint8_t ism330dhcx_mlc_set_samples_per_ts__val(int32_t value);
uint8_t ism330dhcx_mlc_load_file(IIsm330dhcx_Mlc_t *ifn, const char *ucf_data, int32_t ucf_size);

/* IIS2DLPC_ACC PnPL Component -----------------------------------------------*/
uint8_t iis2dlpc_acc_comp_init(void);
char* iis2dlpc_acc_get_key(void);
uint8_t iis2dlpc_acc_get_odr(float *value);
uint8_t iis2dlpc_acc_get_fs(float *value);
uint8_t iis2dlpc_acc_get_enable(bool *value);
uint8_t iis2dlpc_acc_get_samples_per_ts__val(int32_t *value);
uint8_t iis2dlpc_acc_get_samples_per_ts__min(int32_t *value);
uint8_t iis2dlpc_acc_get_samples_per_ts__max(int32_t *value);
uint8_t iis2dlpc_acc_get_dim(int32_t *value);
uint8_t iis2dlpc_acc_get_ioffset(float *value);
uint8_t iis2dlpc_acc_get_measodr(float *value);
uint8_t iis2dlpc_acc_get_usb_dps(int32_t *value);
uint8_t iis2dlpc_acc_get_sd_dps(int32_t *value);
uint8_t iis2dlpc_acc_get_sensitivity(float *value);
uint8_t iis2dlpc_acc_get_data_type(char **value);
uint8_t iis2dlpc_acc_get_stream_id(int8_t *value);
uint8_t iis2dlpc_acc_get_ep_id(int8_t *value);
uint8_t iis2dlpc_acc_set_odr(float value);
uint8_t iis2dlpc_acc_set_fs(float value);
uint8_t iis2dlpc_acc_set_enable(bool value);
uint8_t iis2dlpc_acc_set_samples_per_ts__val(int32_t value);

/* STTS22H_TEMP PnPL Component -----------------------------------------------*/
uint8_t stts22h_temp_comp_init(void);
char* stts22h_temp_get_key(void);
uint8_t stts22h_temp_get_odr(float *value);
uint8_t stts22h_temp_get_fs(float *value);
uint8_t stts22h_temp_get_enable(bool *value);
uint8_t stts22h_temp_get_samples_per_ts__val(int32_t *value);
uint8_t stts22h_temp_get_samples_per_ts__min(int32_t *value);
uint8_t stts22h_temp_get_samples_per_ts__max(int32_t *value);
uint8_t stts22h_temp_get_dim(int32_t *value);
uint8_t stts22h_temp_get_ioffset(float *value);
uint8_t stts22h_temp_get_measodr(float *value);
uint8_t stts22h_temp_get_usb_dps(int32_t *value);
uint8_t stts22h_temp_get_sd_dps(int32_t *value);
uint8_t stts22h_temp_get_sensitivity(float *value);
uint8_t stts22h_temp_get_data_type(char **value);
uint8_t stts22h_temp_get_stream_id(int8_t *value);
uint8_t stts22h_temp_get_ep_id(int8_t *value);
uint8_t stts22h_temp_set_odr(float value);
uint8_t stts22h_temp_set_enable(bool value);
uint8_t stts22h_temp_set_samples_per_ts__val(int32_t value);

/* ILPS22QS_PRESS PnPL Component ---------------------------------------------*/
uint8_t ilps22qs_press_comp_init(void);
char* ilps22qs_press_get_key(void);
uint8_t ilps22qs_press_get_odr(float *value);
uint8_t ilps22qs_press_get_fs(float *value);
uint8_t ilps22qs_press_get_enable(bool *value);
uint8_t ilps22qs_press_get_samples_per_ts__val(int32_t *value);
uint8_t ilps22qs_press_get_samples_per_ts__min(int32_t *value);
uint8_t ilps22qs_press_get_samples_per_ts__max(int32_t *value);
uint8_t ilps22qs_press_get_dim(int32_t *value);
uint8_t ilps22qs_press_get_ioffset(float *value);
uint8_t ilps22qs_press_get_measodr(float *value);
uint8_t ilps22qs_press_get_usb_dps(int32_t *value);
uint8_t ilps22qs_press_get_sd_dps(int32_t *value);
uint8_t ilps22qs_press_get_sensitivity(float *value);
uint8_t ilps22qs_press_get_data_type(char **value);
uint8_t ilps22qs_press_get_stream_id(int8_t *value);
uint8_t ilps22qs_press_get_ep_id(int8_t *value);
uint8_t ilps22qs_press_set_odr(float value);
uint8_t ilps22qs_press_set_fs(float value);
uint8_t ilps22qs_press_set_enable(bool value);
uint8_t ilps22qs_press_set_samples_per_ts__val(int32_t value);

/* IMP34DT05_MIC PnPL Component ----------------------------------------------*/
uint8_t imp34dt05_mic_comp_init(void);
char* imp34dt05_mic_get_key(void);
uint8_t imp34dt05_mic_get_odr(float *value);
uint8_t imp34dt05_mic_get_aop(float *value);
uint8_t imp34dt05_mic_get_enable(bool *value);
uint8_t imp34dt05_mic_get_samples_per_ts__val(int32_t *value);
uint8_t imp34dt05_mic_get_samples_per_ts__min(int32_t *value);
uint8_t imp34dt05_mic_get_samples_per_ts__max(int32_t *value);
uint8_t imp34dt05_mic_get_dim(int32_t *value);
uint8_t imp34dt05_mic_get_ioffset(float *value);
uint8_t imp34dt05_mic_get_measodr(float *value);
uint8_t imp34dt05_mic_get_usb_dps(int32_t *value);
uint8_t imp34dt05_mic_get_sd_dps(int32_t *value);
uint8_t imp34dt05_mic_get_sensitivity(float *value);
uint8_t imp34dt05_mic_get_data_type(char **value);
uint8_t imp34dt05_mic_get_stream_id(int8_t *value);
uint8_t imp34dt05_mic_get_ep_id(int8_t *value);
uint8_t imp34dt05_mic_set_odr(float value);
uint8_t imp34dt05_mic_set_enable(bool value);
uint8_t imp34dt05_mic_set_samples_per_ts__val(int32_t value);

/* IIS2ICLX_ACC PnPL Component -----------------------------------------------*/
uint8_t iis2iclx_acc_comp_init(void);
char* iis2iclx_acc_get_key(void);
uint8_t iis2iclx_acc_get_odr(float *value);
uint8_t iis2iclx_acc_get_fs(float *value);
uint8_t iis2iclx_acc_get_enable(bool *value);
uint8_t iis2iclx_acc_get_samples_per_ts__val(int32_t *value);
uint8_t iis2iclx_acc_get_samples_per_ts__min(int32_t *value);
uint8_t iis2iclx_acc_get_samples_per_ts__max(int32_t *value);
uint8_t iis2iclx_acc_get_dim(int32_t *value);
uint8_t iis2iclx_acc_get_ioffset(float *value);
uint8_t iis2iclx_acc_get_measodr(float *value);
uint8_t iis2iclx_acc_get_usb_dps(int32_t *value);
uint8_t iis2iclx_acc_get_sd_dps(int32_t *value);
uint8_t iis2iclx_acc_get_sensitivity(float *value);
uint8_t iis2iclx_acc_get_data_type(char **value);
uint8_t iis2iclx_acc_get_stream_id(int8_t *value);
uint8_t iis2iclx_acc_get_ep_id(int8_t *value);
uint8_t iis2iclx_acc_set_odr(float value);
uint8_t iis2iclx_acc_set_fs(float value);
uint8_t iis2iclx_acc_set_enable(bool value);
uint8_t iis2iclx_acc_set_samples_per_ts__val(int32_t value);

/* AutoMode PnPL Component ---------------------------------------------------*/
uint8_t automode_comp_init(void);
char* automode_get_key(void);
uint8_t automode_get_enabled(bool *value);
uint8_t automode_get_nof_acquisitions(int32_t *value);
uint8_t automode_get_start_delay_ms(int32_t *value);
uint8_t automode_get_datalog_time_length(int32_t *value);
uint8_t automode_get_idle_time_length(int32_t *value);
uint8_t automode_set_enabled(bool value);
uint8_t automode_set_nof_acquisitions(int32_t value);
uint8_t automode_set_start_delay_ms(int32_t value);
uint8_t automode_set_datalog_time_length(int32_t value);
uint8_t automode_set_idle_time_length(int32_t value);

/* Log Controller PnPL Component ---------------------------------------------*/
uint8_t log_controller_comp_init(void);
char* log_controller_get_key(void);
uint8_t log_controller_get_log_status(bool *value);
uint8_t log_controller_get_sd_mounted(bool *value);
uint8_t log_controller_get_controller_type(int32_t *value);
uint8_t log_controller_save_config(ILog_Controller_t *ifn);
uint8_t log_controller_start_log(ILog_Controller_t *ifn, int32_t interface);
uint8_t log_controller_stop_log(ILog_Controller_t *ifn);
uint8_t log_controller_set_time(ILog_Controller_t *ifn, const char *datetime);

/* Tags Information PnPL Component -------------------------------------------*/
uint8_t tags_info_comp_init(void);
char* tags_info_get_key(void);
uint8_t tags_info_get_max_tags_num(int32_t *value);
uint8_t tags_info_get_sw_tag0__label(char **value);
uint8_t tags_info_get_sw_tag0__enabled(bool *value);
uint8_t tags_info_get_sw_tag0__status(bool *value);
uint8_t tags_info_get_sw_tag1__label(char **value);
uint8_t tags_info_get_sw_tag1__enabled(bool *value);
uint8_t tags_info_get_sw_tag1__status(bool *value);
uint8_t tags_info_get_sw_tag2__label(char **value);
uint8_t tags_info_get_sw_tag2__enabled(bool *value);
uint8_t tags_info_get_sw_tag2__status(bool *value);
uint8_t tags_info_get_sw_tag3__label(char **value);
uint8_t tags_info_get_sw_tag3__enabled(bool *value);
uint8_t tags_info_get_sw_tag3__status(bool *value);
uint8_t tags_info_get_sw_tag4__label(char **value);
uint8_t tags_info_get_sw_tag4__enabled(bool *value);
uint8_t tags_info_get_sw_tag4__status(bool *value);
uint8_t tags_info_set_sw_tag0__label(const char *value);
uint8_t tags_info_set_sw_tag0__enabled(bool value);
uint8_t tags_info_set_sw_tag0__status(bool value);
uint8_t tags_info_set_sw_tag1__label(const char *value);
uint8_t tags_info_set_sw_tag1__enabled(bool value);
uint8_t tags_info_set_sw_tag1__status(bool value);
uint8_t tags_info_set_sw_tag2__label(const char *value);
uint8_t tags_info_set_sw_tag2__enabled(bool value);
uint8_t tags_info_set_sw_tag2__status(bool value);
uint8_t tags_info_set_sw_tag3__label(const char *value);
uint8_t tags_info_set_sw_tag3__enabled(bool value);
uint8_t tags_info_set_sw_tag3__status(bool value);
uint8_t tags_info_set_sw_tag4__label(const char *value);
uint8_t tags_info_set_sw_tag4__enabled(bool value);
uint8_t tags_info_set_sw_tag4__status(bool value);

/* Acquisition Information PnPL Component ------------------------------------*/
uint8_t acquisition_info_comp_init(void);
char* acquisition_info_get_key(void);
uint8_t acquisition_info_get_name(char **value);
uint8_t acquisition_info_get_description(char **value);
uint8_t acquisition_info_get_uuid(char **value);
uint8_t acquisition_info_get_start_time(char **value);
uint8_t acquisition_info_get_end_time(char **value);
uint8_t acquisition_info_get_data_ext(char **value);
uint8_t acquisition_info_get_data_fmt(char **value);
uint8_t acquisition_info_get_tags(JSON_Value *value);
uint8_t acquisition_info_get_interface(char **value);
uint8_t acquisition_info_get_schema_version(char **value);
uint8_t acquisition_info_set_name(const char *value);
uint8_t acquisition_info_set_description(const char *value);

/* Firmware Information PnPL Component ---------------------------------------*/
uint8_t firmware_info_comp_init(void);
char* firmware_info_get_key(void);
uint8_t firmware_info_get_alias(char **value);
uint8_t firmware_info_get_fw_name(char **value);
uint8_t firmware_info_get_fw_version(char **value);
uint8_t firmware_info_get_serial_number(char **value);
uint8_t firmware_info_get_device_url(char **value);
uint8_t firmware_info_get_fw_url(char **value);
uint8_t firmware_info_set_alias(const char *value);

/* Device Information PnPL Component -----------------------------------------*/
uint8_t DeviceInformation_comp_init(void);
char* DeviceInformation_get_key(void);
uint8_t DeviceInformation_get_manufacturer(char **value);
uint8_t DeviceInformation_get_model(char **value);
uint8_t DeviceInformation_get_swVersion(char **value);
uint8_t DeviceInformation_get_osName(char **value);
uint8_t DeviceInformation_get_processorArchitecture(char **value);
uint8_t DeviceInformation_get_processorManufacturer(char **value);
uint8_t DeviceInformation_get_totalStorage(float *value);
uint8_t DeviceInformation_get_totalMemory(float *value);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_H_ */

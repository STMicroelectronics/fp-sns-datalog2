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
  * dtmi:vespucci:steval_mkboxpro:FP_SNS_DATALOG2_Datalog2;6
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
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
  *      uint8_t <comp_name>_<command_name>(
  *                     field1_type field1_name, field2_type field2_name, ...,
  *                     fieldN_type fieldN_name);
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
#include "parson.h"
#include "App_model_Lis2du12_Acc.h"
#include "App_model_Lis2mdl_Mag.h"
#include "App_model_Lps22df_Press.h"
#include "App_model_Lsm6dsv16x_Acc.h"
#include "App_model_Lsm6dsv16x_Gyro.h"
#include "App_model_Lsm6dsv16x_Mlc.h"
#include "App_model_Mp23db01hp_Mic.h"
#include "App_model_Stts22h_Temp.h"
#include "App_model_H3lis331dl_Acc.h"
#include "App_model_Ilps28qsw_Press.h"
#include "App_model_Ism330is_Acc.h"
#include "App_model_Ism330is_Gyro.h"
#include "App_model_Ism330is_Ispu.h"
#include "App_model_Lsm6dsv16bx_Acc.h"
#include "App_model_Lsm6dsv16bx_Gyro.h"
#include "App_model_Lsm6dsv16bx_Mlc.h"
#include "App_model_Lsm6dsv32x_Acc.h"
#include "App_model_Lsm6dsv32x_Gyro.h"
#include "App_model_Lsm6dsv32x_Mlc.h"
#include "App_model_Deviceinformation.h"
#include "App_model_Automode.h"
#include "App_model_Log_Controller.h"
#include "App_model_Tags_Info.h"
#include "App_model_Acquisition_Info.h"
#include "App_model_Firmware_Info.h"
/* USER includes -------------------------------------------------------------*/

#define COMP_TYPE_SENSOR          0x00
#define COMP_TYPE_ALGORITHM       0x01
#define COMP_TYPE_OTHER           0x02
#define COMP_TYPE_ACTUATOR        0x03

#define LOG_CTRL_MODE_SD          0x00
#define LOG_CTRL_MODE_USB         0x01
#define LOG_CTRL_MODE_BLE         0x02

#define SENSOR_NUMBER             19
#define ALGORITHM_NUMBER          0
#define ACTUATOR_NUMBER           0
#define OTHER_COMP_NUMBER         5

#define N_MAX_EP            5

#define UNIT_LEN 20U
#define FORMAT_LEN 10U

#define FW_VERSION_MAJOR    "3"
#define FW_VERSION_MINOR    "0"
#define FW_VERSION_PATCH    "0"

/* Max BLE bandwidth for each sensor (in Byte) */
#define MAX_BLE_BANDWIDTH   4000
/* How often send BLE data in milliseconds */
#define BLE_SEND_PERIOD     50


typedef struct _StreamParams_t
{
  int8_t stream_id;
  int8_t usb_ep;
  uint16_t spts;
  uint32_t sd_dps;
  uint32_t usb_dps;
  uint32_t ble_dps;
  uint32_t serial_dps;
  float ioffset;
  float bandwidth;
  /* Stream Parameters Model USER code */
} StreamParams_t;

typedef struct _StBleStreamSensorObjectModel_t
{
  /* Component that describe the data format to be sent via BLE */
  bool status;
  char format[FORMAT_LEN];
  /* ST BLE Stream property Model USER code */
  char unit[UNIT_LEN];
  uint32_t elements;
  uint32_t channel;
  float multiply_factor;
  uint32_t odr;
} StBleStreamSensorObjectModel_t;

typedef struct _StBleStreamSensorModel_t
{
  /* Component that enable the data stream via BLE */
  int32_t st_ble_stream_id;
  StBleStreamSensorObjectModel_t st_ble_stream_objects;
} StBleStreamSensorModel_t;

#define SENSOR_NOTES_LEN 20U
typedef struct _SensorModel_t
{
  /* E.g. IIS3DWB Component is a sensor (look @ schema field)
  so, its model has the following structure */
  uint8_t id;
  char *comp_name;
  StreamParams_t stream_params;
  SensorStatus_t *sensor_status;
  char annotation[SENSOR_NOTES_LEN];
  /* Sensor Components Model USER code */
  uint16_t fifo_watermark;
  StBleStreamSensorModel_t st_ble_stream;
} SensorModel_t;

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
uint8_t addSensorToAppModel(uint16_t id, SensorModel_t *model);

uint8_t __stream_control(bool status);
sys_error_code_t __sc_set_ble_stream_params(uint32_t id);
uint8_t set_mac_address(const char *value);

#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_H_ */

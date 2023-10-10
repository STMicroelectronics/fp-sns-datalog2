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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_ultrasoundFft;3
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
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
#include "ILog_Controller.h"
#include "ILog_Controller_vtbl.h"
#include "parson.h"
/* USER includes -------------------------------------------------------------*/
#include "TagManager.h"

#define COMP_TYPE_SENSOR          0x00
#define COMP_TYPE_ALGORITHM       0x01
#define COMP_TYPE_OTHER           0x02
#define COMP_TYPE_ACTUATOR        0x03

#define LOG_CTRL_MODE_SD          0x00
#define LOG_CTRL_MODE_USB         0x01
#define LOG_CTRL_MODE_BLE         0x02

#define SENSOR_NUMBER             1
#define ALGORITHM_NUMBER          1
#define ACTUATOR_NUMBER           0
#define OTHER_COMP_NUMBER         3

#define DEVICE_ALIAS_LENGTH       16U

#define HSD_ACQ_NAME_LENGTH                 64U
#define HSD_ACQ_DESC_LENGTH                100U
#define HSD_ACQ_INTERNAL_TIMESTAMP_LENGTH   18U
#define HSD_ACQ_TIMESTAMP_LENGTH            25U

#define N_MAX_EP 5
#define SENSOR_NOTES_LEN 20U

#define FFT_DPU_STREAM_ID 2u
#define FFT_DPU_USB_EP  1u
#define FFT_DPU_USB_DPS 1024u
#define FFT_DPU_PACKECT_SIZE  0u
#define FFT_DPU_DIM 256
#define FFT_DPU_SAMPLE_FREQ 192000u
#define FFT_DPU_FFT_LEN 256u

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
  StreamParams_t stream_params;
  SensorStatus_t sensor_status;
  /* Sensor Components Model USER code */
  bool gui_plot_graph_enable;
  char annotation[SENSOR_NOTES_LEN];
} SensorModel_t;

typedef struct _AlgorithmModel_t
{
  /* E.g. FFT_Algo Component is an algorithm (look @ schema field)
     so, its model has the following structure */
  uint8_t id;
  char *comp_name;
  bool enable;
  StreamParams_t stream_params;
  /* Algorithm Components Model USER code */
  uint16_t packet_size;
  uint16_t dim;
  uint16_t fft_len;
  uint32_t fft_sample_freq;
} AlgorithmModel_t;

typedef struct _LogControllerModel_t
{
  char *comp_name;
  /* LogController Component Model USER code */
  bool status;
  bool sd_mounted;
  int8_t interface;
} LogControllerModel_t;

typedef struct _FirmwareInfoModel_t
{
  char *comp_name;
  /* FirmwareInfo Component Model USER code */
  char alias[DEVICE_ALIAS_LENGTH];
} FirmwareInfoModel_t;

typedef struct _AcquisitionInfoModel_t
{
  char *comp_name;
  /* AcquisitionInfo Component Model USER code */
  char name[HSD_ACQ_NAME_LENGTH];
  char description[HSD_ACQ_DESC_LENGTH];
  int8_t interface;
  char uuid[37]; /* UUID: 8-4-4-4-12 = 36char + \0 */
  char start_time[HSD_ACQ_TIMESTAMP_LENGTH]; /* "YYYY-MM-DDTHH:MM:SS.mmmZ" */
  char end_time[HSD_ACQ_TIMESTAMP_LENGTH]; /* "YYYY-MM-DDTHH:MM:SS.mmmZ" */
} AcquisitionInfoModel_t;

typedef struct _AppModel_t
{
  SensorModel_t *s_models[SENSOR_NUMBER];
  AlgorithmModel_t *a_models[ALGORITHM_NUMBER];
  LogControllerModel_t log_controller_model;
  FirmwareInfoModel_t firmware_info_model;
  AcquisitionInfoModel_t acquisition_info_model;
  /* Insert here your custom App Model code */
} AppModel_t;

AppModel_t *getAppModel(void);

/* Device Components APIs ----------------------------------------------------*/

/* Ultrasound FFT PnPL Component ---------------------------------------------*/
uint8_t fft_dpu_comp_init(void);
char *fft_dpu_get_key(void);
uint8_t fft_dpu_get_enable(bool *value);
uint8_t fft_dpu_get_data_type(char **value);
uint8_t fft_dpu_get_fft_length(int32_t *value);
uint8_t fft_dpu_get_fft_sample_freq(int32_t *value);
uint8_t fft_dpu_get_usb_dps(int32_t *value);
uint8_t fft_dpu_get_algorithm_type(int32_t *value);
uint8_t fft_dpu_get_stream_id(int8_t *value);
uint8_t fft_dpu_get_ep_id(int8_t *value);
uint8_t fft_dpu_set_enable(bool value);

/* IMP23ABSU_MIC PnPL Component ----------------------------------------------*/
uint8_t imp23absu_mic_comp_init(void);
char *imp23absu_mic_get_key(void);
uint8_t imp23absu_mic_get_odr(float *value);
uint8_t imp23absu_mic_get_aop(float *value);
uint8_t imp23absu_mic_get_enable(bool *value);
uint8_t imp23absu_mic_get_samples_per_ts(int32_t *value);
uint8_t imp23absu_mic_get_dim(int32_t *value);
uint8_t imp23absu_mic_get_ioffset(float *value);
uint8_t imp23absu_mic_get_measodr(float *value);
uint8_t imp23absu_mic_get_usb_dps(int32_t *value);
uint8_t imp23absu_mic_get_sd_dps(int32_t *value);
uint8_t imp23absu_mic_get_sensitivity(float *value);
uint8_t imp23absu_mic_get_data_type(char **value);
uint8_t imp23absu_mic_get_sensor_annotation(char **value);
uint8_t imp23absu_mic_get_sensor_category(int32_t *value);
uint8_t imp23absu_mic_get_stream_id(int8_t *value);
uint8_t imp23absu_mic_get_ep_id(int8_t *value);
uint8_t imp23absu_mic_set_enable(bool value);
uint8_t imp23absu_mic_set_sensor_annotation(const char *value);

/* Log Controller PnPL Component ---------------------------------------------*/
uint8_t log_controller_comp_init(void);
char *log_controller_get_key(void);
uint8_t log_controller_get_log_status(bool *value);
uint8_t log_controller_get_sd_mounted(bool *value);
uint8_t log_controller_get_controller_type(int32_t *value);
uint8_t log_controller_save_config(ILog_Controller_t *ifn);
uint8_t log_controller_start_log(ILog_Controller_t *ifn, int32_t interface);
uint8_t log_controller_stop_log(ILog_Controller_t *ifn);
uint8_t log_controller_set_time(ILog_Controller_t *ifn, const char *datetime);
uint8_t log_controller_switch_bank(ILog_Controller_t *ifn);

/* Firmware Information PnPL Component ---------------------------------------*/
uint8_t firmware_info_comp_init(void);
char *firmware_info_get_key(void);
uint8_t firmware_info_get_alias(char **value);
uint8_t firmware_info_get_fw_name(char **value);
uint8_t firmware_info_get_fw_version(char **value);
uint8_t firmware_info_get_part_number(char **value);
uint8_t firmware_info_get_device_url(char **value);
uint8_t firmware_info_get_fw_url(char **value);
uint8_t firmware_info_get_mac_address(char **value);
uint8_t firmware_info_set_alias(const char *value);

/* Device Information PnPL Component -----------------------------------------*/
uint8_t DeviceInformation_comp_init(void);
char *DeviceInformation_get_key(void);
uint8_t DeviceInformation_get_manufacturer(char **value);
uint8_t DeviceInformation_get_model(char **value);
uint8_t DeviceInformation_get_swVersion(char **value);
uint8_t DeviceInformation_get_osName(char **value);
uint8_t DeviceInformation_get_processorArchitecture(char **value);
uint8_t DeviceInformation_get_processorManufacturer(char **value);
uint8_t DeviceInformation_get_totalStorage(float *value);
uint8_t DeviceInformation_get_totalMemory(float *value);

/* Acquisition Information PnPL Component ------------------------------------*/
uint8_t acquisition_info_comp_init(void);
char *acquisition_info_get_key(void);
uint8_t acquisition_info_get_name(char **value);
uint8_t acquisition_info_get_description(char **value);
uint8_t acquisition_info_get_uuid(char **value);
uint8_t acquisition_info_get_start_time(char **value);
uint8_t acquisition_info_get_end_time(char **value);
uint8_t acquisition_info_get_data_ext(char **value);
uint8_t acquisition_info_get_tags(JSON_Value *value);
uint8_t acquisition_info_get_interface(char **value);
uint8_t acquisition_info_get_schema_version(char **value);
uint8_t acquisition_info_set_name(const char *value);
uint8_t acquisition_info_set_description(const char *value);

#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_H_ */

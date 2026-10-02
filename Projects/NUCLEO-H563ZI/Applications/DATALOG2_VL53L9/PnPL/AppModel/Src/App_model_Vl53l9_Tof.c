/**
  ******************************************************************************
  * @file    App_model_Vl53l9_Tof.c
  * @author  SRA
  * @brief   Vl53l9_Tof PnPL Components APIs
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
  * dtmi:appconfig:nucleo_h563zi:FP_SNS_DATALOG2_Datalog2_VL53L9:sensors:vl53l9cx_tof;1
  *
  * Created by: DTDL2PnPL_cGen version 3.1.0-alpha.2
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include "services/SQuery.h"

/* USER includes -------------------------------------------------------------*/

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* VL53L9_TOF PnPL Component -------------------------------------------------*/
static SensorModel_t vl53l9cx_tof_model;
extern AppModel_t app_model;

uint8_t vl53l9cx_tof_comp_init(void)
{
  vl53l9cx_tof_model.comp_name = vl53l9cx_tof_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "vl53l9", COM_TYPE_TOF);
  vl53l9cx_tof_model.id = id;
  vl53l9cx_tof_model.sensor_status = SMSensorGetStatusPointer(id);
  vl53l9cx_tof_model.stream_params.stream_id = -1;
  vl53l9cx_tof_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &vl53l9cx_tof_model);

  vl53l9cx_tof_set_sensor_annotation("\0", NULL);
  vl53l9cx_tof_set_resolution(pnpl_vl53l9cx_tof_resolution_n54x42, NULL);
  vl53l9cx_tof_set_odr(30, NULL);
  vl53l9cx_tof_set_ranging_mode(pnpl_vl53l9cx_tof_ranging_mode_manual, NULL);
  vl53l9cx_tof_set_integration_time(5, NULL);
  vl53l9cx_tof_set_power_mode(pnpl_vl53l9cx_tof_power_mode_regular, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  vl53l9cx_tof_set_samples_per_ts(0, NULL);
#else
  vl53l9cx_tof_set_samples_per_ts(30, NULL);
#endif /* HSD_USE_DUMMY_DATA */
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *vl53l9cx_tof_get_key(void)
{
  return "vl53l9cx_tof";
}


uint8_t vl53l9cx_tof_get_enable(bool *value)
{
  *value = vl53l9cx_tof_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_resolution(pnpl_vl53l9cx_tof_resolution_t *enum_id)
{
  /*#define VL53L9_RESOLUTION_4X4                    (16u)*/
  /*#define VL53L9_RESOLUTION_8X8                    (64u)*/
  /*#define VL53L9_RESOLUTION_12X10                  (120u)*/
  /*#define VL53L9_RESOLUTION_18X14                  (252u)*/
  /*#define VL53L9_RESOLUTION_24X24                  (576u)*/
  /*#define VL53L9_RESOLUTION_54X42                  (2268u)*/
  uint32_t resolution = vl53l9cx_tof_model.sensor_status->type.ranging.profile_config.ranging_profile;
  switch (resolution)
  {
    case 16:
      *enum_id = pnpl_vl53l9cx_tof_resolution_n4x4;
      break;
    case 64:
      *enum_id = pnpl_vl53l9cx_tof_resolution_n8x8;
      break;
    case 120:
      *enum_id = pnpl_vl53l9cx_tof_resolution_n12x10;
      break;
    case 252:
      *enum_id = pnpl_vl53l9cx_tof_resolution_n18x14;
      break;
    case 576:
      *enum_id = pnpl_vl53l9cx_tof_resolution_n24x24;
      break;
    case 2268:
      *enum_id = pnpl_vl53l9cx_tof_resolution_n54x42;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_odr(int32_t *value)
{
  *value = vl53l9cx_tof_model.sensor_status->type.ranging.profile_config.frequency;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_ranging_mode(pnpl_vl53l9cx_tof_ranging_mode_t *enum_id)
{
  /*  VL53L9_SYNC_SLAVE = 0U,*/
  /*  VL53L9_SYNC_MANUAL = 1U,*/
  /*  VL53L9_SYNC_AUTONOMOUS = 2U,*/
  uint32_t ranging_mode = vl53l9cx_tof_model.sensor_status->type.ranging.profile_config.mode;
  switch (ranging_mode)
  {
    case 0:
      *enum_id = pnpl_vl53l9cx_tof_ranging_mode_slave;
      break;
    case 1:
      *enum_id = pnpl_vl53l9cx_tof_ranging_mode_manual;
      break;
    case 2:
      *enum_id = pnpl_vl53l9cx_tof_ranging_mode_autonomous;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_integration_time(int32_t *value)
{
  *value = vl53l9cx_tof_model.sensor_status->type.ranging.profile_config.timing_budget;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_power_mode(pnpl_vl53l9cx_tof_power_mode_t *enum_id)
{
  uint32_t power_mode = vl53l9cx_tof_model.sensor_status->type.ranging.power_mode;
  switch (power_mode)
  {
    case 0:
      *enum_id = pnpl_vl53l9cx_tof_power_mode_regular;
      break;
    case 1:
      *enum_id = pnpl_vl53l9cx_tof_power_mode_ultra_low;
      break;
    default:
      return 1;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_samples_per_ts(int32_t *value)
{
  *value = vl53l9cx_tof_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_ioffset(float_t *value)
{
  *value = vl53l9cx_tof_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_usb_dps(int32_t *value)
{
  *value = vl53l9cx_tof_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_sd_dps(int32_t *value)
{
  *value = vl53l9cx_tof_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_data_type(char **value)
{
  *value = "uint8_t";
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_sensor_annotation(char **value)
{
  *value = vl53l9cx_tof_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_sensor_category(int32_t *value)
{
  *value = vl53l9cx_tof_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_dim(int32_t *value)
{
  uint32_t resolution = vl53l9cx_tof_model.sensor_status->type.ranging.profile_config.ranging_profile;

  #define VL53L9_STATUS_SIZE     (100U)

  /* raw frame size (including cropped pixels) */
  #define FRAME_SIZE_BINNING_2   (54U * 42U)
  #define FRAME_SIZE_BINNING_4   (24U * 24U) /* cropped: 24 * 20 */
  #define FRAME_SIZE_BINNING_6   (18U * 14U)
  #define FRAME_SIZE_BINNING_8   (12U * 10U)
  #define FRAME_SIZE_BINNING_12  (8U * 8U) /* cropped: 8 * 6 */
  #define FRAME_SIZE_BINNING_24  (4U * 4U)

  /* dss size */
  #define DSS_SIZE_BINNING_2     (1134U)
  #define DSS_SIZE_BINNING_4     (288U)
  #define DSS_SIZE_BINNING_6     (126U)
  #define DSS_SIZE_BINNING_8     (60U)
  #define DSS_SIZE_BINNING_12    (32U)
  #define DSS_SIZE_BINNING_24    (8U)

  /* raw buffer size (3 frames + dss + status) */
  #define RAW_BUFFER_SIZE(binning) ((FRAME_SIZE_BINNING_##binning * 3U * 2U) + DSS_SIZE_BINNING_##binning + VL53L9_STATUS_SIZE)

  switch (resolution)
  {
    case 16:
      *value = RAW_BUFFER_SIZE(24);
      break;
    case 64:
      *value = RAW_BUFFER_SIZE(12);
      break;
    case 120:
      *value = RAW_BUFFER_SIZE(8);
      break;
    case 252:
      *value = RAW_BUFFER_SIZE(6);
      break;
    case 576:
      *value = RAW_BUFFER_SIZE(4);
      break;
    case 2268:
      *value = RAW_BUFFER_SIZE(2);
      break;
    default:
      return 1;
  }

  #undef RAW_BUFFER_SIZE
  #undef DSS_SIZE_BINNING_24
  #undef DSS_SIZE_BINNING_12
  #undef DSS_SIZE_BINNING_8
  #undef DSS_SIZE_BINNING_6
  #undef DSS_SIZE_BINNING_4
  #undef DSS_SIZE_BINNING_2
  #undef FRAME_SIZE_BINNING_24
  #undef FRAME_SIZE_BINNING_12
  #undef FRAME_SIZE_BINNING_8
  #undef FRAME_SIZE_BINNING_6
  #undef FRAME_SIZE_BINNING_4
  #undef FRAME_SIZE_BINNING_2
  #undef VL53L9_STATUS_SIZE

  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_output_format(char **value)
{
  *value = "schema_vl53l9_v1";
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_stream_id(int8_t *value)
{
  *value = vl53l9cx_tof_model.stream_params.stream_id;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vl53l9cx_tof_get_ep_id(int8_t *value)
{
  *value = vl53l9cx_tof_model.stream_params.usb_ep;
  return PNPL_NO_ERROR_CODE;
}


uint8_t vl53l9cx_tof_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(vl53l9cx_tof_model.id);
  }
  else
  {
    ret = SMSensorDisable(vl53l9cx_tof_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t vl53l9cx_tof_set_resolution(pnpl_vl53l9cx_tof_resolution_t enum_id, char **response_message)
{
  /*resolution is managed through binning value*/
  /*#define FRAME_SIZE_BINNING_2  (54 * 42)*/
  /*#define FRAME_SIZE_BINNING_4  (24 * 24)*/
  /*#define FRAME_SIZE_BINNING_6  (18 * 14)*/
  /*#define FRAME_SIZE_BINNING_8  (12 * 10)*/
  /*#define FRAME_SIZE_BINNING_12 (8 * 8)*/
  /*#define FRAME_SIZE_BINNING_24 (4 * 4)*/
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint8_t value;
  switch (enum_id)
  {
    case pnpl_vl53l9cx_tof_resolution_n4x4:
      value = 24;
      break;
    case pnpl_vl53l9cx_tof_resolution_n8x8:
      value = 12;
      break;
    case pnpl_vl53l9cx_tof_resolution_n12x10:
      value = 8;
      break;
    case pnpl_vl53l9cx_tof_resolution_n18x14:
      value = 6;
      break;
    case pnpl_vl53l9cx_tof_resolution_n24x24:
      value = 4;
      break;
    case pnpl_vl53l9cx_tof_resolution_n54x42:
      value = 2;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetResolution(vl53l9cx_tof_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
#if (HSD_USE_DUMMY_DATA != 1)
    vl53l9cx_tof_set_samples_per_ts((int32_t)vl53l9cx_tof_model.sensor_status->type.ranging.profile_config.frequency,
                                    NULL);
#endif /* HSD_USE_DUMMY_DATA != 1 */
    __stream_control(true);
  }
  return ret;
}

uint8_t vl53l9cx_tof_set_odr(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  /* USER Code */

  int32_t min_v = 1;
  int32_t max_v = 100;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetFrequency(vl53l9cx_tof_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
#if (HSD_USE_DUMMY_DATA != 1)
      vl53l9cx_tof_set_samples_per_ts((int32_t)value, NULL);
#endif /* HSD_USE_DUMMY_DATA != 1 */
      __stream_control(true);
    }
  }
  else
  {
    ret = 1;
  }
  return ret;
}

uint8_t vl53l9cx_tof_set_ranging_mode(pnpl_vl53l9cx_tof_ranging_mode_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint32_t value;
  switch (enum_id)
  {
    case pnpl_vl53l9cx_tof_ranging_mode_slave:
      value = 0;
      break;
    case pnpl_vl53l9cx_tof_ranging_mode_manual:
      value = 1;
      break;
    case pnpl_vl53l9cx_tof_ranging_mode_autonomous:
      value = 2;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetRangingMode(vl53l9cx_tof_model.id, value);

  return ret;
}

uint8_t vl53l9cx_tof_set_integration_time(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  /* USER Code */

  int32_t min_v = 2;
  int32_t max_v = 30;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetIntegrationTime(vl53l9cx_tof_model.id, value);
  }
  else if (value > max_v)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vl53l9cx_tof_set_power_mode(pnpl_vl53l9cx_tof_power_mode_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint32_t value;
  switch (enum_id)
  {
    case pnpl_vl53l9cx_tof_power_mode_regular:
      value = 0;
      break;
    case pnpl_vl53l9cx_tof_power_mode_ultra_low:
      value = 1;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetPowerMode(vl53l9cx_tof_model.id, value);
  return ret;
}

uint8_t vl53l9cx_tof_set_samples_per_ts(int32_t value, char **response_message)
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
    vl53l9cx_tof_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    vl53l9cx_tof_model.stream_params.spts = max_v;
  }
  else
  {
    vl53l9cx_tof_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t vl53l9cx_tof_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(vl53l9cx_tof_model.annotation, value);
  return ret;
}

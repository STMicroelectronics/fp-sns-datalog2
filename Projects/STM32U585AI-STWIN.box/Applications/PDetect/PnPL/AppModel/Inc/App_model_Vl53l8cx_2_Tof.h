/**
  ******************************************************************************
  * @file    App_model_Vl53l8cx_2_Tof.h
  * @author  SRA
  * @brief   Vl53l8cx_2_Tof PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vl53l8cx_2_tof;2
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

#ifndef APP_MODEL_VL53L8CX_2_TOF_H_
#define APP_MODEL_VL53L8CX_2_TOF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Vl53l8cx_2_Tof_PnPL.h"

/* USER includes -------------------------------------------------------------*/

/* VL53L8CX_2_TOF PnPL Component ---------------------------------------------- */
uint8_t vl53l8cx_2_tof_comp_init(void);
char *vl53l8cx_2_tof_get_key(void);
uint8_t vl53l8cx_2_tof_get_enable(bool *value);
uint8_t vl53l8cx_2_tof_get_resolution(pnpl_vl53l8cx_2_tof_resolution_t *enum_id);
uint8_t vl53l8cx_2_tof_get_odr(int32_t *value);
uint8_t vl53l8cx_2_tof_get_ranging_mode(pnpl_vl53l8cx_2_tof_ranging_mode_t *enum_id);
uint8_t vl53l8cx_2_tof_get_integration_time(int32_t *value);
uint8_t vl53l8cx_2_tof_get_samples_per_ts(int32_t *value);
uint8_t vl53l8cx_2_tof_get_ioffset(float_t *value);
uint8_t vl53l8cx_2_tof_get_usb_dps(int32_t *value);
uint8_t vl53l8cx_2_tof_get_sd_dps(int32_t *value);
uint8_t vl53l8cx_2_tof_get_data_type(char **value);
uint8_t vl53l8cx_2_tof_get_sensor_annotation(char **value);
uint8_t vl53l8cx_2_tof_get_sensor_category(int32_t *value);
uint8_t vl53l8cx_2_tof_get_dim(int32_t *value);
uint8_t vl53l8cx_2_tof_get_output_format__target_status_start_id(int32_t *value);
uint8_t vl53l8cx_2_tof_get_output_format__target_distance_start_id(int32_t *value);
uint8_t vl53l8cx_2_tof_get_output_format__nof_outputs(int32_t *value);
uint8_t vl53l8cx_2_tof_get_mounted(bool *value);
uint8_t vl53l8cx_2_tof_get_stream_id(int8_t *value);
uint8_t vl53l8cx_2_tof_get_ep_id(int8_t *value);
uint8_t vl53l8cx_2_tof_set_enable(bool value, char **response_message);
uint8_t vl53l8cx_2_tof_set_resolution(pnpl_vl53l8cx_2_tof_resolution_t enum_id, char **response_message);
uint8_t vl53l8cx_2_tof_set_odr(int32_t value, char **response_message);
uint8_t vl53l8cx_2_tof_set_ranging_mode(pnpl_vl53l8cx_2_tof_ranging_mode_t enum_id, char **response_message);
uint8_t vl53l8cx_2_tof_set_integration_time(int32_t value, char **response_message);
uint8_t vl53l8cx_2_tof_set_samples_per_ts(int32_t value, char **response_message);
uint8_t vl53l8cx_2_tof_set_sensor_annotation(const char *value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_VL53L8CX_2_TOF_H_ */

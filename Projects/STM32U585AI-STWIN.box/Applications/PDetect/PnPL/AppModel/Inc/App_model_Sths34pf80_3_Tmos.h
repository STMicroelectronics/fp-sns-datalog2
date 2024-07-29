/**
  ******************************************************************************
  * @file    App_model_Sths34pf80_3_Tmos.h
  * @author  SRA
  * @brief   Sths34pf80_3_Tmos PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:sths34pf80_3_tmos;1
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

#ifndef APP_MODEL_STHS34PF80_3_TMOS_H_
#define APP_MODEL_STHS34PF80_3_TMOS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Sths34pf80_3_Tmos_PnPL.h"

/* USER includes -------------------------------------------------------------*/

/* STHS34PF80_3_TMOS PnPL Component ------------------------------------------- */
uint8_t sths34pf80_3_tmos_comp_init(void);
char *sths34pf80_3_tmos_get_key(void);
uint8_t sths34pf80_3_tmos_get_enable(bool *value);
uint8_t sths34pf80_3_tmos_get_odr(pnpl_sths34pf80_3_tmos_odr_t *enum_id);
uint8_t sths34pf80_3_tmos_get_transmittance(float *value);
uint8_t sths34pf80_3_tmos_get_avg_tobject_num(pnpl_sths34pf80_3_tmos_avg_tobject_num_t *enum_id);
uint8_t sths34pf80_3_tmos_get_avg_tambient_num(pnpl_sths34pf80_3_tmos_avg_tambient_num_t *enum_id);
uint8_t sths34pf80_3_tmos_get_lpf_p_m_bandwidth(pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_t *enum_id);
uint8_t sths34pf80_3_tmos_get_lpf_p_bandwidth(pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_t *enum_id);
uint8_t sths34pf80_3_tmos_get_lpf_m_bandwidth(pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_t *enum_id);
uint8_t sths34pf80_3_tmos_get_presence_threshold(int32_t *value);
uint8_t sths34pf80_3_tmos_get_presence_hysteresis(int32_t *value);
uint8_t sths34pf80_3_tmos_get_motion_threshold(int32_t *value);
uint8_t sths34pf80_3_tmos_get_motion_hysteresis(int32_t *value);
uint8_t sths34pf80_3_tmos_get_tambient_shock_threshold(int32_t *value);
uint8_t sths34pf80_3_tmos_get_tambient_shock_hysteresis(int32_t *value);
uint8_t sths34pf80_3_tmos_get_embedded_compensation(bool *value);
uint8_t sths34pf80_3_tmos_get_software_compensation(bool *value);
uint8_t sths34pf80_3_tmos_get_compensation_type(pnpl_sths34pf80_3_tmos_compensation_type_t *enum_id);
uint8_t sths34pf80_3_tmos_get_sw_presence_threshold(int32_t *value);
uint8_t sths34pf80_3_tmos_get_sw_motion_threshold(int32_t *value);
uint8_t sths34pf80_3_tmos_get_compensation_filter_flag(bool *value);
uint8_t sths34pf80_3_tmos_get_absence_static_flag(bool *value);
uint8_t sths34pf80_3_tmos_get_samples_per_ts(int32_t *value);
uint8_t sths34pf80_3_tmos_get_ioffset(float *value);
uint8_t sths34pf80_3_tmos_get_usb_dps(int32_t *value);
uint8_t sths34pf80_3_tmos_get_sd_dps(int32_t *value);
uint8_t sths34pf80_3_tmos_get_data_type(char **value);
uint8_t sths34pf80_3_tmos_get_sensor_annotation(char **value);
uint8_t sths34pf80_3_tmos_get_sensor_category(int32_t *value);
uint8_t sths34pf80_3_tmos_get_dim(int32_t *value);
uint8_t sths34pf80_3_tmos_get_stream_id(int8_t *value);
uint8_t sths34pf80_3_tmos_get_ep_id(int8_t *value);
uint8_t sths34pf80_3_tmos_set_enable(bool value, char **response_message);
uint8_t sths34pf80_3_tmos_set_odr(pnpl_sths34pf80_3_tmos_odr_t enum_id, char **response_message);
uint8_t sths34pf80_3_tmos_set_transmittance(float value, char **response_message);
uint8_t sths34pf80_3_tmos_set_avg_tobject_num(pnpl_sths34pf80_3_tmos_avg_tobject_num_t enum_id,
                                              char **response_message);
uint8_t sths34pf80_3_tmos_set_avg_tambient_num(pnpl_sths34pf80_3_tmos_avg_tambient_num_t enum_id,
                                               char **response_message);
uint8_t sths34pf80_3_tmos_set_lpf_p_m_bandwidth(pnpl_sths34pf80_3_tmos_lpf_p_m_bandwidth_t enum_id,
                                                char **response_message);
uint8_t sths34pf80_3_tmos_set_lpf_p_bandwidth(pnpl_sths34pf80_3_tmos_lpf_p_bandwidth_t enum_id,
                                              char **response_message);
uint8_t sths34pf80_3_tmos_set_lpf_m_bandwidth(pnpl_sths34pf80_3_tmos_lpf_m_bandwidth_t enum_id,
                                              char **response_message);
uint8_t sths34pf80_3_tmos_set_presence_threshold(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_presence_hysteresis(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_motion_threshold(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_motion_hysteresis(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_tambient_shock_threshold(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_tambient_shock_hysteresis(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_embedded_compensation(bool value, char **response_message);
uint8_t sths34pf80_3_tmos_set_software_compensation(bool value, char **response_message);
uint8_t sths34pf80_3_tmos_set_compensation_type(pnpl_sths34pf80_3_tmos_compensation_type_t enum_id,
                                                char **response_message);
uint8_t sths34pf80_3_tmos_set_sw_presence_threshold(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_sw_motion_threshold(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_compensation_filter_flag(bool value, char **response_message);
uint8_t sths34pf80_3_tmos_set_absence_static_flag(bool value, char **response_message);
uint8_t sths34pf80_3_tmos_set_samples_per_ts(int32_t value, char **response_message);
uint8_t sths34pf80_3_tmos_set_sensor_annotation(const char *value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_STHS34PF80_3_TMOS_H_ */

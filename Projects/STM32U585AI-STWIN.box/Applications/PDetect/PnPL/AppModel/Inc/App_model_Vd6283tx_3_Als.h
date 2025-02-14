/**
  ******************************************************************************
  * @file    App_model_Vd6283tx_3_Als.h
  * @author  SRA
  * @brief   Vd6283tx_3_Als PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vd6283tx_3_als;2
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

#ifndef APP_MODEL_VD6283TX_3_ALS_H_
#define APP_MODEL_VD6283TX_3_ALS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Vd6283tx_3_Als_PnPL.h"

/* USER includes -------------------------------------------------------------*/

/* VD6283TX_3_ALS PnPL Component ---------------------------------------------- */
uint8_t vd6283tx_3_als_comp_init(void);
char *vd6283tx_3_als_get_key(void);
uint8_t vd6283tx_3_als_get_enable(bool *value);
uint8_t vd6283tx_3_als_get_exposure_time(int32_t *value);
uint8_t vd6283tx_3_als_get_intermeasurement_time(int32_t *value);
uint8_t vd6283tx_3_als_get_channel1_gain(pnpl_vd6283tx_3_als_channel1_gain_t *enum_id);
uint8_t vd6283tx_3_als_get_channel2_gain(pnpl_vd6283tx_3_als_channel2_gain_t *enum_id);
uint8_t vd6283tx_3_als_get_channel3_gain(pnpl_vd6283tx_3_als_channel3_gain_t *enum_id);
uint8_t vd6283tx_3_als_get_channel4_gain(pnpl_vd6283tx_3_als_channel4_gain_t *enum_id);
uint8_t vd6283tx_3_als_get_channel5_gain(pnpl_vd6283tx_3_als_channel5_gain_t *enum_id);
uint8_t vd6283tx_3_als_get_channel6_gain(pnpl_vd6283tx_3_als_channel6_gain_t *enum_id);
uint8_t vd6283tx_3_als_get_samples_per_ts(int32_t *value);
uint8_t vd6283tx_3_als_get_ioffset(float *value);
uint8_t vd6283tx_3_als_get_usb_dps(int32_t *value);
uint8_t vd6283tx_3_als_get_sd_dps(int32_t *value);
uint8_t vd6283tx_3_als_get_data_type(char **value);
uint8_t vd6283tx_3_als_get_sensor_annotation(char **value);
uint8_t vd6283tx_3_als_get_sensor_category(int32_t *value);
uint8_t vd6283tx_3_als_get_dim(int32_t *value);
uint8_t vd6283tx_3_als_get_mounted(bool *value);
uint8_t vd6283tx_3_als_get_stream_id(int8_t *value);
uint8_t vd6283tx_3_als_get_ep_id(int8_t *value);
uint8_t vd6283tx_3_als_set_enable(bool value, char **response_message);
uint8_t vd6283tx_3_als_set_exposure_time(int32_t value, char **response_message);
uint8_t vd6283tx_3_als_set_intermeasurement_time(int32_t value, char **response_message);
uint8_t vd6283tx_3_als_set_channel1_gain(pnpl_vd6283tx_3_als_channel1_gain_t enum_id, char **response_message);
uint8_t vd6283tx_3_als_set_channel2_gain(pnpl_vd6283tx_3_als_channel2_gain_t enum_id, char **response_message);
uint8_t vd6283tx_3_als_set_channel3_gain(pnpl_vd6283tx_3_als_channel3_gain_t enum_id, char **response_message);
uint8_t vd6283tx_3_als_set_channel4_gain(pnpl_vd6283tx_3_als_channel4_gain_t enum_id, char **response_message);
uint8_t vd6283tx_3_als_set_channel5_gain(pnpl_vd6283tx_3_als_channel5_gain_t enum_id, char **response_message);
uint8_t vd6283tx_3_als_set_channel6_gain(pnpl_vd6283tx_3_als_channel6_gain_t enum_id, char **response_message);
uint8_t vd6283tx_3_als_set_samples_per_ts(int32_t value, char **response_message);
uint8_t vd6283tx_3_als_set_sensor_annotation(const char *value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_VD6283TX_3_ALS_H_ */

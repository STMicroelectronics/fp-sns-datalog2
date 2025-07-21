/**
  ******************************************************************************
  * @file    App_model_Lsm6dsv16x_Gyro.h
  * @author  SRA
  * @brief   Lsm6dsv16x_Gyro PnPL Components APIs
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lsm6dsv16x_gyro;1
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

#ifndef APP_MODEL_LSM6DSV16X_GYRO_H_
#define APP_MODEL_LSM6DSV16X_GYRO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Lsm6dsv16x_Gyro_PnPL.h"

/* USER includes -------------------------------------------------------------*/

/* LSM6DSV16X_GYRO PnPL Component --------------------------------------------- */
uint8_t lsm6dsv16x_gyro_comp_init(void);
char *lsm6dsv16x_gyro_get_key(void);
uint8_t lsm6dsv16x_gyro_get_odr(pnpl_lsm6dsv16x_gyro_odr_t *enum_id);
uint8_t lsm6dsv16x_gyro_get_fs(pnpl_lsm6dsv16x_gyro_fs_t *enum_id);
uint8_t lsm6dsv16x_gyro_get_enable(bool *value);
uint8_t lsm6dsv16x_gyro_get_samples_per_ts(int32_t *value);
uint8_t lsm6dsv16x_gyro_get_dim(int32_t *value);
uint8_t lsm6dsv16x_gyro_get_ioffset(float_t *value);
uint8_t lsm6dsv16x_gyro_get_measodr(float_t *value);
uint8_t lsm6dsv16x_gyro_get_usb_dps(int32_t *value);
uint8_t lsm6dsv16x_gyro_get_sd_dps(int32_t *value);
uint8_t lsm6dsv16x_gyro_get_sensitivity(float_t *value);
uint8_t lsm6dsv16x_gyro_get_data_type(char **value);
uint8_t lsm6dsv16x_gyro_get_sensor_annotation(char **value);
uint8_t lsm6dsv16x_gyro_get_sensor_category(int32_t *value);
uint8_t lsm6dsv16x_gyro_get_stream_id(int8_t *value);
uint8_t lsm6dsv16x_gyro_get_ep_id(int8_t *value);
uint8_t lsm6dsv16x_gyro_set_odr(pnpl_lsm6dsv16x_gyro_odr_t enum_id, char **response_message);
uint8_t lsm6dsv16x_gyro_set_fs(pnpl_lsm6dsv16x_gyro_fs_t enum_id, char **response_message);
uint8_t lsm6dsv16x_gyro_set_enable(bool value, char **response_message);
uint8_t lsm6dsv16x_gyro_set_samples_per_ts(int32_t value, char **response_message);
uint8_t lsm6dsv16x_gyro_set_sensor_annotation(const char *value, char **response_message);


#ifdef __cplusplus
}
#endif

#endif /* APP_MODEL_LSM6DSV16X_GYRO_H_ */

/**
  ******************************************************************************
  * @file    App_model_Sths34pf80_Tmos.c
  * @author  SRA
  * @brief   Sths34pf80_Tmos PnPL Components APIs
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:sths34pf80_tmos;2
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

/* STHS34PF80_TMOS PnPL Component --------------------------------------------*/
static SensorModel_t sths34pf80_tmos_model;
extern AppModel_t app_model;

uint8_t sths34pf80_tmos_comp_init(void)
{
  sths34pf80_tmos_model.comp_name = sths34pf80_tmos_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "sths34pf80", COM_TYPE_TMOS);
  sths34pf80_tmos_model.id = id;
  sths34pf80_tmos_model.sensor_status = SMSensorGetStatusPointer(id);
  sths34pf80_tmos_model.stream_params.stream_id = -1;
  sths34pf80_tmos_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &sths34pf80_tmos_model);

  sths34pf80_tmos_set_sensor_annotation("[EXTERN]\0", NULL);
  sths34pf80_tmos_set_odr(pnpl_sths34pf80_tmos_odr_hz15, NULL);
  sths34pf80_tmos_set_transmittance(1, NULL);
  sths34pf80_tmos_set_avg_tobject_num(pnpl_sths34pf80_tmos_avg_tobject_num_n32, NULL);
  sths34pf80_tmos_set_avg_tambient_num(pnpl_sths34pf80_tmos_avg_tambient_num_n8, NULL);
  sths34pf80_tmos_set_lpf_p_m_bandwidth(pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n9, NULL);
  sths34pf80_tmos_set_lpf_p_bandwidth(pnpl_sths34pf80_tmos_lpf_p_bandwidth_n400, NULL);
  sths34pf80_tmos_set_lpf_m_bandwidth(pnpl_sths34pf80_tmos_lpf_m_bandwidth_n200, NULL);
  sths34pf80_tmos_set_presence_threshold(120, NULL);
  sths34pf80_tmos_set_presence_hysteresis(50, NULL);
  sths34pf80_tmos_set_motion_threshold(120, NULL);
  sths34pf80_tmos_set_motion_hysteresis(50, NULL);
  sths34pf80_tmos_set_tambient_shock_threshold(20, NULL);
  sths34pf80_tmos_set_tambient_shock_hysteresis(5, NULL);
  sths34pf80_tmos_set_embedded_compensation(true, NULL);
  sths34pf80_tmos_set_software_compensation(false, NULL);
  sths34pf80_tmos_set_compensation_type(pnpl_sths34pf80_tmos_compensation_type_ipd_comp_nonlin, NULL);
  sths34pf80_tmos_set_sw_presence_threshold(250, NULL);
  sths34pf80_tmos_set_sw_motion_threshold(150, NULL);
  sths34pf80_tmos_set_compensation_filter_flag(true, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  sths34pf80_tmos_set_samples_per_ts(0, NULL);
#else
  sths34pf80_tmos_set_samples_per_ts(15, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *sths34pf80_tmos_get_key(void)
{
  return "sths34pf80_tmos";
}


uint8_t sths34pf80_tmos_get_enable(bool *value)
{
  *value = sths34pf80_tmos_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_odr(pnpl_sths34pf80_tmos_odr_t *enum_id)
{
  float_t odr = sths34pf80_tmos_model.sensor_status->type.presence.data_frequency;
  if (odr < 2.0f)
  {
    *enum_id = pnpl_sths34pf80_tmos_odr_hz1;
  }
  else if (odr < 3.0f)
  {
    *enum_id = pnpl_sths34pf80_tmos_odr_hz2;
  }
  else if (odr < 5.0f)
  {
    *enum_id = pnpl_sths34pf80_tmos_odr_hz4;
  }
  else if (odr < 9.0f)
  {
    *enum_id = pnpl_sths34pf80_tmos_odr_hz8;
  }
  else if (odr < 16.0f)
  {
    *enum_id = pnpl_sths34pf80_tmos_odr_hz15;
  }
  else
  {
    *enum_id = pnpl_sths34pf80_tmos_odr_hz30;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_transmittance(float_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.Transmittance;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_avg_tobject_num(pnpl_sths34pf80_tmos_avg_tobject_num_t *enum_id)
{
  uint16_t avg_tobject_num = sths34pf80_tmos_model.sensor_status->type.presence.average_tobject;
  switch (avg_tobject_num)
  {
    case 2:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n2;
      break;
    case 8:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n8;
      break;
    case 32:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n32;
      break;
    case 128:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n128;
      break;
    case 256:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n256;
      break;
    case 512:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n512;
      break;
    case 1024:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n1024;
      break;
    case 2048:
      *enum_id = pnpl_sths34pf80_tmos_avg_tobject_num_n2048;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE ;
}

uint8_t sths34pf80_tmos_get_avg_tambient_num(pnpl_sths34pf80_tmos_avg_tambient_num_t *enum_id)
{
  uint16_t avg_tambient_num = sths34pf80_tmos_model.sensor_status->type.presence.average_tambient;
  switch (avg_tambient_num)
  {
    case 1:
      *enum_id = pnpl_sths34pf80_tmos_avg_tambient_num_n1;
      break;
    case 2:
      *enum_id = pnpl_sths34pf80_tmos_avg_tambient_num_n2;
      break;
    case 4:
      *enum_id = pnpl_sths34pf80_tmos_avg_tambient_num_n4;
      break;
    case 8:
      *enum_id = pnpl_sths34pf80_tmos_avg_tambient_num_n8;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_lpf_p_m_bandwidth(pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_t *enum_id)
{
  uint16_t lpf_p_m_bandwidth = sths34pf80_tmos_model.sensor_status->type.presence.lpf_p_m_bandwidth;
  switch (lpf_p_m_bandwidth)
  {
    case 9:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n9;
      break;
    case 20:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n20;
      break;
    case 50:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n50;
      break;
    case 100:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n100;
      break;
    case 200:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n200;
      break;
    case 400:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n400;
      break;
    case 800:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n800;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_lpf_p_bandwidth(pnpl_sths34pf80_tmos_lpf_p_bandwidth_t *enum_id)
{
  uint16_t lpf_p_bandwidth = sths34pf80_tmos_model.sensor_status->type.presence.lpf_p_bandwidth;
  switch (lpf_p_bandwidth)
  {
    case 9:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_bandwidth_n9;
      break;
    case 20:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_bandwidth_n20;
      break;
    case 50:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_bandwidth_n50;
      break;
    case 100:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_bandwidth_n100;
      break;
    case 200:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_bandwidth_n200;
      break;
    case 400:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_bandwidth_n400;
      break;
    case 800:
      *enum_id = pnpl_sths34pf80_tmos_lpf_p_bandwidth_n800;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_lpf_m_bandwidth(pnpl_sths34pf80_tmos_lpf_m_bandwidth_t *enum_id)
{
  uint16_t lpf_m_bandwidth = sths34pf80_tmos_model.sensor_status->type.presence.lpf_m_bandwidth;
  switch (lpf_m_bandwidth)
  {
    case 9:
      *enum_id = pnpl_sths34pf80_tmos_lpf_m_bandwidth_n9;
      break;
    case 20:
      *enum_id = pnpl_sths34pf80_tmos_lpf_m_bandwidth_n20;
      break;
    case 50:
      *enum_id = pnpl_sths34pf80_tmos_lpf_m_bandwidth_n50;
      break;
    case 100:
      *enum_id = pnpl_sths34pf80_tmos_lpf_m_bandwidth_n100;
      break;
    case 200:
      *enum_id = pnpl_sths34pf80_tmos_lpf_m_bandwidth_n200;
      break;
    case 400:
      *enum_id = pnpl_sths34pf80_tmos_lpf_m_bandwidth_n400;
      break;
    case 800:
      *enum_id = pnpl_sths34pf80_tmos_lpf_m_bandwidth_n800;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_presence_threshold(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.presence_threshold;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_presence_hysteresis(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.presence_hysteresis;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_motion_threshold(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.motion_threshold;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_motion_hysteresis(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.motion_hysteresis;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_tambient_shock_threshold(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.tambient_shock_threshold;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_tambient_shock_hysteresis(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.tambient_shock_hysteresis;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_embedded_compensation(bool *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.embedded_compensation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_software_compensation(bool *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.software_compensation;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_compensation_type(pnpl_sths34pf80_tmos_compensation_type_t *enum_id)
{
  uint16_t comp_type = sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.comp_type;
  switch (comp_type)
  {
    case 0:
      *enum_id = pnpl_sths34pf80_tmos_compensation_type_ipd_comp_none;
      break;
    case 1:
      *enum_id = pnpl_sths34pf80_tmos_compensation_type_ipd_comp_lin;
      break;
    case 2:
      *enum_id = pnpl_sths34pf80_tmos_compensation_type_ipd_comp_nonlin;
      break;
    default:
      return 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_sw_presence_threshold(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.pres_ths;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_sw_motion_threshold(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.mot_ths;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_compensation_filter_flag(bool *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.comp_filter_flag;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_absence_static_flag(bool *value)
{
  *value = sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.abs_static_flag;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_samples_per_ts(int32_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_ioffset(float_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_usb_dps(int32_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_sd_dps(int32_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_data_type(char **value)
{
  *value = "int16_t";
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_sensor_annotation(char **value)
{
  *value = sths34pf80_tmos_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_sensor_category(int32_t *value)
{
  *value = sths34pf80_tmos_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_mounted(bool *value)
{
  *value = true;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_dim(int32_t *value)
{
  *value = 11;
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_stream_id(int8_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t sths34pf80_tmos_get_ep_id(int8_t *value)
{
  *value = sths34pf80_tmos_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t sths34pf80_tmos_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(sths34pf80_tmos_model.id);
  }
  else
  {
    ret = SMSensorDisable(sths34pf80_tmos_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_odr(pnpl_sths34pf80_tmos_odr_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  /* See AN5867 chapter 3.3: In continuous mode, for each AVG_TMOS[2:0] value,
   a maximum ODR value can be set (see table 8, where a blank space means incompatibility) */

  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint16_t average_tobject = sths34pf80_tmos_model.sensor_status->type.presence.average_tobject;
  int32_t value = 0;
  switch (enum_id)
  {
    case pnpl_sths34pf80_tmos_odr_hz1:
      value = 1;
      break;
    case pnpl_sths34pf80_tmos_odr_hz2:
      if (average_tobject < 513)
      {
        value = 2;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_odr_hz4:
      if (average_tobject < 257)
      {
        value = 4;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_odr_hz8:
      if (average_tobject < 129)
      {
        value = 8;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_odr_hz15:
      if (average_tobject < 33)
      {
        value = 15;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_odr_hz30:
      if (average_tobject < 33)
      {
        value = 30;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    default:
      ret = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  if (ret == SYS_NO_ERROR_CODE)
  {
    ret = SMSensorSetDataFrequency(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
#if (HSD_USE_DUMMY_DATA != 1)
      sths34pf80_tmos_set_samples_per_ts((int32_t) value, NULL);
#endif
      __stream_control(true);
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_transmittance(float_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  sys_error_code_t ret = SMSensorSetTransmittance(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_avg_tobject_num(pnpl_sths34pf80_tmos_avg_tobject_num_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  /* See AN5867 chapter 3.3: In continuous mode, for each AVG_TMOS[2:0] value,
   a maximum ODR value can be set (see table 8, where a blank space means incompatibility) */

  int32_t value;
  uint16_t data_frequency = (uint16_t)sths34pf80_tmos_model.sensor_status->type.presence.data_frequency;
  uint8_t ret = PNPL_NO_ERROR_CODE;

  switch (enum_id)
  {
    case pnpl_sths34pf80_tmos_avg_tobject_num_n1024:
      if (data_frequency < 2)
      {
        value = 1024;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_avg_tobject_num_n512:
      if (data_frequency < 3)
      {
        value = 512;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_avg_tobject_num_n256:
      if (data_frequency < 5)
      {
        value = 256;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_avg_tobject_num_n128:
      if (data_frequency < 9)
      {
        value = 128;
      }
      else
      {
        ret = SYS_INVALID_PARAMETER_ERROR_CODE;
      }
      break;
    case pnpl_sths34pf80_tmos_avg_tobject_num_n32:
      value = 32;
      break;
    case pnpl_sths34pf80_tmos_avg_tobject_num_n8:
      value = 8;
      break;
    case pnpl_sths34pf80_tmos_avg_tobject_num_n2:
      value = 2;
      break;
    default:
      ret = SYS_INVALID_PARAMETER_ERROR_CODE;
  }

  if (ret == SYS_NO_ERROR_CODE)
  {
    ret = SMSensorSetAverageTObject(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_avg_tambient_num(pnpl_sths34pf80_tmos_avg_tambient_num_t enum_id,
                                             char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint16_t value;
  switch (enum_id)
  {
    case pnpl_sths34pf80_tmos_avg_tambient_num_n1:
      value = 1;
      break;
    case pnpl_sths34pf80_tmos_avg_tambient_num_n2:
      value = 2;
      break;
    case pnpl_sths34pf80_tmos_avg_tambient_num_n4:
      value = 4;
      break;
    case pnpl_sths34pf80_tmos_avg_tambient_num_n8:
      value = 8;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetAverageTAmbient(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_lpf_p_m_bandwidth(pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_t enum_id,
                                              char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint16_t value;
  switch (enum_id)
  {
    case pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n9:
      value = 9;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n20:
      value = 20;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n50:
      value = 50;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n100:
      value = 100;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n200:
      value = 200;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n400:
      value = 400;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_m_bandwidth_n800:
      value = 800;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLPF_P_M_Bandwidth(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_lpf_p_bandwidth(pnpl_sths34pf80_tmos_lpf_p_bandwidth_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint16_t value;
  switch (enum_id)
  {
    case pnpl_sths34pf80_tmos_lpf_p_bandwidth_n9:
      value = 9;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_bandwidth_n20:
      value = 20;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_bandwidth_n50:
      value = 50;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_bandwidth_n100:
      value = 100;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_bandwidth_n200:
      value = 200;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_bandwidth_n400:
      value = 400;
      break;
    case pnpl_sths34pf80_tmos_lpf_p_bandwidth_n800:
      value = 800;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLPF_P_Bandwidth(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_lpf_m_bandwidth(pnpl_sths34pf80_tmos_lpf_m_bandwidth_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  uint16_t value;
  switch (enum_id)
  {
    case pnpl_sths34pf80_tmos_lpf_m_bandwidth_n9:
      value = 9;
      break;
    case pnpl_sths34pf80_tmos_lpf_m_bandwidth_n20:
      value = 20;
      break;
    case pnpl_sths34pf80_tmos_lpf_m_bandwidth_n50:
      value = 50;
      break;
    case pnpl_sths34pf80_tmos_lpf_m_bandwidth_n100:
      value = 100;
      break;
    case pnpl_sths34pf80_tmos_lpf_m_bandwidth_n200:
      value = 200;
      break;
    case pnpl_sths34pf80_tmos_lpf_m_bandwidth_n400:
      value = 400;
      break;
    case pnpl_sths34pf80_tmos_lpf_m_bandwidth_n800:
      value = 800;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLPF_M_Bandwidth(sths34pf80_tmos_model.id, value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_presence_threshold(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 32767;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetPresenceThreshold(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_presence_hysteresis(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 255;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetPresenceHysteresis(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_motion_threshold(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 32767;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetMotionThreshold(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_motion_hysteresis(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 255;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetMotionHysteresis(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_tambient_shock_threshold(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 32767;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetTAmbientShockThreshold(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_tambient_shock_hysteresis(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 255;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetTAmbientShockHysteresis(sths34pf80_tmos_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_embedded_compensation(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ret = SMSensorSetEmbeddedCompensation(sths34pf80_tmos_model.id, (uint8_t)value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_software_compensation(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  ret = SMSensorSetSoftwareCompensation(sths34pf80_tmos_model.id, (uint8_t)value);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_compensation_type(pnpl_sths34pf80_tmos_compensation_type_t enum_id,
                                              char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  //  typedef enum
  //  {
  //    IPD_COMP_NONE,   /* No compensation */
  //    IPD_COMP_LIN,    /* Linear compensation */
  //    IPD_COMP_NONLIN  /* Non-linear compensation */
  //  } IPD_comp_t;

  uint8_t ret = PNPL_NO_ERROR_CODE;

  switch (enum_id)
  {
    case pnpl_sths34pf80_tmos_compensation_type_ipd_comp_none:
      sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.comp_type = 0;
      break;
    case pnpl_sths34pf80_tmos_compensation_type_ipd_comp_lin:
      sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.comp_type = 1;
      break;
    case pnpl_sths34pf80_tmos_compensation_type_ipd_comp_nonlin:
      sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.comp_type = 2;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id, &sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_sw_presence_threshold(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.pres_ths = (uint16_t)value;
  ret = SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id, &sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_sw_motion_threshold(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.mot_ths = (uint16_t)value;
  ret = SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id, &sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_compensation_filter_flag(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.comp_filter_flag = (uint8_t)value;
  ret = SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id, &sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_absence_static_flag(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig.abs_static_flag = (uint8_t)value;
  ret = SMSensorSetSoftwareCompensationAlgorithmConfig(sths34pf80_tmos_model.id, &sths34pf80_tmos_model.sensor_status->type.presence.AlgorithmConfig);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 30;
  if (value >= min_v && value <= max_v)
  {
    sths34pf80_tmos_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    sths34pf80_tmos_model.stream_params.spts = max_v;
  }
  else
  {
    sths34pf80_tmos_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t sths34pf80_tmos_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(sths34pf80_tmos_model.annotation, value);
  return ret;
}




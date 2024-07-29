/**
  ******************************************************************************
  * @file    App_model_Vd6283tx_3_Als.c
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
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_pdetect:sensors:vd6283tx_3_als;1
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

/* VD6283TX_2_ALS PnPL Component ---------------------------------------------*/
static SensorModel_t vd6283tx_3_als_model;
extern AppModel_t app_model;

uint8_t vd6283tx_3_als_comp_init(void)
{
  vd6283tx_3_als_model.comp_name = vd6283tx_3_als_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "vd6283tx_3", COM_TYPE_ALS);
  vd6283tx_3_als_model.id = id;
  vd6283tx_3_als_model.sensor_status = SMSensorGetStatusPointer(id);
  vd6283tx_3_als_model.stream_params.stream_id = -1;
  vd6283tx_3_als_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &vd6283tx_3_als_model);

  vd6283tx_3_als_set_sensor_annotation("[EXTERN]\0", NULL);
  vd6283tx_3_als_set_exposure_time(99200, NULL);
  vd6283tx_3_als_set_intermeasurement_time(1000, NULL);
  vd6283tx_3_als_set_channel1_gain(pnpl_vd6283tx_3_als_channel1_gain_n1, NULL);
  vd6283tx_3_als_set_channel2_gain(pnpl_vd6283tx_3_als_channel2_gain_n1, NULL);
  vd6283tx_3_als_set_channel3_gain(pnpl_vd6283tx_3_als_channel3_gain_n1, NULL);
  vd6283tx_3_als_set_channel4_gain(pnpl_vd6283tx_3_als_channel4_gain_n1, NULL);
  vd6283tx_3_als_set_channel5_gain(pnpl_vd6283tx_3_als_channel5_gain_n1, NULL);
  vd6283tx_3_als_set_channel6_gain(pnpl_vd6283tx_3_als_channel6_gain_n1, NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  vd6283tx_3_als_set_samples_per_ts(0, NULL);
#else
  vd6283tx_3_als_set_samples_per_ts(1, NULL);
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *vd6283tx_3_als_get_key(void)
{
  return "vd6283tx_3_als";
}


uint8_t vd6283tx_3_als_get_enable(bool *value)
{
  *value = vd6283tx_3_als_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_exposure_time(int32_t *value)
{
  *value = vd6283tx_3_als_model.sensor_status->type.light.exposure_time;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_intermeasurement_time(int32_t *value)
{
  *value = vd6283tx_3_als_model.sensor_status->type.light.intermeasurement_time;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_channel1_gain(pnpl_vd6283tx_3_als_channel1_gain_t *enum_id)
{
  float channel1_gain = vd6283tx_3_als_model.sensor_status->type.light.gain[0];
  if (channel1_gain < 0.77)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n0_71;
  }
  else if (channel1_gain < 0.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n0_83;
  }
  else if (channel1_gain < 1.13)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n1;
  }
  else if (channel1_gain < 1.46)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n1_25;
  }
  else if (channel1_gain < 2.09)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n1_67;
  }
  else if (channel1_gain < 2.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n2_5;
  }
  else if (channel1_gain < 4.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n3_33;
  }
  else if (channel1_gain < 6.07)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n5;
  }
  else if (channel1_gain < 8.57)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n7_1;
  }
  else if (channel1_gain < 13.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n10;
  }
  else if (channel1_gain < 20.82)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n16;
  }
  else if (channel1_gain < 29.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n25;
  }
  else if (channel1_gain < 41.67)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n33;
  }
  else if (channel1_gain < 58.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n50;
  }
  else
  {
    *enum_id = pnpl_vd6283tx_3_als_channel1_gain_n66_6;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_channel2_gain(pnpl_vd6283tx_3_als_channel2_gain_t *enum_id)
{
  float channel2_gain = vd6283tx_3_als_model.sensor_status->type.light.gain[1];
  if (channel2_gain < 0.77)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n0_71;
  }
  else if (channel2_gain < 0.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n0_83;
  }
  else if (channel2_gain < 1.13)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n1;
  }
  else if (channel2_gain < 1.46)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n1_25;
  }
  else if (channel2_gain < 2.09)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n1_67;
  }
  else if (channel2_gain < 2.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n2_5;
  }
  else if (channel2_gain < 4.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n3_33;
  }
  else if (channel2_gain < 6.07)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n5;
  }
  else if (channel2_gain < 8.57)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n7_1;
  }
  else if (channel2_gain < 13.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n10;
  }
  else if (channel2_gain < 20.82)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n16;
  }
  else if (channel2_gain < 29.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n25;
  }
  else if (channel2_gain < 41.67)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n33;
  }
  else if (channel2_gain < 58.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n50;
  }
  else
  {
    *enum_id = pnpl_vd6283tx_3_als_channel2_gain_n66_6;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_channel3_gain(pnpl_vd6283tx_3_als_channel3_gain_t *enum_id)
{
  float channel3_gain = vd6283tx_3_als_model.sensor_status->type.light.gain[2];
  if (channel3_gain < 0.77)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n0_71;
  }
  else if (channel3_gain < 0.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n0_83;
  }
  else if (channel3_gain < 1.13)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n1;
  }
  else if (channel3_gain < 1.46)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n1_25;
  }
  else if (channel3_gain < 2.09)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n1_67;
  }
  else if (channel3_gain < 2.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n2_5;
  }
  else if (channel3_gain < 4.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n3_33;
  }
  else if (channel3_gain < 6.07)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n5;
  }
  else if (channel3_gain < 8.57)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n7_1;
  }
  else if (channel3_gain < 13.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n10;
  }
  else if (channel3_gain < 20.82)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n16;
  }
  else if (channel3_gain < 29.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n25;
  }
  else if (channel3_gain < 41.67)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n33;
  }
  else if (channel3_gain < 58.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n50;
  }
  else
  {
    *enum_id = pnpl_vd6283tx_3_als_channel3_gain_n66_6;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_channel4_gain(pnpl_vd6283tx_3_als_channel4_gain_t *enum_id)
{
  float channel4_gain = vd6283tx_3_als_model.sensor_status->type.light.gain[3];
  if (channel4_gain < 0.77)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n0_71;
  }
  else if (channel4_gain < 0.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n0_83;
  }
  else if (channel4_gain < 1.13)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n1;
  }
  else if (channel4_gain < 1.46)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n1_25;
  }
  else if (channel4_gain < 2.09)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n1_67;
  }
  else if (channel4_gain < 2.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n2_5;
  }
  else if (channel4_gain < 4.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n3_33;
  }
  else if (channel4_gain < 6.07)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n5;
  }
  else if (channel4_gain < 8.57)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n7_1;
  }
  else if (channel4_gain < 13.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n10;
  }
  else if (channel4_gain < 20.82)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n16;
  }
  else if (channel4_gain < 29.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n25;
  }
  else if (channel4_gain < 41.67)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n33;
  }
  else if (channel4_gain < 58.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n50;
  }
  else
  {
    *enum_id = pnpl_vd6283tx_3_als_channel4_gain_n66_6;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_channel5_gain(pnpl_vd6283tx_3_als_channel5_gain_t *enum_id)
{
  float channel5_gain = vd6283tx_3_als_model.sensor_status->type.light.gain[4];
  if (channel5_gain < 0.77)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n0_71;
  }
  else if (channel5_gain < 0.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n0_83;
  }
  else if (channel5_gain < 1.13)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n1;
  }
  else if (channel5_gain < 1.46)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n1_25;
  }
  else if (channel5_gain < 2.09)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n1_67;
  }
  else if (channel5_gain < 2.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n2_5;
  }
  else if (channel5_gain < 4.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n3_33;
  }
  else if (channel5_gain < 6.07)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n5;
  }
  else if (channel5_gain < 8.57)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n7_1;
  }
  else if (channel5_gain < 13.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n10;
  }
  else if (channel5_gain < 20.82)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n16;
  }
  else if (channel5_gain < 29.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n25;
  }
  else if (channel5_gain < 41.67)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n33;
  }
  else if (channel5_gain < 58.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n50;
  }
  else
  {
    *enum_id = pnpl_vd6283tx_3_als_channel5_gain_n66_6;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_channel6_gain(pnpl_vd6283tx_3_als_channel6_gain_t *enum_id)
{
  float channel6_gain = vd6283tx_3_als_model.sensor_status->type.light.gain[5];
  if (channel6_gain < 0.77)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n0_71;
  }
  else if (channel6_gain < 0.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n0_83;
  }
  else if (channel6_gain < 1.13)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n1;
  }
  else if (channel6_gain < 1.46)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n1_25;
  }
  else if (channel6_gain < 2.09)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n1_67;
  }
  else if (channel6_gain < 2.92)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n2_5;
  }
  else if (channel6_gain < 4.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n3_33;
  }
  else if (channel6_gain < 6.07)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n5;
  }
  else if (channel6_gain < 8.57)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n7_1;
  }
  else if (channel6_gain < 13.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n10;
  }
  else if (channel6_gain < 20.82)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n16;
  }
  else if (channel6_gain < 29.17)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n25;
  }
  else if (channel6_gain < 41.67)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n33;
  }
  else if (channel6_gain < 58.34)
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n50;
  }
  else
  {
    *enum_id = pnpl_vd6283tx_3_als_channel6_gain_n66_6;
  }
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_samples_per_ts(int32_t *value)
{
  *value = vd6283tx_3_als_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_ioffset(float *value)
{
  *value = vd6283tx_3_als_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_usb_dps(int32_t *value)
{
  *value = vd6283tx_3_als_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_sd_dps(int32_t *value)
{
  *value = vd6283tx_3_als_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_data_type(char **value)
{
  *value = "uint32_t";
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_sensor_annotation(char **value)
{
  *value = vd6283tx_3_als_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_sensor_category(int32_t *value)
{
  *value = vd6283tx_3_als_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_dim(int32_t *value)
{
  *value = 6;
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_stream_id(int8_t *value)
{
  *value = vd6283tx_3_als_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t vd6283tx_3_als_get_ep_id(int8_t *value)
{
  *value = vd6283tx_3_als_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t vd6283tx_3_als_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (value)
  {
    ret = SMSensorEnable(vd6283tx_3_als_model.id);
  }
  else
  {
    ret = SMSensorDisable(vd6283tx_3_als_model.id);
  }
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
    __stream_control(true);
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_exposure_time(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 1600000;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetExposureTime(vd6283tx_3_als_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
    }
  }
  else
  {
    return 1;
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_intermeasurement_time(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 5100;
  if (value >= min_v && value <= max_v)
  {
    ret = SMSensorSetIntermeasurementTime(vd6283tx_3_als_model.id, value);
    if (ret == SYS_NO_ERROR_CODE)
    {
#if (HSD_USE_DUMMY_DATA != 1)
      vd6283tx_3_als_set_samples_per_ts((int32_t)(1000 / value), NULL);
#endif
      __stream_control(true);
    }
  }
  else
  {
    return 1;
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_channel1_gain(pnpl_vd6283tx_3_als_channel1_gain_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_vd6283tx_3_als_channel1_gain_n0_71:
      value = 0.71f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n0_83:
      value = 0.83f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n1:
      value = 1.0f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n1_25:
      value = 1.25f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n1_67:
      value = 1.67f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n2_5:
      value = 2.5f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n3_33:
      value = 3.33f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n5:
      value = 5.0f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n7_1:
      value = 7.1f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n10:
      value = 10.0f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n16:
      value = 16.0f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n25:
      value = 25.0f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n33:
      value = 33.0f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n50:
      value = 50.0f;
      break;
    case pnpl_vd6283tx_3_als_channel1_gain_n66_6:
      value = 66.6f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLightGain(vd6283tx_3_als_model.id, value, 1);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_channel2_gain(pnpl_vd6283tx_3_als_channel2_gain_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_vd6283tx_3_als_channel2_gain_n0_71:
      value = 0.71f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n0_83:
      value = 0.83f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n1:
      value = 1.0f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n1_25:
      value = 1.25f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n1_67:
      value = 1.67f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n2_5:
      value = 2.5f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n3_33:
      value = 3.33f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n5:
      value = 5.0f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n7_1:
      value = 7.1f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n10:
      value = 10.0f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n16:
      value = 16.0f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n25:
      value = 25.0f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n33:
      value = 33.0f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n50:
      value = 50.0f;
      break;
    case pnpl_vd6283tx_3_als_channel2_gain_n66_6:
      value = 66.6f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLightGain(vd6283tx_3_als_model.id, value, 2);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_channel3_gain(pnpl_vd6283tx_3_als_channel3_gain_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_vd6283tx_3_als_channel3_gain_n0_71:
      value = 0.71f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n0_83:
      value = 0.83f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n1:
      value = 1.0f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n1_25:
      value = 1.25f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n1_67:
      value = 1.67f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n2_5:
      value = 2.5f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n3_33:
      value = 3.33f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n5:
      value = 5.0f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n7_1:
      value = 7.1f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n10:
      value = 10.0f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n16:
      value = 16.0f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n25:
      value = 25.0f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n33:
      value = 33.0f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n50:
      value = 50.0f;
      break;
    case pnpl_vd6283tx_3_als_channel3_gain_n66_6:
      value = 66.6f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLightGain(vd6283tx_3_als_model.id, value, 3);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_channel4_gain(pnpl_vd6283tx_3_als_channel4_gain_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_vd6283tx_3_als_channel4_gain_n0_71:
      value = 0.71f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n0_83:
      value = 0.83f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n1:
      value = 1.0f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n1_25:
      value = 1.25f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n1_67:
      value = 1.67f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n2_5:
      value = 2.5f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n3_33:
      value = 3.33f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n5:
      value = 5.0f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n7_1:
      value = 7.1f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n10:
      value = 10.0f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n16:
      value = 16.0f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n25:
      value = 25.0f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n33:
      value = 33.0f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n50:
      value = 50.0f;
      break;
    case pnpl_vd6283tx_3_als_channel4_gain_n66_6:
      value = 66.6f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLightGain(vd6283tx_3_als_model.id, value, 4);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_channel5_gain(pnpl_vd6283tx_3_als_channel5_gain_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_vd6283tx_3_als_channel5_gain_n0_71:
      value = 0.71f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n0_83:
      value = 0.83f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n1:
      value = 1.0f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n1_25:
      value = 1.25f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n1_67:
      value = 1.67f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n2_5:
      value = 2.5f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n3_33:
      value = 3.33f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n5:
      value = 5.0f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n7_1:
      value = 7.1f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n10:
      value = 10.0f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n16:
      value = 16.0f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n25:
      value = 25.0f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n33:
      value = 33.0f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n50:
      value = 50.0f;
      break;
    case pnpl_vd6283tx_3_als_channel5_gain_n66_6:
      value = 66.6f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLightGain(vd6283tx_3_als_model.id, value, 5);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_channel6_gain(pnpl_vd6283tx_3_als_channel6_gain_t enum_id, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  float value;
  switch (enum_id)
  {
    case pnpl_vd6283tx_3_als_channel6_gain_n0_71:
      value = 0.71f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n0_83:
      value = 0.83f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n1:
      value = 1.0f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n1_25:
      value = 1.25f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n1_67:
      value = 1.67f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n2_5:
      value = 2.5f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n3_33:
      value = 3.33f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n5:
      value = 5.0f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n7_1:
      value = 7.1f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n10:
      value = 10.0f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n16:
      value = 16.0f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n25:
      value = 25.0f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n33:
      value = 33.0f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n50:
      value = 50.0f;
      break;
    case pnpl_vd6283tx_3_als_channel6_gain_n66_6:
      value = 66.6f;
      break;
    default:
      return 1;
  }
  ret = SMSensorSetLightGain(vd6283tx_3_als_model.id, value, 6);
  if (ret == SYS_NO_ERROR_CODE)
  {
    /* USER Code */
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_samples_per_ts(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  int32_t min_v = 0;
  int32_t max_v = 50;
  if (value >= min_v && value <= max_v)
  {
    vd6283tx_3_als_model.stream_params.spts = value;
  }
  else if (value > max_v)
  {
    vd6283tx_3_als_model.stream_params.spts = max_v;
  }
  else
  {
    vd6283tx_3_als_model.stream_params.spts = min_v;
  }
  return ret;
}

uint8_t vd6283tx_3_als_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(vd6283tx_3_als_model.annotation, value);
  return ret;
}


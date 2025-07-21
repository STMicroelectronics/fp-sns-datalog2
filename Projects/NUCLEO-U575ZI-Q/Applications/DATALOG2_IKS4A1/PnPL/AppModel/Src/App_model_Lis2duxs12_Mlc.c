/**
  ******************************************************************************
  * @file    App_model_Lis2duxs12_Mlc.c
  * @author  SRA
  * @brief   Lis2duxs12_Mlc PnPL Components APIs
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
  * dtmi:vespucci:nucleo_u575zi_q:x_nucleo_iks4a1:FP_SNS_DATALOG2_Datalog2:sensors:lis2duxs12_mlc;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include "services/SQuery.h"

/* USER includes -------------------------------------------------------------*/
#include "DatalogAppTask.h"
/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* LIS2DUXS12_MLC PnPL Component ---------------------------------------------*/
static SensorModel_t lis2duxs12_mlc_model;
extern AppModel_t app_model;

uint8_t lis2duxs12_mlc_comp_init(void)
{
  lis2duxs12_mlc_model.comp_name = lis2duxs12_mlc_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "lis2duxs12", COM_TYPE_MLC);
  lis2duxs12_mlc_model.id = id;
  lis2duxs12_mlc_model.sensor_status = SMSensorGetStatusPointer(id);
  lis2duxs12_mlc_model.stream_params.stream_id = -1;
  lis2duxs12_mlc_model.stream_params.usb_ep = -1;

  addSensorToAppModel(id, &lis2duxs12_mlc_model);

  lis2duxs12_mlc_set_sensor_annotation("\0", NULL);
#if (HSD_USE_DUMMY_DATA == 1)
  lis2duxs12_mlc_model.stream_params.spts = 0;
#else
  lis2duxs12_mlc_model.stream_params.spts = 1;
#endif
  __stream_control(true);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *lis2duxs12_mlc_get_key(void)
{
  return "lis2duxs12_mlc";
}


uint8_t lis2duxs12_mlc_get_enable(bool *value)
{
  *value = lis2duxs12_mlc_model.sensor_status->is_active;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_samples_per_ts(int32_t *value)
{
  *value = lis2duxs12_mlc_model.stream_params.spts;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_ucf_status(bool *value)
{
  *value = app_model.lis2duxs12_mlc_ucf_valid;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_dim(int32_t *value)
{
  *value = 5;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_ioffset(float_t *value)
{
  *value = lis2duxs12_mlc_model.stream_params.ioffset;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_data_type(char **value)
{
  *value = "int8";
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_usb_dps(int32_t *value)
{
  *value = lis2duxs12_mlc_model.stream_params.usb_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_sd_dps(int32_t *value)
{
  *value = lis2duxs12_mlc_model.stream_params.sd_dps;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_sensor_annotation(char **value)
{
  *value = lis2duxs12_mlc_model.annotation;
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_sensor_category(int32_t *value)
{
  *value = lis2duxs12_mlc_model.sensor_status->isensor_class;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_stream_id(int8_t *value)
{
  *value = lis2duxs12_mlc_model.stream_params.stream_id;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}

uint8_t lis2duxs12_mlc_get_ep_id(int8_t *value)
{
  *value = lis2duxs12_mlc_model.stream_params.usb_ep;
  /* USER Code */
  return PNPL_NO_ERROR_CODE;
}


uint8_t lis2duxs12_mlc_set_enable(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  if (app_model.lis2duxs12_mlc_ucf_valid == true)
  {
    if (value)
    {
      ret = SMSensorEnable(lis2duxs12_mlc_model.id);
    }
    else
    {
      ret = SMSensorDisable(lis2duxs12_mlc_model.id);
    }
    if (ret == SYS_NO_ERROR_CODE)
    {
      /* USER Code */
      __stream_control(true);
      /* USER Code */
    }
  }
  else
  {
    ret = PNPL_BASE_ERROR_CODE;
    if (response_message != NULL)
    {
      *response_message = "UCF not loaded";
    }
  }
  return ret;
}

uint8_t lis2duxs12_mlc_set_sensor_annotation(const char *value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  uint8_t ret = PNPL_NO_ERROR_CODE;
  strcpy(lis2duxs12_mlc_model.annotation, value);
  return ret;
}


uint8_t lis2duxs12_mlc_load_file(const char *data, int32_t size)
{
  DatalogAppTask_load_lis2duxs12_ucf_vtbl(data, size);
  app_model.lis2duxs12_mlc_ucf_valid = true;
  __stream_control(true);
  return PNPL_NO_ERROR_CODE;
}


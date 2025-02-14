/**
  ******************************************************************************
  * @file    App_model_Automode.c
  * @author  SRA
  * @brief   Automode PnPL Components APIs
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
  * dtmi:vespucci:other:automode;3
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"

/* USER includes -------------------------------------------------------------*/

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* AutoMode PnPL Component ---------------------------------------------------*/
extern AppModel_t app_model;

uint8_t automode_comp_init(void)
{
  app_model.automode_model.comp_name = automode_get_key();

  automode_set_logging_period_s(1, NULL);
  automode_set_idle_period_s(1, NULL);
  /* USER Component initialization code */
  return PNPL_NO_ERROR_CODE;
}

char *automode_get_key(void)
{
  return "automode";
}


uint8_t automode_get_started(bool *value)
{
  *value = app_model.automode_model.started;
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_get_enabled(bool *value)
{
  *value = app_model.automode_model.enabled;
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_get_nof_acquisitions(int32_t *value)
{
  *value = app_model.automode_model.nof_acquisitions;
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_get_start_delay_s(int32_t *value)
{
  *value = app_model.automode_model.start_delay_s;
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_get_logging_period_s(int32_t *value)
{
  *value = app_model.automode_model.logging_period_s;
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_get_idle_period_s(int32_t *value)
{
  *value = app_model.automode_model.idle_period_s;
  return PNPL_NO_ERROR_CODE;
}


uint8_t automode_set_started(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  /* USER Code */
  app_model.automode_model.started = value;
  return PNPL_NO_ERROR_CODE;
}


uint8_t automode_set_enabled(bool value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  /* USER Code */
  app_model.automode_model.enabled = value;
  if (app_model.automode_model.start_delay_s == 0)
  {
    /* To avoid issue while opening SD card or handling files, setup a minimum default value */
    app_model.automode_model.start_delay_s = 1;
  }
  if (app_model.automode_model.idle_period_s == 0)
  {
    /* To avoid issue while opening SD card or handling files, setup a minimum default value */
    app_model.automode_model.idle_period_s = 1;
  }
  if (app_model.automode_model.logging_period_s == 0)
  {
    /* To avoid issue while opening SD card or handling files, setup a minimum default value */
    app_model.automode_model.logging_period_s = 1;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_set_nof_acquisitions(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  app_model.automode_model.nof_acquisitions = value;
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_set_start_delay_s(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  app_model.automode_model.start_delay_s = value;
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_set_logging_period_s(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  int32_t min_v = 1;
  if (value >= min_v)
  {
    app_model.automode_model.logging_period_s = value;
  }
  else
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting below minimum threshold (1)";
    }
    app_model.automode_model.logging_period_s = min_v;
  }
  return PNPL_NO_ERROR_CODE;
}

uint8_t automode_set_idle_period_s(int32_t value, char **response_message)
{
  if (response_message != NULL)
  {
    *response_message = "";
  }
  int32_t min_v = 1;
  if (value >= min_v)
  {
    app_model.automode_model.idle_period_s = value;
  }
  else
  {
    if (response_message != NULL)
    {
      *response_message = "Error: Value setting below minimum threshold (1)";
    }
    app_model.automode_model.idle_period_s = min_v;
  }
  return PNPL_NO_ERROR_CODE;
}




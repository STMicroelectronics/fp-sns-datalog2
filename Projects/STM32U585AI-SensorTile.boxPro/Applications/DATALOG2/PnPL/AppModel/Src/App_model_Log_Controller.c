/**
  ******************************************************************************
  * @file    App_model_Log_Controller.c
  * @author  SRA
  * @brief   Log_Controller PnPL Components APIs
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
  * dtmi:vespucci:other:log_controller;3
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"

/* USER includes -------------------------------------------------------------*/
#include "rtc.h"
#include "fx_stm32_sd_driver.h"
#include "TagManager.h"
#include "DatalogAppTask.h"

/* USER private function prototypes ------------------------------------------*/

/* USER defines --------------------------------------------------------------*/

/* Log Controller PnPL Component ---------------------------------------------*/
extern AppModel_t app_model;

uint8_t log_controller_comp_init(void)
{
  app_model.log_controller_model.comp_name = log_controller_get_key();

  app_model.log_controller_model.status = false;
  app_model.log_controller_model.interface = -1;
  return PNPL_NO_ERROR_CODE;
}

char *log_controller_get_key(void)
{
  return "log_controller";
}


uint8_t log_controller_get_log_status(bool *value)
{
  *value = app_model.log_controller_model.status;
  return PNPL_NO_ERROR_CODE;
}

uint8_t log_controller_get_sd_mounted(bool *value)
{
  app_model.log_controller_model.sd_mounted = BSP_SD_IsDetected(FX_STM32_SD_INSTANCE);
  *value = app_model.log_controller_model.sd_mounted;
  return PNPL_NO_ERROR_CODE;
}

uint8_t log_controller_get_sd_failed(bool *value)
{
  *value = app_model.log_controller_model.sd_failed;
  return PNPL_NO_ERROR_CODE;
}


uint8_t log_controller_get_controller_type(int32_t *value)
{
  *value = 0; /* 0 == HSD log controller, 1 == App classifier controller, 2 = generic log controller */
  return PNPL_NO_ERROR_CODE;
}


uint8_t log_controller_save_config(void)
{
  return DatalogAppTask_save_config_vtbl();
}

uint8_t log_controller_start_log(int32_t interface)
{
  app_model.log_controller_model.interface = interface;

  __stream_control(true);

  //Reset Tag counter
  TMResetTagListCounter();

  RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdate, RTC_FORMAT_BIN);

  _tm t =
  { .tm_year = sdate.Year + 2000, .tm_mon = sdate.Month - 1, .tm_mday = sdate.Date, .tm_hour = stime.Hours, .tm_min = stime.Minutes, .tm_sec = stime.Seconds };

// WHY THIS -1 (in months) ???
//  struct tm {
//     int tm_sec;         /* seconds,  range 0 to 59          */
//     int tm_min;         /* minutes, range 0 to 59           */
//     int tm_hour;        /* hours, range 0 to 23             */
//     int tm_mday;        /* day of the month, range 1 to 31  */
//     int tm_mon;         /* month, range 0 to 11             */ <------ (-1) months here (0..11), months from RTC (1..12)
//     int tm_year;        /* The number of years since 1900   */
//     int tm_wday;        /* day of the week, range 0 to 6    */
//     int tm_yday;        /* day in the year, range 0 to 365  */
//     int tm_isdst;       /* daylight saving time             */
//  };

  TMSetStartTime(t);
  char local_timestamp[86];
  (void) sprintf(local_timestamp, "%04d-%02d-%02dT%02d:%02d:%02d", t.tm_year, t.tm_mon + 1,
                 t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
  (void) memcpy(app_model.acquisition_info_model.start_time, local_timestamp,
                sizeof(app_model.acquisition_info_model.start_time) - 1);

  /* last part not done in sprintf to avoid a warning  */
  app_model.acquisition_info_model.start_time[19] = '.';
  app_model.acquisition_info_model.start_time[20] = '0';
  app_model.acquisition_info_model.start_time[21] = '0';
  app_model.acquisition_info_model.start_time[22] = '0';
  app_model.acquisition_info_model.start_time[23] = 'Z';
  app_model.acquisition_info_model.start_time[24] = '\0';

  DatalogAppTask_start_vtbl(interface);
  return PNPL_NO_ERROR_CODE;
}

uint8_t log_controller_stop_log(void)
{
  PnPLGenerateAcquisitionUUID(app_model.acquisition_info_model.uuid);
  TMCloseAllOpenedTags();
  DatalogAppTask_stop_vtbl();
  TMCalculateEndTime(app_model.acquisition_info_model.end_time);
  return PNPL_NO_ERROR_CODE;
}

uint8_t log_controller_set_time(const char *datetime)
{
  return DatalogAppTask_set_time_vtbl(datetime);
}

uint8_t log_controller_switch_bank(void)
{
  return DatalogAppTask_switch_bank_vtbl();
}

uint8_t log_controller_set_dfu_mode(void)
{
  return DatalogAppTask_set_dfu_mode();
}

uint8_t log_controller_enable_all(bool status)
{
  /* USER Code */
  return DatalogAppTask_enable_all(status);
}


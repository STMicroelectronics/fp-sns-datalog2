/**
  ******************************************************************************
  * @file    App_Model.c
  * @author  SRA
  * @brief   App Application Model and PnPL Components APIs
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
  * This file has been auto generated from the following Device Template Model:
  * dtmi:vespucci:steval_stwinbx1:fpSnsDatalog2_ultrasoundFft;3
  *
  * Created by: DTDL2PnPL_cGen version 1.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include <string.h>
#include <stdio.h>
#include "services/SQuery.h"

/* USER includes -------------------------------------------------------------*/
#include "services/sysdebug.h"
#include "rtc.h"

/* USER private function prototypes ------------------------------------------*/
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status);


/* USER defines --------------------------------------------------------------*/

static uint8_t algorithms_cnt = 0;
#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DT, level, message)

AppModel_t app_model;

AppModel_t *getAppModel(void)
{
  return &app_model;
}

/* Device Components APIs ----------------------------------------------------*/

/* Ultrasound FFT PnPL Component ---------------------------------------------*/
static AlgorithmModel_t fft_dpu_model;
uint16_t fft_dpu_id = -1;
uint8_t fft_dpu_comp_init(void)
{
  fft_dpu_model.comp_name = fft_dpu_get_key();

  fft_dpu_id = algorithms_cnt;
  algorithms_cnt += 1;
  fft_dpu_model.stream_params.stream_id = -1;
  fft_dpu_model.stream_params.usb_ep = -1;
  /* USER code */

  //app_model.a_models[id] = &fft_dpu_model;
  app_model.a_models[fft_dpu_id] = &fft_dpu_model;

  /* USER Component initialization code */
  app_model.a_models[fft_dpu_id]->stream_params.stream_id = FFT_DPU_STREAM_ID;
  app_model.a_models[fft_dpu_id]->stream_params.usb_ep = FFT_DPU_USB_EP;
  app_model.a_models[fft_dpu_id]->stream_params.usb_dps = FFT_DPU_USB_DPS;
  app_model.a_models[fft_dpu_id]->packet_size = FFT_DPU_PACKECT_SIZE;
  app_model.a_models[fft_dpu_id]->dim = FFT_DPU_DIM;
  app_model.a_models[fft_dpu_id]->fft_sample_freq = FFT_DPU_SAMPLE_FREQ;
  app_model.a_models[fft_dpu_id]->fft_len = FFT_DPU_FFT_LEN;
  app_model.a_models[fft_dpu_id]->enable = true;
  return 0;
}
char *fft_dpu_get_key(void)
{
  return "fft_dpu";
}

uint8_t fft_dpu_get_enable(bool *value)
{
  *value = app_model.a_models[fft_dpu_id]->enable;
  return 0;
}
uint8_t fft_dpu_get_data_type(char **value)
{
  *value = "float";
  return 0;
}
uint8_t fft_dpu_get_fft_length(int32_t *value)
{
  *value = app_model.a_models[fft_dpu_id]->fft_len;
  return 0;
}
uint8_t fft_dpu_get_fft_sample_freq(int32_t *value)
{
  *value = app_model.a_models[fft_dpu_id]->fft_sample_freq;
  return 0;
}
uint8_t fft_dpu_get_usb_dps(int32_t *value)
{
  *value = app_model.a_models[fft_dpu_id]->stream_params.usb_dps;
  return 0;
}
uint8_t fft_dpu_get_algorithm_type(int32_t *value)
{
  *value = 0;
  return 0;
}
uint8_t fft_dpu_get_stream_id(int8_t *value)
{
  *value = app_model.a_models[fft_dpu_id]->stream_params.stream_id;
  return 0;
}
uint8_t fft_dpu_get_ep_id(int8_t *value)
{
  *value = app_model.a_models[fft_dpu_id]->stream_params.usb_ep;
  return 0;
}
uint8_t fft_dpu_set_enable(bool value)
{
  app_model.a_models[fft_dpu_id]->enable = value;
  return 0;
}

/* IMP23ABSU_MIC PnPL Component ----------------------------------------------*/
static SensorModel_t imp23absu_mic_model;

uint8_t imp23absu_mic_comp_init(void)
{
  imp23absu_mic_model.comp_name = imp23absu_mic_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "imp23absu", COM_TYPE_MIC);
  imp23absu_mic_model.id = id;
  imp23absu_mic_model.sensor_status = SMSensorGetStatus(id);
  imp23absu_mic_model.stream_params.stream_id = -1;
  imp23absu_mic_model.stream_params.usb_ep = -1;
  char default_notes[SENSOR_NOTES_LEN] = "\0";
  imp23absu_mic_set_sensor_annotation(default_notes);
  /* USER code */
  imp23absu_mic_model.gui_plot_graph_enable = true;
  app_model.s_models[id] = &imp23absu_mic_model;
  __stream_control(NULL, true);

  /* USER Component initialization code */
  return 0;
}
char *imp23absu_mic_get_key(void)
{
  return "imp23absu_mic";
}

uint8_t imp23absu_mic_get_odr(float *value) /* TODO: uint8_t imp23absu_mic_get_frequency(uint32_t *value) */
{
  /* Status update to check if the value has been updated by the FW */
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp23absu_mic_model.sensor_status.type.audio.frequency;
  return 0;
}
uint8_t imp23absu_mic_get_aop(float *value) /* TODO: uint8_t imp23absu_mic_get_volume(uint8_t *value) */
{
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp23absu_mic_model.sensor_status.type.audio.volume;
  return 0;
}
uint8_t imp23absu_mic_get_enable(bool *value)
{
  /* Status update to check if the value has been updated by the FW */
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensor_status = SMSensorGetStatus(id);
  //*value = imp23absu_mic_model.sensor_status.is_active;
  *value = imp23absu_mic_model.gui_plot_graph_enable;
  return 0;
}
uint8_t imp23absu_mic_get_samples_per_ts(int32_t *value)
{
  *value = imp23absu_mic_model.stream_params.spts;
  return 0;
}
uint8_t imp23absu_mic_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}
uint8_t imp23absu_mic_get_ioffset(float *value)
{
  *value = imp23absu_mic_model.stream_params.ioffset;
  return 0;
}
uint8_t imp23absu_mic_get_measodr(float *value) /* TODO: delete */
{
  /* Status update to check if the value has been updated by the FW */
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp23absu_mic_model.sensor_status.type.audio.frequency;
  return 0;
}
uint8_t imp23absu_mic_get_usb_dps(int32_t *value)
{
  *value = imp23absu_mic_model.stream_params.usb_dps;
  return 0;
}
uint8_t imp23absu_mic_get_sd_dps(int32_t *value)
{
  *value = imp23absu_mic_model.stream_params.sd_dps;
  return 0;
}
uint8_t imp23absu_mic_get_sensitivity(float *value) /* TODO: uint8_t imp23absu_mic_get_resolution(uint8_t *value) */
{
  *value = 0.000030517578125; //2/(2^imp23absu_mic_model.sensor_status.type.audio.resolution);
  return 0;
}
uint8_t imp23absu_mic_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t imp23absu_mic_get_sensor_annotation(char **value)
{
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensor_status = SMSensorGetStatus(id);
  *value = imp23absu_mic_model.annotation;
  return 0;
}
uint8_t imp23absu_mic_get_sensor_category(int32_t *value)
{
  *value = imp23absu_mic_model.sensor_status.isensor_class;
  return 0;
}
uint8_t imp23absu_mic_get_stream_id(int8_t *value)
{
  *value = imp23absu_mic_model.stream_params.stream_id;
  return 0;
}
uint8_t imp23absu_mic_get_ep_id(int8_t *value)
{
  *value = imp23absu_mic_model.stream_params.usb_ep;
  return 0;
}
uint8_t imp23absu_mic_set_enable(bool value)
{
  imp23absu_mic_model.gui_plot_graph_enable = value;
  return 0;
}
uint8_t imp23absu_mic_set_sensor_annotation(const char *value)
{
  strcpy(imp23absu_mic_model.annotation, value);
  return 0;
}

/* Log Controller PnPL Component ---------------------------------------------*/
uint8_t log_controller_comp_init(void)
{
  app_model.log_controller_model.comp_name = log_controller_get_key();

  /* USER Component initialization code */
  return 0;
}
char *log_controller_get_key(void)
{
  return "log_controller";
}

uint8_t log_controller_get_log_status(bool *value)
{
  /* USER Code */
  return 0;
}
uint8_t log_controller_get_sd_mounted(bool *value)
{
  /* USER Code */
  return 0;
}
uint8_t log_controller_get_controller_type(int32_t *value)
{
  *value = 2; /* 0 == HSD log controller, 1 == App classifier controller, 2 = generic log controller */
  return 0;
}
uint8_t log_controller_save_config(ILog_Controller_t *ifn)
{
  //ILog_Controller_save_config(ifn);
  return 0;
}
uint8_t log_controller_start_log(ILog_Controller_t *ifn, int32_t interface)
{
  app_model.log_controller_model.interface = interface;

  /* Update algorithm stream param at start log cmd */
  fft_dpu_comp_init();

  __stream_control(ifn, TRUE);

  ILog_Controller_start_log(ifn, interface);

  RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdate, RTC_FORMAT_BIN);

  _tm t =
  {
    .tm_year = sdate.Year + 2000,
    .tm_mon = sdate.Month - 1,
    .tm_mday = sdate.Date,
    .tm_hour = stime.Hours,
    .tm_min = stime.Minutes,
    .tm_sec = stime.Seconds
  };

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
  sprintf(app_model.acquisition_info_model.start_time, "%04d-%02d-%02dT%02d:%02d:%02d", t.tm_year, t.tm_mon + 1,
          t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

  /* last part not done in sprintf to avoid a warning  */
  app_model.acquisition_info_model.start_time[19] = '.';
  app_model.acquisition_info_model.start_time[20] = '0';
  app_model.acquisition_info_model.start_time[21] = '0';
  app_model.acquisition_info_model.start_time[22] = '0';
  app_model.acquisition_info_model.start_time[23] = 'Z';
  app_model.acquisition_info_model.start_time[24] = '\0';

  return 0;
}
uint8_t log_controller_stop_log(ILog_Controller_t *ifn)
{
  PnPLGenerateAcquisitionUUID(app_model.acquisition_info_model.uuid);
  ILog_Controller_stop_log(ifn);
  TMCalculateEndTime(app_model.acquisition_info_model.end_time);
  return 0;
}
uint8_t log_controller_set_time(ILog_Controller_t *ifn, const char *datetime)
{
  char datetimeStr[3];

  //internal input format: yyyyMMdd_hh_mm_ss

  RTC_DateTypeDef sdate;
  RTC_TimeTypeDef stime;

  /** extract year string (only the last two digit). It will be necessary to add 2000*/
  datetimeStr[0] = datetime[2];
  datetimeStr[1] = datetime[3];
  datetimeStr[2] = '\0';
  sdate.Year = atoi(datetimeStr);

  /** extract month string */
  datetimeStr[0] = datetime[4];
  datetimeStr[1] = datetime[5];
  sdate.Month = atoi(datetimeStr);

  /** extract day string */
  datetimeStr[0] = datetime[6];
  datetimeStr[1] = datetime[7];
  sdate.Date = atoi(datetimeStr);

  /** Week day initialization (not used)*/
  sdate.WeekDay = RTC_WEEKDAY_MONDAY; //Not used

  /** extract hour string */
  datetimeStr[0] = datetime[9];
  datetimeStr[1] = datetime[10];
  stime.Hours = atoi(datetimeStr);

  /** extract minute string */
  datetimeStr[0] = datetime[12];
  datetimeStr[1] = datetime[13];
  stime.Minutes = atoi(datetimeStr);

  /** extract second string */
  datetimeStr[0] = datetime[15];
  datetimeStr[1] = datetime[16];
  stime.Seconds = atoi(datetimeStr);

  /** not used */
  //stime.TimeFormat = RTC_HOURFORMAT12_AM;
  stime.SecondFraction = 0;
  stime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  stime.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    while (1);
  }
  if (HAL_RTC_SetDate(&hrtc, &sdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    while (1);
  }

  return 0;
}
uint8_t log_controller_switch_bank(ILog_Controller_t *ifn)
{
  //ILog_Controller_switch_bank(ifn);
  return 0;
}

/* Firmware Information PnPL Component ---------------------------------------*/
uint8_t firmware_info_comp_init(void)
{
  app_model.firmware_info_model.comp_name = firmware_info_get_key();

  /* USER Component initialization code */
  return 0;
}
char *firmware_info_get_key(void)
{
  return "firmware_info";
}

uint8_t firmware_info_get_alias(char **value)
{
  *value = app_model.firmware_info_model.alias;
  return 0;
}
uint8_t firmware_info_get_fw_name(char **value)
{
  *value = "FP-SNS-DATALOG2_UltrasoundFFT";
  return 0;
}
uint8_t firmware_info_get_fw_version(char **value)
{
  *value = "2.0.0";
  return 0;
}
uint8_t firmware_info_get_part_number(char **value)
{
  *value = "FP-SNS-DATALOG2";
  return 0;
}
uint8_t firmware_info_get_device_url(char **value)
{
  *value = "https://www.st.com/stwinbox";
  return 0;
}
uint8_t firmware_info_get_fw_url(char **value)
{
  *value = "https://github.com/STMicroelectronics/fp-sns-datalog2";
  return 0;
}
uint8_t firmware_info_get_mac_address(char **value)
{
  *value = "";
  return 0;
}
uint8_t firmware_info_set_alias(const char *value)
{
  strcpy(app_model.firmware_info_model.alias, value);
  return 0;
}

/* Device Information PnPL Component -----------------------------------------*/
uint8_t DeviceInformation_comp_init(void)
{

  /* USER Component initialization code */
  return 0;
}
char *DeviceInformation_get_key(void)
{
  return "DeviceInformation";
}

uint8_t DeviceInformation_get_manufacturer(char **value)
{
  *value = "STMicroelectronics";
  return 0;
}
uint8_t DeviceInformation_get_model(char **value)
{
  *value = "STEVAL-STWINBX1";
  return 0;
}
uint8_t DeviceInformation_get_swVersion(char **value)
{
  *value = "2.0.0";
  return 0;
}
uint8_t DeviceInformation_get_osName(char **value)
{
  *value = "AzureRTOS";
  return 0;
}
uint8_t DeviceInformation_get_processorArchitecture(char **value)
{
  *value = "ARM Cortex-M33";
  return 0;
}
uint8_t DeviceInformation_get_processorManufacturer(char **value)
{
  *value = "STMicroelectronics";
  return 0;
}
uint8_t DeviceInformation_get_totalStorage(float *value)
{
  *value = 0;
  return 0;
}
uint8_t DeviceInformation_get_totalMemory(float *value)
{
  *value = (SRAM1_SIZE + SRAM2_SIZE + SRAM3_SIZE + SRAM4_SIZE) / 1024;
  return 0;
}

/* Acquisition Information PnPL Component ------------------------------------*/
uint8_t acquisition_info_comp_init(void)
{
  app_model.acquisition_info_model.comp_name = acquisition_info_get_key();
  char default_alias[DEVICE_ALIAS_LENGTH] = "STWIN_BOX_001";
  firmware_info_set_alias(default_alias);
  acquisition_info_set_name("STWIN.Box_acquisition");
  acquisition_info_set_description("");
  return 0;
}
char *acquisition_info_get_key(void)
{
  return "acquisition_info";
}

uint8_t acquisition_info_get_name(char **value)
{
  *value = app_model.acquisition_info_model.name;
  return 0;
}
uint8_t acquisition_info_get_description(char **value)
{
  *value = app_model.acquisition_info_model.description;
  return 0;
}
uint8_t acquisition_info_get_uuid(char **value)
{
  *value = app_model.acquisition_info_model.uuid;
  return 0;
}
uint8_t acquisition_info_get_start_time(char **value)
{
  *value = app_model.acquisition_info_model.start_time;
  return 0;
}
uint8_t acquisition_info_get_end_time(char **value)
{
  *value = app_model.acquisition_info_model.end_time;
  return 0;
}
uint8_t acquisition_info_get_data_ext(char **value)
{
  *value = ".dat";
  return 0;
}
uint8_t acquisition_info_get_tags(JSON_Value *value)
{
  /* USER Code */
  return 0;
}
uint8_t acquisition_info_get_interface(char **value)
{
  *value = "USB";
  return 0;
}
uint8_t acquisition_info_get_schema_version(char **value)
{
  *value = "2.0.0";
  return 0;
}
uint8_t acquisition_info_set_name(const char *value)
{
  if (strlen(value) != 0)
  {
    strcpy(app_model.acquisition_info_model.name, value);
  }
  return 0;
}
uint8_t acquisition_info_set_description(const char *value)
{
  strcpy(app_model.acquisition_info_model.description, value);
  return 0;
}

/* USER Code : --> compute stream ids */
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status)
{
  int8_t i;
  if (status) //set stream ids
  {
    int8_t j, st_i = 0;
    uint16_t proposed_fifoWM = 1;
    //sort stream id by bandwidth
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      if (app_model.s_models[i] != NULL)
      {
        if (app_model.s_models[i]->sensor_status.is_active == true)
        {
          if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_MEMS)
          {
            app_model.s_models[i]->stream_params.bandwidth = app_model.s_models[i]->sensor_status.type.mems.odr * SMGetnBytesPerSample(i);
          }
          else if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_AUDIO)
          {
            app_model.s_models[i]->stream_params.bandwidth = app_model.s_models[i]->sensor_status.type.audio.frequency * SMGetnBytesPerSample(i);
          }
          else
          {
            /* TODO: add support for other ISENSOR_CLASS */
          }

          if (app_model.log_controller_model.interface == LOG_CTRL_MODE_SD)
          {
            /* 330ms of sensor data. Access to SD is optimized when buffer dimension is multiple of 512 */
            app_model.s_models[i]->stream_params.sd_dps = (uint32_t)(app_model.s_models[i]->stream_params.bandwidth * 0.33f);
            app_model.s_models[i]->stream_params.sd_dps = app_model.s_models[i]->stream_params.sd_dps - (app_model.s_models[i]->stream_params.sd_dps % 512) + 512;

            /*********** IS IT STILL multiple of 512?  **************/
            if (app_model.s_models[i]->stream_params.sd_dps > app_model.s_models[i]->stream_params.bandwidth)
            {
              app_model.s_models[i]->stream_params.sd_dps = (uint32_t)app_model.s_models[i]->stream_params.bandwidth + 8;
            }

            proposed_fifoWM = app_model.s_models[i]->stream_params.sd_dps / SMGetnBytesPerSample(i) / 2;
            if (proposed_fifoWM == 0)
            {
              proposed_fifoWM = 1;
            }
            sys_error_code_t ret = SMSensorSetFifoWM(i, proposed_fifoWM);
            if (ret != SYS_NO_ERROR_CODE)
            {

            }
          }
          else if (app_model.log_controller_model.interface == LOG_CTRL_MODE_USB)
          {
            /* in case of slow sensor send 1 sample for each usb packet */
            float low_odr = 0;
            if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_MEMS)
            {
              low_odr = app_model.s_models[i]->sensor_status.type.mems.odr;
            }
            else if (app_model.s_models[i]->sensor_status.isensor_class == ISENSOR_CLASS_AUDIO)
            {
              low_odr = app_model.s_models[i]->sensor_status.type.audio.frequency;
            }
            else
            {
              /* TODO: add support for other ISENSOR_CLASS */
            }

            if (low_odr <= 20)
            {
              /* When there's a timestamp, more then one packet will be sent */
              app_model.s_models[i]->stream_params.usb_dps = SMGetnBytesPerSample(i) + 8;  /* 8 = timestamp dimension in bytes */
              proposed_fifoWM = 1;
              sys_error_code_t ret = SMSensorSetFifoWM(i, proposed_fifoWM);
              if (ret != SYS_NO_ERROR_CODE)
              {

              }
            }
            else
            {
              /* 50ms of sensor data; when there's a timestamp packets will be sent fastly */
              app_model.s_models[i]->stream_params.usb_dps = (uint32_t)(app_model.s_models[i]->stream_params.bandwidth * 0.05f);
              if (app_model.s_models[i]->stream_params.usb_dps > 7000)
              {
                app_model.s_models[i]->stream_params.usb_dps = 7000; // set a limit to avoid buffer to big
              }
              else if (app_model.s_models[i]->stream_params.usb_dps < SMGetnBytesPerSample(i) + 8)
              {
                /* In case usb_dps is a very low value, verify the setup to send at least 1 sensor data + timestamp */
                app_model.s_models[i]->stream_params.usb_dps = SMGetnBytesPerSample(i) + 8;
              }

              proposed_fifoWM = app_model.s_models[i]->stream_params.usb_dps / SMGetnBytesPerSample(i) / 2;
              if (proposed_fifoWM == 0)
              {
                proposed_fifoWM = 1;
              }
              sys_error_code_t ret = SMSensorSetFifoWM(i, proposed_fifoWM);
              if (ret != SYS_NO_ERROR_CODE)
              {

              }
            }
#ifdef SYS_DEBUG
            SensorDescriptor_t descriptor = SMSensorGetDescription(i);
            float ms = app_model.s_models[i]->stream_params.usb_dps / app_model.s_models[i]->stream_params.bandwidth;
            SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("**** %s, odr: %f, DPS: %d, ms: %f, proposed FIFO WM: %d \r\n", descriptor.p_name,
                                               app_model.s_models[i]->sensor_status.type.mems.odr, app_model.s_models[i]->stream_params.usb_dps, ms, proposed_fifoWM));
#endif
          }

          app_model.s_models[i]->stream_params.stream_id = st_i;
          st_i++;

          if (i > 0)
          {
            j = i - 1;
            while (j >= 0)
            {
              if (app_model.s_models[j] != NULL)
              {
                if (app_model.s_models[j]->stream_params.bandwidth <= app_model.s_models[i]->stream_params.bandwidth)
                {
                  if (app_model.s_models[j]->stream_params.stream_id >= 0)
                  {
                    if (app_model.s_models[j]->stream_params.stream_id < app_model.s_models[i]->stream_params.stream_id)
                    {
                      app_model.s_models[i]->stream_params.stream_id = app_model.s_models[j]->stream_params.stream_id;
                    }
                    app_model.s_models[j]->stream_params.stream_id++;
                  }
                }
              }
              j--;
            }
          }
        }
        else
        {
          app_model.s_models[i]->stream_params.bandwidth = 0;
          app_model.s_models[i]->stream_params.stream_id = -1;
        }
      }
    }
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      if (app_model.s_models[i] != NULL)
      {
        int8_t stream = app_model.s_models[i]->stream_params.stream_id;
        if (stream < N_MAX_EP - 1)
        {
          app_model.s_models[i]->stream_params.usb_ep = stream;
        }
        else
        {
          app_model.s_models[i]->stream_params.usb_ep = N_MAX_EP - 1;
        }
      }
    }

  }
  else
  {
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      app_model.s_models[i]->stream_params.stream_id = -1;
      app_model.s_models[i]->stream_params.usb_ep = -1;
      app_model.s_models[i]->stream_params.bandwidth = 0;
    }

  }
  return 0;
}

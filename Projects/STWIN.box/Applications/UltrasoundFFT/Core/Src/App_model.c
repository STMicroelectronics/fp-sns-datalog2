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

#include "App_model.h"
#include <string.h>
#include <stdio.h>
/* USER includes */
#include "services/SQuery.h"
#include "services/sysdebug.h"
#include "rtc.h"


/* USER private functions prototypes */

/* USER define */
#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DT, level, message)

AppModel_t app_model;

AppModel_t* getAppModel(void)
{
  return &app_model;
}

/* Private function declaration ===================================================== */
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status);

/* Device Components APIs =========================================================== */

/* Firmware Information PnPL Component ============================================== */
uint8_t firmware_info_comp_init(void)
{
   
   /* USER Component initialization code */
   return 0;
}
char* firmware_info_get_key(void)
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
  *value = "UltrasoundFFT";
   return 0;
}
uint8_t firmware_info_get_fw_version(char **value)
{
  *value = "1.0.0";
   return 0;
}
uint8_t firmware_info_get_serial_number(char **value)
{
  *value = "STEVAL-STWINBX1";
   return 0;
}
uint8_t firmware_info_get_device_url(char **value)
{
  *value = "www.st.com/stwinbox";
   return 0;
}
uint8_t firmware_info_get_fw_url(char **value)
{
  *value = "www.st.com"; //TODO fix this value
   return 0;
}
uint8_t firmware_info_set_alias(const char *value)
{
   strcpy(app_model.firmware_info_model.alias, value);
   return 0;
}

/* FFT DPU PnPL Component =========================================================== */
AlgorithmModel_t fft_dpu_model;

uint8_t fft_dpu_comp_init(void)
{
   fft_dpu_model.comp_name = fft_dpu_get_key();

   app_model.a_models[0] = &fft_dpu_model;

   /* USER Component initialization code */
   app_model.a_models[0]->streamParams.stream_id = FFT_DPU_STREAM_ID;
   app_model.a_models[0]->streamParams.usb_ep = FFT_DPU_USB_EP;
   app_model.a_models[0]->streamParams.usb_dps = FFT_DPU_USB_DPS;
   app_model.a_models[0]->packet_size = FFT_DPU_PACKECT_SIZE;
   app_model.a_models[0]->dim = FFT_DPU_DIM;
   app_model.a_models[0]->fft_sample_freq = FFT_DPU_SAMPLE_FREQ;
   app_model.a_models[0]->fft_len = FFT_DPU_FFT_LEN;
   app_model.a_models[0]->enable = true;
   return 0;
}
char* fft_dpu_get_key(void)
{
   return "fft_dpu";
}

uint8_t fft_dpu_get_FFTLenght(int32_t *value)
{
  /* USER Code */
   return 0;
}
uint8_t fft_dpu_get_graphType(char **value)
{
  /* USER Code */
   return 0;
}
uint8_t fft_dpu_get_enable(bool *value)
{
   *value = app_model.a_models[0]->enable;
   return 0;
}
uint8_t fft_dpu_get_stream_id(int8_t *value)
{
   *value = app_model.a_models[0]->streamParams.stream_id;
   return 0;
}
uint8_t fft_dpu_get_ep_id(int8_t *value)
{
   *value= app_model.a_models[0]->streamParams.usb_ep;
   return 0;
}
uint8_t fft_dpu_get_fft_length(int32_t *value)
{
    *value = app_model.a_models[0]->fft_len;
    return 0;
}
uint8_t fft_dpu_get_algorithm_type(char **value)
{
    *value = "fft";
    return 0;
}
uint8_t fft_dpu_get_fft_sample_freq(int32_t *value)
{
    *value = app_model.a_models[0]->fft_sample_freq;
    return 0;
}
uint8_t fft_dpu_set_enable(bool value)
{
   app_model.a_models[0]->enable = value;
   return 0;
}

uint8_t fft_dpu_get_usb_dps(int32_t *value)
{
	*value= app_model.a_models[0]->streamParams.usb_dps;
	return 0;
}

uint8_t fft_dpu_get_data_type(char **value)
{
  *value = "float";
  return 0;
}
uint8_t fft_dpu_get_packet_size(int32_t *value)
{
  *value = app_model.a_models[0]->packet_size;
  return 0;
}
uint8_t fft_dpu_get_dim(int32_t *value)
{
    *value = app_model.a_models[0]->dim;
    return 0;
}

/* Log Controller PnPL Component ==================================================== */
uint8_t log_controller_comp_init(void)
{
   app_model.log_controller_model.comp_name = log_controller_get_key();

   /* USER Component initialization code */
   return 0;
}
char* log_controller_get_key(void)
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
   /* USER Code */
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
       { .tm_year = sdate.Year + 2000, .tm_mon = sdate.Month, .tm_mday = sdate.Date, .tm_hour = stime.Hours, .tm_min = stime.Minutes, .tm_sec = stime.Seconds };
   TMSetStartTime(t);
   sprintf(app_model.acquisition_info_model.start_time, "%04d-%02d-%02dT%02d:%02d:%02d", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

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

  if(HAL_RTC_SetTime(&hrtc, &stime, RTC_FORMAT_BIN) != HAL_OK)
  {
    while(1)
      ;
  }
  if(HAL_RTC_SetDate(&hrtc, &sdate, RTC_FORMAT_BIN) != HAL_OK)
  {
    while(1)
      ;
  }

  return 0;
}

/* IMP23ABSU_MIC PnPLComponent ====================================================== */
SensorModel_t imp23absu_mic_model;

uint8_t imp23absu_mic_comp_init(void)
{
  imp23absu_mic_model.comp_name = imp23absu_mic_get_key();

  SQuery_t querySM;
  SQInit(&querySM, SMGetSensorManager());
  uint16_t id = SQNextByNameAndType(&querySM, "imp23absu", COM_TYPE_MIC);
  imp23absu_mic_model.id = id;
  imp23absu_mic_model.sensorStatus = SMSensorGetStatus(id);
  imp23absu_mic_model.streamParams.stream_id = -1;
  imp23absu_mic_model.streamParams.usb_ep = -1;
  /* User code */
  imp23absu_mic_model.gui_plot_graph_enable = true;
#if (HSD_USE_DUMMY_DATA == 1)
  imp23absu_mic_set_samples_per_ts__val(0);
#else
  imp23absu_mic_set_samples_per_ts__val(1000);
#endif
  app_model.s_models[id] = &imp23absu_mic_model;
  __stream_control(NULL, true);
  return 0;
}

char* imp23absu_mic_get_key(void)
{
  return "imp23absu_mic";
}

uint8_t imp23absu_mic_get_odr(float *value)
{
  /* Status update to check if the value has been updated by the FW */
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensorStatus = SMSensorGetStatus(id);
  *value = imp23absu_mic_model.sensorStatus.ODR;
  return 0;
}
uint8_t imp23absu_mic_get_aop(float *value)
{
  *value = imp23absu_mic_model.sensorStatus.FS;
  return 0;
}
uint8_t imp23absu_mic_get_enable(bool *value)
{
  /* Status update to check if the value has been updated by the FW */
  *value = imp23absu_mic_model.gui_plot_graph_enable;
  return 0;
}
uint8_t imp23absu_mic_get_samples_per_ts__val(int32_t *value)
{
  *value = imp23absu_mic_model.streamParams.spts;
  return 0;
}
uint8_t imp23absu_mic_get_samples_per_ts__min(int32_t *value)
{
  *value = 0;
  return 0;
}
uint8_t imp23absu_mic_get_samples_per_ts__max(int32_t *value)
{
#if (HSD_USE_DUMMY_DATA == 1)
  *value = 0;
#else
  *value = 1000;
#endif
  return 0;
}
uint8_t imp23absu_mic_get_dim(int32_t *value)
{
  *value = 1;
  return 0;
}
uint8_t imp23absu_mic_get_ioffset(float *value)
{
  *value = imp23absu_mic_model.streamParams.ioffset;
  return 0;
}
uint8_t imp23absu_mic_get_measodr(float *value)
{
  /* Status update to check if the value has been updated by the FW */
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensorStatus = SMSensorGetStatus(id);
  *value = imp23absu_mic_model.sensorStatus.MeasuredODR;
  return 0;
}
uint8_t imp23absu_mic_get_usb_dps(int32_t *value)
{
  *value = imp23absu_mic_model.streamParams.usb_dps;
  return 0;
}
uint8_t imp23absu_mic_get_sd_dps(int32_t *value)
{
  *value = imp23absu_mic_model.streamParams.sd_dps;
  return 0;
}
uint8_t imp23absu_mic_get_sensitivity(float *value)
{
  /* Status update to check if the value has been updated by the FW */
  uint16_t id = imp23absu_mic_model.id;
  imp23absu_mic_model.sensorStatus = SMSensorGetStatus(id);
  *value = imp23absu_mic_model.sensorStatus.Sensitivity;
  return 0;
}
uint8_t imp23absu_mic_get_data_type(char **value)
{
  *value = "int16";
  return 0;
}
uint8_t imp23absu_mic_get_stream_id(int8_t *value)
{
  *value = imp23absu_mic_model.streamParams.stream_id;
  return 0;
}
uint8_t imp23absu_mic_get_ep_id(int8_t *value)
{
  *value = imp23absu_mic_model.streamParams.usb_ep;
  return 0;
}
uint8_t imp23absu_mic_set_odr(float value)
{
  sys_error_code_t ret = SMSensorSetODR(imp23absu_mic_model.id, value);
  if(ret == SYS_NO_ERROR_CODE)
  {
    imp23absu_mic_model.sensorStatus.ODR = value;
    __stream_control(NULL, true);
    return ret;
  }
  return 1;
}
uint8_t imp23absu_mic_set_enable(bool value)
{
  sys_error_code_t ret = SYS_NO_ERROR_CODE;
  imp23absu_mic_model.gui_plot_graph_enable = value;
  return ret;
}
uint8_t imp23absu_mic_set_samples_per_ts__val(int32_t value)
{
  int32_t min_v = 0;
  imp23absu_mic_get_samples_per_ts__min(&min_v);
  int32_t max_v = 0;
  imp23absu_mic_get_samples_per_ts__max(&max_v);
  if(value >= min_v && value <= max_v)
  {
    imp23absu_mic_model.streamParams.spts = value;
  }
  return 0;
}
// Acquisition Information PnPL Component ==============================================
uint8_t acquisition_info_comp_init(void)
{
  app_model.acquisition_info_model.comp_name = acquisition_info_get_key();
  char default_alias[DEVICE_ALIAS_LENGTH] = "STWIN_BOX_001";
  firmware_info_set_alias(default_alias);
  acquisition_info_set_name("STWIN.Box_acquisition");
  acquisition_info_set_description("");
  return 0;
}

char* acquisition_info_get_key(void)
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
   /* USER Code */
   return 0;
}
uint8_t acquisition_info_set_name(const char *value)
{
  if(strlen(value) != 0)
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

/* Device Information PnPL Component ================================================ */
uint8_t DeviceInformation_comp_init(void)
{
   
   /* USER Component initialization code */
   return 0;
}
char* DeviceInformation_get_key(void)
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
   //USER Code
   return 0;
}

//USER CODE: --> compute stream ids
static uint8_t __stream_control(ILog_Controller_t *ifn, bool status)
{
  int8_t i;
  if(status)
  {
    int8_t j, st_i = 0;
    uint16_t proposed_fifoWM = 1;
    //sort stream id by bandwidth
    for(i = 0; i < SENSOR_NUMBER; i++)
    {
      if(app_model.s_models[i] != NULL)
      {
        if(app_model.s_models[i]->sensorStatus.IsActive == true)
        {
          app_model.s_models[i]->streamParams.bandwidth = app_model.s_models[i]->sensorStatus.ODR * SMGetnBytesPerSample(i);

          if(app_model.log_controller_model.interface == LOG_CTRL_MODE_SD)
          {
            app_model.s_models[i]->streamParams.sd_dps = (uint32_t)(app_model.s_models[i]->streamParams.bandwidth*0.33f);
            app_model.s_models[i]->streamParams.sd_dps = app_model.s_models[i]->streamParams.sd_dps - (app_model.s_models[i]->streamParams.sd_dps % 512) + 512;

            /*********** IS IT STILL multiple of 512?  **************/
            if (app_model.s_models[i]->streamParams.sd_dps > app_model.s_models[i]->streamParams.bandwidth)
            {
              app_model.s_models[i]->streamParams.sd_dps = (uint32_t)app_model.s_models[i]->streamParams.bandwidth + 8;
            }
          }
          else if (app_model.log_controller_model.interface == LOG_CTRL_MODE_USB)
          {

            /* in case of slow sensor send 1 sample for each usb packet */
            if(app_model.s_models[i]->sensorStatus.ODR <= 20)
            {
              /* When there's a timestamp, more then one packet will be sent */
              app_model.s_models[i]->streamParams.usb_dps = SMGetnBytesPerSample(i) + 8;  /* 8 = timestamp dimension in bytes */
              proposed_fifoWM = 1;
              sys_error_code_t ret = SMSensorSetFifoWM(i, proposed_fifoWM);
              if(ret != SYS_NO_ERROR_CODE)
              {

              }
            }
            else
            {
              /* 50ms of sensor data; when there's a timestamp packets will be sent fastly */
              app_model.s_models[i]->streamParams.usb_dps = (uint32_t)(app_model.s_models[i]->streamParams.bandwidth*0.05f);
              if(app_model.s_models[i]->streamParams.usb_dps > 7000)
              {
                app_model.s_models[i]->streamParams.usb_dps = 7000; // set a limit to avoid buffer to big
              }
              else if(app_model.s_models[i]->streamParams.usb_dps < SMGetnBytesPerSample(i) + 8)
              {
                /* In case usb_dps is a very low value, verify the setup to send at least 1 sensor data + timestamp */
                app_model.s_models[i]->streamParams.usb_dps = SMGetnBytesPerSample(i) + 8;
              }

              proposed_fifoWM = app_model.s_models[i]->streamParams.usb_dps/SMGetnBytesPerSample(i)/2;
              if(proposed_fifoWM==0)
              {
                proposed_fifoWM = 1;
              }
              sys_error_code_t ret = SMSensorSetFifoWM(i, proposed_fifoWM);
              if(ret != SYS_NO_ERROR_CODE)
              {

              }
            }
#ifdef SYS_DEBUG
            SensorDescriptor_t descriptor = SMSensorGetDescription(i);
            float ms = app_model.s_models[i]->streamParams.usb_dps/app_model.s_models[i]->streamParams.bandwidth;
            SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("**** %s, ODR: %f, DPS: %d, ms: %f, proposed FIFO WM: %d \r\n", descriptor.Name, app_model.s_models[i]->sensorStatus.ODR, app_model.s_models[i]->streamParams.usb_dps, ms, proposed_fifoWM));
#endif
          }

          app_model.s_models[i]->streamParams.stream_id = st_i;
          st_i++;

          if(i > 0)
          {
            j = i - 1;
            while(j >= 0)
            {
              if(app_model.s_models[j] != NULL){
                if(app_model.s_models[j]->streamParams.bandwidth <= app_model.s_models[i]->streamParams.bandwidth)
                {
                  if(app_model.s_models[j]->streamParams.stream_id >=0)
                  {
                    if(app_model.s_models[j]->streamParams.stream_id < app_model.s_models[i]->streamParams.stream_id)
                    {
                      app_model.s_models[i]->streamParams.stream_id = app_model.s_models[j]->streamParams.stream_id;
                    }
                    app_model.s_models[j]->streamParams.stream_id++;
                  }
                }
              }
              j--;
            }
          }
        }
        else
        {
          app_model.s_models[i]->streamParams.bandwidth = 0;
          app_model.s_models[i]->streamParams.stream_id = -1;
        }
      }
    }
    for(i = 0; i < SENSOR_NUMBER; i++)
    {
      if(app_model.s_models[i] != NULL)
      {
        int8_t stream = app_model.s_models[i]->streamParams.stream_id;
        if(stream < N_MAX_EP - 1)
        {
          app_model.s_models[i]->streamParams.usb_ep = stream;
        }
        else
        {
          app_model.s_models[i]->streamParams.usb_ep = N_MAX_EP - 1;
        }
      }
    }

  }
  else
  {
    for(i = 0; i < SENSOR_NUMBER; i++)
    {
      app_model.s_models[i]->streamParams.stream_id = -1;
      app_model.s_models[i]->streamParams.usb_ep = -1;
      app_model.s_models[i]->streamParams.bandwidth = 0;
    }

  }
  return 0;
}

/**
  ******************************************************************************
  * @file    App_Model.c
  * @author  SRA
  * @brief   App Application Model and PnPL Components APIs
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
  * This file has been auto generated from the following Device Template Model:
  * dtmi:appconfig:b_u585i_iot02a:fpSnsDatalog2_datalog2;1
  *
  * Created by: DTDL2PnPL_cGen version 2.1.0
  *
  * WARNING! All changes made to this file will be lost if this is regenerated
  ******************************************************************************
  */

#include "App_model.h"
#include <string.h>
#include <stdio.h>
/* USER includes -------------------------------------------------------------*/
#include "services/SQuery.h"
#include "services/sysdebug.h"
#include "ux_user.h"
/* USER defines --------------------------------------------------------------*/
#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_DT, level, message)

#define SC_DL2_PROTOCOL_COUNTER_SIZE     4U
#define SC_DL2_PROTOCOL_TIMESTAMP_SIZE   8U
/* Max DPS for USB */
#define SC_USB_DPS_MAX                7000U
/* Under this limit the stream is considered "slow" */
#define SC_USB_SLOW_ODR_LIMIT_HZ      20.0f
/* Maximum time between two consecutive stream packets */
#define SC_USB_MAX_PACKETS_PERIOD     0.05f
/* Maximum data bandwidth supported (byte) */
#define SC_SAFE_BANDWIDTH             800000

/* USER private function prototypes ------------------------------------------*/
static sys_error_code_t __sc_set_usb_stream_params(uint32_t id);
static sys_error_code_t __sc_set_fifo_wtm(uint32_t id);
static void __sc_set_usb_enpoints(void);
static void __sc_reset_stream_params(void);

AppModel_t app_model;

AppModel_t *getAppModel(void)
{
  return &app_model;
}

uint8_t addSensorToAppModel(uint16_t id, SensorModel_t *model)
{
  if (id < SENSOR_NUMBER)
  {
    app_model.s_models[id] = model;
    return PNPL_NO_ERROR_CODE;
  }
  return PNPL_BASE_ERROR_CODE;
}

uint8_t __stream_control(bool status)
{
  int8_t i;
  if (status) /* Set stream ids */
  {
    int8_t j, stream_id = 0;
    app_model.total_bandwidth = 0;
    for (i = 0; i < SENSOR_NUMBER; i++)
    {
      if (app_model.s_models[i] != NULL)
      {
        if (app_model.s_models[i]->sensor_status->is_active == true)
        {
          /* Get sensor's bandwidth */
          if (app_model.s_models[i]->sensor_status->isensor_class == ISENSOR_CLASS_MEMS)
          {
            app_model.s_models[i]->stream_params.bandwidth = app_model.s_models[i]->sensor_status->type.mems.odr * SMGetnBytesPerSample(i);
          }
          else if (app_model.s_models[i]->sensor_status->isensor_class == ISENSOR_CLASS_AUDIO)
          {
            app_model.s_models[i]->stream_params.bandwidth = app_model.s_models[i]->sensor_status->type.audio.frequency * SMGetnBytesPerSample(i);
          }
          else
          {
            /* TODO: add support for other ISENSOR_CLASS */
          }
          __sc_set_usb_stream_params(i);
          __sc_set_fifo_wtm(i);

          app_model.s_models[i]->stream_params.stream_id = stream_id;
          stream_id++;

          /*
           * Ensure that elements with higher bandwidth are assigned lower stream_id.
           * Check if the current element has a higher bandwidth than any previous elements (j).
           * If so, the current elements take the place of the previous one that is incremented by one.
           */
          StreamParams_t *p_current = &(app_model.s_models[i]->stream_params);
          StreamParams_t *p_other;
          j = i - 1;
          while (j >= 0)
          {
            if (app_model.s_models[j] != NULL)
            {
              p_other = &(app_model.s_models[j]->stream_params);
              if (p_other->bandwidth <= p_current->bandwidth)
              {
                if (p_other->stream_id >= 0)
                {
                  if (p_other->stream_id < p_current->stream_id)
                  {
                    p_current->stream_id = p_other->stream_id;
                  }
                  p_other->stream_id++;
                }
              }
            }
            j--;
          }
        }
        else
        {
          app_model.s_models[i]->stream_params.bandwidth = 0;
          app_model.s_models[i]->stream_params.stream_id = -1;
        }
        app_model.total_bandwidth += app_model.s_models[i]->stream_params.bandwidth;
        if (app_model.total_bandwidth > SC_SAFE_BANDWIDTH)
        {
          /* PnPL Warning "Safe bandwidth limit exceeded.
           * Consider disabling sensors or lowering ODRs
           * to avoid possible data corruption.uire data correctly" */
          char *SerializedJSON;
          uint32_t size;
          PnPLCreateLogMessage(&SerializedJSON, &size, "Safe bandwidth limit exceeded", PNPL_LOG_WARNING);
        }
      }
    }
    __sc_set_usb_enpoints();
  }
  else
  {
    __sc_reset_stream_params();
  }
  return 0;
}

static sys_error_code_t __sc_set_usb_stream_params(uint32_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SensorModel_t **p_s_models = app_model.s_models;

  /* in case of slow sensor send 1 sample for each usb packet */
  float_t low_odr = 0;

  low_odr = SMSensorGetSamplesPerSecond(id);

  if (low_odr <= SC_USB_SLOW_ODR_LIMIT_HZ)
  {
    /* When there's a timestamp, more then one packet will be sent */
    p_s_models[id]->stream_params.usb_dps = SMGetnBytesPerSample(id) + 8; /* 8 = timestamp dimension in bytes */
  }
  else
  {
    /* 50ms of sensor data; when there's a timestamp packets will be sent fastly */
    p_s_models[id]->stream_params.usb_dps = (uint32_t)(p_s_models[id]->stream_params.bandwidth * SC_USB_MAX_PACKETS_PERIOD);
    if (p_s_models[id]->stream_params.usb_dps > SC_USB_DPS_MAX)
    {
      p_s_models[id]->stream_params.usb_dps = SC_USB_DPS_MAX; // set a limit to avoid buffer to big
    }
    else if (p_s_models[id]->stream_params.usb_dps < SMGetnBytesPerSample(id) + 8)
    {
      /* In case usb_dps is a very low value, verify the setup to send at least 1 sensor data + timestamp */
      p_s_models[id]->stream_params.usb_dps = SMGetnBytesPerSample(id) + 8;
    }
  }
  return res;
}

static sys_error_code_t __sc_set_fifo_wtm(uint32_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  SensorModel_t **p_s_models = app_model.s_models;
  uint16_t fifo_watermark;

  fifo_watermark = p_s_models[id]->stream_params.sd_dps / SMGetnBytesPerSample(id) / 2;
  if (fifo_watermark == 0)
  {
    fifo_watermark = 1;
  }

  if (app_model.log_controller_model.interface == LOG_CTRL_MODE_SD)
  {
    res = SMSensorSetFifoWM(id, fifo_watermark);
  }
  else if (app_model.log_controller_model.interface == LOG_CTRL_MODE_USB)
  {
    /* in case of slow sensor send 1 sample for each usb packet */
    float_t low_odr = 0;
    low_odr = SMSensorGetSamplesPerSecond(id);
    if (low_odr <= SC_USB_SLOW_ODR_LIMIT_HZ)
    {
      fifo_watermark = 1;
    }
    else
    {
      fifo_watermark = p_s_models[id]->stream_params.usb_dps / SMGetnBytesPerSample(id) / 2;
    }
    res = SMSensorSetFifoWM(id, fifo_watermark);
  }
  else
  {
    /**/
  }

  p_s_models[id]->fifo_watermark = fifo_watermark;


#ifdef SYS_DEBUG
  SensorDescriptor_t descriptor = SMSensorGetDescription(id);
  float_t ms = p_s_models[id]->stream_params.usb_dps / p_s_models[id]->stream_params.bandwidth;
  if (p_s_models[id]->sensor_status->isensor_class == ISENSOR_CLASS_MEMS)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("**** %s, odr: %f, DPS: %d, ms: %f, FIFO WM: %d \r\n",
                                       descriptor.p_name, p_s_models[id]->sensor_status->type.mems.odr,
                                       p_s_models[id]->stream_params.usb_dps, ms, fifo_watermark));
  }
  else if (p_s_models[id]->sensor_status->isensor_class == ISENSOR_CLASS_AUDIO)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("**** %s, odr: %d, DPS: %d, ms: %f, FIFO WM: %d \r\n",
                                       descriptor.p_name, p_s_models[id]->sensor_status->type.audio.frequency,
                                       p_s_models[id]->stream_params.usb_dps, ms, fifo_watermark));
  }
  else if (p_s_models[id]->sensor_status->isensor_class == ISENSOR_CLASS_POWERMONITOR)
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("**** %s, odr: %d, DPS: %d, ms: %f \r\n",
                                       descriptor.p_name, 1000000 / p_s_models[id]->sensor_status->type.power_meter.adc_conversion_time,
                                       p_s_models[id]->stream_params.usb_dps, ms));
  }
#endif

  return res;
}

static void __sc_set_usb_enpoints(void)
{
  SensorModel_t **p_s_models = app_model.s_models;
  uint32_t i;
  int8_t stream_id;

  for (i = 0; i < SENSOR_NUMBER; i++)
  {
    if (p_s_models[i] != NULL)
    {
      stream_id = p_s_models[i]->stream_params.stream_id;
      if (stream_id < (int8_t)(SS_N_IN_ENDPOINTS - 1))
      {
        /* Fastest streams on dedicated endpoints */
        p_s_models[i]->stream_params.usb_ep = stream_id;
      }
      else
      {
        /* Slowest streams multiplexed on last endpoint */
        p_s_models[i]->stream_params.usb_ep = SS_N_IN_ENDPOINTS - 1;
      }
    }
  }
}

static void __sc_reset_stream_params()
{
  SensorModel_t **p_s_models = app_model.s_models;
  uint32_t i;

  for (i = 0; i < SENSOR_NUMBER; i++)
  {
    if (p_s_models[i] != NULL)
    {
      p_s_models[i]->stream_params.stream_id = -1;
      p_s_models[i]->stream_params.usb_ep = -1;
      p_s_models[i]->stream_params.bandwidth = 0;
    }
  }
}


/* USER functions ------------------------------------------------------------*/
uint8_t set_mac_address(const char *value)
{
  strcpy(app_model.firmware_info_model.mac_address, value);
  return PNPL_NO_ERROR_CODE;
}


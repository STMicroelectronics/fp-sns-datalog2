/**
  ******************************************************************************
  * @file    SensorManager.c
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "SensorManager.h"
#include "stdlib.h"
#include "string.h"
#include "services/em_data_format.h"

static SensorManager_t spSMObj =
{
  0
};

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint16_t SMGetNsensor(void)
{
  return spSMObj.n_sensors;
}

ISourceObservable *SMGetSensorObserver(uint8_t id)
{
  if (id < SMGetNsensor())
  {
    return (ISourceObservable *)(spSMObj.Sensors[id]);
  }
  else
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    return NULL;
  }
}

sys_error_code_t SMSensorEnable(uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    ISensor_t *p_obj = (ISensor_t *)(spSMObj.Sensors[id]);
    res = ISensorEnable(p_obj);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorDisable(uint8_t id)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    ISensor_t *p_obj = (ISensor_t *)(spSMObj.Sensors[id]);
    res = ISensorDisable(p_obj);
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

SensorDescriptor_t SMSensorGetDescription(uint8_t id)
{
  if (id < SMGetNsensor())
  {
    ISensor_t *p_obj = (ISensor_t *)(spSMObj.Sensors[id]);
    return ISensorGetDescription(p_obj);
  }
  else
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    SensorDescriptor_t device_description;
    memset(&device_description, 0, sizeof(SensorDescriptor_t));
    return device_description;
  }
}

SensorStatus_t SMSensorGetStatus(uint8_t id)
{
  if (id < SMGetNsensor())
  {
    ISensor_t *p_obj = (ISensor_t *)(spSMObj.Sensors[id]);
    return ISensorGetStatus(p_obj);
  }
  else
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    SensorStatus_t device_status;
    memset(&device_status, 0, sizeof(SensorStatus_t));
    return device_status;
  }
}

sys_error_code_t SMDeviceGetDescription(SensorDescriptor_t *device_description)
{
  uint16_t ii;
  uint16_t n_sensors = spSMObj.n_sensors;

  if (n_sensors != 0)
  {
    ISensor_t *p_obj;

    memset(device_description, 0, n_sensors * sizeof(SensorDescriptor_t));
    for (ii = 0; ii < n_sensors; ii++)
    {
      p_obj = (ISensor_t *)(spSMObj.Sensors[ii]);
      device_description[ii] = ISensorGetDescription(p_obj);
    }
    return SYS_NO_ERROR_CODE;
  }
  else
  {
    return SYS_OUT_OF_MEMORY_ERROR_CODE;
  }
}

SensorManager_t *SMGetSensorManager(void)
{
  return &spSMObj;
}

uint32_t SMGetnBytesPerSample(uint8_t id)
{
  if (id < SMGetNsensor())
  {
    ISourceObservable *sensor_observable = SMGetSensorObserver(id);
    EMData_t data_info = ISourceGetDataInfo(sensor_observable);
    uint16_t data_type_size = EMD_GetElementSize(&data_info);
    if (EMD_GetDimensions(&data_info) > 2)
    {
      return (uint32_t)(EMD_GetPayloadSize(&data_info));
    }
    if (EMD_GetDimensions(&data_info) == 2)
    {
      uint16_t data_dimension = EMD_GetShape(&data_info, EMD_GetDimensions(&data_info) - 1);
      return (uint32_t)(data_type_size * data_dimension);
    }
    else
    {
      return (uint32_t)(data_type_size);
    }
  }
  else
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    return 0;
  }
}

/* Specialized for ISensorMems class */
sys_error_code_t SMSensorSetODR(uint8_t id, float odr)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_MEMS)
    {
      ISensorMems_t *p_obj = (ISensorMems_t *)(spSMObj.Sensors[id]);
      res = ISensorSetODR(p_obj, odr);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetFS(uint8_t id, float fs)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_MEMS)
    {
      ISensorMems_t *p_obj = (ISensorMems_t *)(spSMObj.Sensors[id]);
      res = ISensorSetFS(p_obj, fs);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetFifoWM(uint8_t id, uint16_t fifoWM)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_MEMS)
    {
      ISensorMems_t *p_obj = (ISensorMems_t *)(spSMObj.Sensors[id]);
      res = ISensorSetFifoWM(p_obj, fifoWM);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

/* Specialized for ISensorAudio class */
sys_error_code_t SMSensorSetFrequency(uint8_t id, uint32_t frequency)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_AUDIO)
    {
      ISensorAudio_t *p_obj = (ISensorAudio_t *)(spSMObj.Sensors[id]);
      res = ISensorSetFrequency(p_obj, frequency);
    }
    else if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_RANGING)
    {
      ISensorRanging_t *p_obj = (ISensorRanging_t *)(spSMObj.Sensors[id]);
      res = ISensorSetRangingFrequency(p_obj, frequency);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetVolume(uint8_t id, uint8_t volume)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_AUDIO)
    {
      ISensorAudio_t *p_obj = (ISensorAudio_t *)(spSMObj.Sensors[id]);
      res = ISensorSetVolume(p_obj, volume);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetResolution(uint8_t id, uint8_t bit_depth)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_AUDIO)
    {
      ISensorAudio_t *p_obj = (ISensorAudio_t *)(spSMObj.Sensors[id]);
      res = ISensorSetResolution(p_obj, bit_depth);
    }
    else if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_RANGING)
    {
      ISensorRanging_t *p_obj = (ISensorRanging_t *)(spSMObj.Sensors[id]);
      res = ISensorSetRangingResolution(p_obj, bit_depth);
    }
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

/* Specialized for ISensorRanging class */
//sys_error_code_t SMSensorSetFrequency(uint8_t id, uint32_t frequency);  /*Declaration in common with ISensorAudio*/
//sys_error_code_t SMSensorSetResolution(uint8_t id, uint8_t resolution); /*Declaration in common with ISensorAudio*/
sys_error_code_t SMSensorSetRangingMode(uint8_t id, uint8_t mode)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_RANGING)
    {
      ISensorRanging_t *p_obj = (ISensorRanging_t *)(spSMObj.Sensors[id]);
      res = ISensorSetRangingMode(p_obj, mode);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetIntegrationTime(uint8_t id, uint32_t timing_budget)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_RANGING)
    {
      ISensorRanging_t *p_obj = (ISensorRanging_t *)(spSMObj.Sensors[id]);
      res = ISensorSetIntegrationTime(p_obj, timing_budget);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorConfigIT(uint8_t id, ITConfig_t *p_it_config)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_RANGING)
    {
      ISensorRanging_t *p_obj = (ISensorRanging_t *)(spSMObj.Sensors[id]);
      res = ISensorConfigIT(p_obj, p_it_config);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetAddress(uint8_t id, uint32_t address)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_RANGING)
    {
      ISensorRanging_t *p_obj = (ISensorRanging_t *)(spSMObj.Sensors[id]);
      res = ISensorSetAddress(p_obj, address);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetPowerMode(uint8_t id, uint32_t power_mode)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_RANGING)
    {
      ISensorRanging_t *p_obj = (ISensorRanging_t *)(spSMObj.Sensors[id]);
      res = ISensorSetPowerMode(p_obj, power_mode);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

/* Specialized for ISensorPresence class */
sys_error_code_t SMSensorSetDataFrequency(uint8_t id, float data_frequency)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetDataFrequency(p_obj, data_frequency);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetTransmittance(uint8_t id, float Transmittance)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetTransmittance(p_obj, Transmittance);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetAverageTObject(uint8_t id, uint16_t average_tobject)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetAverageTObject(p_obj, average_tobject);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetAverageTAmbient(uint8_t id, uint16_t average_tambient)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetAverageTAmbient(p_obj, average_tambient);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetPresenceThreshold(uint8_t id, uint16_t presence_threshold)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetPresenceThreshold(p_obj, presence_threshold);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetPresenceHysteresis(uint8_t id, uint8_t presence_hysteresis)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetPresenceHysteresis(p_obj, presence_hysteresis);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetMotionThreshold(uint8_t id, uint16_t motion_threshold)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetMotionThreshold(p_obj, motion_threshold);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetMotionHysteresis(uint8_t id, uint8_t motion_hysteresis)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetMotionHysteresis(p_obj, motion_hysteresis);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetTAmbientShockThreshold(uint8_t id, uint16_t tambient_shock_threshold)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetTAmbientShockThreshold(p_obj, tambient_shock_threshold);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetTAmbientShockHysteresis(uint8_t id, uint8_t tambient_shock_hysteresis)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetTAmbientShockHysteresis(p_obj, tambient_shock_hysteresis);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetLPF_P_M_Bandwidth(uint8_t id, uint16_t bandwidth)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetLPF_P_M_Bandwidth(p_obj, bandwidth);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetLPF_P_Bandwidth(uint8_t id, uint16_t bandwidth)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetLPF_P_Bandwidth(p_obj, bandwidth);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetLPF_M_Bandwidth(uint8_t id, uint16_t bandwidth)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetLPF_M_Bandwidth(p_obj, bandwidth);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetEmbeddedCompensation(uint8_t id, uint8_t EmbeddedCompensation)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetEmbeddedCompensation(p_obj, EmbeddedCompensation);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetSoftwareCompensation(uint8_t id, uint8_t SoftwareCompensation)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetSoftwareCompensation(p_obj, SoftwareCompensation);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}
sys_error_code_t SMSensorSetSoftwareCompensationAlgorithmConfig(uint8_t id, CompensationAlgorithmConfig_t *pAlgorithmConfig)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_PRESENCE)
    {
      ISensorPresence_t *p_obj = (ISensorPresence_t *)(spSMObj.Sensors[id]);
      res = ISensorSetSoftwareCompensationAlgorithmConfig(p_obj, pAlgorithmConfig);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

/* Specialized for ISensorLight class */
sys_error_code_t SMSensorSetIntermeasurementTime(uint8_t id, uint32_t intermeasurement_time)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_LIGHT)
    {
      ISensorLight_t *p_obj = (ISensorLight_t *)(spSMObj.Sensors[id]);
      res = ISensorSetIntermeasurementTime(p_obj, intermeasurement_time);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetExposureTime(uint8_t id, float exposure_time)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_LIGHT)
    {
      ISensorLight_t *p_obj = (ISensorLight_t *)(spSMObj.Sensors[id]);
      res = ISensorSetExposureTime(p_obj, exposure_time);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

sys_error_code_t SMSensorSetLightGain(uint8_t id, float LightGain, uint8_t channel)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  if (id < SMGetNsensor())
  {
    if (SMSensorGetStatus(id).isensor_class == ISENSOR_CLASS_LIGHT)
    {
      ISensorLight_t *p_obj = (ISensorLight_t *)(spSMObj.Sensors[id]);
      res = ISensorSetLightGain(p_obj, LightGain, channel);
    }
    else
    {
      res = SYS_INVALID_PARAMETER_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
    }
  }
  else
  {
    res = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return res;
}

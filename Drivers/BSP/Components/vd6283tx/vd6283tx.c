/**
  ******************************************************************************
  * @file    vd6283tx.c
  * @author  IMG SW Application Team
  * @brief   This file provides the VD6283TX light sensor component driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "vd6283tx.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup VD6283TX
  * @brief     This file provides a set of functions needed to drive the
  *            VD6283TX light sensor.
  * @{
  */

/** @defgroup VD6283TX_Private_Types_Definitions Private Types Definitions
  * @{
  */
/**
  * @}
  */
/** @defgroup VD6283TX_Private_Variables
  * @{
  */
VD6283TX_LIGHT_SENSOR_Drv_t VD6283TX_LIGHT_SENSOR_Driver =
{
  VD6283TX_Init,
  VD6283TX_DeInit,
  VD6283TX_ReadID,
  VD6283TX_GetCapabilities,
  VD6283TX_SetExposureTime,
  VD6283TX_GetExposureTime,
  VD6283TX_SetGain,
  VD6283TX_GetGain,
  VD6283TX_SetInterMeasurementTime,
  VD6283TX_GetInterMeasurementTime,
  VD6283TX_Start,
  VD6283TX_Stop,
  VD6283TX_StartFlicker,
  VD6283TX_StopFlicker,
  VD6283TX_GetValues,
  VD6283TX_SetControlMode
};

/**
  * @}
  */

/** @defgroup VD6283TX_Private_Functions_Prototypes Private Functions Prototypes
  * @{
  */
static int32_t vd6283tx_check_filter_valid(void *hdl, uint8_t channels);
static uint8_t vd6283tx_filter_invalid_channels(void *hdl, uint8_t channels);
/**
  * @}
  */

/**
  * @brief Initializes the vd6283tx context object.
  * @param pObj    vd6283tx context object.
  * @param pIO     BSP IO struct.
  * @retval VL53L3CX status
  */
int32_t VD6283TX_RegisterBusIO(VD6283TX_Object_t *pObj, VD6283TX_IO_t *pIO)
{
#if 0
  int32_t ret;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    if (pObj->IO.Init != NULL)
    {
      ret = pObj->IO.Init();
    }
    else
    {
      ret = VD6283TX_ERROR;
    }
  }

  return ret;
#endif
  return VD6283TX_OK;
}

/**
  * @brief Initializes the vd6283tx light sensor.
  * @param pObj    vd6283tx context object.
  * @retval VD6283TX status
  */
int32_t VD6283TX_Init(VD6283TX_Object_t *pObj)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_Init("VD6283", pObj, &(pObj->handle)) != STALS_NO_ERROR)
  {
    ret = VD6283TX_ERROR;
  }
  else if (vd6283tx_check_filter_valid(pObj->handle, VD6283TX_ALL_CHANNELS) == VD6283TX_OK)
  {
    pObj->IsInitialized = 1U;
    pObj->IsStarted = 0U;
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Deinitializes the vd6283tx light sensor.
  * @param pObj    vd6283tx context object.
  * @retval VD6283TX status
  */
int32_t VD6283TX_DeInit(VD6283TX_Object_t *pObj)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_Term(pObj->handle) == STALS_NO_ERROR)
  {
    pObj->IsInitialized = 0U;
    pObj->IsStarted = 0U;
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Read the vd6283tx device ID.
  * @param pObj    vd6283tx context object.
  * @param pId    Pointer to the device ID.
  * @retval VD6283TX status
  */
int32_t VD6283TX_ReadID(VD6283TX_Object_t *pObj, uint32_t *pId)
{
#if 0
  int32_t ret;

  if ((pObj != NULL) && (pId != NULL))
  {
    /* initialize the variable before reading the ID register */
    *pId = 0;
    ret = pObj->IO.ReadReg(pObj->IO.Address, VD6283TX_DEVICE_ID_REG, (uint8_t *) pId, 1);
  }
  else
  {
    ret = VD6283TX_INVALID_PARAM;
  }

  return ret;
#endif
  return VD6283TX_OK;
}

/**
  * @brief Get the vd6283tx capabilities.
  * @param pObj    vd6283tx context object.
  * @param pCapabilities    Pointer to the vd6283tx capabilities.
  * @retval VD6283TX status
  */
int32_t VD6283TX_GetCapabilities(VD6283TX_Object_t *pObj, VD6283TX_Capabilities_t *pCapabilities)
{
  int32_t ret;

  if ((pObj != NULL) && (pCapabilities != NULL))
  {
    pCapabilities->NumberOfChannels = VD6283TX_MAX_CHANNELS;
    pCapabilities->FlickerDetection = 1U;
    pCapabilities->Autogain = 0U;

    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_INVALID_PARAM;
  }

  return ret;
}

/**
  * @brief Set the exposure time.
  * @param pObj    vd6283tx context object.
  * @param ExposureTime    New exposure time to be applied.
  * @warning This function must not be called when a capture is ongoing.
  * @retval VD6283TX status
  */
int32_t VD6283TX_SetExposureTime(VD6283TX_Object_t *pObj, uint32_t ExposureTime)
{
  int32_t ret;
  uint32_t current_exposure;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_SetExposureTime(pObj->handle, ExposureTime, &current_exposure) == STALS_NO_ERROR)
  {
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Get the exposure time.
  * @param pObj    vd6283tx context object.
  * @param pExposureTime    Pointer to the current exposure time value.
  * @retval VD6283TX status
  */
int32_t VD6283TX_GetExposureTime(VD6283TX_Object_t *pObj, uint32_t *pExposureTime)
{
  int32_t ret;

  if ((pObj == NULL) || (pExposureTime == NULL))
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_GetExposureTime(pObj->handle, pExposureTime) == STALS_NO_ERROR)
  {
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Get the exposure time.
  * @param pObj    vd6283tx context object.
  * @param Channel    Device channel.
  * @param Gain    New gain to be applied on the provided channel.
  * @warning This function must not be called when a capture is ongoing.
  * @retval VD6283TX status
  */
int32_t VD6283TX_SetGain(VD6283TX_Object_t *pObj, uint8_t Channel, uint32_t Gain)
{
  int32_t ret;
  uint16_t current_gain;
  uint8_t channel_id;

  channel_id = (1U << Channel);

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_SetGain(
             pObj->handle,
             (enum STALS_Channel_Id_t)channel_id,
             (uint16_t)Gain,
             &current_gain) == STALS_NO_ERROR)
  {
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Get the current gain of a channel.
  * @param pObj    vd6283tx context object.
  * @param Channel    Device channel.
  * @param pGain    Pointer to the current gain value.
  * @retval VD6283TX status
  */
int32_t VD6283TX_GetGain(VD6283TX_Object_t *pObj, uint8_t Channel, uint32_t *pGain)
{
  int32_t ret;
  uint16_t applied_gain;
  uint8_t channel_id;

  channel_id = (1U << Channel);

  if ((pObj == NULL) || (pGain == NULL))
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_GetGain(pObj->handle, (enum STALS_Channel_Id_t)channel_id, &applied_gain) == STALS_NO_ERROR)
  {
    *pGain = (uint32_t)applied_gain;
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Set the inter-measurement time.
  * @param pObj    vd6283tx context object.
  * @param InterMeasurementTime    Inter-measurement to be applied expressed in microseconds.
  * @note This should be configured only when using the device in continuous mode.
  * @warning This function must not be called when a capture is ongoing.
  * @retval VD6283TX status
  */
int32_t VD6283TX_SetInterMeasurementTime(VD6283TX_Object_t *pObj, uint32_t InterMeasurementTime)
{
  int32_t ret;
  uint32_t current_imt;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_SetInterMeasurementTime(pObj->handle, InterMeasurementTime, &current_imt) == STALS_NO_ERROR)
  {
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Get the inter-measurement time.
  * @param pObj    vd6283tx context object.
  * @param pInterMeasurementTime    Pointer to the current inter-measurement time expressed in microseconds.
  * @retval VD6283TX status
  */
int32_t VD6283TX_GetInterMeasurementTime(VD6283TX_Object_t *pObj, uint32_t *pInterMeasurementTime)
{
  int32_t ret;

  if ((pObj == NULL) || (pInterMeasurementTime == NULL))
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_GetInterMeasurementTime(pObj->handle, pInterMeasurementTime) == STALS_NO_ERROR)
  {
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Start the light measurement on all channels.
  * @param pObj    vd6283tx context object.
  * @param Mode    Measurement mode (continuous or single-shot)
  * @retval VD6283TX status
  */
int32_t VD6283TX_Start(VD6283TX_Object_t *pObj, uint8_t Mode)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if ((Mode != VD6283TX_MODE_CONTINUOUS) && (Mode != VD6283TX_MODE_SINGLESHOT))
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_Start(pObj->handle, (enum STALS_Mode_t)Mode, VD6283TX_ALL_CHANNELS) == STALS_NO_ERROR)
  {
    pObj->IsStarted = 1U;
    pObj->IsContinuous = (Mode == VD6283TX_MODE_CONTINUOUS) ? 1U : 0U;
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Stop the measurement on all channels.
  * @param pObj    vd6283tx context object.
  * @retval VD6283TX status
  */
int32_t VD6283TX_Stop(VD6283TX_Object_t *pObj)
{
  int32_t ret;
  enum STALS_Mode_t mode = (enum STALS_Mode_t)pObj->IsContinuous;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (pObj->IsStarted == 0U)
  {
    /* the device must be started in order to be stopped */
    ret = VD6283TX_ERROR;
  }
  else if (STALS_Stop(pObj->handle, mode) == STALS_NO_ERROR)
  {
    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Start flicker capture.
  * @param pObj    vd6283tx context object.
  * @param Channel    The channel that will be used for flicker detection.
  * @param OutputMode    Analog or Digital depending on the hardware configuration.
  * @warning The flicker can be started only on one channel at a time.
  * @note Calling this function will enable ALS capture on all the other channels.
  * @retval VD6283TX status
  */
int32_t VD6283TX_StartFlicker(VD6283TX_Object_t *pObj, uint8_t Channel, uint8_t OutputMode)
{
  int32_t ret;
  enum STALS_FlickerOutputType_t type;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if ((OutputMode != VD6283TX_FLICKER_ANALOG) && (OutputMode != VD6283TX_FLICKER_DIGITAL))
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else
  {
    pObj->FlickerOutputType = OutputMode;

    type = (OutputMode == VD6283TX_FLICKER_ANALOG) ?
           STALS_FLICKER_OUTPUT_ANALOG :
           STALS_FLICKER_OUTPUT_DIGITAL_PDM;

    if (STALS_SetFlickerOutputType(pObj->handle, type) != STALS_NO_ERROR)
    {
      ret = VD6283TX_ERROR;
    }
    /* activate the channel used for the flicker detection */
    else if (STALS_Start(pObj->handle, STALS_MODE_FLICKER, 1U << Channel) != STALS_NO_ERROR)
    {
      ret = VD6283TX_ERROR;
    }
    else
    {
      pObj->IsStarted = 1U;
      pObj->IsFlickerActive = 1U;
      ret = VD6283TX_OK;
    }
  }

  return ret;
}

/**
  * @brief Stop flicker capture.
  * @param pObj     vd6283tx context object.
  * @retval VD6283TX status
  */
int32_t VD6283TX_StopFlicker(VD6283TX_Object_t *pObj)
{
  int32_t ret;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (pObj->IsFlickerActive != 1U)
  {
    /* a flicker capture must be started before before calling this function */
    ret = VD6283TX_ERROR;
  }
  else if (STALS_Stop(pObj->handle, STALS_MODE_FLICKER) != STALS_NO_ERROR)
  {
    ret = VD6283TX_ERROR;
  }
  else
  {
    pObj->IsStarted = 0U;
    pObj->IsFlickerActive = 0U;
    ret = VD6283TX_OK;
  }

  return ret;
}

/**
  * @brief Returns the measurement values for all the channels.
  * @param pObj    vd6283tx context object.
  * @param pResult    Pointer to an array which will be filled with the values of each channel.
  * @note The array size must match the number of channels of the device.
  * @retval VD6283TX status
  */
int32_t VD6283TX_GetValues(VD6283TX_Object_t *pObj, uint32_t *pResult)
{
  int32_t ret;
  uint8_t is_valid;
  uint8_t i;
  struct STALS_Als_t meas;

  if ((pObj == NULL) || (pResult == NULL))
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_GetAlsValues(pObj->handle, VD6283TX_ALL_CHANNELS, &meas, &is_valid) != STALS_NO_ERROR)
  {
    ret = VD6283TX_ERROR;
  }
  else if (is_valid == 1U)
  {
    /* fill the result array with the measurement value */
    for (i = 0; i < (uint8_t)VD6283TX_MAX_CHANNELS; i++)
    {
      pResult[i] = meas.CountValue[i];
    }

    ret = VD6283TX_OK;
  }
  else
  {
    ret = VD6283TX_ERROR;
  }

  return ret;
}

/**
  * @brief Enable and disable control features.
  * @param pObj    vd6283tx context object.
  * @param ControlMode    Feature to be be enabled or disabled.
  * @param Value    Value to be applied.
  * @warning This function must not be called when a capture is ongoing.
  * @retval VD6283TX status
  */
int32_t VD6283TX_SetControlMode(VD6283TX_Object_t *pObj, uint32_t ControlMode, uint32_t Value)
{
  int32_t ret;
  uint32_t applied_value;

  if (pObj == NULL)
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else
  {
    switch (ControlMode)
    {
      case VD6283TX_CTRL_DARK:
        if (STALS_SetControl(pObj->handle, STALS_OUTPUT_DARK_ENABLE, Value) == STALS_NO_ERROR)
        {
          (void)STALS_GetControl(pObj->handle, STALS_OUTPUT_DARK_ENABLE, &applied_value);
          ret = (applied_value == Value) ? VD6283TX_OK : VD6283TX_ERROR;
        }
        else
        {
          ret = VD6283TX_ERROR;
        }

        break;

      default:
        ret = VD6283TX_INVALID_PARAM;
        break;
    }
  }

  return ret;
}

/**
  * @brief Get saturation value from the device.
  * @param pObj    vd6283tx context object.
  * @param pValue    Pointer to the variable where the saturation value is stored.
  * @warning The saturation value is reset when the device is stopped.
  * @retval VD6283TX status
  */
int32_t VD6283TX_GetSaturation(VD6283TX_Object_t *pObj, uint32_t *pValue)
{
  int32_t ret;

  if ((pObj == NULL) || (pValue == NULL))
  {
    ret = VD6283TX_INVALID_PARAM;
  }
  else if (STALS_GetControl(pObj->handle, STALS_SATURATION_VALUE, pValue) != STALS_NO_ERROR)
  {
    ret = VD6283TX_ERROR;
  }
  else
  {
    ret = VD6283TX_OK;
  }

  return ret;
}

/** @defgroup VD6283TX_Private_Functions Private Functions
  * @{
  */

static int32_t vd6283tx_check_filter_valid(void *handle, uint8_t channels)
{
  int32_t ret;
  uint8_t valid_channels;

  valid_channels = vd6283tx_filter_invalid_channels(handle, channels);

  if (valid_channels != channels)
  {
    ret = VD6283TX_ERROR;
  }
  else
  {
    ret = VD6283TX_OK;
  }

  return ret;
}

static uint8_t vd6283tx_filter_invalid_channels(void *handle, uint8_t channels)
{
  STALS_ErrCode_t res;
  enum STALS_Color_Id_t color;
  uint8_t channel_id;

  uint8_t channel;
  uint8_t channels_mask = channels;

  for (channel = 0; channel < (uint8_t)VD6283TX_MAX_CHANNELS; channel++)
  {
    if ((channels_mask & (1U << channel)) == 0U)
    {
      continue;
    }

    channel_id = (1U << channel);
    res = STALS_GetChannelColor(handle, (enum STALS_Channel_Id_t)channel_id, &color);

    if ((color == STALS_COLOR_INVALID) && (res == STALS_NO_ERROR))
    {
      channels_mask &= ~(1U << channel);
    }
  }

  return channels_mask;
}

/**
  * @}
 */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

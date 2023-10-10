/**
  ******************************************************************************
  * @file    STWIN_audio.c
  * @author  SRA
  *
  *
  * @brief   This file provides the Audio driver for the STWIN board
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
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "STWIN_audio.h"
#include "STWIN_bus.h"
#include "audio.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STWIN
  * @{
  */

/** @addtogroup STWIN_AUDIO_IN
  * @brief This file provides set of firmware functions to manage MEMS microphones
  *        initialization on STWIN Kit from STMicroelectronics.
  * @{
  */

/** @defgroup STWIN_AUDIO_IN_Private_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup STWIN_AUDIO_IN_Private_Defines
  * @{
  */

#define SAMPLES_PER_MS (AUDIO_IN_SAMPLING_FREQUENCY/1000)

/**
  * @}
  */

/** @defgroup STWIN_AUDIO_IN_Private_Macros
  * @{
  */

#define DFSDM_OVER_SAMPLING(__FREQUENCY__) \
  ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (256U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_11K)) ? (256U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (128U)  \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_22K)) ? (128U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (64U)  \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_44K)) ? (64U)  \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (64U)  \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_96K)) ? (32U) : (32U)

#define DFSDM_CLOCK_DIVIDER(__FREQUENCY__) \
  ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (6U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (6U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (6U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (4U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_96K)) ? (4U) : (2U)

#define DFSDM_FILTER_ORDER(__FREQUENCY__) \
  ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (DFSDM_FILTER_SINC4_ORDER) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_11K)) ? (DFSDM_FILTER_SINC5_ORDER) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (DFSDM_FILTER_SINC4_ORDER) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_22K)) ? (DFSDM_FILTER_SINC4_ORDER) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (DFSDM_FILTER_SINC5_ORDER) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_44K)) ? (DFSDM_FILTER_SINC5_ORDER) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (DFSDM_FILTER_SINC5_ORDER) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_96K)) ? (DFSDM_FILTER_SINC5_ORDER) : (DFSDM_FILTER_SINC5_ORDER)

#define DFSDM_MIC_BIT_SHIFT(__FREQUENCY__) \
  ((__FREQUENCY__) == (AUDIO_FREQUENCY_8K))  ? (12U)  \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_11K)) ? (5U)  \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_16K)) ? (8U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_22K)) ? (8U)  \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_32K)) ? (10U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_44K)) ? (10U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_48K)) ? (10U) \
  : ((__FREQUENCY__) == (AUDIO_FREQUENCY_96K)) ? (5U) : (5U)

/**
  * @}
  */

/** @defgroup STWIN_AUDIO_IN_Private_Variables
  * @{
  */

/* Recording context */
AUDIO_IN_Ctx_t                         AudioInCtx[AUDIO_IN_INSTANCES_NBR] = {0};

/* AMic OnBoard */
DFSDM_Filter_HandleTypeDef AMic_OnBoard_DfsdmFilter;
static DFSDM_Channel_HandleTypeDef AMic_OnBoard_DfsdmChannel;
DMA_HandleTypeDef AMic_OnBoard_Dma;

static int32_t   DFSDM_OUT[SAMPLES_PER_MS * 2];

/* DMic OnBoard */
DFSDM_Filter_HandleTypeDef DMic_OnBoard_DfsdmFilter;
static DFSDM_Channel_HandleTypeDef DMic_OnBoard_DfsdmChannel;
DMA_HandleTypeDef DMic_OnBoard_Dma;
static int32_t RecBuff[SAMPLES_PER_MS * 2];

static int32_t HAL_ADC_Counter = 0;

/**
  * @}
  */

/** @defgroup STWIN_AUDIO_IN_Private_Function_Prototypes
  * @{
  */
HAL_StatusTypeDef MX_DFSDM_Init(DFSDM_Filter_HandleTypeDef *hDfsdmFilter, DFSDM_Channel_HandleTypeDef *hDfsdmChannel,
                                MX_DFSDM_Config *MXConfig);
void MX_ADC1_Init(void);

void DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hDfsdmChannel);
void DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hDfsdmFilter);

void HAL_ADC_Audio_MspInit(ADC_HandleTypeDef *hadc);

/**
  * @}
  */

/** @defgroup STWIN_AUDIO_IN_Exported_Functions
  * @{
  */

__weak int32_t BSP_AUDIO_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit)
{
  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Store the audio record context */
    AudioInCtx[Instance].Device          = AudioInit->Device;
    AudioInCtx[Instance].ChannelsNbr     = AudioInit->ChannelsNbr;
    AudioInCtx[Instance].SampleRate      = AudioInit->SampleRate;
    AudioInCtx[Instance].BitsPerSample   = AudioInit->BitsPerSample;
    AudioInCtx[Instance].Volume          = AudioInit->Volume;
    AudioInCtx[Instance].State           = AUDIO_IN_STATE_RESET;

    if (Instance == 0U)
    {
      return BSP_ERROR_WRONG_PARAM;
    }
    else
    {
      if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
      {
        MX_DFSDM_Config dfsdm_config;

        DMic_OnBoard_DfsdmChannel.Instance = DMIC_ONBOARD_CHANNEL;
        DMic_OnBoard_DfsdmFilter.Instance  = DMIC_ONBOARD_FILTER;

        DFSDM_FilterMspInit(&DMic_OnBoard_DfsdmFilter);
        DFSDM_ChannelMspInit(&DMic_OnBoard_DfsdmChannel);

        dfsdm_config.FilterInstance  = DMIC_ONBOARD_FILTER;
        dfsdm_config.ChannelInstance = DMIC_ONBOARD_CHANNEL;
        dfsdm_config.DigitalMicPins  = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
        dfsdm_config.DigitalMicType  = DFSDM_CHANNEL_SPI_FALLING;
        dfsdm_config.Channel4Filter  = DFSDM_CHANNEL_5;
        dfsdm_config.RegularTrigger  = DFSDM_FILTER_SW_TRIGGER;
        dfsdm_config.DmaMode         = DISABLE;
        dfsdm_config.Activation      = ENABLE;
        dfsdm_config.Multiplexer     = DFSDM_CHANNEL_EXTERNAL_INPUTS;
        dfsdm_config.SincOrder       = DFSDM_FILTER_ORDER(AudioInCtx[Instance].SampleRate);
        dfsdm_config.Oversampling    = DFSDM_OVER_SAMPLING(AudioInCtx[Instance].SampleRate);
        dfsdm_config.ClockDivider    = DFSDM_CLOCK_DIVIDER(AudioInCtx[Instance].SampleRate);
        dfsdm_config.RightBitShift   = DFSDM_MIC_BIT_SHIFT(AudioInCtx[Instance].SampleRate);

        /* Default configuration of DFSDM filters and channels */
        if (MX_DFSDM_Init(&DMic_OnBoard_DfsdmFilter, &DMic_OnBoard_DfsdmChannel, &dfsdm_config) != HAL_OK)
        {
          /* Return BSP_ERROR_PERIPH_FAILURE when operations are not correctly done */
          return BSP_ERROR_PERIPH_FAILURE;
        }

      }
      if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
      {
        MX_DFSDM_Config dfsdm_config;

        dfsdm_config.FilterInstance  = AMIC_ONBOARD_FILTER;
        dfsdm_config.ChannelInstance = AMIC_ONBOARD_CHANNEL;
        dfsdm_config.DigitalMicPins  = DFSDM_CHANNEL_SAME_CHANNEL_PINS; /*NU*/
        dfsdm_config.DigitalMicType  = DFSDM_CHANNEL_SPI_FALLING;/*NU*/
        dfsdm_config.Channel4Filter  = DFSDM_CHANNEL_0;
        if (AudioInCtx[Instance].ChannelsNbr == 1U)
        {
          dfsdm_config.RegularTrigger = DFSDM_FILTER_SW_TRIGGER;
        }
        else
        {
          dfsdm_config.RegularTrigger = DFSDM_FILTER_SYNC_TRIGGER;
        }
        dfsdm_config.DmaMode       = DISABLE;
        dfsdm_config.Activation      = DISABLE;
        dfsdm_config.Multiplexer     = DFSDM_CHANNEL_ADC_OUTPUT;
        dfsdm_config.SincOrder       = SINC_ORDER;
        dfsdm_config.Oversampling    = DECIMATION_RATIO_DFSDM;
        dfsdm_config.ClockDivider    = 1;/*NU*/
        dfsdm_config.RightBitShift   = RBITSHIFT;

        /* Default configuration of DFSDM filters and channels */
        if (MX_DFSDM_Init(&AMic_OnBoard_DfsdmFilter, &AMic_OnBoard_DfsdmChannel, &dfsdm_config) != HAL_OK)
        {
          /* Return BSP_ERROR_PERIPH_FAILURE when operations are not correctly done */
          return BSP_ERROR_PERIPH_FAILURE;
        }

        /*adc init*/
        MX_ADC1_Init();

      }
    }
  }

  /* Update BSP AUDIO IN state */
  AudioInCtx[Instance].State = AUDIO_IN_STATE_STOP;
  /* Return BSP status */
  return BSP_ERROR_NONE;
}


/**
  * @brief  Deinit the audio IN peripherals.
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @retval BSP status
  */

__weak int32_t BSP_AUDIO_IN_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if (Instance == 0U)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      /* De-initializes DFSDM Filter handle */
      if (DMic_OnBoard_DfsdmFilter.Instance != NULL)
      {
        if (HAL_OK != HAL_DFSDM_FilterDeInit(&DMic_OnBoard_DfsdmFilter))
        {
          ret =  BSP_ERROR_PERIPH_FAILURE;
        }
        DMic_OnBoard_DfsdmFilter.Instance = NULL;
      }

      /* De-initializes DFSDM Channel handle */
      if (DMic_OnBoard_DfsdmChannel.Instance != NULL)
      {
        if (HAL_OK != HAL_DFSDM_ChannelDeInit(&DMic_OnBoard_DfsdmChannel))
        {
          ret =  BSP_ERROR_PERIPH_FAILURE;
        }
        DMic_OnBoard_DfsdmChannel.Instance = NULL;
      }
    }
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      /* De-initializes DFSDM Filter handle */
      if (AMic_OnBoard_DfsdmFilter.Instance != NULL)
      {
        if (HAL_OK != HAL_DFSDM_FilterDeInit(&AMic_OnBoard_DfsdmFilter))
        {
          ret =  BSP_ERROR_PERIPH_FAILURE;
        }
        AMic_OnBoard_DfsdmFilter.Instance = NULL;
      }

      /* De-initializes DFSDM Channel handle */
      if (AMic_OnBoard_DfsdmChannel.Instance != NULL)
      {
        if (HAL_OK != HAL_DFSDM_ChannelDeInit(&AMic_OnBoard_DfsdmChannel))
        {
          ret =  BSP_ERROR_PERIPH_FAILURE;
        }
        AMic_OnBoard_DfsdmChannel.Instance = NULL;
      }

      /* De-initializes ADC handle */
      if (AMic_OnBoard_DfsdmChannel.Instance != NULL)
      {
        (void)BSP_ADC1_DeInitialization(ADC1_FOR_AUDIO);
      }
    }
  }

  /* Update BSP AUDIO IN state */
  AudioInCtx[Instance].State = AUDIO_IN_STATE_RESET;
  return ret;
}


/**
  * @brief  Start audio recording.
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  pbuf     Main buffer pointer for the recorded data storing
  * @param  Size     Size of the record buffer
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Record(uint32_t Instance, uint8_t *pBuf, uint32_t NbrOfBytes)
{
  int32_t ret = BSP_ERROR_NONE;
  AudioInCtx[Instance].pBuff = (uint16_t *)pBuf;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if (Instance == 0U)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if (HAL_ADC_Start(&ADC1_Handle) != HAL_OK)
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
      (void)HAL_DFSDM_FilterRegularStart_DMA(&AMic_OnBoard_DfsdmFilter, DFSDM_OUT, NbrOfBytes);
    }

    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&DMic_OnBoard_DfsdmFilter,
                                                     (int32_t *) RecBuff,
                                                     NbrOfBytes))
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }

  /* Update BSP AUDIO IN state */
  AudioInCtx[Instance].State = AUDIO_IN_STATE_RECORDING;
  return ret;
}


/**
  * @brief  Stop audio recording.
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Stop(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if (Instance == 0U)
  {
    return  BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if (HAL_DFSDM_FilterRegularStop_DMA(&DMic_OnBoard_DfsdmFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if (HAL_DFSDM_FilterRegularStop_DMA(&AMic_OnBoard_DfsdmFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }

      if (HAL_ADC_Stop(&ADC1_Handle) != HAL_OK)
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }

  /* Update BSP AUDIO IN state */
  AudioInCtx[Instance].State = AUDIO_IN_STATE_STOP;
  return ret;
}


/**
  * @brief  Pause the audio file stream.
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Pause(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if (Instance == 0U)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if (HAL_DFSDM_FilterRegularStop_DMA(&DMic_OnBoard_DfsdmFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if (HAL_DFSDM_FilterRegularStop_DMA(&AMic_OnBoard_DfsdmFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }
  /* Update BSP AUDIO IN state */
  AudioInCtx[Instance].State = AUDIO_IN_STATE_PAUSE;
  return ret;
}


/**
  * @brief  Resume the audio file stream.
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_Resume(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if (Instance == 0U)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if (HAL_OK != HAL_DFSDM_FilterRegularStart_DMA(&DMic_OnBoard_DfsdmFilter,
                                                     (int32_t *) RecBuff,
                                                     AudioInCtx[Instance].SampleRate / 1000U * 2U * N_MS_PER_INTERRUPT))
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
    }
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if (HAL_ADC_Start(&ADC1_Handle) != HAL_OK)
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
      (void)HAL_DFSDM_FilterRegularStart_DMA(&AMic_OnBoard_DfsdmFilter, DFSDM_OUT,
                                             (AudioInCtx[Instance].SampleRate / 1000U * 2U));
    }
  }

  /* Update BSP AUDIO IN state */
  AudioInCtx[Instance].State = AUDIO_IN_STATE_RECORDING;
  return ret;
}


/**
  * @brief  Starts audio recording.
  * @param  Instance  AUDIO IN Instance. It can be 1(DFSDM used)
  * @param  pBuf      Main buffer pointer for the recorded data storing
  * @param  size      Size of the recorded buffer
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_RecordChannels(uint32_t Instance, uint8_t **pBuf, uint32_t NbrOfBytes)
{
  UNUSED(Instance);
  UNUSED(pBuf);
  UNUSED(NbrOfBytes);
  return BSP_NOT_IMPLEMENTED;
}


/**
  * @brief  Stop audio recording.
  * @param  Instance  AUDIO IN Instance. It can be 1(DFSDM used)
  * @param  Device    Digital input device to be stopped
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_StopChannels(uint32_t Instance, uint32_t Device)
{
  UNUSED(Instance);
  UNUSED(Device);
  return BSP_NOT_IMPLEMENTED;
}


/**
  * @brief  Pause the audio file stream.
  * @param  Instance  AUDIO IN Instance. It can be 1(DFSDM used)
  * @param  Device    Digital mic to be paused
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_PauseChannels(uint32_t Instance, uint32_t Device)
{
  UNUSED(Instance);
  UNUSED(Device);
  return BSP_NOT_IMPLEMENTED;
}


/**
  * @brief  Resume the audio file stream
  * @param  Instance  AUDIO IN Instance. It can be 1(DFSDM used)
  * @param  Device    Digital mic to be resumed
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_ResumeChannels(uint32_t Instance, uint32_t Device)
{
  UNUSED(Instance);
  UNUSED(Device);
  return BSP_NOT_IMPLEMENTED;
}


/**
  * @brief  Set Audio In device
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  Device    The audio input device to be used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetDevice(uint32_t Instance, uint32_t Device)
{
  int32_t ret = BSP_ERROR_NONE;
  BSP_AUDIO_Init_t audio_init;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }

    audio_init.Device = Device;
    audio_init.ChannelsNbr   = AudioInCtx[Instance].ChannelsNbr;
    audio_init.SampleRate    = AudioInCtx[Instance].SampleRate;
    audio_init.BitsPerSample = AudioInCtx[Instance].BitsPerSample;
    audio_init.Volume        = AudioInCtx[Instance].Volume;

    if (BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_NO_INIT;
    }
  }
  return ret;
}


/**
  * @brief  Get Audio In device
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  Device    The audio input device used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetDevice(uint32_t Instance, uint32_t *Device)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Return audio Input Device */
    *Device = AudioInCtx[Instance].Device;
  }
  return ret;
}


/**
  * @brief  Set Audio In frequency
  * @param  Instance     Audio IN instance
  * @param  SampleRate  Input frequency to be set
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetSampleRate(uint32_t Instance, uint32_t  SampleRate)
{
  int32_t ret = BSP_ERROR_NONE;
  BSP_AUDIO_Init_t audio_init;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }

    audio_init.Device      = AudioInCtx[Instance].Device;
    audio_init.ChannelsNbr   = AudioInCtx[Instance].ChannelsNbr;
    audio_init.SampleRate    = SampleRate;
    audio_init.BitsPerSample = AudioInCtx[Instance].BitsPerSample;
    audio_init.Volume        = AudioInCtx[Instance].Volume;

    if (BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_NO_INIT;
    }
  }
  return ret;
}


/**
  * @brief  Get Audio In frequency
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  SampleRate  Audio Input frequency to be returned
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetSampleRate(uint32_t Instance, uint32_t *SampleRate)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Return audio in frequency */
    *SampleRate = AudioInCtx[Instance].SampleRate;
  }

  /* Return BSP status */
  return ret;
}


/**
  * @brief  Set Audio In Resolution
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  BitsPerSample  Input resolution to be set
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample)
{
  int32_t ret = BSP_ERROR_NONE;
  BSP_AUDIO_Init_t audio_init;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }

    audio_init.Device      = AudioInCtx[Instance].Device;
    audio_init.ChannelsNbr   = AudioInCtx[Instance].ChannelsNbr;
    audio_init.SampleRate    = AudioInCtx[Instance].SampleRate;
    audio_init.BitsPerSample = BitsPerSample;
    audio_init.Volume        = AudioInCtx[Instance].Volume;

    if (BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_NO_INIT;
    }
  }
  return ret;
}


/**
  * @brief  Get Audio In Resolution
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  BitsPerSample  Input resolution to be returned
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Return audio in resolution */
    *BitsPerSample = AudioInCtx[Instance].BitsPerSample;
  }
  return ret;
}


/**
  * @brief  Set Audio In Channel number
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  ChannelNbr  Channel number to be used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr)
{
  int32_t ret = BSP_ERROR_NONE;
  BSP_AUDIO_Init_t audio_init;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }

    audio_init.Device        = AudioInCtx[Instance].Device;
    audio_init.ChannelsNbr   = ChannelNbr;
    audio_init.SampleRate    = AudioInCtx[Instance].SampleRate;
    audio_init.BitsPerSample = AudioInCtx[Instance].BitsPerSample;
    audio_init.Volume        = AudioInCtx[Instance].Volume;

    if (BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_NO_INIT;
    }
  }
  return ret;
}


/**
  * @brief  Get Audio In Channel number
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  ChannelNbr  Channel number to be used
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Channel number to be returned */
    *ChannelNbr = AudioInCtx[Instance].ChannelsNbr;
  }
  return ret;
}


/**
  * @brief  Set the current audio in volume level.
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  Volume    Volume level to be returnd
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_SetVolume(uint32_t Instance, uint32_t Volume)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if (Instance == 0U)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Update AudioIn Context */
    AudioInCtx[Instance].Volume = Volume;
    /* Return BSP status */
    return ret;
  }
}


/**
  * @brief  Get the current audio in volume level.
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  Volume    Volume level to be returnd
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetVolume(uint32_t Instance, uint32_t *Volume)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Input Volume to be returned */
    *Volume = AudioInCtx[Instance].Volume;
  }
  /* Return BSP status */
  return ret;
}


/**
  * @brief  Get Audio In device
  * @param  Instance  AUDIO IN Instance. It can be 0 when I2S / SPI is used or 1 if DFSDM is used
  * @param  State     Audio Out state
  * @retval BSP status
  */
int32_t BSP_AUDIO_IN_GetState(uint32_t Instance, uint32_t *State)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Input State to be returned */
    *State = AudioInCtx[Instance].State;
  }
  return ret;
}


/**
  * @brief  Regular conversion complete callback.
  * @note   In interrupt mode, user has to read conversion value in this function
using HAL_DFSDM_FilterGetRegularValue.
  * @param  hdfsdm_filter   DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  UNUSED(hdfsdm_filter);
  uint32_t j;

  if (AudioInCtx[1].IsMultiBuff == 1U)
  {
    /* Call the record update function to get the second half */
    BSP_AUDIO_IN_TransferComplete_CallBack(1);
  }
  else
  {
    if ((AudioInCtx[1].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      for (j = 0U; j < ((AudioInCtx[1].SampleRate / 1000U) * N_MS_PER_INTERRUPT); j++)
      {
        /*DMIC_ONBOARD_CHANNEL*/
        AudioInCtx[1].HP_Filters[0].Z = ((RecBuff[j + ((AudioInCtx[1].SampleRate / 1000U) * N_MS_PER_INTERRUPT)] / 256) * (int32_t)(AudioInCtx[1].Volume)) / 128;
        AudioInCtx[1].HP_Filters[0].oldOut = (0xFC * (AudioInCtx[1].HP_Filters[0].oldOut + AudioInCtx[1].HP_Filters[0].Z - AudioInCtx[1].HP_Filters[0].oldIn)) / 256;
        AudioInCtx[1].HP_Filters[0].oldIn = AudioInCtx[1].HP_Filters[0].Z;
        AudioInCtx[1].pBuff[AudioInCtx[1].ChannelsNbr * j] = (uint16_t) SaturaLH(AudioInCtx[1].HP_Filters[0].oldOut, -32760, 32760);
        /*AMIC_ONBOARD_CHANNEL*/
        AudioInCtx[1].HP_Filters[1].Z = ((DFSDM_OUT[j + ((AudioInCtx[1].SampleRate / 1000U) * N_MS_PER_INTERRUPT)] / 256) * (int32_t)(AudioInCtx[1].Volume)) / 512;
        AudioInCtx[1].HP_Filters[1].oldOut = (0xFC * (AudioInCtx[1].HP_Filters[1].oldOut + AudioInCtx[1].HP_Filters[1].Z - AudioInCtx[1].HP_Filters[1].oldIn)) / 256;
        AudioInCtx[1].HP_Filters[1].oldIn = AudioInCtx[1].HP_Filters[1].Z;
        AudioInCtx[1].pBuff[(AudioInCtx[1].ChannelsNbr * j) + 1U] = (uint16_t) SaturaLH(AudioInCtx[1].HP_Filters[1].oldOut, -32760, 32760);
      }
    }
    if (AudioInCtx[1].Device == ONBOARD_ANALOG_MIC_MASK)
    {
      for (j = 0U; j < ((AudioInCtx[1].SampleRate / 1000U) * N_MS_PER_INTERRUPT); j++)
      {
        AudioInCtx[1].HP_Filters[1].Z = ((DFSDM_OUT[j + ((AudioInCtx[1].SampleRate / 1000U) * N_MS_PER_INTERRUPT)] / 256) * (int32_t)(AudioInCtx[1].Volume)) / 512;
        AudioInCtx[1].HP_Filters[1].oldOut = (0xFC * (AudioInCtx[1].HP_Filters[1].oldOut + AudioInCtx[1].HP_Filters[1].Z - AudioInCtx[1].HP_Filters[1].oldIn)) / 256;
        AudioInCtx[1].HP_Filters[1].oldIn = AudioInCtx[1].HP_Filters[1].Z;
        AudioInCtx[1].pBuff[AudioInCtx[1].ChannelsNbr * j] = (uint16_t) SaturaLH(AudioInCtx[1].HP_Filters[1].oldOut, -32760, 32760);
      }
    }
    BSP_AUDIO_IN_TransferComplete_CallBack(1);
  }
}

/**
  * @brief  Half regular conversion complete callback.
  * @param  hdfsdm_filter   DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  UNUSED(hdfsdm_filter);
  uint32_t j;

  if (AudioInCtx[1].IsMultiBuff == 1U)
  {
    /* Call the record update function to get the second half */
    BSP_AUDIO_IN_HalfTransfer_CallBack(1);
  }
  else
  {
    if ((AudioInCtx[1].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      for (j = 0U; j < ((AudioInCtx[1].SampleRate / 1000U)* N_MS_PER_INTERRUPT); j++)
      {
        /*DMIC_ONBOARD_CHANNEL*/
        AudioInCtx[1].HP_Filters[0].Z = ((RecBuff[j] / 256) * (int32_t)(AudioInCtx[1].Volume)) / 128;
        AudioInCtx[1].HP_Filters[0].oldOut = (0xFC * (AudioInCtx[1].HP_Filters[0].oldOut + AudioInCtx[1].HP_Filters[0].Z - AudioInCtx[1].HP_Filters[0].oldIn)) / 256;
        AudioInCtx[1].HP_Filters[0].oldIn = AudioInCtx[1].HP_Filters[0].Z;
        AudioInCtx[1].pBuff[AudioInCtx[1].ChannelsNbr * (j)] = (uint16_t) SaturaLH(AudioInCtx[1].HP_Filters[0].oldOut, -32760, 32760);
        /*AMIC_ONBOARD_CHANNEL*/
        AudioInCtx[1].HP_Filters[1].Z = ((DFSDM_OUT[j]  / 256) * (int32_t)(AudioInCtx[1].Volume)) / 512;
        AudioInCtx[1].HP_Filters[1].oldOut = (0xFC * (AudioInCtx[1].HP_Filters[1].oldOut + AudioInCtx[1].HP_Filters[1].Z - AudioInCtx[1].HP_Filters[1].oldIn)) / 256;
        AudioInCtx[1].HP_Filters[1].oldIn = AudioInCtx[1].HP_Filters[1].Z;
        AudioInCtx[1].pBuff[(AudioInCtx[1].ChannelsNbr * j) + 1U] = (uint16_t) SaturaLH(AudioInCtx[1].HP_Filters[1].oldOut, -32760, 32760);
      }
    }
    if (AudioInCtx[1].Device == ONBOARD_ANALOG_MIC_MASK)
    {
      for (j = 0U; j < ((AudioInCtx[1].SampleRate / 1000U)* N_MS_PER_INTERRUPT); j++)
      {
        AudioInCtx[1].HP_Filters[1].Z = ((DFSDM_OUT[j]  / 256) * (int32_t)(AudioInCtx[1].Volume)) / 512;
        AudioInCtx[1].HP_Filters[1].oldOut = (0xFC * (AudioInCtx[1].HP_Filters[1].oldOut + AudioInCtx[1].HP_Filters[1].Z - AudioInCtx[1].HP_Filters[1].oldIn)) / 256;
        AudioInCtx[1].HP_Filters[1].oldIn = AudioInCtx[1].HP_Filters[1].Z;
        AudioInCtx[1].pBuff[AudioInCtx[1].ChannelsNbr * j] = (uint16_t) SaturaLH(AudioInCtx[1].HP_Filters[1].oldOut, -32760, 32760);
      }
    }
    BSP_AUDIO_IN_HalfTransfer_CallBack(1);
  }

}


/**
  * @brief  User callback when record buffer is filled.
  * @retval None
  */
__weak void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  /* This function should be implemented by the user application.
  It is called into this driver when the current buffer is filled
  to prepare the next buffer pointer and its size. */
}

/**
  * @brief  Manages the DMA Half Transfer complete event.
  * @retval None
  */
__weak void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  /* This function should be implemented by the user application.
  It is called into this driver when the current buffer is filled
  to prepare the next buffer pointer and its size. */
}

/**
  * @brief  Audio IN Error callback function.
  * @retval None
  */
__weak void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);

  /* This function is called when an Interrupt due to transfer error on or peripheral
  error occurs. */
}

/**
  * @brief  Initializes the Audio instance (DFSDM).
  * @param  hDfsdmFilter  DFSDM Filter Handle
  * @param  hDfsdmChannel DFSDM Channel Handle
  * @param  SampleRate    Audio frequency to be configured for the DFSDM Channel.
  * @note   Being __weak it can be overwritten by the application
  * @note   Channel output Clock Divider and Filter Oversampling are calculated as follow:
  *         - Clock_Divider = CLK(input DFSDM)/CLK(micro)
  *         - Oversampling = CLK(input DFSDM)/(Clock_Divider * AudioFreq)
  * @retval HAL_status
  */
__weak HAL_StatusTypeDef MX_DFSDM_Init(DFSDM_Filter_HandleTypeDef *hDfsdmFilter,
                                       DFSDM_Channel_HandleTypeDef *hDfsdmChannel, MX_DFSDM_Config *MXConfig)
{
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DFSDM_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* MIC channels initialization */
  __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(hDfsdmChannel);
  hDfsdmChannel->Instance                      = MXConfig->ChannelInstance;
  hDfsdmChannel->Init.OutputClock.Activation   = MXConfig->Activation;
  hDfsdmChannel->Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  hDfsdmChannel->Init.OutputClock.Divider      = (uint32_t)(MXConfig->ClockDivider);
  hDfsdmChannel->Init.Input.Multiplexer        = MXConfig->Multiplexer;
  hDfsdmChannel->Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  hDfsdmChannel->Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;
  hDfsdmChannel->Init.Awd.FilterOrder          = DFSDM_CHANNEL_FASTSINC_ORDER;
  hDfsdmChannel->Init.Awd.Oversampling         = 10;
  hDfsdmChannel->Init.Offset                   = 0;
  hDfsdmChannel->Init.RightBitShift            = MXConfig->RightBitShift;
  hDfsdmChannel->Init.Input.Pins               = MXConfig->DigitalMicPins;
  hDfsdmChannel->Init.SerialInterface.Type     = MXConfig->DigitalMicType;

  if (HAL_OK != HAL_DFSDM_ChannelInit(hDfsdmChannel))
  {
    return HAL_ERROR;
  }

  /* MIC filters  initialization */
  __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(hDfsdmFilter);
  hDfsdmFilter->Instance                          = MXConfig->FilterInstance;
  hDfsdmFilter->Init.RegularParam.Trigger         = MXConfig->RegularTrigger;
  hDfsdmFilter->Init.RegularParam.FastMode        = ENABLE;
  hDfsdmFilter->Init.RegularParam.DmaMode         = ENABLE;
  hDfsdmFilter->Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  hDfsdmFilter->Init.InjectedParam.ScanMode       = DISABLE;
  hDfsdmFilter->Init.InjectedParam.DmaMode        = MXConfig->DmaMode;
  hDfsdmFilter->Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM1_TRGO;
  hDfsdmFilter->Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_RISING_EDGE;
  hDfsdmFilter->Init.FilterParam.SincOrder        = MXConfig->SincOrder;
  hDfsdmFilter->Init.FilterParam.Oversampling     = MXConfig->Oversampling;
  hDfsdmFilter->Init.FilterParam.IntOversampling  = 1;

  if (HAL_DFSDM_FilterInit(hDfsdmFilter) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Configure injected channel */
  if (HAL_DFSDM_FilterConfigRegChannel(hDfsdmFilter, MXConfig->Channel4Filter, DFSDM_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Configure DMA for AMic_Onboard */
  if (MXConfig->FilterInstance == AMIC_ONBOARD_FILTER)
  {
    AMic_OnBoard_Dma.Init.Request = DMA_REQUEST_ANALOG;
    AMic_OnBoard_Dma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    AMic_OnBoard_Dma.Init.PeriphInc = DMA_PINC_DISABLE;
    AMic_OnBoard_Dma.Init.MemInc = DMA_MINC_ENABLE;
    AMic_OnBoard_Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    AMic_OnBoard_Dma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    AMic_OnBoard_Dma.Init.Mode = DMA_CIRCULAR;
    AMic_OnBoard_Dma.Init.Priority = DMA_PRIORITY_HIGH;
    AMic_OnBoard_Dma.Instance = DFSDM_DMA_ANALOG;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hDfsdmFilter, hdmaReg, AMic_OnBoard_Dma);

    if (AudioInCtx[1].ChannelsNbr == 1U)
    {
      /* Reset DMA handle state */
      __HAL_DMA_RESET_HANDLE_STATE(&AMic_OnBoard_Dma);
    }

    /* Configure the DMA Channel */
    (void)HAL_DMA_Init(&AMic_OnBoard_Dma);

    if (AudioInCtx[1].ChannelsNbr == 1U)
    {
      HAL_NVIC_SetPriority(DFSDM_DMA_ANALOG_IRQn, BSP_AUDIO_IN_IT_PRIORITY, 0);
      HAL_NVIC_EnableIRQ(DFSDM_DMA_ANALOG_IRQn);
    }
  }

  return HAL_OK;
}

/**
  * @brief  Initialize the ADC channel
  * @param  None
  * @retval None
  */
/* ADC1 init function */
void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /* Enable ADC1 if it's necessary */
  (void)BSP_ADC1_Initialization(ADC1_FOR_AUDIO);
  HAL_ADC_Audio_MspInit(&ADC1_Handle);

  /**Configure Regular Channel */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLE_TIME;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;

#if (DECIMATION_RATIO_DFSDM != 1)
  sConfig.OffsetNumber = ADC_OFFSET_1;             /* Offset subtraction */
  sConfig.Offset = 0x800;                                 /* Parameter discarded because offset correction is disabled */
#endif

  (void)HAL_ADC_ConfigChannel(&ADC1_Handle, &sConfig);
}



/**
  * @brief  Initialize the DFSDM channel MSP.
  * @param  hDfsdmChannel DFSDM Channel handle
  * @retval None
  */
void DFSDM_ChannelMspInit(DFSDM_Channel_HandleTypeDef *hDfsdmChannel)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable DFSDM clock */
  __HAL_RCC_DFSDM1_CLK_ENABLE();

  if ((AudioInCtx[1].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
  {
    if (hDfsdmChannel->Instance == DMIC_ONBOARD_CHANNEL)
    {
      /* Enable GPIO clock */
      DMIC_ONBOARD_DATAIN_GPIO_CLK_ENABLE();
      DMIC_ONBOARD_CKOUT_GPIO_CLK_ENABLE();

      /* DFSDM pins configuration: DFSDM_CKOUT, DMIC_DATIN1 pins ------------------*/
      GPIO_InitStruct.Pin = DMIC_ONBOARD_CKOUT_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLDOWN;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = DMIC_ONBOARD_CKOUT_DATIN_AF;
      HAL_GPIO_Init(DMIC_ONBOARD_CKOUT_GPIO_PORT, &GPIO_InitStruct);

      /* DFSDM pin configuration: DMIC_DATIN1 pin --------------------------------*/
      GPIO_InitStruct.Pin = DMIC_ONBOARD_DATIN_PIN;
      GPIO_InitStruct.Alternate = DMIC_ONBOARD_DATAIN_DATIN_AF ;
      HAL_GPIO_Init(DMIC_ONBOARD_DATIN_GPIO_PORT, &GPIO_InitStruct);
    }
  }
}


/**
  * @brief  Initialize the DFSDM filter MSP.
  * @param  hDfsdmFilter DFSDM Filter handle
  * @retval None
  */
void DFSDM_FilterMspInit(DFSDM_Filter_HandleTypeDef *hDfsdmFilter)
{
  /* Enable DFSDM clock */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DFSDM_CLK_ENABLE();

  /* Enable the DMA clock */
  __HAL_RCC_DMA1_CLK_ENABLE();

  if ((AudioInCtx[1].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
  {
    if (hDfsdmFilter->Instance == DMIC_ONBOARD_FILTER)
    {
      /*********** Configure DMA stream for DIGITAL microphone *******************/
      DMic_OnBoard_Dma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
      DMic_OnBoard_Dma.Init.PeriphInc           = DMA_PINC_DISABLE;
      DMic_OnBoard_Dma.Init.MemInc              = DMA_MINC_ENABLE;
      DMic_OnBoard_Dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
      DMic_OnBoard_Dma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
      DMic_OnBoard_Dma.Init.Mode                = DMA_CIRCULAR;
      DMic_OnBoard_Dma.Init.Priority            = DMA_PRIORITY_HIGH;
      DMic_OnBoard_Dma.Init.Request             = DMA_REQUEST_DIGITAL;

      DMic_OnBoard_Dma.Instance                 = DFSDM_DMA_DIGITAL;

      /* Associate the DMA handle */
      __HAL_LINKDMA(&DMic_OnBoard_DfsdmFilter, hdmaReg, DMic_OnBoard_Dma);

      /* Reset DMA handle state */
      __HAL_DMA_RESET_HANDLE_STATE(&DMic_OnBoard_Dma);

      /* Configure the DMA Channel */
      (void)HAL_DMA_Init(&DMic_OnBoard_Dma);

      /* DMA IRQ Channel configuration */
      HAL_NVIC_SetPriority(DFSDM_DMA_DIGITAL_IRQn, BSP_AUDIO_IN_IT_PRIORITY, 0);
      HAL_NVIC_EnableIRQ(DFSDM_DMA_DIGITAL_IRQn);
    }
  }
}

/**
  * @brief  Initialize the ADC MSP.
  * @param  hDfsdmFilter ADC handle
  * @retval None
  */
void HAL_ADC_Audio_MspInit(ADC_HandleTypeDef *hadc)
{
  if (HAL_ADC_Counter == 0)
  {
    GPIO_InitTypeDef          GPIO_InitStruct;
    static DMA_HandleTypeDef         DmaHandle;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO clock ****************************************/
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /* ADC Periph clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();
    /* ADC Periph interface clock configuration */
    __HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_PLLSAI1);
    /* Enable DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMAMUX1_CLK_ENABLE();

    /*##- 2- Configure peripheral GPIO #########################################*/
    /* ADC Channel GPIO pin configuration */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*##- 3- Configure DMA #####################################################*/

    /*********************** Configure DMA parameters ***************************/
    DmaHandle.Instance                 = DMA1_Channel1;
    DmaHandle.Init.Request             = DMA_REQUEST_ADC1;
    DmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    DmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    DmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
    DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    DmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    DmaHandle.Init.Mode                = DMA_CIRCULAR;
    DmaHandle.Init.Priority            = DMA_PRIORITY_MEDIUM;
    /* Deinitialize  & Initialize the DMA for new transfer */
    (void)HAL_DMA_DeInit(&DmaHandle);
    (void)HAL_DMA_Init(&DmaHandle);

    /* Associate the DMA handle */
    __HAL_LINKDMA(hadc, DMA_Handle, DmaHandle);

    /* NVIC configuration for DMA Input data interrupt */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  }

  HAL_ADC_Counter++;
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



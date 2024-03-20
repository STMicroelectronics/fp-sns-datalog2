/**
  ******************************************************************************
  * @file    SensorTileBoxPro_audio.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   This file provides the Audio driver for the SensorTileBoxPro board 
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "SensorTileBoxPro_audio.h"
#include "SensorTileBoxPro_bus.h"
#include "audio.h"
#include "SensorTileBoxPro_errno.h"

/** @addtogroup BSP
* @{
*/

/** @addtogroup SENSORTILEBOXPRO
* @{
*/

/** @addtogroup SENSORTILEBOXPRO_AUDIO_IN
* @brief This file provides set of firmware functions to manage MEMS microphones
*        initialization on SensorTileBoxPro Kit from STMicroelectronics.
* @{
*/ 

/** @defgroup SENSORTILEBOXPRO_AUDIO_IN_Private_Types
* @{
*/ 

/**
* @}
*/ 

/** @defgroup SENSORTILEBOXPRO_AUDIO_IN_Private_Defines 
* @{
*/ 

#define SAMPLES_PER_MS (AUDIO_IN_SAMPLING_FREQUENCY/1000)

/**
* @}
*/ 

/** @defgroup SENSORTILEBOXPRO_AUDIO_IN_Private_Macros 
* @{
*/

/**
* @}
*/ 

/** @defgroup SENSORTILEBOXPRO_AUDIO_IN_Private_Variables
* @{
*/
          
/* Recording context */
AUDIO_IN_Ctx_t                          AudioInCtx[AUDIO_IN_INSTANCES_NBR] = {0};






/* DMic OnBoard */
MDF_HandleTypeDef                       DMic_OnBoard_MDFFilter;
static MDF_FilterConfigTypeDef          DMic_OnBoard_MdfFilterConfig;	
DMA_HandleTypeDef                       DMic_OnBoard_DmaHandle;
MDF_DmaConfigTypeDef                    DMic_OnBoard_DmaConfig;
DMA_QListTypeDef                        DMic_OnBoard_MdfQueue;
DMA_NodeTypeDef                         DMic_OnBoard_DmaNode;


           /********* TO BE CHECKED int16_t or int32_t?  number of ms ?? nel SWTIN non sembra essere gestito? ***********/
/*Buffer location and size should aligned to cache line size (32 bytes) */
int16_t DMIC_Buffer[DEFAULT_AUDIO_IN_BUFFER_SIZE];

/**
* @}
*/ 

/** @defgroup SENSORTILEBOXPRO_AUDIO_IN_Private_Function_Prototypes 
* @{
*/ 
void MX_ADC1_Init(void);

/**
* @}
*/ 

/** @defgroup SENSORTILEBOXPRO_AUDIO_IN_Exported_Functions 
* @{
*/  
      
 /***** TO BE CHECKED: cambiare parametri init per supportare diverse frequenze *****/

__weak int32_t BSP_AUDIO_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t* AudioInit)
{
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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
      
      if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
      {
        switch(AudioInit->SampleRate)
        {
          case AUDIO_FREQUENCY_16K:
            /**
              * Input clock: 15.36MHz
              * Output clock divider = 10
              * MIC_CLK = 15.36MHz / 10 = 1.536MHz
              */
            DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Divider = 10;
            break;
          case AUDIO_FREQUENCY_32K:
            /**
              * Input clock: 15.36MHz
              * Output clock divider = 10
              * MIC_CLK = 15.36MHz / 10 = 1.536MHz
              */
            DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Divider = 10;
            break;
          case AUDIO_FREQUENCY_48K:
          /**
            * Input clock: 15.36MHz
            * Output clock divider = 5
            * MIC_CLK = 15.36MHz / 5 = 3.072MHz
            */
            DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Divider = 5;
            break;
          default:
            return BSP_ERROR_WRONG_PARAM;
            break;
        }

        /* Initialize MDF */
        DMic_OnBoard_MDFFilter.Instance                                        = DMIC_ONBOARD_FILTER;
        DMic_OnBoard_MDFFilter.Init.CommonParam.InterleavedFilters             = 0U;
        DMic_OnBoard_MDFFilter.Init.CommonParam.ProcClockDivider               = 1U;
        DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Activation         = ENABLE;
        DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Pins               = MDF_OUTPUT_CLOCK_0;
        DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Trigger.Activation = ENABLE;
        DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Trigger.Source     = MDF_CLOCK_TRIG_TRGO;
        DMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Trigger.Edge       = MDF_CLOCK_TRIG_RISING_EDGE;
        DMic_OnBoard_MDFFilter.Init.SerialInterface.Activation                 = ENABLE;
        DMic_OnBoard_MDFFilter.Init.SerialInterface.Mode                       = MDF_SITF_NORMAL_SPI_MODE;
        DMic_OnBoard_MDFFilter.Init.SerialInterface.ClockSource                = MDF_SITF_CCK0_SOURCE;
        DMic_OnBoard_MDFFilter.Init.SerialInterface.Threshold                  = 31U;
        DMic_OnBoard_MDFFilter.Init.FilterBistream                             = MDF_BITSTREAM0_RISING;

        if (HAL_MDF_Init(&DMic_OnBoard_MDFFilter) != HAL_OK)  
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }

        switch(AudioInit->SampleRate)
        {
          case AUDIO_FREQUENCY_16K:
            /**
              * MIC_CLK = 1.536MHz
              * Filter = SINC5
              * Decimation Ratio = 24
              * ReshapeFilter Enabled - Decimation Ratio = 4
              * Audio signal = 1.536MHz / (24 * 4) = 16kHz
              *
              * Sinc5, d=24 --> 24bit --> Gain = -4
              * Gain it's expressed in 3dB steps --> half a bit
              */
            DMic_OnBoard_MdfFilterConfig.Gain = 0;
            DMic_OnBoard_MdfFilterConfig.DecimationRatio = 24;
            break;
          case AUDIO_FREQUENCY_32K:
            /**
              * MIC_CLK = 1.536MHz
              * Filter = SINC5
              * Decimation Ratio = 12
              * ReshapeFilter Enabled - Decimation Ratio = 4
              * Audio signal = 1.536MHz / (12 * 4) = 32kHz
              *
              * Sinc5, d=12 --> 19bit --> Gain = 5
              * Gain it's expressed in 3dB steps --> half a bit
              */
            DMic_OnBoard_MdfFilterConfig.Gain = 6;
            DMic_OnBoard_MdfFilterConfig.DecimationRatio = 12;
            break;
          case AUDIO_FREQUENCY_48K:
            /**
              * MIC_CLK = 3.072MHz
              * Filter = SINC5
              * Decimation Ratio = 16
              * ReshapeFilter Enabled - Decimation Ratio = 4
              * Audio signal = 3.072MHz / (16 * 4) = 48kHz
              *
              * Sinc5, d=16 --> 21bit --> Gain = 1
              * Gain it's expressed in 3dB steps --> half a bit
              */
            DMic_OnBoard_MdfFilterConfig.Gain = 4;
            DMic_OnBoard_MdfFilterConfig.DecimationRatio = 16;
            break;
          default:
            return BSP_ERROR_WRONG_PARAM;
            break;
        }
        
        /** Initialize filter configuration parameters */
        DMic_OnBoard_MdfFilterConfig.DataSource                         = MDF_DATA_SOURCE_BSMX;
        DMic_OnBoard_MdfFilterConfig.Delay                              = 0U;
        DMic_OnBoard_MdfFilterConfig.CicMode                            = MDF_ONE_FILTER_SINC5;
        DMic_OnBoard_MdfFilterConfig.Offset                             = 0;
        DMic_OnBoard_MdfFilterConfig.ReshapeFilter.Activation           = ENABLE;
        DMic_OnBoard_MdfFilterConfig.ReshapeFilter.DecimationRatio      = MDF_RSF_DECIMATION_RATIO_4;
        DMic_OnBoard_MdfFilterConfig.HighPassFilter.Activation          = ENABLE;
        DMic_OnBoard_MdfFilterConfig.HighPassFilter.CutOffFrequency     = MDF_HPF_CUTOFF_0_000625FPCM;
        DMic_OnBoard_MdfFilterConfig.Integrator.Activation              = DISABLE;
        DMic_OnBoard_MdfFilterConfig.SoundActivity.Activation           = DISABLE;
        DMic_OnBoard_MdfFilterConfig.FifoThreshold                      = MDF_FIFO_THRESHOLD_NOT_EMPTY;
        DMic_OnBoard_MdfFilterConfig.DiscardSamples                     = 0U;
        DMic_OnBoard_MdfFilterConfig.SnapshotFormat                     = MDF_SNAPSHOT_23BITS;
        DMic_OnBoard_MdfFilterConfig.Trigger.Source                     = MDF_FILTER_TRIG_TRGO;
        DMic_OnBoard_MdfFilterConfig.Trigger.Edge                       = MDF_FILTER_TRIG_RISING_EDGE;
        DMic_OnBoard_MdfFilterConfig.AcquisitionMode                    = MDF_MODE_SYNC_CONT;
 
        /* Initialize DMA configuration parameters */
        DMic_OnBoard_DmaConfig.Address        = (uint32_t)&DMIC_Buffer[0];
        DMic_OnBoard_DmaConfig.DataLength     = (DEFAULT_AUDIO_IN_BUFFER_SIZE * 2U);
        DMic_OnBoard_DmaConfig.MsbOnly        = ENABLE;
        
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }	
  else
  {
    
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      /* De-initializes DFSDM Filter handle */
      if(DMic_OnBoard_MDFFilter.Instance != NULL)
      {
        if(HAL_MDF_DeInit(&DMic_OnBoard_MDFFilter) != HAL_OK)
        {
          ret =  BSP_ERROR_PERIPH_FAILURE;
        }
        DMic_OnBoard_MDFFilter.Instance = NULL;
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
int32_t BSP_AUDIO_IN_Record(uint32_t Instance, uint8_t* pBuf, uint32_t NbrOfBytes)
{
  int32_t ret = BSP_ERROR_NONE;
  AudioInCtx[Instance].pBuff = (uint16_t*)pBuf;
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {	
    
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if (HAL_MDF_AcqStart_DMA(&DMic_OnBoard_MDFFilter, &DMic_OnBoard_MdfFilterConfig, &DMic_OnBoard_DmaConfig) != HAL_OK) 
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      if (HAL_MDF_GenerateTrgo(&DMic_OnBoard_MDFFilter) != HAL_OK) 
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }    
  }
  
  if(ret != BSP_ERROR_PERIPH_FAILURE)
  {
    /* Update BSP AUDIO IN state */     
    AudioInCtx[Instance].State = AUDIO_IN_STATE_RECORDING;
  }
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
  if(Instance >= AUDIO_IN_INSTANCES_NBR) 
  {
    return BSP_ERROR_WRONG_PARAM;  
  }
  else
  {
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if(HAL_MDF_AcqStop_DMA(&DMic_OnBoard_MDFFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }	
    }
  }
  
  if(ret != BSP_ERROR_PERIPH_FAILURE)
  {
    /* Update BSP AUDIO IN state */     
    AudioInCtx[Instance].State = AUDIO_IN_STATE_STOP;
  }
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
  if(Instance >= AUDIO_IN_INSTANCES_NBR) 
  {
    return BSP_ERROR_WRONG_PARAM;  
  }
  else
  {
    
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if(HAL_MDF_AcqStop_DMA(&DMic_OnBoard_MDFFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }	
    }
  }
  
  if(ret != BSP_ERROR_PERIPH_FAILURE)
  {  
    /* Update BSP AUDIO IN state */     
    AudioInCtx[Instance].State = AUDIO_IN_STATE_PAUSE;
  }
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
  if(Instance >= AUDIO_IN_INSTANCES_NBR) 
  {
    return BSP_ERROR_WRONG_PARAM;  
  }
  else
  {
    
    if ((AudioInCtx[Instance].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      if (HAL_MDF_AcqStart_DMA(&DMic_OnBoard_MDFFilter, &DMic_OnBoard_MdfFilterConfig, &DMic_OnBoard_DmaConfig) != HAL_OK) 
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }   		
    }  
  }
  
  if(ret != BSP_ERROR_PERIPH_FAILURE)
  {  
    /* Update BSP AUDIO IN state */     
    AudioInCtx[Instance].State = AUDIO_IN_STATE_RECORDING;
  }
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }
    
    audio_init.Device           = Device;
    audio_init.ChannelsNbr      = AudioInCtx[Instance].ChannelsNbr;  
    audio_init.SampleRate       = AudioInCtx[Instance].SampleRate;   
    audio_init.BitsPerSample    = AudioInCtx[Instance].BitsPerSample;
    audio_init.Volume           = AudioInCtx[Instance].Volume;
    
    if(BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {	
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }	
    
    audio_init.Device           = AudioInCtx[Instance].Device;
    audio_init.ChannelsNbr      = AudioInCtx[Instance].ChannelsNbr;  
    audio_init.SampleRate       = SampleRate;   
    audio_init.BitsPerSample    = AudioInCtx[Instance].BitsPerSample;
    audio_init.Volume           = AudioInCtx[Instance].Volume;
    
    if(BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {	
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }	
    
    audio_init.Device           = AudioInCtx[Instance].Device;
    audio_init.ChannelsNbr      = AudioInCtx[Instance].ChannelsNbr;  
    audio_init.SampleRate       = AudioInCtx[Instance].SampleRate; 
    audio_init.BitsPerSample    = BitsPerSample;
    audio_init.Volume           = AudioInCtx[Instance].Volume;
    
    if(BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {	
    if (BSP_AUDIO_IN_DeInit(Instance) != BSP_ERROR_NONE)
    {
      ret = BSP_ERROR_BUSY;
    }	
    
    audio_init.Device 	     = AudioInCtx[Instance].Device;
    audio_init.ChannelsNbr   = ChannelNbr;  
    audio_init.SampleRate    = AudioInCtx[Instance].SampleRate; 
    audio_init.BitsPerSample = AudioInCtx[Instance].BitsPerSample;
    audio_init.Volume        = AudioInCtx[Instance].Volume;
    
    if(BSP_AUDIO_IN_Init(Instance, &audio_init) != BSP_ERROR_NONE)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
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



/********* TO BE CHECKED: come mai nel if digital processa anche il buffer amic? ******/
/********* TO BE CHECKED: come gestire due canali diversi (analog e digital) con frequenze audio diverse, ha senso? ******/
/**
* @brief  Regular conversion complete callback. 
* @note   In interrupt mode, user has to read conversion value in this function
using HAL_DFSDM_FilterGetRegularValue.
* @param  hmdf   MDF filter handle.
* @retval None
*/
void HAL_MDF_AcqCpltCallback(MDF_HandleTypeDef *hmdf)
{
  UNUSED(hmdf);
  uint32_t j;
 
//  HAL_GPIO_TogglePin(DIL_DEN_GPIO_Port, DIL_DEN_Pin);   /// debug by oscilloscope
  
  if(AudioInCtx[0].IsMultiBuff == 1U)
  {
    /* Call the record update function to get the second half */
    BSP_AUDIO_IN_TransferComplete_CallBack(1);
  }
  else
  { 
    if ((AudioInCtx[0].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      for (j = 0U; j < ((AudioInCtx[0].SampleRate / 1000U) * N_MS_PER_INTERRUPT); j++)
      {
        /*DMIC_ONBOARD_CHANNEL*/
        //AudioInCtx[0].HP_Filters[0].Z = ((DMIC_Buffer[j + ((AudioInCtx[0].SampleRate / 1000U)* N_MS_PER_INTERRUPT)]) * (int32_t)(AudioInCtx[0].Volume))/100;
        AudioInCtx[0].HP_Filters[0].Z = ((DMIC_Buffer[j + ((AudioInCtx[0].SampleRate / 1000U)* N_MS_PER_INTERRUPT)]) * (int32_t)(AudioInCtx[0].Volume));
        AudioInCtx[0].HP_Filters[0].oldOut = (0xFC * (AudioInCtx[0].HP_Filters[0].oldOut + AudioInCtx[0].HP_Filters[0].Z - AudioInCtx[0].HP_Filters[0].oldIn)) / 256;
        AudioInCtx[0].HP_Filters[0].oldIn = AudioInCtx[0].HP_Filters[0].Z;
        AudioInCtx[0].pBuff[AudioInCtx[0].ChannelsNbr * j] = (uint16_t) SaturaLH(AudioInCtx[0].HP_Filters[0].oldOut, -32760, 32760); 
      }
    }
    BSP_AUDIO_IN_TransferComplete_CallBack(1);
  }
}



/**
* @brief  Half regular conversion complete callback. 
* @param  hmdf   MDF filter handle.
* @retval None
*/
void HAL_MDF_AcqHalfCpltCallback(MDF_HandleTypeDef *hmdf)
{
  UNUSED(hmdf);
  uint32_t j;
  
//  HAL_GPIO_TogglePin(DIL_DEN_GPIO_Port, DIL_DEN_Pin);   /// debug by oscilloscope
  
  if(AudioInCtx[0].IsMultiBuff == 1U)
  {
    /* Call the record update function to get the second half */
    BSP_AUDIO_IN_HalfTransfer_CallBack(1);
  }
  else
  {
    if ((AudioInCtx[0].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      for (j = 0U; j < ((AudioInCtx[0].SampleRate / 1000U)* N_MS_PER_INTERRUPT); j++)
      {
        /*DMIC_ONBOARD_CHANNEL*/
        //AudioInCtx[0].HP_Filters[0].Z = (DMIC_Buffer[j] *  (int32_t)(AudioInCtx[0].Volume))/100,
         AudioInCtx[0].HP_Filters[0].Z = (DMIC_Buffer[j] *  (int32_t)(AudioInCtx[0].Volume)),
        AudioInCtx[0].HP_Filters[0].oldOut = (0xFC * (AudioInCtx[0].HP_Filters[0].oldOut + AudioInCtx[0].HP_Filters[0].Z - AudioInCtx[0].HP_Filters[0].oldIn)) / 256;
        AudioInCtx[0].HP_Filters[0].oldIn = AudioInCtx[0].HP_Filters[0].Z;
        AudioInCtx[0].pBuff[AudioInCtx[0].ChannelsNbr * (j)] = (uint16_t) SaturaLH(AudioInCtx[0].HP_Filters[0].oldOut, -32760, 32760);
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




/***** TO BE CHECKED creare callback e registrarle come per i bus ?  retunr al posto di While(1) in caso di errore? *****/


void HAL_MDF_MspInit(MDF_HandleTypeDef *hmdf)
{
  DMA_NodeConfTypeDef dmaLinkNodeConfig;
  GPIO_InitTypeDef  GPIO_Init;

  if(hmdf->Instance == DMIC_ONBOARD_FILTER) 
  {
    /** Initializes the peripherals clock */
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADF1;
    PeriphClkInit.Adf1ClockSelection = RCC_ADF1CLKSOURCE_PLL3;
    PeriphClkInit.PLL3.PLL3Source = RCC_PLLSOURCE_HSE;
    PeriphClkInit.PLL3.PLL3M = 2;
    PeriphClkInit.PLL3.PLL3N = 48;
    PeriphClkInit.PLL3.PLL3P = 2;
    PeriphClkInit.PLL3.PLL3Q = 25;
    PeriphClkInit.PLL3.PLL3R = 2;
    PeriphClkInit.PLL3.PLL3RGE = RCC_PLLVCIRANGE_0;
    PeriphClkInit.PLL3.PLL3FRACN = 0;
    PeriphClkInit.PLL3.PLL3ClockOut = RCC_PLL3_DIVQ;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
       for(;;){}   /* Blocking error */
    }
    
    __HAL_RCC_ADF1_CONFIG(RCC_ADF1CLKSOURCE_PLL3);
    
    /* Enable MDF1 clock */
    DMIC_ONBOARD_MDFx_CLK_ENABLE();

    /* Configure GPIOs used for ADF1 */
    DMIC_ONBOARD_CKOUT_GPIO_CLK_ENABLE();
    DMIC_ONBOARD_DATAIN_GPIO_CLK_ENABLE();
   
    GPIO_Init.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init.Pull      = /*GPIO_NOPULL*/GPIO_PULLDOWN;
    GPIO_Init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;

    GPIO_Init.Alternate = DMIC_ONBOARD_CKOUT_AF;
    GPIO_Init.Pin       = DMIC_ONBOARD_CKOUT_PIN;
    HAL_GPIO_Init(DMIC_ONBOARD_CKOUT_GPIO_PORT, &GPIO_Init);

    GPIO_Init.Alternate = DMIC_ONBOARD_DATAIN_AF;
    GPIO_Init.Pin       = DMIC_ONBOARD_DATIN_PIN;
    HAL_GPIO_Init(DMIC_ONBOARD_DATIN_GPIO_PORT, &GPIO_Init);
   
    /* Configure DMA used for ADF1 */
    DMIC_ONBOARD_DMAx_CLK_ENABLE();
    
    /* Set node type */
    dmaLinkNodeConfig.NodeType                            = DMA_GPDMA_LINEAR_NODE;
    /* Set common node parameters */
    dmaLinkNodeConfig.Init.Request                        = DMIC_ONBAORD_DMA_REQUEST;
    dmaLinkNodeConfig.Init.BlkHWRequest                   = DMA_BREQ_SINGLE_BURST;
    dmaLinkNodeConfig.Init.Direction                      = DMA_PERIPH_TO_MEMORY;
    dmaLinkNodeConfig.Init.SrcInc                         = DMA_SINC_FIXED;
    dmaLinkNodeConfig.Init.DestInc                        = DMA_DINC_INCREMENTED;
    dmaLinkNodeConfig.Init.SrcDataWidth                   = DMA_SRC_DATAWIDTH_HALFWORD; 
    dmaLinkNodeConfig.Init.DestDataWidth                  = DMA_DEST_DATAWIDTH_HALFWORD;
    dmaLinkNodeConfig.Init.SrcBurstLength                 = 1;
    dmaLinkNodeConfig.Init.DestBurstLength                = 1;
    dmaLinkNodeConfig.Init.Priority                       = DMA_HIGH_PRIORITY;
    dmaLinkNodeConfig.Init.TransferEventMode              = DMA_TCEM_EACH_LL_ITEM_TRANSFER;
    dmaLinkNodeConfig.Init.TransferAllocatedPort          = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
    dmaLinkNodeConfig.Init.Mode                           = DMA_NORMAL;
    
    /* Set node data handling parameters */
    dmaLinkNodeConfig.DataHandlingConfig.DataExchange     = DMA_EXCHANGE_NONE;
    dmaLinkNodeConfig.DataHandlingConfig.DataAlignment    = DMA_DATA_UNPACK;
    
    /* Set node trigger parameters */
    dmaLinkNodeConfig.TriggerConfig.TriggerPolarity       = DMA_TRIG_POLARITY_MASKED;
    dmaLinkNodeConfig.RepeatBlockConfig.RepeatCount         = 1U;
    dmaLinkNodeConfig.RepeatBlockConfig.SrcAddrOffset       = 0;
    dmaLinkNodeConfig.RepeatBlockConfig.DestAddrOffset      = 0;
    dmaLinkNodeConfig.RepeatBlockConfig.BlkSrcAddrOffset    = 0;
    dmaLinkNodeConfig.RepeatBlockConfig.BlkDestAddrOffset   = 0;
    
    
    /* Build NodeTx */
    HAL_DMAEx_List_BuildNode(&dmaLinkNodeConfig, &DMic_OnBoard_DmaNode);
    
    /* Insert NodeTx to SAI queue */
    HAL_DMAEx_List_InsertNode_Tail(&DMic_OnBoard_MdfQueue, &DMic_OnBoard_DmaNode);
    
    /* Select the DMA instance to be used for the transfer : GPDMA_Channel1 */
    DMic_OnBoard_DmaHandle.Instance                         = GPDMA1_Channel1;
    
    /* Set queue circular mode for sai queue */
    HAL_DMAEx_List_SetCircularMode(&DMic_OnBoard_MdfQueue);
    
    DMic_OnBoard_DmaHandle.InitLinkedList.Priority          = DMA_HIGH_PRIORITY;
    DMic_OnBoard_DmaHandle.InitLinkedList.LinkStepMode      = DMA_LSM_FULL_EXECUTION;
    DMic_OnBoard_DmaHandle.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
    DMic_OnBoard_DmaHandle.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
    DMic_OnBoard_DmaHandle.InitLinkedList.LinkedListMode    = DMA_LINKEDLIST_CIRCULAR;
    
    /* DMA linked list init */
    HAL_DMAEx_List_Init(&DMic_OnBoard_DmaHandle);
    
    /* Link SAI queue to DMA channel */
    HAL_DMAEx_List_LinkQ(&DMic_OnBoard_DmaHandle, &DMic_OnBoard_MdfQueue);
    
    /* Associate the DMA handle */
    __HAL_LINKDMA(hmdf, hdma, DMic_OnBoard_DmaHandle);

    HAL_NVIC_SetPriority(DMIC_ONBAORD_DMA_IRQn, 0x01, 0);
    HAL_NVIC_EnableIRQ(DMIC_ONBAORD_DMA_IRQn);
}
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

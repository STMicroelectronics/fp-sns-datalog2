/**
 ******************************************************************************
 * @file    STWIN.box_audio.c
 * @author  SRA
 * @brief   This file provides the Audio driver for the STWIN.box board 
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
#include "STWIN.box_audio.h"
#include "STWIN.box_bus.h"
#include "audio.h"

/** @addtogroup BSP
* @{
*/

/** @addtogroup STWIN_BOX
* @{
*/

/** @addtogroup STWIN_BOX_AUDIO_IN
* @brief This file provides set of firmware functions to manage MEMS microphones
*        initialization on STWIN.box Kit from STMicroelectronics.
* @{
*/ 

/** @defgroup STWIN_BOX_AUDIO_IN_Private_Types
* @{
*/ 

/**
* @}
*/ 

/** @defgroup STWIN_BOX_AUDIO_IN_Private_Defines 
* @{
*/ 

#define SAMPLES_PER_MS (AUDIO_IN_SAMPLING_FREQUENCY/1000)

/**
* @}
*/ 

/** @defgroup STWIN_BOX_AUDIO_IN_Private_Macros 
* @{
*/

/**
* @}
*/ 

/** @defgroup STWIN_BOX_AUDIO_IN_Private_Variables
* @{
*/
          
/* Recording context */
AUDIO_IN_Ctx_t                          AudioInCtx[AUDIO_IN_INSTANCES_NBR] = {0};

/* AMic OnBoard */
MDF_HandleTypeDef                       AMic_OnBoard_MDFFilter;
static MDF_FilterConfigTypeDef          AMic_OnBoard_MdfFilterConfig;
DMA_QListTypeDef                        AMic_OnBoard_MdfQueue;
DMA_NodeTypeDef                         AMic_OnBoard_DmaNode;
static MDF_DmaConfigTypeDef             AMic_OnBoard_DmaConfig;
static DMA_HandleTypeDef                AMic_OnBoard_DmaHandle;

/**** TO BE CHECKED: use same adc for BC ? con U5 ce ne sono due, uno a 12 e uno a 14 bit ******/
ADC_HandleTypeDef                       AMic_AdcHandle;

/* DMic OnBoard */
MDF_HandleTypeDef                       DMic_OnBoard_MDFFilter;
static MDF_FilterConfigTypeDef          DMic_OnBoard_MdfFilterConfig;	
DMA_QListTypeDef                        DMic_OnBoard_MdfQueue;
DMA_NodeTypeDef                         DMic_OnBoard_DmaNode;
static MDF_DmaConfigTypeDef             DMic_OnBoard_DmaConfig;
static DMA_HandleTypeDef                DMic_OnBoard_DmaHandle;

static int32_t HAL_ADC_Counter = 0;

           /********* TO BE CHECKED int16_t or int32_t?  number of ms ?? nel SWTIN non sembra essere gestito? ***********/
/*Buffer location and size should aligned to cache line size (32 bytes) */
static int16_t DMIC_Buffer[DEFAULT_AUDIO_IN_BUFFER_SIZE];
static int16_t AMIC_Buffer[DEFAULT_AUDIO_IN_BUFFER_SIZE];

/**
* @}
*/ 

/** @defgroup STWIN_BOX_AUDIO_IN_Private_Function_Prototypes 
* @{
*/ 
void MX_ADC1_Init(void);

/**
* @}
*/ 

/** @defgroup STWIN_BOX_AUDIO_IN_Exported_Functions 
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
    
      /********* TO BE CHECKED: configurazione troppo diverse per fare una funzione comune come con STWIN MX_DFSDM_Init ******/
      if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
      {
        /* Initialize MDF */
        AMic_OnBoard_MDFFilter.Instance                                 = AMIC_ONBOARD_FILTER;
        AMic_OnBoard_MDFFilter.Init.CommonParam.InterleavedFilters      = 0;
        AMic_OnBoard_MDFFilter.Init.CommonParam.ProcClockDivider        = 1;
        AMic_OnBoard_MDFFilter.Init.CommonParam.OutputClock.Activation  = DISABLE;
        AMic_OnBoard_MDFFilter.Init.SerialInterface.Activation          = DISABLE;
         
        if (HAL_MDF_Init(&AMic_OnBoard_MDFFilter) != HAL_OK)
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }
        
        switch(AudioInit->SampleRate)
        {
          case AUDIO_FREQUENCY_16K:
            /** ADC Clk: 768kHz --> Decimation 12 = 64kHz
              * ReshapeFilter Enabled - Decimation Ratio 4 = 16KHz
              * Sinc5, d=4 --> 22bit --> gain 2bit --> 8 bit shift obtained with DMA MsbOnly = 16bit
              * Gain it's expressed in 3dB steps --> half a bit
              */
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.Activation = ENABLE;
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.DecimationRatio = MDF_RSF_DECIMATION_RATIO_4;
            AMic_OnBoard_MdfFilterConfig.CicMode = MDF_TWO_FILTERS_MCIC_SINC3;
            AMic_OnBoard_MdfFilterConfig.Gain = -2;
            AMic_OnBoard_MdfFilterConfig.DecimationRatio = 12;
            break;
          case AUDIO_FREQUENCY_32K:
            /** ADC Clk: 768kHz --> Decimation 6 = 128kHz
              * ReshapeFilter Enabled - Decimation Ratio 4 = 32KHz
              * Sinc5, d=4 --> 22bit --> gain 2bit --> 8 bit shift obtained with DMA MsbOnly = 16bit
              * Gain it's expressed in 3dB steps --> half a bit
              */
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.Activation = ENABLE;
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.DecimationRatio = MDF_RSF_DECIMATION_RATIO_4;
            AMic_OnBoard_MdfFilterConfig.CicMode = MDF_ONE_FILTER_SINC4;
            AMic_OnBoard_MdfFilterConfig.Gain = -4;
            AMic_OnBoard_MdfFilterConfig.DecimationRatio = 6;
            break;
          case AUDIO_FREQUENCY_48K:
            /** ADC Clk: 768kHz --> Decimation 4 = 192kHz
              * ReshapeFilter Enabled - Decimation Ratio 4 = 48KHz
              * Sinc5, d=4 --> 22bit --> gain 2bit --> 8 bit shift obtained with DMA MsbOnly = 16bit
              * Gain it's expressed in 3dB steps --> half a bit
              */
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.Activation = ENABLE;
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.DecimationRatio = MDF_RSF_DECIMATION_RATIO_4;
            AMic_OnBoard_MdfFilterConfig.CicMode = MDF_ONE_FILTER_SINC5;
            AMic_OnBoard_MdfFilterConfig.Gain = -2;
            AMic_OnBoard_MdfFilterConfig.DecimationRatio = 4;
            break;
          case AUDIO_FREQUENCY_96K:
            /** ADC Clk: 768kHz --> Decimation 2 = 394kHz
              * ReshapeFilter Enabled - Decimation Ratio 4 = 96KHz
              * Sinc5, d=2 --> 21bit --> gain 1bit --> 8 bit shift obtained with DMA MsbOnly = 16bit
              * Gain it's expressed in 3dB steps --> half a bit
              */
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.Activation = ENABLE;
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.DecimationRatio = MDF_RSF_DECIMATION_RATIO_4;
            AMic_OnBoard_MdfFilterConfig.CicMode = MDF_ONE_FILTER_SINC5;
            AMic_OnBoard_MdfFilterConfig.Gain = 8;
            AMic_OnBoard_MdfFilterConfig.DecimationRatio = 2;
            break;
          case AUDIO_FREQUENCY_192K:
            /** ADC Clk: 768kHz --> Decimation 4 = 192kHz
              * Sinc5, d=4 --> 22bit --> gain 2bit --> 8 bit shift obtained with DMA MsbOnly = 16bit
              * Gain it's expressed in 3dB steps --> half a bit
              */
            AMic_OnBoard_MdfFilterConfig.ReshapeFilter.Activation = DISABLE;
            AMic_OnBoard_MdfFilterConfig.CicMode = MDF_ONE_FILTER_SINC5;
            AMic_OnBoard_MdfFilterConfig.Gain = 2;
            AMic_OnBoard_MdfFilterConfig.DecimationRatio = 4;
            break;
          default:
            return BSP_ERROR_WRONG_PARAM;
            break;
        }
          
        AMic_OnBoard_MdfFilterConfig.DataSource = MDF_DATA_SOURCE_ADCITF1;
        AMic_OnBoard_MdfFilterConfig.Delay = 0;        
        AMic_OnBoard_MdfFilterConfig.Offset = 0;
        AMic_OnBoard_MdfFilterConfig.HighPassFilter.Activation = ENABLE;
        AMic_OnBoard_MdfFilterConfig.HighPassFilter.CutOffFrequency = MDF_HPF_CUTOFF_0_000625FPCM;
        AMic_OnBoard_MdfFilterConfig.Integrator.Activation = DISABLE;
        AMic_OnBoard_MdfFilterConfig.SoundActivity.Activation = DISABLE;
        AMic_OnBoard_MdfFilterConfig.FifoThreshold = MDF_FIFO_THRESHOLD_NOT_EMPTY;
        AMic_OnBoard_MdfFilterConfig.DiscardSamples = 0;
        if (AudioInCtx[Instance].ChannelsNbr == 1U)
        {
          AMic_OnBoard_MdfFilterConfig.AcquisitionMode = MDF_MODE_ASYNC_CONT;
        }
        else
        {
          AMic_OnBoard_MdfFilterConfig.AcquisitionMode = MDF_MODE_SYNC_CONT;
          AMic_OnBoard_MdfFilterConfig.Trigger.Source = MDF_FILTER_TRIG_ADF_TRGO;
          AMic_OnBoard_MdfFilterConfig.Trigger.Edge = MDF_FILTER_TRIG_RISING_EDGE;
        }

        /* Initialize DMA configuration parameters */
        AMic_OnBoard_DmaConfig.Address        = (uint32_t)&AMIC_Buffer[0];
        AMic_OnBoard_DmaConfig.DataLength     = (DEFAULT_AUDIO_IN_BUFFER_SIZE * 2U);
        AMic_OnBoard_DmaConfig.MsbOnly        = ENABLE;
  
        /*adc init*/
        MX_ADC1_Init();
        
      }
      
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
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      /* De-initializes DFSDM Filter handle */
      if(AMic_OnBoard_MDFFilter.Instance != NULL)
      {
        if(HAL_MDF_DeInit(&AMic_OnBoard_MDFFilter) != HAL_OK)
        {
          ret =  BSP_ERROR_PERIPH_FAILURE;
        }
        AMic_OnBoard_MDFFilter.Instance = NULL;
      }
      
      /* De-initializes ADC handle */
      if(AMic_AdcHandle.Instance != NULL)
      {     
        /***** TO BE CHECKED: usare lo stesso ADC del BC e quindi implementare deinit in STWIN.box.c? *******/
        if(HAL_ADC_DeInit(&AMic_AdcHandle) != HAL_OK)
        {
          ret =  BSP_ERROR_PERIPH_FAILURE;
        }
        AMic_AdcHandle.Instance = NULL;
      }
    }
    
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
  UNUSED(NbrOfBytes);
  int32_t ret = BSP_ERROR_NONE;
  AudioInCtx[Instance].pBuff = (uint16_t*)pBuf;
  
  if(Instance >= AUDIO_IN_INSTANCES_NBR)
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else
  {	
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if (HAL_ADC_Start(&AMic_AdcHandle) != HAL_OK)
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
      if (HAL_MDF_AcqStart_DMA(&AMic_OnBoard_MDFFilter, &AMic_OnBoard_MdfFilterConfig, &AMic_OnBoard_DmaConfig) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    
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
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if(HAL_MDF_AcqStop_DMA(&AMic_OnBoard_MDFFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      
      if (HAL_ADC_Stop(&AMic_AdcHandle) != HAL_OK)
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
    }
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
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if(HAL_MDF_AcqStop_DMA(&AMic_OnBoard_MDFFilter) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      
      if (HAL_ADC_Stop(&AMic_AdcHandle) != HAL_OK)
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
    }
    
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
    if ((AudioInCtx[Instance].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      if (HAL_ADC_Start(&AMic_AdcHandle) != HAL_OK)
      {
        ret =  BSP_ERROR_PERIPH_FAILURE;
      }
      if (HAL_MDF_AcqStart_DMA(&AMic_OnBoard_MDFFilter, &AMic_OnBoard_MdfFilterConfig, &AMic_OnBoard_DmaConfig) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
    
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
  uint8_t aMic_idx = 0;
 
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6); 
  
  if(AudioInCtx[0].IsMultiBuff == 1U)
  {
    /* Call the record update function to get the second half */
    BSP_AUDIO_IN_TransferComplete_CallBack(1);
  }
  else
  { 
    if ((AudioInCtx[0].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      aMic_idx = 1;
      for (j = 0U; j < ((AudioInCtx[0].SampleRate / 1000U) * N_MS_PER_INTERRUPT); j++)
      {
        /*DMIC_ONBOARD_CHANNEL*/
        AudioInCtx[0].HP_Filters[0].Z = ((DMIC_Buffer[j + ((AudioInCtx[0].SampleRate / 1000U)* N_MS_PER_INTERRUPT)]) * (int32_t)(AudioInCtx[0].Volume))/100;
        AudioInCtx[0].HP_Filters[0].oldOut = (0xFC * (AudioInCtx[0].HP_Filters[0].oldOut + AudioInCtx[0].HP_Filters[0].Z - AudioInCtx[0].HP_Filters[0].oldIn)) / 256;
        AudioInCtx[0].HP_Filters[0].oldIn = AudioInCtx[0].HP_Filters[0].Z;
        AudioInCtx[0].pBuff[AudioInCtx[0].ChannelsNbr * j] = (uint16_t) SaturaLH(AudioInCtx[0].HP_Filters[0].oldOut, -32760, 32760); 
      }
    }
    if ((AudioInCtx[0].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      for (j = 0U; j < ((AudioInCtx[0].SampleRate / 1000U) * N_MS_PER_INTERRUPT); j++)
      {
        AudioInCtx[0].HP_Filters[1].Z = ((AMIC_Buffer[j + ((AudioInCtx[0].SampleRate / 1000U)* N_MS_PER_INTERRUPT)]) * (int32_t)(AudioInCtx[0].Volume))/100;
        AudioInCtx[0].HP_Filters[1].oldOut = (0xFC * (AudioInCtx[0].HP_Filters[1].oldOut + AudioInCtx[0].HP_Filters[1].Z - AudioInCtx[0].HP_Filters[1].oldIn)) / 256;
        AudioInCtx[0].HP_Filters[1].oldIn = AudioInCtx[0].HP_Filters[1].Z;
        AudioInCtx[0].pBuff[(AudioInCtx[0].ChannelsNbr*j)+aMic_idx] = (uint16_t) SaturaLH(AudioInCtx[0].HP_Filters[1].oldOut, -32760, 32760);
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
  uint8_t aMic_idx = 0;
  
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
  
  if(AudioInCtx[0].IsMultiBuff == 1U)
  {
    /* Call the record update function to get the second half */
    BSP_AUDIO_IN_HalfTransfer_CallBack(1);
  }
  else
  {
    if ((AudioInCtx[0].Device & ONBOARD_DIGITAL_MIC_MASK) != 0U)
    {
      aMic_idx = 1;
      for (j = 0U; j < ((AudioInCtx[0].SampleRate / 1000U)* N_MS_PER_INTERRUPT); j++)
      {
        /*DMIC_ONBOARD_CHANNEL*/
        AudioInCtx[0].HP_Filters[0].Z = (DMIC_Buffer[j] *  (int32_t)(AudioInCtx[0].Volume))/100;
        AudioInCtx[0].HP_Filters[0].oldOut = (0xFC * (AudioInCtx[0].HP_Filters[0].oldOut + AudioInCtx[0].HP_Filters[0].Z - AudioInCtx[0].HP_Filters[0].oldIn)) / 256;
        AudioInCtx[0].HP_Filters[0].oldIn = AudioInCtx[0].HP_Filters[0].Z;
        AudioInCtx[0].pBuff[AudioInCtx[0].ChannelsNbr * (j)] = (uint16_t) SaturaLH(AudioInCtx[0].HP_Filters[0].oldOut, -32760, 32760);
      }
    }
    if ((AudioInCtx[0].Device & ONBOARD_ANALOG_MIC_MASK) != 0U)
    {
      for (j = 0U; j < ((AudioInCtx[0].SampleRate / 1000U)* N_MS_PER_INTERRUPT); j++)
      {
        AudioInCtx[0].HP_Filters[1].Z = (AMIC_Buffer[j] *  (int32_t)(AudioInCtx[0].Volume))/100;
        AudioInCtx[0].HP_Filters[1].oldOut = (0xFC * (AudioInCtx[0].HP_Filters[1].oldOut + AudioInCtx[0].HP_Filters[1].Z - AudioInCtx[0].HP_Filters[1].oldIn)) / 256;
        AudioInCtx[0].HP_Filters[1].oldIn = AudioInCtx[0].HP_Filters[1].Z;
        AudioInCtx[0].pBuff[(AudioInCtx[0].ChannelsNbr*j)+aMic_idx] = (uint16_t) SaturaLH(AudioInCtx[0].HP_Filters[1].oldOut, -32760, 32760);
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


/******* TO BE CHECKED: return void ? *****/
/**
* @brief  Initialize the ADC channel
* @param  None
* @retval None
*/
/* ADC1 init function */
void MX_ADC1_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /******* TO BE CHECKED: cos'� ADC_TEST_IRQ ? ******/
   /** Common config */
  AMic_AdcHandle.Instance = ADC1;
  AMic_AdcHandle.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  AMic_AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
  AMic_AdcHandle.Init.ScanConvMode = ADC_SCAN_DISABLE;
  AMic_AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  AMic_AdcHandle.Init.LowPowerAutoWait = DISABLE;
  AMic_AdcHandle.Init.ContinuousConvMode = ENABLE;
  AMic_AdcHandle.Init.NbrOfConversion = 1;
  AMic_AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AMic_AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  AMic_AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  AMic_AdcHandle.Init.DMAContinuousRequests = DISABLE;
  AMic_AdcHandle.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  AMic_AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  AMic_AdcHandle.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
#ifdef ADC_TEST_IRQ
  AMic_AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
#else
  AMic_AdcHandle.Init.ConversionDataManagement = ADC_CONVERSIONDATA_MDF;
#endif
  AMic_AdcHandle.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&AMic_AdcHandle) != HAL_OK)
  {

  }
  
  /** Start calibration */
  if (HAL_ADCEx_Calibration_Start(&AMic_AdcHandle, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
  {

  }
  
        /******* TO BE CHECKED: ADC1 just for mic or also for BC? ******/
        /******* TO BE CHECKED: MSP per ADC dedicated HAL_ADC_Audio_MspInit or in a common HAL_ADC_MspInit in STWIN.box.c? ******/
  
  /** Configure Regular Channel */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_5CYCLE;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  
  /** Trasform from unsigned to signed data 
   *  ADC is 12bits (4096). Offset is half: 11 bit (2048) */
  sConfig.OffsetNumber = ADC_OFFSET_1;    
  sConfig.Offset = 2048;                  

  if (HAL_ADC_ConfigChannel(&AMic_AdcHandle, &sConfig) != HAL_OK)
  {

  }
  
#ifdef ADC_TEST_IRQ  
  HAL_NVIC_SetPriority(ADC1_IRQn, 0x01, 0);
  HAL_NVIC_EnableIRQ(ADC1_IRQn);
#endif
  
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
    (void)HAL_DMAEx_List_BuildNode(&dmaLinkNodeConfig, &DMic_OnBoard_DmaNode);
    
    /* Insert NodeTx to SAI queue */
    (void)HAL_DMAEx_List_InsertNode_Tail(&DMic_OnBoard_MdfQueue, &DMic_OnBoard_DmaNode);
    
    /* Select the DMA instance to be used for the transfer : GPDMA_Channel1 */
    DMic_OnBoard_DmaHandle.Instance                         = GPDMA1_Channel1;
    
    /* Set queue circular mode for sai queue */
    (void)HAL_DMAEx_List_SetCircularMode(&DMic_OnBoard_MdfQueue);
    
    DMic_OnBoard_DmaHandle.InitLinkedList.Priority          = DMA_HIGH_PRIORITY;
    DMic_OnBoard_DmaHandle.InitLinkedList.LinkStepMode      = DMA_LSM_FULL_EXECUTION;
    DMic_OnBoard_DmaHandle.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
    DMic_OnBoard_DmaHandle.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
    DMic_OnBoard_DmaHandle.InitLinkedList.LinkedListMode    = DMA_LINKEDLIST_CIRCULAR;
    
    /* DMA linked list init */
    (void)HAL_DMAEx_List_Init(&DMic_OnBoard_DmaHandle);
    
    /* Link SAI queue to DMA channel */
    (void)HAL_DMAEx_List_LinkQ(&DMic_OnBoard_DmaHandle, &DMic_OnBoard_MdfQueue);
    
    /* Associate the DMA handle */
    __HAL_LINKDMA(hmdf, hdma, DMic_OnBoard_DmaHandle);

    HAL_NVIC_SetPriority(DMIC_ONBAORD_DMA_IRQn, 0x01, 0);
    HAL_NVIC_EnableIRQ(DMIC_ONBAORD_DMA_IRQn);
  }
  else if(hmdf->Instance == AMIC_ONBOARD_FILTER) 
  {
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_MDF1;
    PeriphClkInit.Mdf1ClockSelection = RCC_MDF1CLKSOURCE_HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      for(;;){}   /* Blocking error */
    }
    
    __HAL_RCC_MDF1_CONFIG(RCC_MDF1CLKSOURCE_HCLK);
    
    /* MDF1 clock enable */
    AMIC_ONBOARD_MDFx_CLK_ENABLE();
    
    /* Configure DMA used for MDF */
    AMIC_ONBOARD_DMAx_CLK_ENABLE();
    
    /* Set node type */
    dmaLinkNodeConfig.NodeType                            = DMA_GPDMA_LINEAR_NODE;
    /* Set common node parameters */
    dmaLinkNodeConfig.Init.Request                        = AMIC_ONBOARD_DMA_REQUEST;
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
    dmaLinkNodeConfig.Init.TransferAllocatedPort          = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
    dmaLinkNodeConfig.Init.Mode                           = DMA_NORMAL;
   
    /* Set node data handling parameters */
    dmaLinkNodeConfig.DataHandlingConfig.DataExchange     = DMA_EXCHANGE_NONE;
    dmaLinkNodeConfig.DataHandlingConfig.DataAlignment    = DMA_DATA_UNPACK;
    
    /* Set node trigger parameters */
    dmaLinkNodeConfig.TriggerConfig.TriggerPolarity       = DMA_TRIG_POLARITY_MASKED;
    dmaLinkNodeConfig.RepeatBlockConfig.RepeatCount       = 1U;
    dmaLinkNodeConfig.RepeatBlockConfig.SrcAddrOffset     = 0;
    dmaLinkNodeConfig.RepeatBlockConfig.DestAddrOffset    = 0;
    dmaLinkNodeConfig.RepeatBlockConfig.BlkSrcAddrOffset  = 0;
    dmaLinkNodeConfig.RepeatBlockConfig.BlkDestAddrOffset = 0;
    
    /* Build NodeTx */
    (void)HAL_DMAEx_List_BuildNode(&dmaLinkNodeConfig, &AMic_OnBoard_DmaNode);
    
    /* Insert NodeTx to SAI queue */
    (void)HAL_DMAEx_List_InsertNode_Tail(&AMic_OnBoard_MdfQueue, &AMic_OnBoard_DmaNode);
    
    AMic_OnBoard_DmaHandle.Instance = GPDMA1_Channel2;
    
    /* Set queue circular mode for sai queue */
    (void)HAL_DMAEx_List_SetCircularMode(&AMic_OnBoard_MdfQueue);
        
    AMic_OnBoard_DmaHandle.InitLinkedList.Priority = DMA_HIGH_PRIORITY;
    AMic_OnBoard_DmaHandle.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
    AMic_OnBoard_DmaHandle.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
    AMic_OnBoard_DmaHandle.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
    AMic_OnBoard_DmaHandle.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
    if (HAL_DMAEx_List_Init(&AMic_OnBoard_DmaHandle) != HAL_OK)
    {
      for(;;){}   /* Blocking error */
    }
    if (HAL_DMA_ConfigChannelAttributes(&AMic_OnBoard_DmaHandle, DMA_CHANNEL_NPRIV) != HAL_OK)
    {
      for(;;){}   /* Blocking error */
    }
    
    /* DMA linked list init */
    (void)HAL_DMAEx_List_Init(&AMic_OnBoard_DmaHandle);
    
    /* Link SAI queue to DMA channel */
    (void)HAL_DMAEx_List_LinkQ(&AMic_OnBoard_DmaHandle, &AMic_OnBoard_MdfQueue);
    
    __HAL_LINKDMA(hmdf, hdma, AMic_OnBoard_DmaHandle);
    
    /* if both microphones are enabled you should only have one IRQ */
    if (AudioInCtx[0].ChannelsNbr == 1U)
    {
      HAL_NVIC_SetPriority(AMIC_ONBOARD_DMA_IRQn, 1, 0);
      HAL_NVIC_EnableIRQ(AMIC_ONBOARD_DMA_IRQn); 
    }
  }    
}


/***** TO BE CHECKED creare callback e registrarle come per i bus ? *****/

/**
* @brief  Initialize the ADC MSP.
* @param  hDfsdmFilter ADC handle
* @retval None
*/
void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
  if (HAL_ADC_Counter == 0)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    
    if(adcHandle->Instance==ADC1)
    {
    /** Initializes the peripherals clock */
      PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADCDAC;  // 768kHz
      PeriphClkInit.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_PLL2;
      PeriphClkInit.PLL2.PLL2Source = RCC_PLLSOURCE_HSE;
      PeriphClkInit.PLL2.PLL2M = 2;
      PeriphClkInit.PLL2.PLL2N = 48;
      PeriphClkInit.PLL2.PLL2P = 2;
      PeriphClkInit.PLL2.PLL2Q = 7;
  #ifdef ADC_TEST_IRQ
      PeriphClkInit.PLL2.PLL2R = 50;
  #else
      PeriphClkInit.PLL2.PLL2R = 25;
  #endif
      PeriphClkInit.PLL2.PLL2RGE = RCC_PLLVCIRANGE_1;
      PeriphClkInit.PLL2.PLL2FRACN = 0;
      PeriphClkInit.PLL2.PLL2ClockOut = RCC_PLL2_DIVR;
      if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
      {

      }

      __HAL_RCC_ADCDAC_CONFIG(RCC_ADCDACCLKSOURCE_PLL2);
      
      /* ADC1 clock enable */
      __HAL_RCC_ADC1_CLK_ENABLE();

      __HAL_RCC_GPIOC_CLK_ENABLE();
      /**ADC1 GPIO Configuration
      PC1     ------> ADC1_IN2
      */
      GPIO_InitStruct.Pin = AMIC_ONBOARD_ADC1_IN2_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(AMIC_ONBOARD_ADC1_IN2_GPIO_PORT, &GPIO_InitStruct);
    }
  }
  
  HAL_ADC_Counter++;
}


void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
  if(adcHandle->Instance==ADC1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PC1     ------> ADC1_IN2
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
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

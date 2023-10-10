/**
  ******************************************************************************
  * @file    STWIN_audio.h
  * @author  SRA
  *
  *
  * @brief   This file contains the common defines and functions prototypes for
  *          STWIN_audio.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_AUDIO_H
#define STWIN_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "STWIN_conf.h"
#include "STWIN.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STWIN
  * @{
  */

/** @addtogroup STWIN_AUDIO
  * @{
  */

/** @defgroup STWIN_AUDIO_Exported_Variables
  * @{
  */
/* AMic OnBoard */
//extern ADC_HandleTypeDef             ADC1_Handle;

extern DFSDM_Filter_HandleTypeDef   AMic_OnBoard_DfsdmFilter;
extern DMA_HandleTypeDef AMic_OnBoard_Dma;
/* DMic OnBoard */
extern DFSDM_Filter_HandleTypeDef   DMic_OnBoard_DfsdmFilter;
extern DMA_HandleTypeDef DMic_OnBoard_Dma;

#ifndef AUDIO_SHARED_TYPES
#define AUDIO_SHARED_TYPES

#define DMA_MAX(_X_)                (((_X_) <= DMA_MAX_SZE)? (_X_):DMA_MAX_SZE)

#define HTONS(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
                   (((uint16_t)(A) & 0x00ff) << 8))

#define SaturaLH(N, L, H) (((N)<(L))?(L):(((N)>(H))?(H):(N)))

/**
  * @}
  */

/** @defgroup STWIN_AUDIO_Exported_Types STWIN_AUDIO Exported Types
  * @{
  */

typedef struct
{
  int32_t Z;
  int32_t oldOut;
  int32_t oldIn;
} HP_FilterState_TypeDef;

typedef struct
{
  uint32_t                    Device;
  uint32_t                    SampleRate;
  uint32_t                    BitsPerSample;
  uint32_t                    ChannelsNbr;
  uint32_t                    Volume;
} BSP_AUDIO_Init_t;

typedef struct
{
  uint32_t                    Instance;            /* Audio IN instance              */
  uint32_t                    Device;              /* Audio IN device to be used     */
  uint32_t                    SampleRate;          /* Audio IN Sample rate           */
  uint32_t                    BitsPerSample;       /* Audio IN Sample resolution     */
  uint32_t                    ChannelsNbr;         /* Audio IN number of channel     */
  uint16_t                    *pBuff;              /* Audio IN record buffer         */
  uint8_t                     **pMultiBuff;        /* Audio IN multi-buffer          */
  uint32_t                    Size;                /* Audio IN record buffer size    */
  uint32_t                    Volume;              /* Audio IN volume                */
  uint32_t                    State;               /* Audio IN State                 */
  uint32_t                    IsMultiBuff;         /* Audio IN multi-buffer usage    */
  uint32_t                    IsMspCallbacksValid; /* Is Msp Callbacks registred     */
  HP_FilterState_TypeDef    HP_Filters[2];       /*!< HP filter state for each channel*/
  uint32_t DecimationFactor;
} AUDIO_IN_Ctx_t;

typedef struct
{
  uint32_t                    Instance;            /* Audio OUT instance              */
  uint32_t                    Device;              /* Audio OUT device to be used     */
  uint32_t                    SampleRate;          /* Audio OUT Sample rate           */
  uint32_t                    BitsPerSample;       /* Audio OUT Sample Bit Per Sample */
  uint32_t                    Volume;              /* Audio OUT volume                */
  uint32_t                    ChannelsNbr;         /* Audio OUT number of channel     */
  uint32_t                    IsMute;              /* Mute state                      */
  uint32_t                    State;               /* Audio OUT State                 */
  uint32_t                    IsMspCallbacksValid; /* Is Msp Callbacks registred      */
} AUDIO_OUT_Ctx_t;

typedef struct
{
  /* Filter parameters */
  DFSDM_Filter_TypeDef   *FilterInstance;
  uint32_t               RegularTrigger;
  uint32_t               SincOrder;
  uint32_t               Oversampling;
  /* Channel parameters */
  DFSDM_Channel_TypeDef *ChannelInstance;
  uint32_t              DigitalMicPins;
  uint32_t              DigitalMicType;
  uint32_t              Channel4Filter;
  uint32_t              ClockDivider;
  uint32_t              RightBitShift;
  FunctionalState       DmaMode;
  uint32_t              Multiplexer;
  FunctionalState       Activation;
} MX_DFSDM_Config;


/**
  * @}
  */

/** @defgroup STWIN_AUDIO_Exported_Constants STWIN_AUDIO Exported Constants
  * @{
  */

/* Audio In devices */
#define ONBOARD_ANALOG_MIC_MASK     (0x01U)
#define ONBOARD_DIGITAL_MIC_MASK    (0x02U)

/* AUDIO FREQUENCY */
#ifndef AUDIO_FREQUENCY_192K
#define AUDIO_FREQUENCY_192K     (uint32_t)192000U
#endif
#ifndef AUDIO_FREQUENCY_176K
#define AUDIO_FREQUENCY_176K     (uint32_t)176400U
#endif
#ifndef AUDIO_FREQUENCY_96K
#define AUDIO_FREQUENCY_96K       (uint32_t)96000U
#endif
#ifndef AUDIO_FREQUENCY_88K
#define AUDIO_FREQUENCY_88K       (uint32_t)88200U
#endif
#ifndef AUDIO_FREQUENCY_48K
#define AUDIO_FREQUENCY_48K       (uint32_t)48000U
#endif
#ifndef AUDIO_FREQUENCY_44K
#define AUDIO_FREQUENCY_44K       (uint32_t)44100U
#endif
#ifndef AUDIO_FREQUENCY_32K
#define AUDIO_FREQUENCY_32K       (uint32_t)32000U
#endif
#ifndef AUDIO_FREQUENCY_22K
#define AUDIO_FREQUENCY_22K       (uint32_t)22050U
#endif
#ifndef AUDIO_FREQUENCY_16K
#define AUDIO_FREQUENCY_16K       (uint32_t)16000U
#endif
#ifndef AUDIO_FREQUENCY_11K
#define AUDIO_FREQUENCY_11K       (uint32_t)11025U
#endif
#ifndef AUDIO_FREQUENCY_8K
#define AUDIO_FREQUENCY_8K         (uint32_t)8000U
#endif

/* AUDIO RESOLUTION */
#ifndef AUDIO_RESOLUTION_16b
#define AUDIO_RESOLUTION_16b                16U
#endif
#ifndef AUDIO_RESOLUTION_24b
#define AUDIO_RESOLUTION_24b                24U
#endif
#ifndef AUDIO_RESOLUTION_32b
#define AUDIO_RESOLUTION_32b                32U
#endif


#if (ONBOARD_ANALOG_MIC == 1)
#if (ONBOARD_DIGITAL_MIC == 1)
#define ACTIVE_MICROPHONES_MASK   (ONBOARD_ANALOG_MIC_MASK|ONBOARD_DIGITAL_MIC_MASK)
#else
#define ACTIVE_MICROPHONES_MASK   (ONBOARD_ANALOG_MIC_MASK)
#endif
#else
#if (ONBOARD_DIGITAL_MIC == 1)
#define ACTIVE_MICROPHONES_MASK   (ONBOARD_DIGITAL_MIC_MASK)
#else
#error "Please select at least one of the microphone in STWIN.conf file"
#endif
#endif

#if (ONBOARD_DIGITAL_MIC == 1 && AUDIO_IN_SAMPLING_FREQUENCY>48000)
#error "If the digital microphone is enabled then the max frequency is 48000Hz"
#endif


#ifndef AUDIO_IN_CHANNELS
#define AUDIO_IN_CHANNELS 2
#endif

#ifndef AUDIO_IN_SAMPLING_FREQUENCY
#define AUDIO_IN_SAMPLING_FREQUENCY 16000
#endif

#ifndef AUDIO_VOLUME_INPUT
#define AUDIO_VOLUME_INPUT              64U
#endif

#ifndef BSP_AUDIO_IN_IT_PRIORITY
#define BSP_AUDIO_IN_IT_PRIORITY    6U
#endif

/* Buffer size defines*/
#define CHANNEL_DEMUX_MASK                      0x55U

#define MAX_CH_NUMBER           (4)
#define MAX_FS                  (192000)
#define MAX_SAMPLES_PER_CH      ((MAX_FS/1000)*2)

#define MAX_MIC_FREQ                    3072  /*KHz*/
#define MAX_AUDIO_IN_CHANNEL_NBR_PER_IF   2
#define MAX_AUDIO_IN_CHANNEL_NBR_TOTAL    4

#ifndef PDM_FREQ_16K
#define PDM_FREQ_16K 1280
#endif

/*Number of millisecond of audio at each DMA interrupt*/
#ifndef N_MS_PER_INTERRUPT
#define N_MS_PER_INTERRUPT               (1U)
#endif

/* Default Audio IN internal buffer size */
#define DEFAULT_AUDIO_IN_BUFFER_SIZE        (AUDIO_IN_SAMPLING_FREQUENCY/1000)*2*N_MS_PER_INTERRUPT

/*BSP internal buffer size in half words (16 bits)*/
#define PDM_INTERNAL_BUFFER_SIZE_I2S          ((MAX_MIC_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF * N_MS_PER_INTERRUPT)
#if MAX_AUDIO_IN_CHANNEL_NBR_TOTAL > 2
#define PDM_INTERNAL_BUFFER_SIZE_SPI          ((MAX_MIC_FREQ / 8) * MAX_AUDIO_IN_CHANNEL_NBR_PER_IF * N_MS_PER_INTERRUPT)
#else
#define PDM_INTERNAL_BUFFER_SIZE_SPI          1
#endif

/* Audio In states */
#define AUDIO_IN_STATE_RESET               0U
#define AUDIO_IN_STATE_RECORDING           1U
#define AUDIO_IN_STATE_STOP                2U
#define AUDIO_IN_STATE_PAUSE               3U

/* Audio In instances available in STWIN */
#define AUDIO_IN_INSTANCES_NBR 2U
/* Define used for communication with AD1978 */
#define AUDIO_OUT_INSTANCES_NBR 1U

#endif /* AUDIO_SHARED_TYPES */

/*------------------------------------------------------------------------------
                        AMic_OnBoard defines parameters
------------------------------------------------------------------------------*/
#if (AUDIO_IN_SAMPLING_FREQUENCY == 8000)
#define DECIMATION_RATIO_DFSDM 384
#define SINC_ORDER DFSDM_FILTER_SINC2_ORDER
#define RBITSHIFT 9
#define RSHFT_ADC ADC_RIGHTBITSHIFT_NONE
#define RSHFT_ADC_OFFST 0
#define ADC_CLOCK (uint32_t)(3072000)
#define ADC_SAMPLE_TIME ADC_SAMPLETIME_12CYCLES_5

#elif (AUDIO_IN_SAMPLING_FREQUENCY == 16000)
#define DECIMATION_RATIO_DFSDM 192
#define SINC_ORDER DFSDM_FILTER_SINC2_ORDER
#define RBITSHIFT 7
#define RSHFT_ADC ADC_RIGHTBITSHIFT_NONE
#define RSHFT_ADC_OFFST 0
#define ADC_CLOCK (uint32_t)(3072000)
#define ADC_SAMPLE_TIME ADC_SAMPLETIME_12CYCLES_5

#elif (AUDIO_IN_SAMPLING_FREQUENCY == 32000)
#define DECIMATION_RATIO_DFSDM 96
#define SINC_ORDER DFSDM_FILTER_SINC2_ORDER
#define RBITSHIFT 5
#define RSHFT_ADC ADC_RIGHTBITSHIFT_NONE
#define RSHFT_ADC_OFFST 0
#define ADC_CLOCK (uint32_t)(3072000)
#define ADC_SAMPLE_TIME ADC_SAMPLETIME_12CYCLES_5

#elif (AUDIO_IN_SAMPLING_FREQUENCY == 48000)
#define DECIMATION_RATIO_DFSDM 64
#define SINC_ORDER DFSDM_FILTER_SINC3_ORDER
#define RBITSHIFT 10
#define RSHFT_ADC ADC_RIGHTBITSHIFT_NONE
#define RSHFT_ADC_OFFST 0
#define ADC_CLOCK (uint32_t)(3072000)
#define ADC_SAMPLE_TIME ADC_SAMPLETIME_12CYCLES_5

#elif (AUDIO_IN_SAMPLING_FREQUENCY == 96000)
#define DECIMATION_RATIO_DFSDM 32
#define SINC_ORDER DFSDM_FILTER_SINC3_ORDER
#define RBITSHIFT 7
#define RSHFT_ADC ADC_RIGHTBITSHIFT_NONE
#define RSHFT_ADC_OFFST 0
#define ADC_CLOCK (uint32_t)(3072000)
#define ADC_SAMPLE_TIME ADC_SAMPLETIME_12CYCLES_5

#elif (AUDIO_IN_SAMPLING_FREQUENCY == 192000)
#define DECIMATION_RATIO_DFSDM 16
#define SINC_ORDER DFSDM_FILTER_SINC4_ORDER
#define RBITSHIFT 8
#define RSHFT_ADC ADC_RIGHTBITSHIFT_NONE
#define RSHFT_ADC_OFFST 0
#define ADC_CLOCK (uint32_t)(3072000)
#define ADC_SAMPLE_TIME ADC_SAMPLETIME_12CYCLES_5
#endif

#define AMIC_ONBOARD_CHANNEL                   DFSDM1_Channel0
#define AMIC_ONBOARD_FILTER                    DFSDM1_Filter1

#define DMA_REQUEST_ANALOG                     DMA_REQUEST_DFSDM1_FLT1
#define DFSDM_DMA_ANALOG                       DMA1_Channel4
#define DFSDM_DMA_ANALOG_IRQn                  DMA1_Channel4_IRQn

/*------------------------------------------------------------------------------
                        DMic_OnBoard defines parameters
------------------------------------------------------------------------------*/
/* DFSDM Configuration defines */
#define DMIC_ONBOARD_CHANNEL                   DFSDM_Channel5
#define DMIC_ONBOARD_FILTER                    DFSDM_Filter0

#define DMA_REQUEST_DIGITAL                     DMA_REQUEST_DFSDM1_FLT0
#define DFSDM_DMA_DIGITAL                       DMA1_Channel7
#define DFSDM_DMA_DIGITAL_IRQn                  DMA1_Channel7_IRQn

#define DMIC_ONBOARD_CKOUT_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOE_CLK_ENABLE()
#define DMIC_ONBOARD_CKOUT_PIN                 GPIO_PIN_9
#define DMIC_ONBOARD_CKOUT_GPIO_PORT           GPIOE
#define DMIC_ONBOARD_CKOUT_DATIN_AF            GPIO_AF6_DFSDM1

#define DMIC_ONBOARD_DATAIN_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define DMIC_ONBOARD_DATIN_GPIO_PORT           GPIOB
#define DMIC_ONBOARD_DATIN_PIN                 GPIO_PIN_6
#define DMIC_ONBOARD_DATAIN_DATIN_AF           GPIO_AF6_DFSDM1

/*************************************************************************/


/**
  * @}
  */

/** @defgroup STWIN_AUDIO_Exported_Macros STWIN_AUDIO Exported Macros
  * @{
  */
#define POS_VAL(VAL)                  (POSITION_VAL(VAL) - 4)
#define VOLUME_OUT_CONVERT(Volume)    (((Volume) > 100)? 63:((uint8_t)(((Volume) * 63) / 100)))
#define VOLUME_IN_CONVERT(Volume)     (((Volume) >= 100)? 239:((uint8_t)(((Volume) * 239) / 100)))
/**
  * @}
  */
/** @addtogroup STWIN_AUDIO_Exported_Variables
  * @{
  */
/* Recording context */
extern AUDIO_IN_Ctx_t                         AudioInCtx[];
/**
  * @}
  */

/** @defgroup STWIN_AUDIO_IN_Exported_Functions STWIN_AUDIO_IN Exported Functions
  * @{
  */
int32_t BSP_AUDIO_IN_Init(uint32_t Instance, BSP_AUDIO_Init_t *AudioInit);
int32_t BSP_AUDIO_IN_DeInit(uint32_t Instance);
int32_t BSP_AUDIO_IN_Record(uint32_t Instance, uint8_t *pBuf, uint32_t NbrOfBytes);
int32_t BSP_AUDIO_IN_Stop(uint32_t Instance);
int32_t BSP_AUDIO_IN_Pause(uint32_t Instance);
int32_t BSP_AUDIO_IN_Resume(uint32_t Instance);

int32_t BSP_AUDIO_IN_RecordChannels(uint32_t Instance, uint8_t **pBuf, uint32_t NbrOfBytes);
int32_t BSP_AUDIO_IN_StopChannels(uint32_t Instance, uint32_t Device);
int32_t BSP_AUDIO_IN_PauseChannels(uint32_t Instance, uint32_t Device);
int32_t BSP_AUDIO_IN_ResumeChannels(uint32_t Instance, uint32_t Device);

int32_t BSP_AUDIO_IN_SetDevice(uint32_t Instance, uint32_t Device);
int32_t BSP_AUDIO_IN_GetDevice(uint32_t Instance, uint32_t *Device);
int32_t BSP_AUDIO_IN_SetSampleRate(uint32_t Instance, uint32_t SampleRate);
int32_t BSP_AUDIO_IN_GetSampleRate(uint32_t Instance, uint32_t *SampleRate);
int32_t BSP_AUDIO_IN_SetBitsPerSample(uint32_t Instance, uint32_t BitsPerSample);
int32_t BSP_AUDIO_IN_GetBitsPerSample(uint32_t Instance, uint32_t *BitsPerSample);
int32_t BSP_AUDIO_IN_SetChannelsNbr(uint32_t Instance, uint32_t ChannelNbr);
int32_t BSP_AUDIO_IN_GetChannelsNbr(uint32_t Instance, uint32_t *ChannelNbr);
int32_t BSP_AUDIO_IN_SetVolume(uint32_t Instance, uint32_t Volume);
int32_t BSP_AUDIO_IN_GetVolume(uint32_t Instance, uint32_t *Volume);
int32_t BSP_AUDIO_IN_GetState(uint32_t Instance, uint32_t *State);

/* User Callbacks: user has to implement these functions in his code if they are needed. */
/* This function should be implemented by the user application.
   It is called into this driver when the current buffer is filled to prepare the next
   buffer pointer and its size. */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance);
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance);

/* This function is called when an Interrupt due to transfer error on or peripheral
   error occurs. */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance);


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


#ifdef __cplusplus
}
#endif

#endif /* STWIN_AUDIO_H */



/**
 ******************************************************************************
 * @file    STWIN.box_conf_template.h
 * @author  SRA
 * @brief   Template for STWIN.box_conf.h (to be placed in user space)
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
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STWIN_BOX_CONF_TEMPLATE_H__
#define STWIN_BOX_CONF_TEMPLATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "STWIN.box_bus.h"
#include "STWIN.box_errno.h"

/* Analog and digital mics */

#define ONBOARD_ANALOG_MIC          1
#define ONBOARD_DIGITAL_MIC         1

/* Select the sampling frequencies for the microphones
   If the digital microphone is enabled then the max frequency is 48000Hz,
   otherwise is 192000Hz.  */
#define AUDIO_IN_SAMPLING_FREQUENCY 48000


#define AUDIO_IN_CHANNELS     (ONBOARD_ANALOG_MIC+ONBOARD_DIGITAL_MIC)

#if (AUDIO_IN_CHANNELS==0)
#error "Please enable at least one of the microphones"
#endif


/* The default value of the N_MS_PER_INTERRUPT directive in the driver is set to 1,
for backward compatibility: leaving this values as it is allows to avoid any
modification in the application layer developed with older versions of the driver */
/*Number of millisecond of audio at each DMA interrupt*/
#define N_MS_PER_INTERRUPT               (1U)

#define AUDIO_VOLUME_INPUT              64U
#define BSP_AUDIO_IN_INSTANCE           1U   /* Define the audio peripheral used: 1U = DFSDM */
#define BSP_AUDIO_IN_IT_PRIORITY        6U

/* BSP COM Port */
#define USE_BSP_COM_FEATURE             1U
#define USE_COM_LOG                     1U

/* SD card interrupt priority */
#define BSP_SD_IT_PRIORITY              14U  /* Default is lowest priority level */
#define BSP_SD_RX_IT_PRIORITY           14U  /* Default is lowest priority level */
#define BSP_SD_TX_IT_PRIORITY           15U  /* Default is lowest priority level */


#define BUTTON_USER_IT_PRIORITY         14U
#define BUTTON_PWR_IT_PRIORITY          14U

#define MXCHIP_NOTIFY_IT_PRIORITY       5U
#define MXCHIP_FLOW_IT_PRIORITY         4U

/* Define 1 to use already implemented callback; 0 to implement callback
   into an application file */
#define USE_BC_TIM_IRQ_CALLBACK         0U  
#define USE_BC_GPIO_IRQ_HANDLER         1U  
#define USE_BC_GPIO_IRQ_CALLBACK        1U  

#define USE_MOTION_SENSOR_IIS2DLPC_0    1U
#define USE_MOTION_SENSOR_IIS2MDC_0     1U
#define USE_MOTION_SENSOR_IIS3DWB_0     1U
#define USE_MOTION_SENSOR_ISM330DHCX_0  1U
#define USE_MOTION_SENSOR_IIS2ICLX_0    1U

#define USE_ENV_SENSOR_ILPS22QS_0       1U
#define USE_ENV_SENSOR_STTS22H_0        1U
  
/* IIS2DLPC */
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_PP           7
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_SP           0
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_PP           7
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_SP           0

/* IIS3DWB */
#define BSP_IIS3DWB_INT1_EXTI_IRQ_PP            7
#define BSP_IIS3DWB_INT1_EXTI_IRQ_SP            0
  
/* ISM330DHCX */
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_PP         7
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_SP         0
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_PP         7
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_SP         0  
  
/* IIS2ICLX */
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_PP           7
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_SP           0
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_PP           7
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_SP           0

/* ILPS22QS */  
#define BSP_ILPS22QS_INT_EXTI_IRQ_PP            7
#define BSP_ILPS22QS_INT_EXTI_IRQ_SP            0

/* STTS22H */  
#define BSP_STTS22H_INT_EXTI_IRQ_PP             7
#define BSP_STTS22H_INT_EXTI_IRQ_SP             0
 
#ifdef __cplusplus
}
#endif

#endif /* STWIN_BOX_CONF_TEMPLATE_H__*/


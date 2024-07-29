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

/* ILPS22QS Pressure-Temperature-Qvar Sensor */
#define BSP_ILPS22QS_0_I2C_INIT                 BSP_I2C2_Init
#define BSP_ILPS22QS_0_I2C_DEINIT               BSP_I2C2_DeInit
#define BSP_ILPS22QS_0_I2C_READ_REG             BSP_I2C2_ReadReg
#define BSP_ILPS22QS_0_I2C_WRITE_REG            BSP_I2C2_WriteReg

/* STTS22H Temperature Sensor */
extern EXTI_HandleTypeDef hexti5;
#define H_EXTI_5         hexti5
#define H_EXTI_INT_STTS22H                      hexti5
#define STTS22H_INT_EXTI_LINE                   EXTI_LINE_5
#define BSP_STTS22H_INT_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_STTS22H_INT_PORT                    GPIOF
#define BSP_STTS22H_INT_PIN                     GPIO_PIN_5
#define BSP_STTS22H_INT_EXTI_IRQn               EXTI5_IRQn
#ifndef BSP_STTS22H_INT_EXTI_IRQ_PP
#define BSP_STTS22H_INT_EXTI_IRQ_PP             7
#endif
#ifndef BSP_STTS22H_INT_EXTI_IRQ_SP
#define BSP_STTS22H_INT_EXTI_IRQ_SP             0
#endif
#define BSP_STTS22H_0_I2C_INIT                  BSP_I2C2_Init
#define BSP_STTS22H_0_I2C_DEINIT                BSP_I2C2_DeInit
#define BSP_STTS22H_0_I2C_READ_REG              BSP_I2C2_ReadReg
#define BSP_STTS22H_0_I2C_WRITE_REG             BSP_I2C2_WriteReg

/* IIS2MDC magneto Sensor */
#define BSP_IIS2MDC_0_I2C_INIT                  BSP_I2C2_Init
#define BSP_IIS2MDC_0_I2C_DEINIT                BSP_I2C2_DeInit
#define BSP_IIS2MDC_0_I2C_READ_REG              BSP_I2C2_ReadReg
#define BSP_IIS2MDC_0_I2C_WRITE_REG             BSP_I2C2_WriteReg

/* IIS2DLPC acc Sensor */
extern EXTI_HandleTypeDef hexti1;
#define H_EXTI_1         hexti1
#define H_EXTI_INT1_IIS2DLPC                      hexti1
#define IIS2DLPC_INT1_EXTI_LINE                   EXTI_LINE_1
#define BSP_IIS2DLPC_INT1_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2DLPC_INT1_PORT                    GPIOF
#define BSP_IIS2DLPC_INT1_PIN                     GPIO_PIN_1
#define BSP_IIS2DLPC_INT1_EXTI_IRQn               EXTI1_IRQn
#ifndef BSP_IIS2DLPC_INT1_EXTI_IRQ_PP
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_PP             7
#endif
#ifndef BSP_IIS2DLPC_INT1_EXTI_IRQ_SP
#define BSP_IIS2DLPC_INT1_EXTI_IRQ_SP             0
#endif
extern EXTI_HandleTypeDef hexti2;
#define H_EXTI_2         hexti2
#define H_EXTI_INT2_IIS2DLPC                      hexti2
#define IIS2DLPC_INT2_EXTI_LINE                   EXTI_LINE_2
#define BSP_IIS2DLPC_INT2_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2DLPC_INT2_PORT                    GPIOF
#define BSP_IIS2DLPC_INT2_PIN                     GPIO_PIN_2
#define BSP_IIS2DLPC_INT2_EXTI_IRQn               EXTI2_IRQn
#ifndef BSP_IIS2DLPC_INT2_EXTI_IRQ_PP
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_PP             7
#endif
#ifndef BSP_IIS2DLPC_INT2_EXTI_IRQ_SP
#define BSP_IIS2DLPC_INT2_EXTI_IRQ_SP             0
#endif
#define BSP_IIS2DLPC_0_SPI_INIT                  BSP_SPI2_Init
#define BSP_IIS2DLPC_0_SPI_DEINIT                BSP_SPI2_DeInit
#define BSP_IIS2DLPC_0_SPI_SEND                  BSP_SPI2_Send
#define BSP_IIS2DLPC_0_SPI_RECV                  BSP_SPI2_Recv
#define BSP_IIS2DLPC_CS_PORT                     GPIOH
#define BSP_IIS2DLPC_CS_PIN                      GPIO_PIN_6

/* IIS3DWB acc Sensor */
extern EXTI_HandleTypeDef hexti15;
#define H_EXTI_15          hexti15
#define H_EXTI_INT1_IIS3DWB                       hexti15
#define IIS3DWB_INT1_EXTI_LINE                    EXTI_LINE_15
#define BSP_IIS3DWB_INT1_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS3DWB_INT1_PORT                     GPIOF
#define BSP_IIS3DWB_INT1_PIN                      GPIO_PIN_15
#define BSP_IIS3DWB_INT1_EXTI_IRQn                EXTI15_IRQn
#ifndef BSP_IIS3DWB_INT1_EXTI_IRQ_PP
#define BSP_IIS3DWB_INT1_EXTI_IRQ_PP              7
#endif
#ifndef BSP_IIS3DWB_INT1_EXTI_IRQ_SP
#define BSP_IIS3DWB_INT1_EXTI_IRQ_SP              0
#endif
#define BSP_IIS3DWB_0_SPI_INIT                   BSP_SPI2_Init
#define BSP_IIS3DWB_0_SPI_DEINIT                 BSP_SPI2_DeInit
#define BSP_IIS3DWB_0_SPI_SEND                   BSP_SPI2_Send
#define BSP_IIS3DWB_0_SPI_RECV                   BSP_SPI2_Recv
#define BSP_IIS3DWB_CS_PORT                      GPIOF
#define BSP_IIS3DWB_CS_PIN                       GPIO_PIN_12

/* ISM330DHCX acc - gyro Sensor */
extern EXTI_HandleTypeDef hexti8;
#define H_EXTI_8         hexti8
#define H_EXTI_INT1_ISM330DHCX                      hexti8
#define ISM330DHCX_INT1_EXTI_LINE                   EXTI_LINE_8
#define BSP_ISM330DHCX_INT1_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_ISM330DHCX_INT1_PORT                    GPIOB
#define BSP_ISM330DHCX_INT1_PIN                     GPIO_PIN_8
#define BSP_ISM330DHCX_INT1_EXTI_IRQn               EXTI8_IRQn
#ifndef BSP_ISM330DHCX_INT1_EXTI_IRQ_PP
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_PP             7
#endif
#ifndef BSP_ISM330DHCX_INT1_EXTI_IRQ_SP
#define BSP_ISM330DHCX_INT1_EXTI_IRQ_SP             0
#endif
extern EXTI_HandleTypeDef hexti4;
#define H_EXTI_4         hexti4
#define H_EXTI_INT2_ISM330DHCX                      hexti4
#define ISM330DHCX_INT2_EXTI_LINE                   EXTI_LINE_4
#define BSP_ISM330DHCX_INT2_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_ISM330DHCX_INT2_PORT                    GPIOF
#define BSP_ISM330DHCX_INT2_PIN                     GPIO_PIN_4
#define BSP_ISM330DHCX_INT2_EXTI_IRQn               EXTI4_IRQn
#ifndef BSP_ISM330DHCX_INT2_EXTI_IRQ_PP
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_PP             7
#endif
#ifndef BSP_ISM330DHCX_INT2_EXTI_IRQ_SP
#define BSP_ISM330DHCX_INT2_EXTI_IRQ_SP             0
#endif
#define BSP_ISM330DHCX_0_SPI_INIT                  BSP_SPI2_Init
#define BSP_ISM330DHCX_0_SPI_DEINIT                BSP_SPI2_DeInit
#define BSP_ISM330DHCX_0_SPI_SEND                  BSP_SPI2_Send
#define BSP_ISM330DHCX_0_SPI_RECV                  BSP_SPI2_Recv
#define BSP_ISM330DHCX_CS_PORT                     GPIOH
#define BSP_ISM330DHCX_CS_PIN                      GPIO_PIN_15

/* IIS2ICLX acc Sensor */
extern EXTI_HandleTypeDef hexti3;
#define H_EXTI_3           hexti3
#define H_EXTI_INT1_IIS2ICLX                        hexti3
#define IIS2ICLX_INT1_EXTI_LINE                     EXTI_LINE_3
#define BSP_IIS2ICLX_INT1_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2ICLX_INT1_PORT                      GPIOF
#define BSP_IIS2ICLX_INT1_PIN                       GPIO_PIN_3
#define BSP_IIS2ICLX_INT1_EXTI_IRQn                 EXTI3_IRQn
#ifndef BSP_IIS2ICLX_INT1_EXTI_IRQ_PP
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_PP               7
#endif
#ifndef BSP_IIS2ICLX_INT1_EXTI_IRQ_SP
#define BSP_IIS2ICLX_INT1_EXTI_IRQ_SP               0
#endif
extern EXTI_HandleTypeDef hexti11;
#define H_EXTI_11           hexti11
#define H_EXTI_INT2_IIS2ICLX                        hexti11
#define IIS2ICLX_INT2_EXTI_LINE                     EXTI_LINE_11
#define BSP_IIS2ICLX_INT2_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOF_CLK_ENABLE()
#define BSP_IIS2ICLX_INT2_PORT                      GPIOF
#define BSP_IIS2ICLX_INT2_PIN                       GPIO_PIN_11
#define BSP_IIS2ICLX_INT2_EXTI_IRQn                 EXTI11_IRQn
#ifndef BSP_IIS2ICLX_INT2_EXTI_IRQ_PP
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_PP               7
#endif
#ifndef BSP_IIS2ICLX_INT2_EXTI_IRQ_SP
#define BSP_IIS2ICLX_INT2_EXTI_IRQ_SP               0
#endif
#define BSP_IIS2ICLX_0_SPI_INIT                    BSP_SPI2_Init
#define BSP_IIS2ICLX_0_SPI_DEINIT                  BSP_SPI2_DeInit
#define BSP_IIS2ICLX_0_SPI_SEND                    BSP_SPI2_Send
#define BSP_IIS2ICLX_0_SPI_RECV                    BSP_SPI2_Recv
#define BSP_IIS2ICLX_CS_PORT                       GPIOI
#define BSP_IIS2ICLX_CS_PIN                        GPIO_PIN_7

/* EXT_SPI */
#define BSP_EXT_SPI_CS_GPIO_CLK_ENABLE()           __GPIOA_CLK_ENABLE()
#define BSP_EXT_SPI_CS_PORT                        GPIOA
#define BSP_EXT_SPI_CS_PIN                         GPIO_PIN_15

/* ST25DV nfc Device */
#define BSP_ST25DV_I2C_INIT                     BSP_I2C2_Init
#define BSP_ST25DV_I2C_DEINIT                   BSP_I2C2_DeInit
#define BSP_ST25DV_I2C_READ_REG_16              BSP_I2C2_ReadReg16
#define BSP_ST25DV_I2C_WRITE_REG_16             BSP_I2C2_WriteReg16
#define BSP_ST25DV_I2C_RECV                     BSP_I2C2_Recv
#define BSP_ST25DV_I2C_IS_READY                 BSP_I2C2_IsReady

/* nfctag GPO pin */
extern EXTI_HandleTypeDef                       hexti13;
#define H_EXTI_13                               hexti13
#define GPO_EXTI                                hexti13
#define BSP_GPO_PIN                             GPIO_PIN_13
#define BSP_GPO_GPIO_PORT                       GPIOB
#define BSP_GPO_EXTI_LINE                       EXTI_LINE_13
#define BSP_GPO_EXTI_IRQN                       EXTI13_IRQn
#define BSP_GPO_CLK_ENABLE()                    __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_GPO_EXTI_IRQHANDLER                 EXTI13_IRQHandler

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
#define BSP_COM_BAUDRATE      115200

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


/**  Definition for SD DETECT INTERRUPT PIN  **/
#define BSP_SD_DETECT_PIN                       GPIO_PIN_1
#define BSP_SD_DETECT_GPIO_PORT                 GPIOG
#define BSP_SD_DETECT_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOG_CLK_ENABLE()
#define BSP_SD_DETECT_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOG_CLK_DISABLE()
#define BSP_SD_DETECT_EXTI_LINE                 EXTI_LINE_1
#define BSP_SD_DETECT_EXTI_IRQN                 EXTI1_IRQn
#define BSP_SD_DETECT_IRQHANDLER                EXTI1_IRQHandler


/* Define 1 to use already implemented callback; 0 to implement callback
   into an application file */

#define USE_MOTION_SENSOR_IIS2DLPC_0    1U
#define USE_MOTION_SENSOR_IIS2MDC_0     1U
#define USE_MOTION_SENSOR_IIS3DWB_0     1U
#define USE_MOTION_SENSOR_ISM330DHCX_0  1U
#define USE_MOTION_SENSOR_IIS2ICLX_0    1U

#define USE_ENV_SENSOR_ILPS22QS_0       1U
#define USE_ENV_SENSOR_STTS22H_0        1U

#define BSP_NFCTAG_INSTANCE             1U

#if (USE_MOTION_SENSOR_IIS2DLPC_0 + USE_MOTION_SENSOR_ISM330DHCX_0 + USE_MOTION_SENSOR_IIS2MDC_0 + USE_MOTION_SENSOR_IIS3DWB_0 + USE_MOTION_SENSOR_IIS2ICLX_0 == 0)
#undef USE_MOTION_SENSOR_ISM330DHCX_0
#define USE_MOTION_SENSOR_ISM330DHCX_0     1U
#endif

#if (USE_ENV_SENSOR_STTS22H_0 + USE_ENV_SENSOR_ILPS22QS_0 == 0)
#undef USE_ENV_SENSOR_STTS22H_0
#define USE_ENV_SENSOR_STTS22H_0     1U
#endif

#ifdef __cplusplus
}
#endif

#endif /* STWIN_BOX_CONF_TEMPLATE_H__*/


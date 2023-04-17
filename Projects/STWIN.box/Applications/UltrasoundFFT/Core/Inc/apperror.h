/**
 ******************************************************************************
 * @file    apperror.h
 * @author  SRA
 * @brief Application specific error code
 *
 * Application defines its own error code in this file starting form the
 * constant APP_BASE_ERROR_CODE.
 * It is recommended to group the error code in the following groups:
 * - Low Level API error code
 * - Service Level error code
 * - Task Level error code
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
#ifndef APPERROR_H_
#define APPERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Low Level API error code */
/****************************/

#define SYS_BASE_LL_ERROR_CODE                          APP_BASE_ERROR_CODE
#define SYS_LL_UNDEFINED_ERROR_CODE                     SYS_BASE_LL_ERROR_CODE + 1

/* MDF driver error code */
/*************************/
#define SYS_BASE_MDF_DRV_ERROR_CODE                     SYS_BASE_LL_ERROR_CODE + SYS_GROUP_ERROR_COUNT

/* SPI Master error code */
/*************************/
#define SYS_BASE_SPI_M_ERROR_CODE                       SYS_BASE_MDF_DRV_ERROR_CODE + SYS_GROUP_ERROR_COUNT

/* I2C Master error code */
/*************************/
#define SYS_BASE_I2C_M_ERROR_CODE                       SYS_BASE_SPI_M_ERROR_CODE + SYS_GROUP_ERROR_COUNT

/* Service Level error code */
/****************************/

/* CircularBuffer error code */
/*****************************/
#define SYS_CB_BASE_ERROR_CODE                          SYS_BASE_IEVTSRC_ERROR_CODE + SYS_GROUP_ERROR_COUNT
#define SYS_CB_INVALID_ITEM_ERROR_CODE                  SYS_CB_BASE_ERROR_CODE + 1
#define SYS_CB_FULL_ERROR_CODE                          SYS_CB_BASE_ERROR_CODE + 2
#define SYS_CB_NO_READY_ITEM_ERROR_CODE                 SYS_CB_BASE_ERROR_CODE + 3

/* Task Level error code */
/*************************/

/* Generic task error code */
/***************************/
#define SYS_BASE_APP_TASK_ERROR_CODE                    SYS_CB_BASE_ERROR_CODE + SYS_GROUP_ERROR_COUNT
#define SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE          SYS_BASE_APP_TASK_ERROR_CODE +1
#define SYS_APP_TASK_REPORT_LOST_ERROR_CODE             SYS_BASE_APP_TASK_ERROR_CODE + 2

/* SPI Bus task error code */
/**************************/
#define SYS_BASE_SPIBUS_TASK_ERROR_CODE                 SYS_BASE_APP_TASK_ERROR_CODE + SYS_GROUP_ERROR_COUNT

/* I2C Bus task error code */
/***************************/
#define SYS_BASE_I2CBUS_TASK_ERROR_CODE                 SYS_BASE_SPIBUS_TASK_ERROR_CODE + SYS_GROUP_ERROR_COUNT

/* Utility task error code */
/***************************/
#define SYS_UTIL_TASK_BASE_ERROR_CODE                   SYS_BASE_I2CBUS_TASK_ERROR_CODE + SYS_GROUP_ERROR_COUNT
#define SYS_UTIL_TASK_INIT_ERROR_CODE                   SYS_UTIL_TASK_BASE_ERROR_CODE + 1
#define SYS_UTIL_TASK_LP_TIMER_ERROR_CODE               SYS_UTIL_TASK_BASE_ERROR_CODE + 2

/* Sensor task generic error code */
/**********************************/
#define SYS_SENSOR_TASK_BASE_ERROR_CODE                 SYS_UTIL_TASK_BASE_ERROR_CODE + SYS_GROUP_ERROR_COUNT

/* FFT DPU2.0 error code */
/*************************/
#define SYS_BASE_FFT_DPU_ERROR_CODE                     SYS_SENSOR_TASK_BASE_ERROR_CODE + SYS_GROUP_ERROR_COUNT
#define SYS_FFT_DPU_INPUT_NOT_READY_RET_VAL             SYS_BASE_FFT_DPU_ERROR_CODE + 1

#ifdef __cplusplus
}
#endif

#endif /* APPERROR_H_ */

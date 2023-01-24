/**
 ******************************************************************************
 * @file    sysconfig.h
  * @author  SRA
 * @brief   Global System configuration file
 *
 * This file include some configuration parameters grouped here for user
 * convenience. This file override the default configuration value, and it is
  * used in the "Preinclude file" section of the "compiler > preprocessor"
 * options.
 *
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

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

/* Include */
/***********/
#include "SMPinConfig.h"

/**
 *
 *  file NucleoDriver.c
 *  uncomment the following line to change the drive common parameters
 */
/* #define NUCLEO_DRV_CFG_IRQ_PRIORITY             13 */

/* Board and FW ID  */
/* ******************/
#define BOARD_ID                                  0x0E

/* Other hardware configuration */
/********************************/

#define SYS_DBG_AUTO_START_TA4                    0

/* Services configuration */
/**************************/

/* files syslowpower.h, SysDefPowerModeHelper.c*/
#define SYS_CFG_USE_DEFAULT_PM_HELPER             0
#define SYS_CFG_DEF_PM_HELPER_STANDBY             0  /* if defined to 1 then the MCU goes in STANDBY mode when the system enters in SLEEP_1. */

/* file SysTimestamp.c */
#define SYS_TS_CFG_ENABLE_SERVICE 1

/**
 * Hardware timer clock frequency in Hz
 */
/* #define SYS_TS_CFG_TSDRIVER_FREQ_HZ SystemCoreClock */

/**
 * ThreadX clock frequency in Hz
 */
#define SYS_TS_CFG_TSDRIVER_FREQ_HZ TX_TIMER_TICKS_PER_SECOND

/* #define SYS_TS_CFG_TSDRIVER_PARAMS &MX_TIM3InitParams */
#define SYS_TS_CFG_TSDRIVER_PARAMS SYS_TS_USE_SW_TSDRIVER

#define COM_MAX_SENSORS                           16


/* Tasks configuration */
/***********************/
#ifdef ENABLE_THREADX_DBG_PIN
#include "STWIN.box_debug_pins.h"
#endif

/* file IManagedTask.h */
/***********************/
#define MT_ALLOWED_ERROR_COUNT                    0x2

/* file sysinit.c */
/******************/
#define INIT_TASK_CFG_ENABLE_BOOT_IF              0
#define INIT_TASK_CFG_STACK_SIZE                  (TX_MINIMUM_STACK*8)

/**
 * Memory used by eloom to build up the system using azure rtos
 */
#define INIT_TASK_CFG_HEAP_SIZE                   (560*1024)

/* file UtilTask.c */
/*******************/
#define UTIL_TASK_CFG_STACK_DEPTH                 (TX_MINIMUM_STACK*5)
#define UTIL_TASK_CFG_PRIORITY                    (14)

/* file DatalogAppTask.c */
/*************************/
#define DT_TASK_CFG_STACK_DEPTH                   (TX_MINIMUM_STACK*8)
#define DT_TASK_CFG_PRIORITY                      (12)

/* file FDM_ACOTask.c */
/**********************/
#define ACO_TASK_CFG_TAG                          (CON34_PIN_14)
#define ACO_TASK_CFG_STACK_DEPTH                  (TX_MINIMUM_STACK*8)
#define ACO_TASK_CFG_PRIORITY                     (12)


#endif /* SYSCONFIG_H_ */

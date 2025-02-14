/**
  ******************************************************************************
  * @file    sysconfig.h
  * @author  SRA - GPM
  *
  *
  * @brief   Global System configuration file
  *
  * This file include some configuration parameters grouped here for user
  * convenience. This file override the default configuration value, and it is
  * used in the "Preinclude file" section of the "compiler > prepocessor"
  * options.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

// Board ID and FW_ID
// *********************

#define BOARD_ID           0x09
#define BLE_FW_ID_DATALOG2 0x10
#define USB_FW_ID_DATALOG2 0x03

// Other hardware configuration
// ****************************

#define SYS_DBG_AUTO_START_TA4                    0

// Services configuration
// **********************

// files syslowpower.h, SysDefPowerModeHelper.c
#define SYS_CFG_USE_DEFAULT_PM_HELPER             0
#define SYS_CFG_DEF_PM_HELPER_STANDBY             0  ///< if defined to 1 then the MCU goes in STANDBY mode when the system enters in SLEEP_1.


// Tasks configuration
// *******************

// file IManagedTask.h
#define MT_ALLOWED_ERROR_COUNT                    0x2

// file sysinit.c
#define INIT_TASK_CFG_ENABLE_BOOT_IF              0
#define INIT_TASK_CFG_STACK_SIZE                  (TX_MINIMUM_STACK*10)

// HEAP memory used for SysAlloc
#define INIT_TASK_CFG_HEAP_SIZE                   (545*1024)

#define DT_TASK_CFG_STACK_DEPTH                   (TX_MINIMUM_STACK*12)
#define DT_TASK_CFG_PRIORITY                      (12)
#ifdef ENABLE_THREADX_DBG_PIN
//#define DT_TASK_CFG_TAG                           (CON34_PIN_14)
#define DT_TASK_CFG_TAG                           (CON34_PIN_5)
#endif

// file filex_dctrl_class.c
#define FILEX_CFG_STACK_DEPTH                     (TX_MINIMUM_STACK*13)
#define FILEX_SEND_CFG_PRIORITY                   (10)
#define FILEX_CFG_PREEMPTION_THRESHOLD            FILEX_SEND_CFG_PRIORITY
#ifdef ENABLE_THREADX_DBG_PIN
//#define FILEX_TASK_CFG_TAG                        (CON34_PIN_14)
#define FILEX_TASK_CFG_TAG                        (CON34_PIN_5)
#endif

// file ble_stream_class.c
/* BLE Send Thread parameters */
#define BLE_SEND_CFG_STACK_DEPTH                  (TX_MINIMUM_STACK*15)
#define BLE_SEND_CFG_PRIORITY                     (15)
#define BLE_SEND_CFG_PREEMPTION_THRESHOLD         BLE_SEND_CFG_PRIORITY
#ifdef ENABLE_THREADX_DBG_PIN
#define BLE_SEND_TASK_CFG_TAG                     (CON34_PIN_5)
#endif
/* BLE Receive Thread parameters */
#define BLE_RECEIVE_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*20)
#define BLE_RECEIVE_CFG_PRIORITY                  (18)
#define BLE_RECEIVE_CFG_PREEMPTION_THRESHOLD      BLE_RECEIVE_CFG_PRIORITY
#ifdef ENABLE_THREADX_DBG_PIN
#define BLE_RECEIVE_TASK_CFG_TAG                  (CON34_PIN_5)
#endif

/* SensorManager configuration */
#define SM_MAX_SENSORS                            13U

// file UtilTask.c
#define UTIL_TASK_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*7)
#define UTIL_TASK_CFG_PRIORITY                  (14)

// App configuration

// USBX Tasks
#define USB_EP_BULKIN_CFG_STACK_DEPTH             (1 * 1024)
#define USB_EP_BULKIN_CFG_PRIORITY                (3)
#define USB_EP_BULKIN_CFG_PREEMPTION_THRESHOLD    USB_EP_BULKIN_CFG_PRIORITY
#ifdef ENABLE_THREADX_DBG_PIN
#define USB_EP_BULKIN_TASK_CFG_TAG                (CON34_PIN_30)
#endif


#define ENABLE_USBX_EP_DBG_PIN

#if defined(ENABLE_THREADX_DBG_PIN) && defined(ENABLE_USBX_EP_DBG_PIN)
#define USB_EP1_BULKIN_CFG_TAG                (CON34_PIN_28)
#define USB_EP2_BULKIN_CFG_TAG                (CON34_PIN_14)
#define USB_EP3_BULKIN_CFG_TAG                (CON34_PIN_22)
#define USB_EP4_BULKIN_CFG_TAG                (CON34_PIN_26)
#define USB_EP5_BULKIN_CFG_TAG                (CON34_PIN_12)
#endif

// file SysTimestamp.c
#define SYS_TS_CFG_ENABLE_SERVICE 1
/**
  * Configuration parameter for the timer used for the eLooM timestamp service.
  * Valid value are:
  * - SYS_TS_USE_SW_TSDRIVER to use the RTOS tick
  * - The configuration structure for an hardware timer. It must be compatible with SysTimestamp_t type.
  */
#define SYS_TS_CFG_TSDRIVER_PARAMS &MX_TIM5InitParams
//#define SYS_TS_CFG_TSDRIVER_PARAMS SYS_TS_USE_SW_TSDRIVER

#define SYS_TS_CFG_TSDRIVER_FREQ_HZ SystemCoreClock ///< hardware timer clock frequency in Hz
//#define SYS_TS_CFG_TSDRIVER_FREQ_HZ TX_TIMER_TICKS_PER_SECOND ///< ThreadX clock frequency in Hz


// file HelloWorldTask.c
// uncomment the following lines to change the task common parameters
#define HW_TASK_CFG_STACK_DEPTH                    (TX_MINIMUM_STACK*7)
#define HW_TASK_CFG_PRIORITY                       (TX_MAX_PRIORITIES-2)

#endif /* SYSCONFIG_H_ */

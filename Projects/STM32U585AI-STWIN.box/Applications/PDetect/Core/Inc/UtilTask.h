/**
  ******************************************************************************
  * @file    UtilTask.h
  * @author  SRA
  * @brief  Utility task declaration.
  *
  * The Utility task is in export minor functionalities of the application:
  * - Time stamp for the Sensor Manager
  * - Manage the user button and the user LED1
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
#ifndef INC_UTILTASK_H_
#define INC_UTILTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "drivers/BCAdcDriver.h"
#include "drivers/BCAdcDriver_vtbl.h"
#include "drivers/BCTimerDriver.h"
#include "drivers/BCTimerDriver_vtbl.h"
#include "drivers/BCTimChgDriver.h"
#include "drivers/BCTimChgDriver_vtbl.h"
#include "services/BCProtocol.h"
#include "tx_api.h"

#define UTIL_CMD_ID_BUTTON_EVT                        ((uint16_t)0x0004)              ///< Button event. It can be BUTTON_DOWN or BUTTON_UP
#define UTIL_CMD_ID_PB_TIMER_ELAPSED                  ((uint16_t)0x0005)              ///< PB timer is elapsed.
#define UTIL_CMD_ID_DATALOG_LED                       ((uint16_t)0x0006)              ///< Button event. Toggle led

#define UTIL_PARAM_BUTTON_EVT_DOWN                    ((uint16_t)0x0001)              ///< Button event parameter: BUTTON_DOWN
#define UTIL_PARAM_BUTTON_EVT_UP                      ((uint16_t)0x0002)              ///< Button event parameter: BUTTON_UP

#define UTIL_BATTERY_STATUS_DISCHARGING         ((uint8_t)0x01)         ///< Battery Status: discharging
#define UTIL_BATTERY_STATUS_CHARGING          ((uint8_t)0x02)         ///< Battery Status: charging
#define UTIL_BATTERY_STATUS_NOT_CONNECTED       ((uint8_t)0x03)         ///< Battery Status: battery not connected
#define UTIL_BATTERY_STATUS_FULL            ((uint8_t)0x04)         ///< Battery Status: battery fully charged
#define UTIL_BATTERY_STATUS_UNKNOWN           ((uint8_t)0xff)         ///< Battery Status: UNKNOWN


/**
  * Create  type name for _UtilTask_t.
  */
typedef struct _UtilTask_t UtilTask_t;

/**
  *  UtilTask_t internal structure.
  */
struct _UtilTask_t
{
  /**
    * Base class object.
    */
  AManagedTaskEx super;

  /* Task variables should be added here. */

  /**
    * Specifies the timer driver for the Single Wire protocol used by the task. It is an instance of ::BCTimerDriver_t.
    */
  IDriver *p_bc_timer_driver;

  /**
    * Specifies the timer driver for CHG freq detection used by the task. It is an instance of ::BCTimChgDriver_t.
    */
  IDriver *p_bc_tim_chg_driver;

  /**
    * Specifies the battery charger adc driver used by the task. It is an instance of ::BCAdcDriver_t.
    */
  IDriver *p_bc_adc_driver;

  /**
    * HAL TIM driver configuration parameters for the ::BCDriver_t.
    */
  const void *p_mx_bc_tim_drv_cfg;

  /**
    * HAL GPIO driver configuration parameters for the ::BCTimerDriver_t.
    */
  const void *p_mx_bc_gpio_sw_drv_cfg;

  /**
    * HAL GPIO driver configuration parameters for the ::BCTimChgDriver_t.
    */
  const void *p_mx_bc_gpio_chg_drv_cfg;

  /**
    * HAL GPIO driver configuration parameters for the ::BCTimerDriver_t.
    */
  const void *p_mx_bc_gpio_cen_drv_cfg;

  /**
    * HAL TIM driver configuration parameters for the ::BCDriver_t.
    */
  const void *p_mx_bc_tim_chg_drv_cfg;

  /**
    * HAL ADC driver configuration parameters for the ::BCDriver_t.
    */
  const void *p_mx_bc_adc_drv_cfg;

  /**
    * HAL user button configuration parameters.
    */
  const void *p_mx_ub_drv_cfg;

  /**
    * HAL led1 configuration parameters.
    */
  const void *p_mx_led1_drv_cfg;

  /**
    * HAL led2 configuration parameters.
    */
  const void *p_mx_led2_drv_cfg;

  /**
    * Battery Charger Protocol object.
    */
  BCProtocol_t bc_protocol;

  /**
    * Software timer used to synthesize the button events:
    *  - SHORT_PRESS
    *  - LONG_PRESS
    *  - DOUBLE_PRESS
    */
  TX_TIMER pb_timer;

  /**
    * Rechargeble Battery Voltage
    */
  uint16_t BattVolt;

  /**
    * Rechargeble Battery Voltage
    */
  ESTBC02_STATE_t STBC02_state;

  /**
    * Input queue used by other task to request services.
    */
  TX_QUEUE in_queue;

  /**
    * Software timer used make user led blinking
    */
  TX_TIMER user_led_timer;
};

/* Public API declaration */
/**************************/

/**
  * Allocate an instance of UtilTask.
  *
  * @param p_mx_bc_tim_drv_cfg [IN] specifies a ::MX_TIMParams_t instance declared in the mx.h file.
  *        This is the hardware timer used by the ::BCDriver_t to implement the 1-wire communication with STBC02.
  * @param p_mx_bc_gpio_drv_cfg [IN] specifies a ::MX_TIMParams_t instance declared in the mx.h file.
  *        This is the hardware timer used by the ::BCDriver_t to implement the 1-wire communication with STBC02.
  * @param p_mx_bc_adc_drv_cfg [IN] specifies a ::MX_GPIOParams_t instance declared in the mx.h file.
  *        This is the GPIO used by the ::BCDriver_t to implement the 1-wire communication with STBC02.
  * @return a pointer to the generic object ::AManagedTaskEx if success, or NULL if out of memory error occurs.
  */
AManagedTaskEx *UtilTaskAlloc(const void *p_mx_bc_tim_drv_cfg, const void *p_mx_bc_gpio_sw_drv_cfg,
                              const void *p_mx_bc_gpio_chg_drv_cfg,
                              const void *p_mx_bc_gpio_cen_drv_cfg, const void *p_mx_bc_tim_chg_drv_cfg, const void *p_mx_bc_adc_drv_cfg,
                              const void *p_mx_ub_drv_cfg, const void *p_mx_led1_drv_cfg, const void *p_mx_led2_drv_cfg);

/**
  * Get the battery level value in percentage
  *
  * @param batt_percentage [OUT]: variable to store battery level value
  *
  * @return error value.
  */
sys_error_code_t UtilTask_GetBatteryStatus(uint8_t *batt_percentage, uint8_t *status);

/**
  * Switch memory bank
  *
  *
  * @return error value.
  */
void SwitchBank(void);

/* Inline functions definition */
/*******************************/

#ifdef __cplusplus
}
#endif

#endif /* INC_UTILTASK_H_ */

/**
 ******************************************************************************
 * @file    BCProtocol.h
 * @author  SRA
 * @brief   Battery Charger protocol declaration
 *
 * This file declare the public API for the Battery Charger protocol for
 * STBC02 device. This protocol class uses a driver of type ::BCDriver_t.
 *
 * To use the protocol the application must:
 * - allocate a protocol object. The allocation can be static or dynamic
 *   using the BCPAlloc() method.
 * - initialize the protocol object to connect it with the driver using the
 *   BCPInit() method.
 * - Use the generic BCPSendCmd() to control the battery charger, or the
 *   the others available public API.
 *
 * Features
 *
 * Single Wire protocol is implemented using 1 Timer in free running mode
 * That generates an interrupt each time the counter met the ARR value
 * The ARR value is directly updated i the ISR function to so that, in output
 *
 * SWire protocol example:
 * \code
 *
 *   Start  ....   N pulses (Short)  ....   Stop
 *    ____    __    __         __    __    ______
 *   |    |  |  |  |  |       |  |  |  |  |      |
 *   |    |  |  |  |  |       |  |  |  |  |      |
 * __|    |__|  |__|  |_ ... _|  |__|  |__|      |__
 *
 *  Start: 360 us
 *  Short: 105 us
 *  Stop:  505 us
 *
 * \endcode
 *
 * Battery charger state through CHG pin frequency detection
 * The CHG provides status information about VIN voltage level, battery charging
 * status and faults by toggling at different frequencies.
 * The frequency is detected thanks to a Timer set in Input Capture mode.
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 ******************************************************************************
 */
 
#ifndef INC_SERVICES_BCPROTOCOL_H_
#define INC_SERVICES_BCPROTOCOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "drivers/IDriver.h"
#include "drivers/IDriver_vtbl.h"


/**
  * SW selection pulse number
  */
typedef enum _ESTBC02_SW_CMD_t
{
  E_SW1_OA_OFF              = 1,
  E_SW1_OA_ON               = 2,
  E_SW1_OB_OFF              = 3,
  E_SW1_OB_ON               = 4,
  E_SW2_OA_OFF              = 5,
  E_SW2_OA_ON               = 6,
  E_SW2_OB_OFF              = 7,
  E_SW2_OB_ON               = 8,
  E_BATMS_OFF               = 9,
  E_BATMS_ON                = 10,
  E_IEND_OFF                = 11,
  E_IEND_5_PC_IFAST         = 12,
  E_IEND_2_5_PC_IFAST       = 13,
  E_IBAT_OCP_900_mA         = 14,
  E_IBAT_OCP_450_mA         = 15,
  E_IBAT_OCP_250_mA         = 16,
  E_IBAT_OCP_100_mA         = 17,
  E_VFLOAT_ADJ_OFF          = 18,
  E_VFLOAT_ADJ_PLUS_50_mV   = 19,
  E_VFLOAT_ADJ_PLUS_100_mV  = 20,
  E_VFLOAT_ADJ_PLUS_150_mV  = 21,
  E_VFLOAT_ADJ_PLUS_200_mV  = 22,
  E_SHIPPING_MODE_ON        = 23,
  E_AUTORECH_OFF            = 24,
  E_AUTORECH_ON             = 25,
  E_WATCHDOG_OFF            = 26,
  E_WATCHDOG_ON             = 27,
  E_IFAST_IPRE_50_PC_OFF    = 28,
  E_IFAST_IPRE_50_PC_ON     = 29
} ESTBC02_SW_CMD_t;

/**
 * Specifies the 1-wire pulse state.
 */
typedef enum _ESTBC02_SW_STATE_t
{
  E_BC_IDLE,   /**< IDLE */
  E_BC_START,  /**< START */
  E_BC_PULSE_L,/**< PULSE_L */
  E_BC_PULSE_H,/**< PULSE_H */
  E_BC_STOP_L, /**< STOP_L */
  E_BC_STOP_H, /**< STOP_H */
  E_BC_WAIT    /**< wait */
} ESTBC02_SW_STATE_t;

/**
 * Specifies the CHG state.
 */
typedef enum _ESTBC02_CHG_STATE_t
{
  NOT_VALID_INPUT,
  VALID_INPUT,
  VBAT_LOW,
  END_OF_CHARGE,
  CHARGING_PHASE,
  OVERCHARGE_FAULT,
  CHARGING_TIMEOUT,
  BATTERY_VOLTAGE_BE_LOW_VPRE,
  CHARGING_THERMAL_LIMITATION,
  BATTERY_TEMPERATURE_FAULT,
  CHG_STATE_MAX_ID
} ESTBC02_CHG_STATE_t;

/**
 * Specifies the STBC02 state.
 */
typedef enum _ESTBC02_STATE_t
{
  LOW_BATTERY,
  DISCHARGING,
  PLUGGED_NOT_CHARGING,
  CHARGING,
  UNKNOWN,
  STATE_ERROR,
  BATTERY_NOT_CONNECTED,
  BATTERY_STATE_MAX_ID
} ESTBC02_STATE_t;


/**
 * Create  type name for struct _BCProtocol_t.
 */
typedef struct _BCProtocol_t BCProtocol_t;

/**
 * ::BCProtocol_t internal structure.
 */
struct _BCProtocol_t
{
  /**
   * Specifies the driver object used by the protocol to send command via Single Wire Protocol.
   * It must be an object of type ::BCTimerDriver_t;
   */
  IDriver *p_bc_tim_sw_driver;

  /**
   * Specifies the driver object used by the protocol to read the CHG status of STBC02.
   * It must be an object of type ::BCTimChgDriver_t;
   */
  IDriver *p_bc_tim_chg_driver;

  /**
   * Specifies the driver object used by the protocol to read the battery voltage by ADC.
   * It must be an object of type ::BCDriver_t;
   */
  IDriver *p_bc_adc_driver;

  /**
   * Specifies the actual state of the 1-wire channel state machine.
   */
  ESTBC02_SW_STATE_t sw_state;

  /**
   * Specifies the current command.
   */
  ESTBC02_SW_CMD_t cmd;

  /**
   * Specifies the current CHG state.
   */
  ESTBC02_CHG_STATE_t chg_state;

  /**
   * Input Capture values (1)
   */
  uint32_t ic_value1;

  /**
   * Input Capture values (2)
   */
  uint32_t ic_value2;

  /**
   * Input Capture flag
   */
  uint8_t ic_flag;

  /**
   * CHG pin IRQ counter
   */
  uint32_t chg_irq_counter;

  /**
   * Previous CHG pin IRQ counter
   */
  uint32_t chg_irq_counter_prev;

  /**
   * Pulse counter for Single Wire implementation.
   */
  uint8_t pulse;
};

/* Public API declaration */
/**************************/

/**
 * Allocate an instance of ::BCProtocol_t. The protocol object is allocated
 * in the FreeRTOS heap.
 *
 * @return a pointer to the new object ::BCProtocol_t if success, or NULL if out of memory error occurs.
 */
BCProtocol_t *BCPAlloc(void);

/**
 * Initialize the protocol object. It must be called after the object allocation and before using it.
 *
 * @param _this [IN] specifies a pointer to the protocol object.
 * @param p_bc_driver [IN] specifies a ::BCDriver_t object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t BCPInit(BCProtocol_t *_this, IDriver *p_bc_tim_sw_driver, IDriver *p_bc_tim_chg_driver, IDriver *p_bc_adc_driver);

/**
 * Send a command over the 1-wire channel to STBC02
 *
 * @param _this [IN] specifies a pointer to the protocol object.
 * @param cmd [IN] specifies a command. Supported commands are:
 *              -
 * @return SYS_NO_ERROR_CODE if success, SYS_BC_CMD_NOT_SUPPORTED otherwise
 */
sys_error_code_t BCPSendCmd(BCProtocol_t *_this, ESTBC02_SW_CMD_t cmd);

/**
 * Acquire the rechargeable battery voltage by the ADC
 *
 * @param _this [IN] specifies a pointer to the protocol object.
 * @param voltage [IN] specifies a pointer to the voltage variable
 *              -
 * @return SYS_NO_ERROR_CODE if success
 */
sys_error_code_t BCPAcquireBatteryVoltage(BCProtocol_t *_this, uint16_t *voltage);

/**
 * Acquire the STBC02 charging state
 *
 * @param _this [IN] specifies a pointer to the protocol object.
 * @param voltage [IN] specifies a pointer to the rechargeable battery state
 * @param state [IN] specifies a pointer to the STBC02 state
 *
 *              -
 * @return SYS_NO_ERROR_CODE if success
 */
sys_error_code_t BCPAcquireState(BCProtocol_t *_this, uint16_t *voltage, ESTBC02_STATE_t *state);

/**
 * Sends the command E_SHIPPING_MODE_ON to power off the battery charger.
 *
 * @param _this [IN] specifies a pointer to the protocol object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t BCPPowerOff(BCProtocol_t *_this);


#ifdef __cplusplus
}
#endif

#endif /* INC_SERVICES_BCPROTOCOL_H_ */

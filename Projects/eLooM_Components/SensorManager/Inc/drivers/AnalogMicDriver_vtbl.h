/**
  ******************************************************************************
  * @file    AnalogMicDriver_vtbl.h
  * @author  SRA - MCD
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */
#ifndef HSDCORE_INC_DRIVERS_ANALOGMICDRIVER_VTBL_H_
#define HSDCORE_INC_DRIVERS_ANALOGMICDRIVER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
  * @sa IDrvInit
  */
sys_error_code_t AnalogMicDriver_vtblInit(IDriver *_this, void *p_params);

/**
  * @sa IDrvStart
  */
sys_error_code_t AnalogMicDriver_vtblStart(IDriver *_this);

/**
  * @sa IDrvStop
  */
sys_error_code_t AnalogMicDriver_vtblStop(IDriver *_this);

/**
  *
  * @sa IDrvDoEnterPowerMode
  */
sys_error_code_t AnalogMicDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                      const EPowerMode new_power_mode);

/**
  * @sa IDrvReset
  */
sys_error_code_t AnalogMicDriver_vtblReset(IDriver *_this, void *p_params);

#ifdef __cplusplus
}
#endif

#endif /* HSDCORE_INC_DRIVERS_ANALOGMICDRIVER_VTBL_H_ */

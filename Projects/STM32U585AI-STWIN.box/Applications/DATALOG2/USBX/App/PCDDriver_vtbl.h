/**
  ******************************************************************************
  * @file    PCDDriver_vtbl.h
  * @author  SRA
  * @brief
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
#ifndef PCDDRIVER_VTBL_H_
#define PCDDRIVER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @sa IDrvInit
  */
sys_error_code_t PCDDriver_vtblInit(IDriver *_this, void *p_params);

/**
  * @sa IDrvStart
  */
sys_error_code_t PCDDriver_vtblStart(IDriver *_this);

/**
  * @sa IDrvStop
  */
sys_error_code_t PCDDriver_vtblStop(IDriver *_this);

/**
  *
  * @sa IDrvDoEnterPowerMode
  */
sys_error_code_t PCDDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode active_power_mode,
                                                const EPowerMode new_power_mode);

/**
  * @sa IDrvReset
  */
sys_error_code_t PCDDriver_vtblReset(IDriver *_this, void *p_params);

#ifdef __cplusplus
}
#endif

#endif /* PCDDRIVER_VTBL_H_ */

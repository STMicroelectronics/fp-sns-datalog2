/**
  ******************************************************************************
  * @file    AppPowerModeHelper_vtbl.h
  * @author  SRA
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

#ifndef APPPOWERMODEHELPER_VTBL_H_
#define APPPOWERMODEHELPER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


sys_error_code_t AppPowerModeHelper_vtblInit(IAppPowerModeHelper *_this);
EPowerMode AppPowerModeHelper_vtblComputeNewPowerMode(IAppPowerModeHelper *_this,
                                                      const SysEvent event);
boolean_t AppPowerModeHelper_vtblCheckPowerModeTransaction(IAppPowerModeHelper *_this,
                                                           const EPowerMode active_power_mode,
                                                           const EPowerMode new_power_mode);
sys_error_code_t AppPowerModeHelper_vtblDidEnterPowerMode(IAppPowerModeHelper *_this,
                                                          EPowerMode power_mode);
EPowerMode AppPowerModeHelper_vtblGetActivePowerMode(IAppPowerModeHelper *_this);
SysPowerStatus AppPowerModeHelper_vtblGetPowerStatus(IAppPowerModeHelper *_this);
boolean_t AppPowerModeHelper_vtblIsLowPowerMode(IAppPowerModeHelper *_this,
                                                const EPowerMode power_mode);


#ifdef __cplusplus
}
#endif

#endif /* APPPOWERMODEHELPER_VTBL_H_ */

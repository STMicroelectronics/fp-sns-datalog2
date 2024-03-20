/**
  ******************************************************************************
  * @file    AppPowerModeHelper_vtbl.h
  * @author  SRA - GPM
  *
  *
  *
  * @brief   Virtual table for the ::AppPowerModeHelper_t class.
  *
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef APPPOWERMODEHELPER_VTBL_H_
#define APPPOWERMODEHELPER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


sys_error_code_t AppPowerModeHelper_vtblInit(IAppPowerModeHelper *_this); ///< @sa IapmhInit
EPowerMode AppPowerModeHelper_vtblComputeNewPowerMode(IAppPowerModeHelper *_this,
                                                      const SysEvent event); ///< @sa IapmhComputeNewPowerMode
boolean_t AppPowerModeHelper_vtblCheckPowerModeTransaction(IAppPowerModeHelper *_this,
                                                           const EPowerMode active_power_mode, const EPowerMode new_power_mode); ///< @sa IapmhCheckPowerModeTransaction
sys_error_code_t AppPowerModeHelper_vtblDidEnterPowerMode(IAppPowerModeHelper *_this,
                                                          EPowerMode power_mode); ///< @sa IapmhDidEnterPowerMode
EPowerMode AppPowerModeHelper_vtblGetActivePowerMode(IAppPowerModeHelper *_this); ///< @sa IapmhGetActivePowerMode
SysPowerStatus AppPowerModeHelper_vtblGetPowerStatus(IAppPowerModeHelper *_this); ///< @sa IapmhGetPowerStatus
boolean_t AppPowerModeHelper_vtblIsLowPowerMode(IAppPowerModeHelper *_this,
                                                const EPowerMode power_mode); ///< @sa IapmhIsLowPowerMode


#ifdef __cplusplus
}
#endif

#endif /* APPPOWERMODEHELPER_VTBL_H_ */

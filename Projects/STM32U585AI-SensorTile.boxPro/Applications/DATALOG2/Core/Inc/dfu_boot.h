/**
  ******************************************************************************
  * @file    dfu_boot.h
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef DFU_BOOT_H_
#define DFU_BOOT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "services/IBoot.h"
#include "services/IBootVtbl.h"

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief Set a flag in RAM to indicate that the device should boot into DFU mode
  * and reset the system to enter DFU mode.
  */
void DfuBoot_set_flag_and_reset(void);


#ifdef __cplusplus
}
#endif

#endif /* DFU_BOOT_H_ */

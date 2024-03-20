/**
  ******************************************************************************
  * @file    automode.h
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

#ifndef AUTOMODE_H_
#define AUTOMODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

uint8_t automode_setup(void);
uint8_t automode_forced_stop(void);

#ifdef __cplusplus
}
#endif

#endif /* AUTOMODE_H_ */

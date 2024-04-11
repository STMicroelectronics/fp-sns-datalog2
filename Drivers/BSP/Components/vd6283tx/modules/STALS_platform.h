/**
  ******************************************************************************
  * @file    STALS.h
  * @author  IMG SW Application Team
  * @brief   Platform adaptation header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __STALS_PLATFORM__
#define __STALS_PLATFORM__ 1

#include "STALS.h"

/* following functions must be implemented by platform integrator */
STALS_ErrCode_t STALS_WrByte(void *pClient, uint8_t index, uint8_t data);
STALS_ErrCode_t STALS_RdByte(void *pClient, uint8_t index, uint8_t *data);
/* following function is optional. There is a default weak implementation */
STALS_ErrCode_t STALS_RdMultipleBytes(void *pClient, uint8_t index, uint8_t *data, int nb);

#endif

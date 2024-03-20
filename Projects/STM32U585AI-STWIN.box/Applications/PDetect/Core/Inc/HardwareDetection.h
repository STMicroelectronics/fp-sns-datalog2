/**
  ******************************************************************************
  * @file    HardwareDetection.h
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

#ifndef HARDWAREDETECTION_H_
#define HARDWAREDETECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"


typedef enum
{
  ST25DV04 = 0,
  ST25DV64,
  ST25DV04KC,
  ST25DV64KC,
  ST25DV_ERROR,
} hwd_st25dv_version;

boolean_t HardwareDetection_Check_Ext_PDETECT(uint8_t *device_address);
hwd_st25dv_version HardwareDetection_Check_ST25DV(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWAREDETECTION_H_ */

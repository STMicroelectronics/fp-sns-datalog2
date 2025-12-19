/**
  ******************************************************************************
  * @file    HardwareDetection.h
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

#ifndef HARDWAREDETECTION_H_
#define HARDWAREDETECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

boolean_t HardwareDetection_Check_Ext_IIS2DULPX(void);
boolean_t HardwareDetection_Check_Ext_IIS3DWB(void);
boolean_t HardwareDetection_Check_Ext_IIS3DWB10IS(void);
boolean_t HardwareDetection_Check_Ext_ILPS28QSW(void);
boolean_t HardwareDetection_Check_Ext_ISM330BX(void);
boolean_t HardwareDetection_Check_Ext_ISM6HG256X(void);
boolean_t HardwareDetection_Check_Ext_ISM330IS(void);
boolean_t HardwareDetection_Check_Ext_STTS22H(uint8_t *device_address);
boolean_t HardwareDetection_Check_Ext_TSC1641(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWAREDETECTION_H_ */

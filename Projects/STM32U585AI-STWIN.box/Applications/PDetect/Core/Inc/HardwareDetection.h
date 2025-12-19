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


boolean_t HardwareDetection_Check_Ext_PDETECT(uint8_t *device_address);
boolean_t HardwareDetection_Check_Ext_PDETECT2(uint8_t *device_address);
boolean_t HardwareDetection_Check_Ext_PDETECT3(uint8_t *device_address);
boolean_t HardwareDetection_Check_Ext_SENSIRION(void);

#ifdef __cplusplus
}
#endif

#endif /* HARDWAREDETECTION_H_ */

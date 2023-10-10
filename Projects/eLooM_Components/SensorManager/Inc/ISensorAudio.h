/**
  ******************************************************************************
  * @file    ISensorAudio.h
  * @author  SRA - MCD
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

#ifndef INCLUDE_ISENSORAUDIO_H_
#define INCLUDE_ISENSORAUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"
#include "SensorDef.h"
#include "ISensor.h"

/**
  * Create  type name for ISensorAudio.
  */
typedef struct _ISensorAudio_t ISensorAudio_t;

// Public API declaration
//***********************
/** Public interface **/
static inline uint32_t ISensorGetFrequency(ISensorAudio_t *_this);
static inline uint8_t ISensorGetVolume(ISensorAudio_t *_this);
static inline uint8_t ISensorGetResolution(ISensorAudio_t *_this);
static inline sys_error_code_t ISensorSetFrequency(ISensorAudio_t *_this, uint32_t frequency);
static inline sys_error_code_t ISensorSetVolume(ISensorAudio_t *_this, uint8_t volume);
static inline sys_error_code_t ISensorSetResolution(ISensorAudio_t *_this, uint8_t bit_depth);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISENSORAUDIO_H_ */

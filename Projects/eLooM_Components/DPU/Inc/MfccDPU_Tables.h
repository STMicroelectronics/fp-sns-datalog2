/**
  ******************************************************************************
  * @file    mfccDPU_tables.h
  * @author  MCD Application Team
  * @brief   Header for common_tables.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
  
#ifndef _MFCC_DPU_TABLES_H
#define _MFCC_DPU_TABLES_H

#include "arm_math.h"
#include "MfccDPU.h"

extern const float32_t hannWin_8192[8192];
extern const float32_t hannWin_4096[4096];
extern const uint32_t  melFiltersStartIndices_8192_128[128];
extern const uint32_t  melFiltersStopIndices_8192_128[128];
extern const float32_t melFilterLut_8192_128[8016];
extern const uint32_t  melFiltersStartIndices_4096_32[32];
extern const uint32_t  melFiltersStopIndices_4096_32[32];
extern const float32_t melFilterLut_4096_32[3772];
extern const float32_t dct2_60_128[60*128];
extern const float32_t dct2_32_32[32*32];


#endif /* _MFCC_DPU_TABLES_H */


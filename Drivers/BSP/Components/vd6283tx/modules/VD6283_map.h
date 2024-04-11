/**
  ******************************************************************************
  * @file    VD6283_map.h
  * @author  IMG SW Application Team
  * @brief   Register map header file
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

#ifndef __VD6283_MAP__
#define __VD6283_MAP__ 1

#ifndef BIT
#define BIT(b)							(1UL << (b))
#endif


#define VERSION_MAJOR				2
#define VERSION_MINOR				3
#define VERSION_REVISION			0

#define VD6283_DEVICE				0x70
#define VD6283_REVISION		0xBD

#define VD6283_DEVICE_ID					0x00
#define VD6283_REVISION_ID					0x01
#define VD6283_IRQ_CTRL_ST					0x02
#define VD6283_ALS_CTRL						0x03
#define VD6283_ALS_START					BIT(0)
#define VD6283_ALS_CONTINUOUS					BIT(1)
#define VD6283_ALS_CONTINUOUS_SLAVED				BIT(2)
#define VD6283_CONTINUOUS_PERIOD				0x04

#define VD6283_CHANNELx(c)					(0x05 + 4 * (c))
#define VD6283_CHANNELx_MM(c)					(0x06 + 4 * (c))
#define VD6283_CHANNELx_LM(c)					(0x07 + 4 * (c))
#define VD6283_CHANNELx_LL(c)					(0x08 + 4 * (c))

#define VD6283_EXPOSURE_M					0x1d
#define VD6283_EXPOSURE_L					0x1e
#define VD6283_CHANNEL_VREF(chan)				(0x25 + (chan))

#define VD6283_AC_EN						0x2D
#define VD6283_DC_EN						0x2E
#define VD6283_AC_CLAMP_EN					0x2F
#define VD6283_DC_CLAMP_EN					0x30
#define VD6283_AC_MODE						0x31
#define AC_EXTRACTOR						BIT(0)
#define AC_EXTRACTOR_ENABLE					BIT(0)
#define AC_EXTRACTOR_DISABLE					0
#define AC_CHANNEL_SELECT					0x0e
#define PDM_SELECT_OUTPUT					BIT(4)
#define PDM_SELECT_GPIO1					0
#define PDM_SELECT_GPIO2					BIT(4)
#define PDM_SELECT_CLK						BIT(5)
#define PDM_SELECT_INTERNAL_CLK					0
#define PDM_SELECT_EXTERNAL_CLK					BIT(5)
#define AC_PEDESTAL						BIT(6)
#define AC_PEDESTAL_ENABLE					0
#define AC_PEDESTAL_DISABLE					BIT(6)
#define VD6283_AC_PEDESTAL					0x32
#define VD6283_PEDESTAL_VALUE_MASK				0x07
#define VD6283_AC_SAT_METRIC_M					0x33
#define VD6283_AC_SAT_METRIC_L					0x34
#define VD6283_AC_ACC_PERIODS_M					0x35
#define VD6283_AC_ACC_PERIODS_L					0x36
#define VD6283_AC_NB_PERIODS					0x37
#define VD6283_AC_AMPLITUDE_M					0x38
#define VD6283_AC_AMPLITUDE_L					0x39

#define VD6283_SDA_DRIVE					0x3c
#define VD6283_SDA_DRIVE_MASK					0x07
#define VD6283_OSC10M						0x3d
#define VD6283_OSC10M_TRIM_M					0x3e
#define VD6283_OSC10M_TRIM_L					0x3f
#define VD6283_OSC50K_TRIM					0x40

#define VD6283_INTR_CFG						0x41
#define VD6283_DTEST_SELECTION					0x47

#define VD6283_SEL_PD_x(c)					(0x6B + (c))

#define VD6283_OTP_CTRL1					0x58

#define VD6283_OTP_STATUS					0x5a
#define VD6283_OTP1_DATA_READY					BIT(1)
#define VD6283_OTP2_DATA_READY					BIT(3)
#define VD6283_OTP_DATA_READY					(VD6283_OTP1_DATA_READY | VD6283_OTP2_DATA_READY)

#define VD6283_OTP_BANK_0					0x5b
#define VD6283_OTP_BANK_1					0x63

#define VD6283_SPARE_0						0x71
#define VD6283_SPARE_0_OUT_GPIO2				0
#define VD6283_SPARE_0_IN_GPIO2					BIT(0)
#define VD6283_SPARE_0_ZC_GPIO2					BIT(1)
#define VD6283_SPARE_0_ZC_GPIO1					BIT(2)
#define VD6283_SPARE_1						0x72

#define VD6283_OSC_DITHERING_CTRL				0x76
#define VD6283_OSC_DITHERING_STEP_VALUE				0x77
#define VD6283_OSC_DITHERING_STEP_DURATION			0x78
#define VD6283_OSC_DITHERING_STEP_NUMBER			0x79

#define VD6283_GLOBAL_RESET					0xFE

#endif

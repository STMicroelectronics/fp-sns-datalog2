/**
  ******************************************************************************
  * @file    bluenrg1_dev_config.c
  * @author  AMG - RF Application Team
  * @brief   BlueNRG-1/2 device configuration file
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
#include "bluenrg_utils.h"

/************** Do not change this define section ************/

#define HS_CRYSTAL_16MHZ        0xFF
#define HS_CRYSTAL_32MHZ        0x00

#define LS_SOURCE_RO            0xFF
#define LS_SOURCE_32KHZ         0x00

#define SMSP_10_UH              0xFF
#define SMSPS_4_7_UH            0x00
#define SMPS_IND_OFF            0x01

#define USER_MODE               0xFF
#define LS_CRYSTAL_MEASURE      0x01
#define HS_STARTUP_TIME_MEASURE 0x02

#define MASTER_SCA_500_PPM       0 /*!< 251 ppm to 500 ppm */
#define MASTER_SCA_250_PPM       1 /*!< 151 ppm to 250 ppm */
#define MASTER_SCA_150_PPM       2 /*!< 101 ppm to 150 ppm */
#define MASTER_SCA_100_PPM       3 /*!< 76 ppm to 100 ppm */
#define MASTER_SCA_75_PPM        4 /*!< 51 ppm to 75 ppm */
#define MASTER_SCA_50_PPM        5 /*!< 31 ppm to 50 ppm */
#define MASTER_SCA_30_PPM        6 /*!< 21 ppm to 30 ppm */
#define MASTER_SCA_20_PPM        7 /*!< 0 ppm to 20 ppm */

/************************************************************/


/************** Definitions that can be changed. ************/

#define HS_SPEED_XTAL           HS_CRYSTAL_16MHZ
#define LS_SOURCE               LS_SOURCE_32KHZ

#define SLAVE_SCA_PPM           250     /* Max 500 */
#define MASTER_SCA              MASTER_SCA_30_PPM
#define HS_STARTUP_TIME_US      800
#define MAX_CONN_EVT_LEN        4000

#define DAY                     02
#define MONTH                   05
#define YEAR                    85
/************************************************************/

#ifndef HS_SPEED_XTAL
#define HS_SPEED_XTAL   HS_CRYSTAL_32MHZ
#endif /* HS_SPEED_XTAL */

#ifndef LS_SOURCE
#define LS_SOURCE       LS_SOURCE_RO
#endif /* LS_SOURCE */

#ifndef SMPS_INDUCTOR
#define SMPS_INDUCTOR   SMSP_10_UH
#endif /* SMPS_INDUCTOR */

#ifndef TEST_MODE
#define TEST_MODE       USER_MODE
#endif /* TEST_MODE */


const dev_config_t device_config =
{
  HS_SPEED_XTAL,
  LS_SOURCE,
  SMPS_INDUCTOR,
  0xFF,
  HTOB(FROM_US_TO_SYS_TIME(HS_STARTUP_TIME_US), 2),
  HTOB(SLAVE_SCA_PPM, 0),
  MASTER_SCA,
  HTOB(FROM_US_TO_SYS_TIME((MAX_CONN_EVT_LEN * 1000)), 4),
  TEST_MODE
};

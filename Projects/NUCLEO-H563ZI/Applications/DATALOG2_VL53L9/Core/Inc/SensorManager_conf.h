/**
  ******************************************************************************
  * @file    SensorManager_conf.h
  * @author  SRA
  * @brief   Global System configuration file
  *
  * This file include some configuration parameters grouped here for user
  * convenience. This file override the default configuration value, and it is
  * used in the "Preinclude file" section of the "compiler > prepocessor"
  * options.
  *
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

#ifndef SENSORMANAGERCONF_H_
#define SENSORMANAGERCONF_H_


/* #define HSD_USE_DUMMY_DATA 1 */


/* file VL53L9Task.c */
#define VL53L9_TASK_CFG_STACK_DEPTH     (TX_MINIMUM_STACK*14)
#define VL53L9_TASK_CFG_PRIORITY        (8)
#define VL53L9_TASK_CFG_TIMER_PERIOD_MS (500u)
#define VL53L9_TASK_CFG_EXT_CLOCK_HZ    12000000UL


/* file I3CBusTask.c */
#define I3CBUS_TASK_CFG_STACK_DEPTH (TX_MINIMUM_STACK*6)
#define I3CBUS_TASK_CFG_PRIORITY    (4)


#endif /* SENSORMANAGERCONF_H_ */

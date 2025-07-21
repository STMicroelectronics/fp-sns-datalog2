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

/**
  * Uncomment the code below to enable features
  */
/*#define HSD_USE_DUMMY_DATA 1*/

/* file IMP23ABSUTask.c */
#define IMP23ABSU_TASK_CFG_STACK_DEPTH            (TX_MINIMUM_STACK*7)
#define IMP23ABSU_TASK_CFG_PRIORITY               (8)
#ifdef ENABLE_THREADX_DBG_PIN
#define IMP23ABSU_TASK_CFG_TAG                   (CON34_PIN_16)
#endif

#endif /* SENSORMANAGERCONF_H_ */

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


//#define HSD_USE_DUMMY_DATA 1

// file I2CBusTask.c
#define I2CBUS_TASK_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*6)
#define I2CBUS_TASK_CFG_PRIORITY                  (4)

// file SPIBusTask.c
#define SPIBUS_TASK_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*6)
#define SPIBUS_TASK_CFG_PRIORITY                  (4)

#define ADC_CONVERTED_DATA_BUFFER_SIZE   ((uint32_t)  20000)   /* Size of array aADCxConvertedData[] */


#endif /* SENSORMANAGERCONF_H_ */

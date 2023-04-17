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

#ifndef SENSORMANAGERCONF_H_
#define SENSORMANAGERCONF_H_

#define LPS22DF_FIFO_ENABLED	1
#define LIS2DU12_FIFO_ENABLED	1
#define LSM6DSV16X_FIFO_ENABLED	1

//#define HSD_USE_DUMMY_DATA 1

//#define EXTERNAL_IIS3DWB 1

// file LSM6DSV16XTask.c
#define LSM6DSV16X_TASK_CFG_STACK_DEPTH           (TX_MINIMUM_STACK*8)
#define LSM6DSV16X_TASK_CFG_PRIORITY              (8)
#ifdef ENABLE_THREADX_DBG_PIN
#define LSM6DSV16X_TASK_CFG_TAG                   (CON34_PIN_12)
#endif

// file LIS2DU12Task.c
#define LIS2DU12_TASK_CFG_STACK_DEPTH             (TX_MINIMUM_STACK*7)
#define LIS2DU12_TASK_CFG_PRIORITY                (8)

// file LPS22DFTask.c
#define LPS22DF_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*12)
#define LPS22DF_TASK_CFG_PRIORITY                 (8)
#ifdef ENABLE_THREADX_DBG_PIN
#define LPS22DF_TASK_CFG_TAG                      (CON34_PIN_14)
#endif
#define LPS22DF_TASK_CFG_I2C_ADDRESS              LPS22DF_I2C_ADD_H

// file LIS2MDLTask.c
#define LIS2MDL_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*7)
#define LIS2MDL_TASK_CFG_PRIORITY                 (8)

// file STTS22HTask.c
#define STTS22H_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*7)
#define STTS22H_TASK_CFG_PRIORITY                 (8)
#define STTS22H_TASK_CFG_I2C_ADDRESS              STTS22H_I2C_ADD_H

// file MP23DB01HPTask.c
#define MP23DB01HP_TASK_CFG_STACK_DEPTH            (TX_MINIMUM_STACK*7)
#define MP23DB01HP_TASK_CFG_PRIORITY               (8)
#ifdef ENABLE_THREADX_DBG_PIN
//#define IMP34DT05_TASK_CFG_TAG                    (CON34_PIN_22)
#define MP23DB01HP_TASK_CFG_TAG                    (CON34_PIN_5)
#endif

// file I2CBusTask.c
#define I2CBUS_TASK_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*6)
#define I2CBUS_TASK_CFG_PRIORITY                  (4)

// file SPIBusTask.c
#define SPIBUS_TASK_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*6)
#define SPIBUS_TASK_CFG_PRIORITY                  (4)

#endif /* SENSORMANAGERCONF_H_ */

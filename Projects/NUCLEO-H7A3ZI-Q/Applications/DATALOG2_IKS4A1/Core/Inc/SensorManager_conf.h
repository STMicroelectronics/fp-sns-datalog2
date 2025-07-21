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

#define LIS2DUXS12_FIFO_ENABLED          1
#define LSM6DSV16X_FIFO_ENABLED          1

#define LIS2DUXS12_MLC_DISABLED          1
//#define LSM6DSO16IS_ISPU_DISABLED        1
//#define LSM6DSV16X_MLC_DISABLED          1

//#define HSD_USE_DUMMY_DATA               1


// file LIS2DUXS12Task.c
#define LIS2DUXS12_TASK_CFG_STACK_DEPTH  (TX_MINIMUM_STACK*7)
#define LIS2DUXS12_TASK_CFG_PRIORITY     (8)

// file LIS2MDLTask.c
#define LIS2MDL_TASK_CFG_STACK_DEPTH     (TX_MINIMUM_STACK*7)
#define LIS2MDL_TASK_CFG_PRIORITY        (8)

// file LPS22DFTask.c
#define LPS22DF_TASK_CFG_STACK_DEPTH     (TX_MINIMUM_STACK*7)
#define LPS22DF_TASK_CFG_PRIORITY        (8)

// file LSM6DSO16ISTask.c
#define LSM6DSO16IS_TASK_CFG_STACK_DEPTH (TX_MINIMUM_STACK*7)
#define LSM6DSO16IS_TASK_CFG_PRIORITY    (8)
#define LSM6DSO16IS_TASK_CFG_I2C_ADDRESS LSM6DSO16IS_I2C_ADD_L

// file LSM6DSV16XTask.c
#define LSM6DSV16X_TASK_CFG_STACK_DEPTH  (TX_MINIMUM_STACK*7)
#define LSM6DSV16X_TASK_CFG_PRIORITY     (8)

// file SHT40Task.c
#define SHT40_TASK_CFG_STACK_DEPTH       (TX_MINIMUM_STACK*7)
#define SHT40_TASK_CFG_PRIORITY          (8)

// file STTS22HTask.c
#define STTS22H_TASK_CFG_STACK_DEPTH     (TX_MINIMUM_STACK*7)
#define STTS22H_TASK_CFG_PRIORITY        (8)


// file I2CBusTask.c
#define I2CBUS_TASK_CFG_STACK_DEPTH      (TX_MINIMUM_STACK*6)
#define I2CBUS_TASK_CFG_PRIORITY         (4)

// file SPIBusTask.c
#define SPIBUS_TASK_CFG_STACK_DEPTH      (TX_MINIMUM_STACK*6)
#define SPIBUS_TASK_CFG_PRIORITY         (4)


#endif /* SENSORMANAGERCONF_H_ */

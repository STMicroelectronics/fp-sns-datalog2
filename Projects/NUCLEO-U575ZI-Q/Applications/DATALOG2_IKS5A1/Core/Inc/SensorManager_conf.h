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

#define IIS2DULPX_FIFO_ENABLED 1
#define ISM6HG256X_FIFO_ENABLED 1
#define IIS3DWB10IS_FIFO_ENABLED 1

//#define IIS3DWB10IS_ISPU_DISABLED 1
#define IIS2DULPX_MLC_DISABLED 1
//#define ISM330IS_ISPU_DISABLED        1
//#define ISM6HG256X_MLC_DISABLED          1

//#define HSD_USE_DUMMY_DATA               1


// file IIS2DULPXTask.c
#define IIS2DULPX_TASK_CFG_STACK_DEPTH  (TX_MINIMUM_STACK*7)
#define IIS2DULPX_TASK_CFG_PRIORITY     (8)

// file IIS2MDCTask.c
#define IIS2MDC_TASK_CFG_STACK_DEPTH     (TX_MINIMUM_STACK*7)
#define IIS2MDC_TASK_CFG_PRIORITY        (8)

// file ILPS22QSTask.c
#define ILPS22QS_TASK_CFG_STACK_DEPTH     (TX_MINIMUM_STACK*7)
#define ILPS22QS_TASK_CFG_PRIORITY        (8)

// file ISM330ISTask.c
#define ISM330IS_TASK_CFG_STACK_DEPTH (TX_MINIMUM_STACK*7)
#define ISM330IS_TASK_CFG_PRIORITY    (8)

// file ISM6HG256XTask.c
#define ISM6HG256X_TASK_CFG_STACK_DEPTH  (TX_MINIMUM_STACK*7)
#define ISM6HG256X_TASK_CFG_PRIORITY     (8)
#define ISM6HG256X_TASK_CFG_I2C_ADDRESS ISM6HG256X_I2C_ADD_L

// file IIS3DWB10ISTask.c
#define IIS3DWB10IS_TASK_CFG_STACK_DEPTH          (TX_MINIMUM_STACK*12)
#define IIS3DWB10IS_TASK_CFG_PRIORITY             (8)


// file I2CBusTask.c
#define I2CBUS_TASK_CFG_STACK_DEPTH      (TX_MINIMUM_STACK*6)
#define I2CBUS_TASK_CFG_PRIORITY         (4)

// file SPIBusTask.c
#define SPIBUS_TASK_CFG_STACK_DEPTH      (TX_MINIMUM_STACK*6)
#define SPIBUS_TASK_CFG_PRIORITY         (4)


#endif /* SENSORMANAGERCONF_H_ */

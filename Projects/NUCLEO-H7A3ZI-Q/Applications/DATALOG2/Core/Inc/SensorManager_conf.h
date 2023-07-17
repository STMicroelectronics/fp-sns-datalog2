/**
  ******************************************************************************
  * @file    SensorManager_conf_template.h
  * @author  SRA
  * @brief   Global System configuration file
  *
  * This file include some configuration parameters grouped here for user
  * convenience. This file override the default configuration value, and it is
  * used in the "Preinclude file" section of the "compiler > prepocessor"
  * options.
  * This file must be moved in the application folder by the user 
  *	and renamed 'SensorManager_conf'
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

#define IIS2DLPC_FIFO_ENABLED                    1
#define ISM330DHCX_FIFO_ENABLED                  1

//#define HSD_USE_DUMMY_DATA                       1


// file ISM330DHCXTask.c
#define ISM330DHCX_TASK_CFG_STACK_DEPTH           (TX_MINIMUM_STACK*7)
#define ISM330DHCX_TASK_CFG_PRIORITY              (8)
#ifdef ENABLE_THREADX_DBG_PIN
#define ISM330DHCX_TASK_CFG_TAG                   (CON34_PIN_12)
#endif

// file IIS2DLPCTask.c
#define IIS2DLPC_TASK_CFG_STACK_DEPTH             (TX_MINIMUM_STACK*7)
#define IIS2DLPC_TASK_CFG_PRIORITY                (8)

// file IIS2MDCTask.c
#define IIS2MDC_TASK_CFG_STACK_DEPTH              (TX_MINIMUM_STACK*7)
#define IIS2MDC_TASK_CFG_PRIORITY                 (8)

// file IMP34DT05Task.c
#define IMP34DT05_TASK_CFG_STACK_DEPTH            (TX_MINIMUM_STACK*7)
#define IMP34DT05_TASK_CFG_PRIORITY               (8)
#ifdef ENABLE_THREADX_DBG_PIN
//#define IMP34DT05_TASK_CFG_TAG                    (CON34_PIN_22)
#define IMP34DT05_TASK_CFG_TAG                    (CON34_PIN_5)
#endif

// file I2CBusTask.c
#define I2CBUS_TASK_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*6)
#define I2CBUS_TASK_CFG_PRIORITY                  (4)

// file SPIBusTask.c
#define SPIBUS_TASK_CFG_STACK_DEPTH               (TX_MINIMUM_STACK*6)
#define SPIBUS_TASK_CFG_PRIORITY                  (4)


#endif /* SENSORMANAGERCONF_H_ */

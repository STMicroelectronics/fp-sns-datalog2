/**
  ******************************************************************************
  * @file    apperror.h
  * @author  SRA
  *
  * @brief Application specific error code
  *
  * Application defines its own error code in this file starting form the
  * constant APP_BASE_ERROR_CODE.
  * It is recommended to group the error code in the following groups:
  * - Low Level API error code
  * - Service Level error code
  * - Task Level error code
  *
  *********************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *********************************************************************************
  */
#ifndef APPERROR_H_
#define APPERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

// Low Level API error code
// ************************

#define SYS_BASE_LL_ERROR_CODE                                APP_BASE_ERROR_CODE
#define SYS_LL_UNDEFINED_ERROR_CODE                           SYS_BASE_LL_ERROR_CODE + 1

// IP error
//#define SYS_BASE_XX_ERROR_CODE                               APP_BASE_ERROR_CODE


// Service Level error code
// ************************

// CircularBuffer error code
#define SYS_CB_BASE_ERROR_CODE                                APP_BASE_ERROR_CODE + SYS_GROUP_ERROR_COUNT


// Task Level error code
// *********************

// Generic task error code
#define SYS_BASE_APP_TASK_ERROR_CODE                           SYS_CB_BASE_ERROR_CODE + SYS_GROUP_ERROR_COUNT
#define SYS_APP_TASK_UNKNOWN_MSG_ERROR_CODE                    SYS_BASE_APP_TASK_ERROR_CODE +1
#define SYS_APP_TASK_MSG_LOST_ERROR_CODE                       SYS_BASE_APP_TASK_ERROR_CODE + 2

// file eLooM_Components/SensorManager/Inc/I2CBusTask.h
#define SYS_BASE_I2CBUS_TASK_ERROR_CODE                        SYS_BASE_APP_TASK_ERROR_CODE + SYS_GROUP_ERROR_COUNT


#ifdef __cplusplus
}
#endif

#endif /* APPERROR_H_ */

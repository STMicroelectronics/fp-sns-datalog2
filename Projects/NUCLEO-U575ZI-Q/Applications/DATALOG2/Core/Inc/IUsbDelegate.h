/**
  ******************************************************************************
  * @file    IUsbDelegate.h
  * @author  SRA
  * @brief
  *
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CORE_INC_IUSBDELEGATE_H_
#define CORE_INC_IUSBDELEGATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "services/systypes.h"
#include "services/syserror.h"
#include "services/systp.h"

/* Exported macro ------------------------------------------------------------*/
/**
  * Create  type name for _IUsbDelegate.
  */
typedef struct _IUsbDelegate IUsbDelegate;

/* Exported functions --------------------------------------------------------*/

// Public API declaration
//***********************

/**
  * Initialize the interface IUsbDelegate. It should be called after the object allocation and before using the object.
  *
  * @param _this [IN] specifies a pointer to the object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
//inline sys_error_code_t IUsbDelegateInit(IUsbDelegate *_this);

/**
  * Initialize the interface IUsbDelegate. It should be called after the object allocation and before using the object.
  *
  * @param _this [IN] specifies a pointer to the object.
  * @param _this [IN] specifies a pointer to the object.
  * @param _this [IN] specifies a pointer to the object.
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
static inline sys_error_code_t IUsbDelegateOnNewData(IUsbDelegate *_this, const uint8_t *buffer, uint32_t length);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_IUSBDELEGATE_H_ */

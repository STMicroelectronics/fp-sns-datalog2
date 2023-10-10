/**
  ******************************************************************************
  * @file    ApplicationContext.c
  * @author  STMicroelectronics - ST-Korea - MCD Team
  * @version 3.0.0
  * @date    Jan 13, 2017
  * @brief
  *
  * TODO - insert here the file description
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "services/ApplicationContext.h"

// Private member function declaration
// ***********************************


// Inline function forward declaration
// ***********************************


// Public API definition
// *********************

sys_error_code_t ACInit(ApplicationContext *_this)
{
  assert_param(_this != NULL);

  _this->m_nListSize = 0;
  _this->m_pHead = NULL;

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t ACAddTask(ApplicationContext *_this, AManagedTask *pTask)
{
  assert_param(_this != NULL);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (pTask != NULL)
  {
    // check if pTask is already in the list
    const AManagedTask *pTaskTemp = ACGetFirstTask(_this);
    while ((pTaskTemp != NULL) && (pTaskTemp != pTask))
    {
      pTaskTemp = ACGetNextTask(_this, pTaskTemp);
    }

    if ((pTaskTemp == pTask))
    {
      // the task is already in this context.
      xRes = SYS_AC_TASK_ALREADY_ADDED_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    }
    else
    {
      // add the task in the context.
      pTask->m_pNext = _this->m_pHead;
      _this->m_pHead = pTask;
      _this->m_nListSize++;
    }
  }

  return xRes;
}

void ACRemoveTask(ApplicationContext *_this, AManagedTask *pTask)
{
  assert_param(_this != NULL);
  UNUSED(pTask);

  // TODO: STF - TO BE IMPLEMENTED

  sys_error_handler();
}

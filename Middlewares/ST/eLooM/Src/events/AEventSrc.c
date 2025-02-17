/**
  ******************************************************************************
  * @file    AEventSrc.c
  * @author  STMicroelectronics - AIS - MCD Team
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "events/AEventSrc.h"
#include "events/AEventSrc_vtbl.h"
#include <string.h>


/* Public functions definition */
/*******************************/

sys_error_code_t AEvtSrcSetOwner(IEventSrc *_this, void *pxOwner)
{
  assert_param(_this != NULL);
  AEventSrc *pObj = (AEventSrc *)_this;

  pObj->m_pxOwner = pxOwner;

  return SYS_NO_ERROR_CODE;
}

void *AEvtSrcGetOwner(IEventSrc *_this)
{
  assert_param(_this != NULL);
  AEventSrc *pObj = (AEventSrc *)_this;

  return pObj->m_pxOwner;
}


/* IEventSrc virtual functions definition */
/******************************************/

sys_error_code_t AEventSrv_vtblInit(IEventSrc *_this)
{
  assert_param(_this != NULL);
  AEventSrc *pObj = (AEventSrc *)_this;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  (void)memset(pObj->m_pxListeners, 0, sizeof(pObj->m_pxListeners));

  return xRes;
}

sys_error_code_t AEventSrv_vtblAddEventListener(IEventSrc *_this, IEventListener *pListener)
{
  assert_param(_this != NULL);
  AEventSrc *pObj = (AEventSrc *)_this;
  sys_error_code_t xRes = SYS_IEVTSRC_FULL_ERROR_CODE;

  uint8_t i;
  bool found = false;
  for (i = 0; i < AEVENT_SRC_CFG_MAX_LISTENERS; ++i)
  {
    if (pObj->m_pxListeners[i] == NULL)
    {
      pObj->m_pxListeners[i] = pListener;
      xRes = SYS_NO_ERROR_CODE;
      break;
    }
    else if (pObj->m_pxListeners[i] == pListener)
    {
      found = true;
      xRes = SYS_NO_ERROR_CODE;
      break;
    }
  }

  /* check if the listener is already in the set */
  if (!found && (i < AEVENT_SRC_CFG_MAX_LISTENERS))
  {
    for (uint8_t j = i+1; j < AEVENT_SRC_CFG_MAX_LISTENERS; ++j)
    {
      if (pObj->m_pxListeners[j] == pListener)
      {
        /* the listener was already in the set, so we free the previous position.*/
    	pObj->m_pxListeners[i] = NULL;
        break;
      }
    }
  }

  return xRes;
}

sys_error_code_t AEventSrv_vtblRemoveEventListener(IEventSrc *_this, IEventListener *pListener)
{
  assert_param(_this != NULL);
  AEventSrc *pObj = (AEventSrc *)_this;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  for (uint8_t i = 0; i < AEVENT_SRC_CFG_MAX_LISTENERS; ++i)
  {
    if (pObj->m_pxListeners[i] == pListener)
    {
      pObj->m_pxListeners[i] = NULL;
      break;
    }
  }

  return xRes;
}

uint32_t AEventSrv_vtblGetMaxListenerCount(const IEventSrc *_this)
{
  UNUSED(_this);

  return AEVENT_SRC_CFG_MAX_LISTENERS;
}

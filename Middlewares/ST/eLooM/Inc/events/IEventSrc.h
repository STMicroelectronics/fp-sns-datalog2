/**
  ******************************************************************************
  * @file    IEventSrc.h
  * @author  STMicroelectronics - ST-Korea - MCD Team
  * @version 3.0.0
  * @date    Apr 6, 2017
  *
  * @brief   Event Source interface.
  *
  * An Event Source object provides the API to add / remove Event Listener
  * objects, and to notify the registered listeners.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef INCLUDE_EVENTS_IEVENTSRC_H_
#define INCLUDE_EVENTS_IEVENTSRC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "events/IEvent.h"
#include "events/IEventListener.h"
#include "events/IEventListener_vtbl.h"


// Public API declaration
//***********************

/**
  * Initialize an event source. This function should be called after the
  *
  * @param _this [IN] specifies a pointer to an IEventSrc object.
  * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
  */
static inline sys_error_code_t IEventSrcInit(IEventSrc *_this);

/**
  * Register a listener with this event source.
  *
  * @param _this [IN] specifies a pointer to an IEventSrc object.
  * @param pListener [IN] specifies a pointer to an IEventListener object.
  * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
  */
static inline sys_error_code_t IEventSrcAddEventListener(IEventSrc *_this, IEventListener *pListener);

/**
  * Remove a listener from this event source,
  *
  * @param _this [IN] specifies a pointer to an IEventSrc object.
  * @param pListener [IN] specifies a pointer to an IEventListener object.
  * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
  */
static inline sys_error_code_t IEventSrcRemoveEventListener(IEventSrc *_this, IEventListener *pListener);

/**
  * Get the maximum number of IEventListener that can be registered with this event source.
  *
  * @param _this [IN] specifies a pointer to an IEventSrc object.
  * @return the maximum number of IEventListener that can be registered with _this event source
  */
static inline uint32_t IEventSrcGetMaxListenerCount(const IEventSrc *_this);

/**
  * Send an IEvent to all the registered IEventListener.
  *
  * @param _this [IN] specifies a pointer to an IEventSrc object.
  * @param pxEvent [IN] specifies a pointer to an IEvent object.
  * @param pvParams [IN] specifies a generic pointer that can be used by the application
  *                      extend the semantic  Event design pattern.
  * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
  */
static inline sys_error_code_t IEventSrcSendEvent(const IEventSrc *_this, const IEvent *pxEvent, void *pvParams);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_EVENTS_IEVENTSRC_H_ */

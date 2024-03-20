/**
  ******************************************************************************
  * @file    ISourceObservable.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 3.0.0
  * @date    Jun 8, 2021
  *
  * @brief   Stream data source generic interface.
  *
  * This interface describe the API to observe the configuration of a
  * stream data source object (for example a sensor). This kind
  * of object has some properties like:
  * - Full Scale (FS)
  * - Output Data Rate (ODR)
  * - Sensitivity
  * - Object ID.
  *
  * An stream data source also expose an ::IEventSrc interface to allow an
  * observer object to monitor the data stream.
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#ifndef INCLUDE_ISOURCEOBSERVER_H_
#define INCLUDE_ISOURCEOBSERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "events/IEvent.h"
#include "services/em_data_format.h"


/**
  * Create  type name for ::_ISourceObservable.
  */
typedef struct _ISourceObservable ISourceObservable;


/* Public interface */
/********************/

/**
  * Get an ID value. This value should be unique between all the ::ISourceObservable object in the application.
  *
  * @param _this [IN] specifies a pointer to an object that implements the ::ISourceObservable interface.
  * @return the ID of the interface instance.
  */
static inline uint8_t ISourceGetId(ISourceObservable *_this);

/**
  * Get an ::IEventSrc interface to observe the data stream.
  *
  * @param _this [IN] specifies a pointer to an object that implements the ::ISourceObservable interface.
  * @return a pointer to an ::IEventSrc interface.
  */
static inline IEventSrc *ISourceGetEventSrcIF(ISourceObservable *_this);

/**
  * Get information about the data format produced by this source. The information are coded using the
  * ::EMData_t data format.
  *
  * @param _this specifies a pointer to an object that implements the ::ISourceObservable interface.
  * @return information about the data produced by this source.
  */
static inline EMData_t ISourceGetDataInfo(ISourceObservable *_this);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_ISOURCEOBSERVER_H_ */

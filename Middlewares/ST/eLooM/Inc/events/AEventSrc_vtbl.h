/**
  ******************************************************************************
  * @file    AEventSrc_vtbl.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 3.0.0
  * @date    Jul 13, 2020
  *
  * @brief
  *
  * <DESCRIPTIOM>
  *
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
#ifndef INCLUDE_EVENTS_AEVENTSRC_VTBL_H_
#define INCLUDE_EVENTS_AEVENTSRC_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// IEventSrc virtual functions
sys_error_code_t AEventSrv_vtblInit(IEventSrc *_this); ///< @sa IEventSrcInit
sys_error_code_t AEventSrv_vtblAddEventListener(IEventSrc *_this,
                                                IEventListener *pListener); ///< @sa IEventSrcAddEventListener
sys_error_code_t AEventSrv_vtblRemoveEventListener(IEventSrc *_this,
                                                   IEventListener *pListener); ///< @sa IEventSrcRemoveEventListener
uint32_t AEventSrv_vtblGetMaxListenerCount(const IEventSrc *_this); ///< @sa IEventSrcGetMaxListenerCount


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_AEVENTSRC_VTBL_H_ */

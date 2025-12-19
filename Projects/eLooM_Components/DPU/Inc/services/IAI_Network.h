/**
  ********************************************************************************
  * @file    IAI_Network.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 1.0.0
  * @date    Oct 21, 2022
  *
  * @brief
  *
  * <ADD_FILE_DESCRIPTION>
  *
  ********************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ********************************************************************************
  */
#ifndef DPU_INC_SERVICES_IAI_NETWORK_H_
#define DPU_INC_SERVICES_IAI_NETWORK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/systp.h"
#include "services/systypes.h"
#include "services/syserror.h"

#define SYS_IAIN_NO_ERROR_CODE                            0
#ifndef SYS_IAIN_BASE_ERROR_CODE
#define SYS_IAIN_BASE_ERROR_CODE                          1
#endif
#define SYS_IAIN_GENERIC_ERROR_CODE                       SYS_IAIN_BASE_ERROR_CODE + 1
#define SYS_IAIN_NULL_PTR_ERROR_CODE                      SYS_IAIN_BASE_ERROR_CODE + 2

/**
  * Create  type name for _IAI_Network.
  */
typedef struct _IAI_Network IAI_Network_t;


/* Public API declaration */
/**************************/

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_network_config
  * @return
  */
static inline ai_error IAI_Network_Create(IAI_Network_t *_this, const ai_buffer *p_network_config);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_activations
  * @param p_weights
  * @return
  */
static inline ai_error IAI_Network_Create_and_Init(IAI_Network_t *_this, const ai_handle *p_activations,
                                                   const ai_handle *p_weights);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @return
  */
static inline ai_handle IAI_Network_Destroy(IAI_Network_t *_this);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_params
  * @return
  */
static inline ai_bool IAI_Network_Init(IAI_Network_t *_this, const ai_network_params *p_params);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_report
  * @return
  */
static inline ai_bool IAI_Network_GetReport(IAI_Network_t *_this, ai_network_report *p_report);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_buffer
  * @return
  */
static inline ai_buffer *IAI_Network_InputsGet(IAI_Network_t *_this, ai_u16 *p_buffer);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_buffer
  * @return
  */
static inline ai_buffer *IAI_Network_OutputsGet(IAI_Network_t *_this, ai_u16 *p_buffer);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @param p_input
  * @param p_output
  * @return
  */
static inline ai_i32 IAI_Network_Run(IAI_Network_t *_this, const ai_buffer *p_input, ai_buffer *p_output);

/**
  *
  * @param _this [IN] specifies a instance of the interface.
  * @return
  */
static inline ai_error IAI_Network_GetError(IAI_Network_t *_this);


#ifdef __cplusplus
}
#endif

#endif /* DPU_INC_SERVICES_IAI_NETWORK_H_ */

/**
 ******************************************************************************
 * @file    FDM_ACOTask.h
 * @author  SRA
 * @brief
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

#ifndef FDM_ACOTask_H_
#define FDM_ACOTask_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FFT_DPU2.h"
#include "services/systp.h"
#include "services/syserror.h"
#include "services/AManagedTaskEx.h"
#include "services/AManagedTaskEx_vtbl.h"
#include "DefDataBuilder.h"
#include "DefDataBuilder_vtbl.h"

/* Command ID. These are all commands supported by a sensor task. */
#define ACO_CMD_ID_SET_MODE                ((uint16_t)0x0001)
#define ACO_CMD_ID_SET_DRY                 ((uint16_t)0x0002)
#define ACO_CMD_ID_CONFIG                   ((uint16_t)0x0003)
#define ACO_CMD_ID_SET_THRESHOLD           ((uint16_t)0x0004)

#define ACO_CMD_ID_START_ACQ                   ((uint16_t)0x0008)
#define ACO_CMD_ID_STOP                    ((uint16_t)0x0009)
#define ACO_CMD_ID_INIT                    ((uint16_t)0x000A)

/**
 * Create  type name for _FDM_ACOTask.
 */
typedef struct _FDM_ACOTask FDM_ACOTask;

/* Public API declaration */
/**************************/

/**
 * Allocate an instance of FDM_ACOTask.
 *
 * @return a pointer to the generic obejct ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx* FDM_ACOTaskAlloc(void);

void FDM_ACOTaskAddDPUListener(FDM_ACOTask *_this, IDataEventListener_t *p_listener);

/**
 * Set the source for the FDM_AcoTask.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @param dpu [IN] specifies a pointer to the data source to be used.
 */
void FDM_ACOTaskSetSourceIF(FDM_ACOTask *_this, ISourceObservable *source);

// Inline functions definition
// ***************************
#ifdef __cplusplus
}
#endif

#endif /* USER_INC_FDM_ACOTask_H_ */

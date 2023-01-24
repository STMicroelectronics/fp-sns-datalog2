/**
  ******************************************************************************
  * @file    usbx_dctrl_class.h
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
#ifndef USBX_DCTRL_CLASS_H_
#define USBX_DCTRL_CLASS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IStream.h"
#include "IStream_vtbl.h"
#include "drivers/IDriver.h"
#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"

#include "ux_device_class_sensor_streaming.h"


#define USBX_MEMORY_SIZE          (45 * 1024)
#define UX_DEVICE_APP_MEM_POOL_SIZE               USBX_MEMORY_SIZE + 1024


/**
  * Create  type name for _usbx_dctrl_class_t.
  */
typedef struct _usbx_dctrl_class_t usbx_dctrl_class_t;

/**
  *  usbx_dctrl_class_t internal structure.
  */
struct _usbx_dctrl_class_t
{
  /**
    * Base class object.
    */
  IStream_t super;

  /**
    * Driver object.
    */
  IDriver *m_pxDriver;

  UCHAR ux_device_byte_pool_buffer[UX_DEVICE_APP_MEM_POOL_SIZE];
  TX_BYTE_POOL ux_device_app_byte_pool;
  UCHAR *memory_pointer;

  UX_SLAVE_CLASS_SENSOR_STREAMING *sensor_streaming_device;
  uint8_t *TxBuffer[N_CHANNELS_MAX];
  boolean_t isClassInitiaizedByTheHost;
  UINT ux_opened;

  uint8_t state;

  ICommandParse_t *cmd_parser;

  /**
    * HAL driver configuration parameters.
    */
  const void *mx_drv_cfg;
};

/** Public API declaration */
/***************************/

/**
  * Allocate an instance of usbx_dctrl_class_t. The driver is allocated
  * in the FreeRTOS heap.
  *
  * @return a pointer to the generic interface ::IDriver if success,
  * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
  */
IStream_t *usbx_dctrl_class_alloc(const void *mx_drv_cfg);
int8_t datalog_class_control(void *_this, uint8_t isHostToDevice, uint8_t cmd, uint16_t wValue, uint16_t wIndex,
                             uint8_t *pbuf, uint16_t length);
int8_t usbx_dctrl_class_set_ep(usbx_dctrl_class_t *_this, uint8_t id_stream, uint8_t ep);
sys_error_code_t usbx_dctrl_set_ICommandParseIF(usbx_dctrl_class_t *_this, ICommandParse_t *inf);

/** Inline functions definition */
/********************************/

//SYS_DEFINE_INLINE
//sys_error_code_t DFSDMDriverFilterRegisterCallback(DFSDMDriver_t *_this, HAL_DFSDM_Filter_CallbackIDTypeDef CallbackID, pDFSDM_Filter_CallbackTypeDef pCallback)
//{
//  assert_param(_this != NULL);
//
//  HAL_DFSDM_Filter_RegisterCallback(_this->mx_handle.p_mx_dfsdm_cfg->p_dfsdm_filter, CallbackID, pCallback);
//
//  return SYS_NO_ERROR_CODE;
//}
#ifdef __cplusplus
}
#endif

#endif /* USBX_DCTRL_CLASS_H_ */

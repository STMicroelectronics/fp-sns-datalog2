/**
  ******************************************************************************
  * @file    ble_dctrl_class.h
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
  
#ifndef BLE_DCTRL_CLASS_H_
#define BLE_DCTRL_CLASS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IStream.h"
#include "IStream_vtbl.h"
#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"
#include "tx_api.h"
#include "sysconfig.h"


#define DEFAULT_BLE_QUEUE_SIZE                20

#define BLE_DCTRL_CMD_SEND_PNPL               (0x0010)
#define BLE_DCTRL_CMD_SEND_MLC                (0x0020)
#define BLE_DCTRL_CMD_CONNECTED               (0x0001)
#define BLE_DCTRL_CMD_DISCONNECTED            (0x0002)
#define BLE_DCTRL_CMD_UPDATE_ADV              (0x0003)
#define BLE_DCTRL_CMD_START_ADV_OB            (0x0004)  // start timer for option bytes advertise updated
#define BLE_DCTRL_CMD_STOP_ADV_OB             (0x0005)  // stop timer for option bytes advertise updated

/**
  * Create  type name for _ble_dctrl_class_t.
  */
typedef struct _ble_dctrl_class_t ble_dctrl_class_t;

/**
  *  ble_dctrl_class_t internal structure.
  */
struct _ble_dctrl_class_t
{
  /**
    * Base class object.
    */
  IStream_t super;

  ICommandParse_t *cmd_parser;

  UCHAR *send_thread_memory_pointer;
  UCHAR *receive_thread_memory_pointer;
  UCHAR *queue_memory_pointer;

  /* Define ThreadX global data structures.  */
  TX_THREAD ble_send_thread;
  TX_THREAD ble_receive_thread;

  TX_SEMAPHORE send_semaphore;
  TX_SEMAPHORE receive_semaphore;

  TX_QUEUE ble_app_queue;

  boolean_t status;

  boolean_t connected;

  char *serialized_cmd;

  uint32_t serialized_cmd_size;


  // TODO: to be generalized as stream_buf for different characteristics with stream_id
  uint16_t mlc_id;
  uint8_t mlc_out[9];
  uint16_t adv_id;
  // TODO: come settare valori default?
  uint8_t adv_buf[3];

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
IStream_t *ble_dctrl_class_alloc(void);
int8_t datalog_class_control(void *_this, uint8_t isHostToDevice, uint8_t cmd, uint16_t wValue, uint16_t wIndex, uint8_t *pbuf, uint16_t length);
sys_error_code_t ble_dctrl_msg(ULONG *msg);

#ifdef __cplusplus
}
#endif

#endif /* USBX_DCTRL_CLASS_H_ */

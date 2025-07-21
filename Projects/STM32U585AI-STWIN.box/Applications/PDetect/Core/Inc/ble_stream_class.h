/**
  ******************************************************************************
  * @file    ble_stream_class.h
  * @author  SRA
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#ifndef BLE_STREAM_CLASS_H_
#define BLE_STREAM_CLASS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IStream.h"
#include "IStream_vtbl.h"
#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"
#include "tx_api.h"
#include "sysconfig.h"
#include "CircularBufferDL2.h"


#define DEFAULT_BLE_QUEUE_SIZE                  20

#define BLE_ISTREAM_MSG_CONNECTED               (0x0001)
#define BLE_ISTREAM_MSG_DISCONNECTED            (0x0002)
#define BLE_ISTREAM_MSG_UPDATE_ADV              (0x0003)
#define BLE_ISTREAM_MSG_START_ADV_OB            (0x0004)  // start timer for option bytes advertise updated
#define BLE_ISTREAM_MSG_STOP_ADV_OB             (0x0005)  // stop timer for option bytes advertise updated
#define BLE_ISTREAM_MSG_SEND_CMD                (0x0010)
#define BLE_ISTREAM_MSG_SEND_CUSTOM_DATA        (0x0020)


/* Number of BLE data buffer */
#define BLE_DATA_BUFFER_COUNT             SM_MAX_SENSORS

#define BLE_DATA_BUFFER_ITEMS   2U

/**
  * Create  type name for _ble_stream_class_t.
  */
typedef struct _ble_stream_class_t ble_stream_class_t;

/**
  *  ble_stream_class_t internal structure.
  */
struct _ble_stream_class_t
{
  /**
    * Base class object.
    */
  IStream_t super;

  /**
    * Identification for this specific communication interface
    */
  uint8_t comm_interface_id;

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

  uint16_t adv_id;
  uint8_t adv_buf[3];

};


typedef struct
{
  uint8_t  messageId;
  uint8_t  streamID;
} streamMsg_t;


/* Data buffers sent via BLE */
extern CircularBufferDL2 *ble_cbdl2[];
extern uint8_t *ble_write_buffer[];


/* Exported Function prototypes --------------------------------------------- */

typedef void (*ble_stream_SetCustomStreamID_t)(void);
extern ble_stream_SetCustomStreamID_t ble_stream_SetCustomStreamIDCallback;

typedef sys_error_code_t (*ble_stream_PostCustomData_t)(uint8_t id_stream, uint8_t *buf, uint32_t size);
extern ble_stream_PostCustomData_t ble_stream_PostCustomDataCallback;

typedef void (*ble_stream_SendCustomData_t)(uint8_t id_stream);
extern ble_stream_SendCustomData_t ble_stream_SendCustomDataCallback;

typedef void (*ble_stream_SendCommand_t)(char *buf, uint32_t size);
extern ble_stream_SendCommand_t ble_stream_SendCommandCallback;

/** Public API declaration */
/***************************/

/**
  * Allocate an instance of ble_stream_class_t. The driver is allocated
  * in the FreeRTOS heap.
  *
  * @return a pointer to the generic interface ::IDriver if success,
  * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
  */
IStream_t *ble_stream_class_alloc(void);
int8_t datalog_class_control(void *_this, uint8_t isHostToDevice, uint8_t cmd, uint16_t wValue, uint16_t wIndex,
                             uint8_t *pbuf, uint16_t length);
sys_error_code_t ble_stream_msg(streamMsg_t *msg);
sys_error_code_t ble_sendMSG_wait(void);
void WriteRequestCommandLikeFunction(uint8_t *received_msg, uint8_t msg_length);

#ifdef __cplusplus
}
#endif

#endif /* BLE_STREAM_CLASS_H_ */

/**
  ******************************************************************************
  * @file    ble_stream_class.c
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

#include "ble_stream_class.h"
#include "ble_stream_class_vtbl.h"
#include "BLE_Manager.h"

#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

#ifndef MAX_DATA_STREAM_ID
#define MAX_DATA_STREAM_ID    (0U)
#endif
#define ADV_OB_STREAM_ID        (MAX_DATA_STREAM_ID+1)

#ifndef MIN
#define MIN(a,b)            ((a) < (b) )? (a) : (b)
#endif

/* Private define ------------------------------------------------------------*/

static ble_stream_class_t sObj;

const static IStream_vtbl ble_stream_vtbl =
{
  ble_stream_vtblStream_init,
  ble_stream_vtblStream_enable,
  ble_stream_vtblStream_disable,
  ble_stream_vtblStream_is_enabled,
  ble_stream_vtblStream_deinit,
  ble_stream_vtblStream_start,
  ble_stream_vtblStream_stop,
  ble_stream_vtblStream_post_data,
  ble_stream_vtblStream_alloc_resource,
  ble_stream_vtblStream_set_mode,
  ble_stream_vtblStream_dealloc,
  ble_stream_vtblStream_set_parse_IF,
  ble_stream_vtblStream_send_async
};

/* Private member function declaration */

static void AciGattTxPoolAvailableEvent(void);
static void ble_send_thread_entry(ULONG thread_input);
static void ble_receive_thread_entry(ULONG thread_input);
static sys_error_code_t ble_sendMSG_resume(void);
static sys_error_code_t ble_receiveMSG_resume(void);
static sys_error_code_t ble_receiveMSG_wait(void);


/* Exported variables -----------------------------------------------------------*/

/* callback to be implemented in separate file*/

/* Set ID for custom data stream */
ble_stream_SetCustomStreamID_t ble_stream_SetCustomStreamIDCallback;
/* Post data for custom stream */
ble_stream_PostCustomData_t ble_stream_PostCustomDataCallback;
/* Send data for custom stream */
ble_stream_SendCustomData_t ble_stream_SendCustomDataCallback;
/* Send command with specific protocol defined in separate file */
ble_stream_SendCommand_t ble_stream_SendCommandCallback;


/* Public API definition */
/*************************/
IStream_t *ble_stream_class_alloc(void)
{
  IStream_t *p = (IStream_t *) &sObj;

  p->vptr = &ble_stream_vtbl;

  return p;
}


/* IStream virtual functions definition */
/*******************************/
sys_error_code_t ble_stream_vtblStream_init(IStream_t *_this, uint8_t comm_interface_id, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;

  obj->comm_interface_id = comm_interface_id;
  obj->status = false;
  obj->connected = false;

  /* Allocate BLE_SEND_CFG_STACK_DEPTH. */
  obj->send_thread_memory_pointer = (UCHAR *) SysAlloc(BLE_SEND_CFG_STACK_DEPTH);
  if (obj->send_thread_memory_pointer == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Create the send thread.  */
  tx_thread_create(&obj->ble_send_thread, "BLE Send Thread", ble_send_thread_entry, (ULONG)obj,
                   obj->send_thread_memory_pointer, BLE_SEND_CFG_STACK_DEPTH, BLE_SEND_CFG_PRIORITY,
                   BLE_SEND_CFG_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);

#ifdef ENABLE_THREADX_DBG_PIN
  obj->ble_send_thread.pxTaskTag = BLE_SEND_TASK_CFG_TAG;
#endif

  /* Allocate BLE_SEND_CFG_STACK_DEPTH. */
  obj->receive_thread_memory_pointer = (UCHAR *) SysAlloc(BLE_RECEIVE_CFG_STACK_DEPTH);
  if (obj->receive_thread_memory_pointer == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Create the receive thread.  */
  tx_thread_create(&obj->ble_receive_thread, "BLE Receive Thread", ble_receive_thread_entry, (ULONG)obj,
                   obj->receive_thread_memory_pointer, BLE_RECEIVE_CFG_STACK_DEPTH, BLE_RECEIVE_CFG_PRIORITY,
                   BLE_RECEIVE_CFG_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);


#ifdef ENABLE_THREADX_DBG_PIN
  obj->ble_receive_thread.pxTaskTag = BLE_RECEIVE_TASK_CFG_TAG;
#endif

  // initialize send semaphore
  if (TX_SUCCESS != tx_semaphore_create(&obj->send_semaphore, "SEND_SEMAPHORE", 0))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  // initialize receive semaphore
  if (TX_SUCCESS != tx_semaphore_create(&obj->receive_semaphore, "RECEIVE_SEMAPHORE", 0))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    res = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  /* Allocate the message queue. */
  obj->queue_memory_pointer = (UCHAR *) SysAlloc(DEFAULT_BLE_QUEUE_SIZE * sizeof(ULONG));
  if (obj->queue_memory_pointer == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  // initialize receive semaphore
  if (TX_SUCCESS != tx_queue_create(&obj->ble_app_queue, "BLE App queue", TX_1_ULONG, obj->queue_memory_pointer,
                                    DEFAULT_BLE_QUEUE_SIZE * sizeof(ULONG)))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_BASE_INIT_TASK_ERROR_CODE);
    res = SYS_BASE_INIT_TASK_ERROR_CODE;
  }

  /* Callback to be implemented by the user in a different file depending on the app functionalities */
  ble_stream_SetCustomStreamIDCallback = NULL;
  ble_stream_PostCustomDataCallback = NULL;
  ble_stream_SendCustomDataCallback = NULL;
  ble_stream_SendCommandCallback = NULL;

  return res;
}

sys_error_code_t ble_stream_vtblStream_enable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;

  HCI_TL_SPI_Reset();

  /* Initialize the BlueNRG stack and services */
  BLE_BluetoothInit();

  setConnectable();

  obj->adv_id = MAX_CUSTOM_DATA_STREAM_ID + 1;

  if (ble_stream_SetCustomStreamIDCallback != NULL)
  {
    ble_stream_SetCustomStreamIDCallback();
  }

  /* Pool available callback */
  CustomAciGattTxPoolAvailableEvent = &AciGattTxPoolAvailableEvent;

  obj->status = true;

  uint32_t ctrl_msg = BLE_ISTREAM_MSG_START_ADV_OB;
  ISendCtrlMsg(sObj.cmd_parser, &ctrl_msg, 1);

  return res;
}

sys_error_code_t ble_stream_vtblStream_disable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

boolean_t ble_stream_vtblStream_is_enabled(IStream_t *_this)
{
  assert_param(_this != NULL);
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;

  return obj->connected;
}

sys_error_code_t ble_stream_vtblStream_deinit(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;

  tx_queue_delete(&obj->ble_app_queue);
  tx_thread_delete(&obj->ble_receive_thread);
  tx_thread_delete(&obj->ble_send_thread);
  tx_semaphore_delete(&obj->receive_semaphore);
  tx_semaphore_delete(&obj->send_semaphore);
  SysFree(obj->receive_thread_memory_pointer);
  SysFree(obj->send_thread_memory_pointer);
  SysFree(obj->queue_memory_pointer);

  return res;
}

sys_error_code_t ble_stream_vtblStream_start(IStream_t *_this, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t ble_stream_vtblStream_stop(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}


sys_error_code_t ble_stream_vtblStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NOT_IMPLEMENTED_ERROR_CODE;
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;

  if (id_stream == obj->adv_id)
  {
    obj->adv_buf[0] = buf[0];
    obj->adv_buf[1] = buf[1];
    obj->adv_buf[2] = buf[2];
  }
  else
  {
    if (ble_stream_PostCustomDataCallback != NULL)
    {
      res = ble_stream_PostCustomDataCallback(id_stream, buf, size);
    }

    if (res == SYS_NO_ERROR_CODE)
    {
      streamMsg_t msg;
      msg.messageId = BLE_ISTREAM_MSG_SEND_CUSTOM_DATA;
      msg.streamID = id_stream;
      ble_stream_msg(&msg);
    }
  }

  return res;
}

sys_error_code_t ble_stream_vtblStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                                      const char *stream_name)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t ble_stream_vtblStream_set_mode(IStream_t *_this, IStreamMode_t mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;
  char *response_name;

  if (mode == TRANSMIT)
  {
    ISerializeResponse(obj->cmd_parser, &response_name, &obj->serialized_cmd, &obj->serialized_cmd_size, 0);

    streamMsg_t msg;
    msg.messageId = BLE_ISTREAM_MSG_SEND_CMD;
    ble_stream_msg(&msg);
  }

  return res;
}

sys_error_code_t ble_stream_vtblStream_dealloc(IStream_t *_this, uint8_t id_stream)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t ble_stream_vtblStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn)
{
  assert_param(_this != NULL);
  assert_param(ifn != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;

  obj->cmd_parser = ifn;
  return res;
}

sys_error_code_t ble_stream_vtblStream_send_async(IStream_t *_this, uint8_t *buf, uint32_t size)
{
  assert_param(_this != NULL);
  assert_param(buf != NULL);
  assert_param(size > 0);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_stream_class_t *obj = (ble_stream_class_t *) _this;
  streamMsg_t msg;

  obj->serialized_cmd = (char *)buf;
  obj->serialized_cmd_size = size;
  msg.messageId = BLE_ISTREAM_MSG_SEND_CMD;
  ble_stream_msg(&msg);

  return res;
}

void hci_tl_lowlevel_isr(uint16_t nPin)
{
  /* Call hci_notify_asynch_evt() */
  while (IsDataAvailable())
  {
    hci_notify_asynch_evt(NULL);
  }

  if (sObj.status)
  {
    ble_receiveMSG_resume();
  }
}


sys_error_code_t ble_stream_msg(streamMsg_t *msg)
{
  assert_param(msg != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  streamMsg_t message = *msg;

  if (tx_queue_send(&sObj.ble_app_queue, &message, TX_NO_WAIT) != TX_SUCCESS)
  {
    res = 1;
  }

  return res;
}

void BLE_SetCustomAdvertiseData(uint8_t *manuf_data)
{
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1] = BLE_GetFWID();
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2] = sObj.adv_buf[0];
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3] = sObj.adv_buf[1];
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4] = sObj.adv_buf[2];
}

sys_error_code_t ble_sendMSG_wait(void)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_get(&sObj.send_semaphore, TX_WAIT_FOREVER))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

/* Private function definition */
/*******************************/

/**
  * @brief  This function is called when there is a Bluetooth Write request.
  * @param  None
  * @retval None
  */
void WriteRequestCommandLikeFunction(uint8_t *received_msg, uint8_t msg_length)
{
  if (sObj.status)
  {
    IParseCommand(sObj.cmd_parser, (char *) received_msg, sObj.comm_interface_id);
  }
}


static void AciGattTxPoolAvailableEvent(void)
{
  if (sObj.status)
  {
    ble_sendMSG_resume();
  }
}

static sys_error_code_t ble_sendMSG_resume(void)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_put(&sObj.send_semaphore))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ble_receiveMSG_wait(void)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_get(&sObj.receive_semaphore, TX_WAIT_FOREVER))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ble_receiveMSG_resume(void)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_put(&sObj.receive_semaphore))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

static void ble_send_thread_entry(ULONG thread_input)
{
  ble_stream_class_t *obj = (ble_stream_class_t *) thread_input;
  streamMsg_t received_message = { 0 };
  uint32_t ctrl_msg;

  while (1)
  {
    /* Retrieve a message from the queue. */
    if (TX_SUCCESS == tx_queue_receive(&obj->ble_app_queue, &received_message, TX_WAIT_FOREVER))
    {
      if (sObj.status)
      {
        switch (received_message.messageId)
        {
          case BLE_ISTREAM_MSG_SEND_CMD:
            if (ble_stream_SendCommandCallback != NULL)
            {
              ble_stream_SendCommandCallback(obj->serialized_cmd, obj->serialized_cmd_size);
              BLE_FREE_FUNCTION(obj->serialized_cmd);
            }
            break;

          case BLE_ISTREAM_MSG_SEND_CUSTOM_DATA:
            if (ble_stream_SendCustomDataCallback != NULL)
            {
              ble_stream_SendCustomDataCallback(received_message.streamID);
            }
            break;

          case BLE_ISTREAM_MSG_CONNECTED:
            obj->connected = true;
            ctrl_msg = BLE_ISTREAM_MSG_STOP_ADV_OB;
            ISendCtrlMsg(sObj.cmd_parser, &ctrl_msg, 1);
            break;

          case BLE_ISTREAM_MSG_DISCONNECTED:
            obj->connected = false;
            ctrl_msg = BLE_ISTREAM_MSG_START_ADV_OB;
            ISendCtrlMsg(sObj.cmd_parser, &ctrl_msg, 1);
            break;

          case BLE_ISTREAM_MSG_UPDATE_ADV:
            updateAdvData();
            break;

          default:
            break;
        }
      }
    }
  }
}


static void ble_receive_thread_entry(ULONG thread_input)
{

  ble_stream_class_t *obj = (ble_stream_class_t *) thread_input;

  while (1)
  {
    ble_receiveMSG_wait();

    if (obj->status == true)
    {
      hci_user_evt_proc();
    }
  }

}


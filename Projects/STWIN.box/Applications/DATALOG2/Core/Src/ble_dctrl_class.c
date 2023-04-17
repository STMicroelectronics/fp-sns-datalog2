/**
  ******************************************************************************
  * @file    ble_dctrl_class.c
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

#include "ble_dctrl_class.h"
#include "ble_dctrl_class_vtbl.h"
#include "BLE_Manager.h"
#include "PnPLCompManager.h"

#include "services/sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

#ifndef MIN
#define MIN(a,b)            ((a) < (b) )? (a) : (b)
#endif

/* Private define ------------------------------------------------------------*/

static ble_dctrl_class_t sObj;

const static IStream_vtbl ble_dctrl_vtbl =
{
  ble_dctrl_vtblStream_init,
  ble_dctrl_vtblStream_enable,
  ble_dctrl_vtblStream_disable,
  ble_dctrl_vtblStream_is_enabled,
  ble_dctrl_vtblStream_deinit,
  ble_dctrl_vtblStream_start,
  ble_dctrl_vtblStream_stop,
  ble_dctrl_vtblStream_post_data,
  ble_dctrl_vtblStream_alloc_resource,
  ble_dctrl_vtblStream_set_mode,
  ble_dctrl_vtblStream_dealloc,
  ble_dctrl_vtblStream_set_parse_IF
};

/* Private member function declaration */
/***************************************/
/* For PnPL Configuration features */
static void WriteRequestPnPLikeFunction(uint8_t *received_msg, uint8_t msg_length);
static void AciGattTxPoolAvailableEvent(void);
static void ble_send_thread_entry(ULONG thread_input);
static void ble_receive_thread_entry(ULONG thread_input);
static sys_error_code_t ble_sendMSG_wait(ble_dctrl_class_t *_this);
static sys_error_code_t ble_sendMSG_resume(ble_dctrl_class_t *_this);
static sys_error_code_t ble_receiveMSG_resume(ble_dctrl_class_t *_this);
static sys_error_code_t ble_receiveMSG_wait(ble_dctrl_class_t *_this);


/* Public API definition */
/*************************/
IStream_t *ble_dctrl_class_alloc(void)
{
  IStream_t *p = (IStream_t *) &sObj;

  p->vptr = &ble_dctrl_vtbl;

  return p;
}


/* IStream virtual functions definition */
/*******************************/
sys_error_code_t ble_dctrl_vtblStream_init(IStream_t *_this, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) _this;

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
  obj->queue_memory_pointer = (UCHAR*) SysAlloc(DEFAULT_BLE_QUEUE_SIZE * sizeof(ULONG));
  if(obj->queue_memory_pointer == NULL)
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


  return res;
}

sys_error_code_t ble_dctrl_vtblStream_enable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) _this;

  HCI_TL_SPI_Reset();

  /* Initialize the BlueNRG stack and services */
  BluetoothInit();

  setConnectable();

  /* Define Custom Function for Write Request PnPLike */
  CustomWriteRequestPnPLike = &WriteRequestPnPLikeFunction;

  /* Pool available callback */
  CustomAciGattTxPoolAvailableEvent = &AciGattTxPoolAvailableEvent;

  obj->status = true;

  uint32_t ctrl_msg = BLE_DCTRL_CMD_START_ADV_OB;
  ISendCtrlMsg(sObj.cmd_parser, &ctrl_msg, 1);

  return res;
}

sys_error_code_t ble_dctrl_vtblStream_disable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

boolean_t ble_dctrl_vtblStream_is_enabled(IStream_t *_this)
{
  assert_param(_this != NULL);
  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) _this;

  return obj->connected;
}

sys_error_code_t ble_dctrl_vtblStream_deinit(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_dctrl_class_t *obj = (ble_dctrl_class_t*) _this;

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

sys_error_code_t ble_dctrl_vtblStream_start(IStream_t *_this, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t ble_dctrl_vtblStream_stop(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}


sys_error_code_t ble_dctrl_vtblStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) _this;


 if(id_stream == obj->mlc_id)
 {
   for(uint8_t i=0; i<9; i++)
   {
     obj->mlc_out[i]=buf[i];
   }

   ULONG msg = BLE_DCTRL_CMD_SEND_MLC;
   ble_dctrl_msg(&msg);
 }
 else if(id_stream == obj->adv_id)
 {
   obj->adv_buf[0] = buf[0];
   obj->adv_buf[1] = buf[1];
   obj->adv_buf[2] = buf[2];
 }


  return res;
}

sys_error_code_t ble_dctrl_vtblStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                                     const char *stream_name)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t ble_dctrl_vtblStream_set_mode(IStream_t *_this, IStreamMode_t mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) _this;
  char *response_name;

  if (mode == TRANSMIT)
  {
    ISerializeResponse(obj->cmd_parser, &response_name, &obj->serialized_cmd, &obj->serialized_cmd_size, 0);

    ULONG msg = BLE_DCTRL_CMD_SEND_PNPL;
    ble_dctrl_msg(&msg);
  }

  return res;
}

sys_error_code_t ble_dctrl_vtblStream_dealloc(IStream_t *_this, uint8_t id_stream)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}

sys_error_code_t ble_dctrl_vtblStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn)
{
  assert_param(_this != NULL);
  assert_param(ifn != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) _this;

  obj->cmd_parser = ifn;
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
    ble_receiveMSG_resume(&sObj);
  }

}


sys_error_code_t ble_dctrl_msg(ULONG *msg)
{
  assert_param(msg != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  ULONG message = *msg;

  if(tx_queue_send(&sObj.ble_app_queue, &message, TX_NO_WAIT) != TX_SUCCESS)
  {
    res = 1;
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
static void WriteRequestPnPLikeFunction(uint8_t *received_msg, uint8_t msg_length)
{
  uint8_t mode = 0x02;

  if (sObj.status)
  {
    IParseCommand(sObj.cmd_parser, (char *) received_msg, mode);
  }

}


static void AciGattTxPoolAvailableEvent(void)
{
  if (sObj.status)
  {
    ble_sendMSG_resume(&sObj);
  }
}


void BLE_SetCustomAdvertiseData(uint8_t *manuf_data)
{
  manuf_data[BLE_MANAGER_CUSTOM_FIELD1] = PnPLGetFWID();
  manuf_data[BLE_MANAGER_CUSTOM_FIELD2] = sObj.adv_buf[0]; /* Battery % */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD3] = sObj.adv_buf[1]; /* Alarm */
  manuf_data[BLE_MANAGER_CUSTOM_FIELD4] = sObj.adv_buf[2]; /* Icon */
}



static sys_error_code_t ble_sendMSG_wait(ble_dctrl_class_t *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_get(&_this->send_semaphore, TX_WAIT_FOREVER))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ble_sendMSG_resume(ble_dctrl_class_t *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_put(&_this->send_semaphore))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ble_receiveMSG_wait(ble_dctrl_class_t *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_get(&_this->receive_semaphore, TX_WAIT_FOREVER))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}

static sys_error_code_t ble_receiveMSG_resume(ble_dctrl_class_t *_this)
{
  assert_param(_this);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  //TODO: STF.Port - how to check the sem is initialized ??
  if (TX_SUCCESS != tx_semaphore_put(&_this->receive_semaphore))
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    res = SYS_UNDEFINED_ERROR_CODE;
  }

  return res;
}


static void ble_send_thread_entry(ULONG thread_input)
{
  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) thread_input;
  ULONG received_message;
  uint32_t ctrl_msg;
  tBleStatus ret;
  uint32_t j = 0, chunk, tot_len;
  uint8_t *buffer_out;
  uint32_t length_wTP;
  uint8_t mlc_status_mainpage;

  while (1)
  {
    /* Retrieve a message from the queue. */
    if(TX_SUCCESS == tx_queue_receive(&obj->ble_app_queue, &received_message, TX_WAIT_FOREVER))
    {
      if (sObj.status)
      {
        switch(received_message)
        {
          case BLE_DCTRL_CMD_SEND_PNPL:

            if ((obj->serialized_cmd_size % 19U) == 0U)
            {
              length_wTP = (obj->serialized_cmd_size / 19U) + obj->serialized_cmd_size;
            }
            else
            {
              length_wTP = (obj->serialized_cmd_size / 19U) + 1U + obj->serialized_cmd_size;
            }

            buffer_out = BLE_MallocFunction(sizeof(uint8_t) * length_wTP);

            if (buffer_out == NULL)
            {
              BLE_MANAGER_PRINTF("Error: Mem calloc error [%ld]: %d@%s\r\n", length_wTP, __LINE__, __FILE__);
              // TODO: manage error
              break;
            }
            else
            {
              tot_len = BLE_Command_TP_Encapsulate(buffer_out, (uint8_t *) obj->serialized_cmd, obj->serialized_cmd_size, 20);

              j = 0;

              /* Data are sent as notifications*/
              while (j < tot_len)
              {
                /* TODO: different MTU must be managed with MaxBLECharLen */

                chunk = MIN(20, tot_len - j);

                ret = BLE_PnPLikeUpdate(&buffer_out[j], chunk);

                if (ret == BLE_STATUS_INSUFFICIENT_RESOURCES)
                {
                  ble_sendMSG_wait(obj);
                }
                else if (ret == BLE_STATUS_SUCCESS)
                {
                  j += chunk;
                }
              }
            }

            BLE_FreeFunction(buffer_out);
            BLE_FreeFunction(obj->serialized_cmd);

            break;

          case BLE_DCTRL_CMD_SEND_MLC:

            mlc_status_mainpage = obj->mlc_out[8];

            BLE_MachineLearningCoreUpdate((uint8_t*) &obj->mlc_out, (uint8_t*) &mlc_status_mainpage);

            break;

          case BLE_DCTRL_CMD_CONNECTED:
            obj->connected = true;
            ctrl_msg = BLE_DCTRL_CMD_STOP_ADV_OB;
            ISendCtrlMsg(sObj.cmd_parser, &ctrl_msg, 1);
            break;

          case BLE_DCTRL_CMD_DISCONNECTED:
            obj->connected = false;
            ctrl_msg = BLE_DCTRL_CMD_START_ADV_OB;
            ISendCtrlMsg(sObj.cmd_parser, &ctrl_msg, 1);
            break;

          case BLE_DCTRL_CMD_UPDATE_ADV:
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

  ble_dctrl_class_t *obj = (ble_dctrl_class_t *) thread_input;

  while (1)
  {
    ble_receiveMSG_wait(obj);

    if (obj->status == true)
    {
      hci_user_evt_proc();
    }
  }

}






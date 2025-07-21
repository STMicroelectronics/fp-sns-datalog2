/**
  ******************************************************************************
  * @file    usbx_dctrl_class.c
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

#include "usbx_dctrl_class.h"
#include "usbx_dctrl_class_vtbl.h"
#include "usbx_callbacks.h"

#include "PCDDriver.h"
#include "ux_api.h"
#include "ux_device_descriptors.h"
#include "services/sysdebug.h"
#include "services/sysmem.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

/* Private define ------------------------------------------------------------*/

#define CMD_SET_REQ           (uint8_t)(0x00)
#define CMD_SIZE_GET          (uint8_t)(0x01)
#define CMD_DATA_GET          (uint8_t)(0x02)
#define CMD_SIZE_SET          (uint8_t)(0x03)
#define CMD_DATA_SET          (uint8_t)(0x04)

#define USBD_WCID_WAITING_FOR_SIZE               (uint8_t)(0x00)
#define USBD_WCID_WAITING_FOR_SIZE_REQUEST       (uint8_t)(0x01)
#define USBD_WCID_WAITING_FOR_DATA_REQUEST       (uint8_t)(0x02)
#define USBD_WCID_WAITING_FOR_DATA               (uint8_t)(0x05)

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sObj                                  sUsbXObj
#endif

static usbx_dctrl_class_t sObj;

const static IStream_vtbl usbx_dctrl_vtbl =
{
  usbx_dctrl_vtblStream_init,
  usbx_dctrl_vtblStream_enable,
  usbx_dctrl_vtblStream_disable,
  usbx_dctrl_vtblStream_is_enabled,
  usbx_dctrl_vtblStream_deinit,
  usbx_dctrl_vtblStream_start,
  usbx_dctrl_vtblStream_stop,
  usbx_dctrl_vtblStream_post_data,
  usbx_dctrl_vtblStream_alloc_resource,
  usbx_dctrl_vtblStream_set_mode,
  usbx_dctrl_vtblStream_dealloc,
  usbx_dctrl_vtblStream_set_parse_IF
};

/* Private member function declaration */
/***************************************/
void datalog_class_active(void *_this, void *class);
void datalog_class_deactive(void *_this, void *class);

/* Public API definition */
/*************************/
IStream_t *usbx_dctrl_class_alloc(const void *mx_drv_cfg)
{

  IStream_t *p = (IStream_t *) &sObj;

  p->vptr = &usbx_dctrl_vtbl;

  ((usbx_dctrl_class_t *) p)->state = USBD_WCID_WAITING_FOR_SIZE;

  ((usbx_dctrl_class_t *) p)->mx_drv_cfg = mx_drv_cfg;
  ((usbx_dctrl_class_t *) p)->m_pxDriver = PCDDriverAlloc();

  return p;
}

int8_t usbx_dctrl_class_set_ep(usbx_dctrl_class_t *_this, uint8_t id_stream, uint8_t ep)
{
  if (ux_device_class_sensor_streaming_SetTransmissionEP(_this->sensor_streaming_device, id_stream, ep) != UX_SUCCESS)
  {
    return -1;
  }
  return 0;
}

/**
  * @brief  datalog_class_control
  *         Manage the Sensor Streaming Class class requests
  * @param  _this: pointer to the context
  * @param  isHostToDevice: 1 if the direction o the request is from Host to Device, 0 otherwise
  * @param  cmd: Command code
  * @param  wValue: not used
  * @param  wIndex: not used
  * @param  pBuf: Data Buffer, input for Host-To-Device and output for Device-To-Host
  * @param  length: Buffer size (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
int8_t datalog_class_control(void *_this, uint8_t isHostToDevice, uint8_t cmd, uint16_t wValue, uint16_t wIndex,
                             uint8_t *pbuf, uint16_t length)
{
  assert_param(_this != NULL);

  static uint32_t USB_packet_size = 0;
  static uint32_t counter = 0;
  static char *serialized_cmd = 0;
  static char *p = 0;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  if (isHostToDevice)
  {
    switch (obj->state)
    {
      case USBD_WCID_WAITING_FOR_SIZE :

        if (cmd != CMD_SIZE_SET) /* discard if it isn't a size set command */
        {
          return -1;
        }

        SysFree(serialized_cmd);

        if (length == 2)
        {
          USB_packet_size = *(uint16_t *) pbuf;
        }
        else
        {
          USB_packet_size = *(uint32_t *) pbuf;
        }

        /* adding one character to include '\0'*/
        USB_packet_size++;

        serialized_cmd = SysAlloc(USB_packet_size); /* Allocate the buffer to receive next command */
        if (serialized_cmd == NULL)
        {
          /*Mem alloc error*/
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE);
          return -1;
        }
        else
        {
          /*Mem alloc ok*/
        }

        p = serialized_cmd;
        obj->state = USBD_WCID_WAITING_FOR_DATA;
        counter = USB_packet_size;

        break;
      case USBD_WCID_WAITING_FOR_DATA :
      {
        uint32_t i = 0;

        if (cmd != CMD_DATA_SET)
        {
          return -1;  /* error */
        }

        for (i = 0; i < length; i++)
        {
          *p++ = pbuf[i];
          counter--;
        }

        if (counter - 1 == 0) /* The complete message has been received */
        {
          *p = '\0';
          IParseCommand(obj->cmd_parser, serialized_cmd, sObj.comm_interface_id);
        }
        break;
      }
    }
  }
  else /* Device to host */
  {
    switch (obj->state)
    {
      case USBD_WCID_WAITING_FOR_SIZE_REQUEST : /* Host needs size */
      {
        if (cmd != CMD_SIZE_GET)
        {
          return -1;  /* error*/
        }

        SysFree(serialized_cmd);
        /** not used here, but could be the file name **/
        char *response_name;

        ISerializeResponse(obj->cmd_parser, &response_name, &serialized_cmd, &USB_packet_size, 0);

        *(uint32_t *) pbuf = USB_packet_size;
        p = serialized_cmd;
        obj->state = USBD_WCID_WAITING_FOR_DATA_REQUEST;
        counter = USB_packet_size;

        break;
      }
      case USBD_WCID_WAITING_FOR_DATA_REQUEST :
      {
        uint32_t i = 0;

        if (cmd != CMD_DATA_GET)
        {
          return -1;  /* error*/
        }

        for (i = 0; i < length; i++)
        {
          pbuf[i] = *p++;
          counter--;
        }

        if (counter == 0) /* The complete message has been received */
        {
          SysFree(serialized_cmd);
          serialized_cmd = NULL;
          obj->state = USBD_WCID_WAITING_FOR_SIZE;
        }
        break;
      }
    }
  }

  return (0);
}

/* IStream virtual functions definition */
/*******************************/
sys_error_code_t usbx_dctrl_vtblStream_init(IStream_t *_this, uint8_t comm_interface_id, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  /* Device framework FS length*/
  ULONG device_framework_fs_length;
  /* Device String framework length*/
  ULONG string_framework_length;
  /* Device language id framework length*/
  ULONG language_id_framework_length;
  /* Device Framework Full Speed */
  UCHAR *device_framework_full_speed;
  /* String Framework*/
  UCHAR *string_framework;
  /* Language_Id_Framework*/
  UCHAR *language_id_framework;
  /* Status Tx */
  UINT tx_status = UX_SUCCESS;

  UCHAR *pointer = obj->memory_pointer;
  obj->is_class_initialized_by_the_host = false;

  obj->comm_interface_id = comm_interface_id;

  if (tx_byte_pool_create(&obj->ux_device_app_byte_pool, "Ux App memory pool", obj->ux_device_byte_pool_buffer,
                          UX_DEVICE_APP_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Allocate USBX_MEMORY_SIZE. */
  tx_status = tx_byte_allocate(&obj->ux_device_app_byte_pool, (VOID **) &pointer, USBX_MEMORY_SIZE, TX_NO_WAIT);

  /* Check memory allocation */
  if (UX_SUCCESS != tx_status)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Initialize USBX Memory */
  ux_system_initialize(pointer, USBX_MEMORY_SIZE, UX_NULL, 0);

  /* Get_Device_Framework_Full_Speed and get the length */
  device_framework_full_speed = USBD_Get_Device_Framework_Speed(&device_framework_fs_length);

  /* Get_String_Framework and get the length */
  string_framework = USBD_Get_StringDescription_Framework(&string_framework_length);

  if (string_framework == NULL)
  {
    res = SYS_TASK_INVALID_PARAM_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Get_Language_Id_Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&language_id_framework_length);

#if (USBX_MAJOR_VERSION == 6) && (USBX_MINOR_VERSION >= 2)
  tx_status = ux_device_stack_initialize(NULL, 0, device_framework_full_speed, device_framework_fs_length, string_framework, string_framework_length,
                                         language_id_framework, language_id_framework_length, USBD_ChangeFunction);
#else
  tx_status = ux_device_stack_initialize(NULL, 0, device_framework_full_speed, device_framework_fs_length, string_framework, string_framework_length,
                                         language_id_framework, language_id_framework_length, UX_NULL);
#endif
  /* Check the device stack class status */
  if (tx_status != UX_SUCCESS)
  {
    res = SYS_INIT_TASK_FAILURE_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  IDrvInit(obj->m_pxDriver, (void *) obj->mx_drv_cfg);
  PCDDrvSetFIFO((PCDDriver_t *) obj->m_pxDriver, PCD_FIFO_SIZE, PCD_FIFO_RX_EP, PCD_FIFO_CTRL_EP, SS_N_IN_ENDPOINTS);
  PCDDrvSetExtDCD((PCDDriver_t *) obj->m_pxDriver, _ux_dcd_stm32_initialize);

  UX_SLAVE_CLASS_SENSOR_STREAMING_PARAMETER *ss_class_parameter = _ux_utility_memory_allocate(UX_NO_ALIGN, UX_REGULAR_MEMORY,
                                                                  sizeof(UX_SLAVE_CLASS_SENSOR_STREAMING_PARAMETER));

  /** Init Sensor Streaming Class **/
  ss_class_parameter->ux_slave_class_sensor_streaming_instance_activate = datalog_class_active;
  ss_class_parameter->ux_slave_class_sensor_streaming_instance_deactivate = datalog_class_deactive;
  ss_class_parameter->ux_slave_class_sensor_streaming_instance_control = datalog_class_control;
  ss_class_parameter->app_obj_ctrl = (void *) obj;

  /* registers a slave class to the slave stack. */
  tx_status = ux_device_class_sensor_streaming_stack_class_register(ss_class_parameter);

  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_enable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  res = IDrvStart(obj->m_pxDriver);

  if (res == SYS_NO_ERROR_CODE)
  {
    obj->ux_opened = UX_SUCCESS;
  }
  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_disable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  res = IDrvStop(obj->m_pxDriver);

  if (res == SYS_NO_ERROR_CODE)
  {
    obj->ux_opened = UX_NO_DEVICE_CONNECTED;
  }

  return res;
}

boolean_t usbx_dctrl_vtblStream_is_enabled(IStream_t *_this)
{
  assert_param(_this != NULL);
  boolean_t res;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  if (UX_SUCCESS != obj->ux_opened)
  {
    res = FALSE;
  }
  else
  {
    res = TRUE;
  }
  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_deinit(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  ux_device_stack_uninitialize();
  ux_system_uninitialize();

  // ToDo: should this be "tx_byte_release()" ??? --> allocation is done with "tx_byte_allocate()"
  _ux_utility_memory_free(obj->memory_pointer);

  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_start(IStream_t *_this, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  if (ux_device_class_sensor_streaming_StartStreaming(obj->sensor_streaming_device) != UX_SUCCESS)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_stop(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  if (ux_device_class_sensor_streaming_StopStreaming(obj->sensor_streaming_device) != UX_SUCCESS)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  if (ux_device_class_sensor_streaming_FillTxDataBuffer(obj->sensor_streaming_device, id_stream, buf, size) != UX_SUCCESS)
  {
    res = SYS_BASE_ERROR_CODE;
  }

  return res;
}


sys_error_code_t usbx_dctrl_vtblStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                                      const char *stream_name)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;
  assert_param(obj->sensor_streaming_device != NULL);

  obj->TxBuffer[id_stream] = NULL;
  obj->TxBuffer[id_stream] = (uint8_t *) SysAlloc((size + SS_HEADER_SIZE) * SS_CH_QUEUE_ITEMS);  /* Double buffer: data + id_stream + pkt counter */

  if (obj->TxBuffer[id_stream] != NULL)
  {
    ux_device_class_sensor_streaming_SetTxDataBuffer(obj->sensor_streaming_device, id_stream, obj->TxBuffer[id_stream],
                                                     size + SS_HEADER_SIZE, SS_CH_QUEUE_ITEMS);
    //ux_device_class_sensor_streaming_CleanTxDataBuffer(obj->sensor_streaming_device, id_stream);
  }
  else
  {
    return SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_set_mode(IStream_t *_this, IStreamMode_t mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  if (mode == RECEIVE)
  {
    obj->state = USBD_WCID_WAITING_FOR_SIZE;
  }
  else if (mode == TRANSMIT)
  {
    obj->state = USBD_WCID_WAITING_FOR_SIZE_REQUEST;
  }
  else
  {
    return SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_dealloc(IStream_t *_this, uint8_t id_stream)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  if (obj->TxBuffer[id_stream] != NULL)
  {
    ux_device_class_sensor_streaming_CleanTxDataBuffer(obj->sensor_streaming_device, id_stream);
    SysFree(obj->TxBuffer[id_stream]);
    obj->TxBuffer[id_stream] = NULL;
  }

  return res;
}

sys_error_code_t usbx_dctrl_vtblStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn)
{
  assert_param(_this != NULL);
  assert_param(ifn != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  obj->cmd_parser = ifn;
  return res;
}

/* Private function definition */
/*******************************/
void datalog_class_active(void *_this, void *class)
{
  assert_param(_this != NULL);
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  /* Save the SENSOR_STREAMING instance.  */
  obj->sensor_streaming_device = (UX_SLAVE_CLASS_SENSOR_STREAMING *) class;
  obj->is_class_initialized_by_the_host = true;
}

void datalog_class_deactive(void *_this, void *class)
{
  assert_param(_this != NULL);
  usbx_dctrl_class_t *obj = (usbx_dctrl_class_t *) _this;

  /* Reset the SENSOR_STREAMING instance.  */
  obj->sensor_streaming_device = UX_NULL;
  obj->is_class_initialized_by_the_host = false;
}


/**
 ******************************************************************************
 * @file    filex_dctrl_class.c
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

#include "filex_dctrl_class.h"
#include "filex_dctrl_class_vtbl.h"
#include "stdio.h"
#include "rtc.h"

#include "services/sysdebug.h"
#include "services/sysmem.h"
#include "services/SUcfProtocol.h"

#include "PnPLDef.h"
#include "PnPLCompManager.h"

#include "App_model.h"

#include "automode.h"

#define SYS_DEBUGF(level, message)        	SYS_DEBUGF3(SYS_DBG_SDC, level, message)
#define MAX_INSTANCES 1

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sObj                              	sFileXObj
#endif

/* The other files are located in fx_file[] from SENSOR_DAT_FILES position */
#define OTHER_FILES_ID                    (SENSOR_DAT_FILES - 1)

#define SKIP_LIST_SIZE           	(1)

/* Private variable ------------------------------------------------------------*/
//char *components_names[SKIP_LIST_SIZE] =
//{
//    "deviceinfo",
//    "fwinfo",
//    "iis2dlpc_acc",
//    "iis2iclx_acc",
//    "iis2mdc_mag",
//    "iis3dwb_acc",
//    "ilps22qs_press",
//    "imp23absu_mic",
//    "imp34dt05_mic",
//    "ism330dhcx_acc",
//    "ism330dhcx_gyro",
//    "ism330dhcx_mlc",
//    "log_controller",
//    "stts22h_temp",
//    "tags_info" };

char *skip_list[SKIP_LIST_SIZE] =
{
    "acquisition_info" };

//filex_dctrl_class_t sObj[MAX_INSTANCES] = {0};
static filex_dctrl_class_t sObj;

const static IStream_vtbl filex_dctrl_vtbl =
{
    filex_dctrl_vtblStream_init,
    filex_dctrl_vtblStream_enable,
    filex_dctrl_vtblStream_disable,
    filex_dctrl_vtblStream_is_enabled,
    filex_dctrl_vtblStream_deinit,
    filex_dctrl_vtblStream_start,
    filex_dctrl_vtblStream_stop,
    filex_dctrl_vtblStream_post_data,
    filex_dctrl_vtblStream_alloc_resource,
    filex_dctrl_vtblStream_set_mode,
    filex_dctrl_vtblStream_dealloc,
    filex_dctrl_vtblStream_set_parse_IF };

/* Private member function declaration */
/***************************************/
void fx_thread_entry(unsigned long thread_input);
void filex_data_ready(filex_dctrl_class_t *_this, uint32_t data_ready_mask);
void filex_data_flush(filex_dctrl_class_t *_this, uint8_t sId);
sys_error_code_t filex_write_config_file(filex_dctrl_class_t *_this, char *file, uint32_t size);
sys_error_code_t filex_write_acquisition_info(filex_dctrl_class_t *_this, char *acquisition_info, uint32_t size);

static sys_error_code_t filex_check_root_folder(filex_dctrl_class_t *_this);
static const char* get_file_ext(const char *filename);

/* Public API definition */
/*************************/
IStream_t* filex_dctrl_class_alloc(void)
{
  IStream_t *p = (IStream_t*) &sObj;
  p->vptr = &filex_dctrl_vtbl;
  return p;
}

/* IStream virtual functions definition */
/*******************************/
sys_error_code_t filex_dctrl_vtblStream_init(IStream_t *_this, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;

  /* Allocate FILEX_CFG_STACK_DEPTH. */
  obj->thread_memory_pointer = (UCHAR*) SysAlloc(FILEX_CFG_STACK_DEPTH);
  if(obj->thread_memory_pointer == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Create the main thread.  */
  tx_thread_create(&obj->fx_app_thread, "FileX App Thread", fx_thread_entry, (uint32_t )obj, obj->thread_memory_pointer, FILEX_CFG_STACK_DEPTH,
                   FILEX_SEND_CFG_PRIORITY, FILEX_CFG_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);

#ifdef ENABLE_THREADX_DBG_PIN
  obj->fx_app_thread.pxTaskTag = FILEX_TASK_CFG_TAG;
#endif

  /* Allocate the message queue. */
  obj->queue_memory_pointer = (UCHAR*) SysAlloc(DEFAULT_QUEUE_SIZE * sizeof(uint32_t));
  if(obj->queue_memory_pointer == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Create the message queue */
  tx_queue_create(&obj->fx_app_queue, "FileX App queue", TX_1_ULONG, obj->queue_memory_pointer, DEFAULT_QUEUE_SIZE * sizeof(uint32_t));
  /* Initialize FILEX Memory */
  fx_system_initialize();

  obj->dir_n = 0;
  obj->fx_opened = FX_INVALID_STATE;

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_enable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;
  bool sd_detected = false;

  char return_entry_name[50];
  uint16_t tmp = -1;

  /* Open the SD disk driver */
  obj->fx_opened = fx_media_open(&obj->sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, &obj->media_memory, sizeof(obj->media_memory));

  if(TX_SUCCESS == obj->fx_opened) /* SD card detected */
  {
    /* Scan the root folder... */
    while(fx_directory_next_entry_find(&obj->sdio_disk, return_entry_name) != FX_NO_MORE_ENTRIES)
    {
      int16_t nLenght = strlen(return_entry_name);
      for(int i = nLenght - 1; i >= 0; --i)
      {
        if(return_entry_name[i] == '.')
        {
          nLenght = i;
          break;
        }
      }
      /* ...and check if there is already a folder named STWINBOX_xxxxx */
      if(strncmp(return_entry_name, LOG_DIR_PREFIX, 8) == 0)
      {
        tmp = strtol(&return_entry_name[sizeof(LOG_DIR_PREFIX)], NULL, 10);
        if(tmp > obj->dir_n)
        {
          obj->dir_n = tmp;
        }
      }
    }
  }
  else /* No SD card inserted */
  {
    res = SYS_BASE_ERROR_CODE;
  }

  log_controller_get_sd_mounted(&sd_detected);

  if(sd_detected)
  {
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
  }

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_disable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;

  /* Close the SD disk driver.  */
  if(TX_SUCCESS != fx_media_close(&obj->sdio_disk))
  {
    res = SYS_BASE_ERROR_CODE;
  }
  else
  {
    res = SYS_NO_ERROR_CODE;
  }

  obj->fx_opened = FX_INVALID_STATE;
  return res;
}

bool filex_dctrl_vtblStream_is_enabled(IStream_t *_this)
{
  assert_param(_this != NULL);
  bool res;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;

  if(TX_SUCCESS != obj->fx_opened)
  {
    res = false;
  }
  else
  {
    res = true;
  }

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_deinit(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;

  tx_queue_delete(&obj->fx_app_queue);
  tx_thread_delete(&obj->fx_app_thread);
  SysFree(obj->thread_memory_pointer);
  SysFree(obj->queue_memory_pointer);

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_start(IStream_t *_this, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;
  uint32_t ret = FX_SUCCESS;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  /*Check directories on SD and setup the new acquisition folder*/
  fx_directory_default_set(&obj->sdio_disk, FX_NULL);

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  char *p_name = obj->dir_name;
  if(sDate.Year != 0)
  {
    /* split in two to avoid warning */
    int32_t size;
    size = sprintf(p_name, "%04d%02d%02d_", sDate.Year + 2000, sDate.Month, sDate.Date);
    sprintf(&p_name[size], "%02d_%02d_%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
  }
  else
  {
    obj->dir_n++;
    sprintf(obj->dir_name, "%s%05d", LOG_DIR_PREFIX, obj->dir_n);
  }

  ret = fx_directory_create(&obj->sdio_disk, obj->dir_name);
  if(TX_SUCCESS != ret)
  {
    res = SYS_UNDEFINED_ERROR_CODE;
    return res;
  }

  fx_file_date_time_set(&obj->sdio_disk, obj->dir_name, ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                        (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);

  fx_directory_default_set(&obj->sdio_disk, obj->dir_name);

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_stop(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;
  uint8_t ii;
  uint32_t attributes_ptr;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  for(ii = 0; ii < SENSOR_DAT_FILES; ii++) /*Close all files*/
  {
    /*if file exists*/
    if(TX_SUCCESS == fx_file_attributes_read(&obj->sdio_disk, obj->data_name[ii], (unsigned int*)&attributes_ptr))
    {
      if(attributes_ptr == FX_ARCHIVE)
      {
        /*flush the data buffer*/
        filex_data_flush(obj, ii);
        fx_media_flush(&obj->sdio_disk);
        /*close the file*/
        if(TX_SUCCESS != fx_file_close(&obj->file_dat[ii]))
        {
          return SYS_BASE_ERROR_CODE;
        }

        fx_file_date_time_set(&obj->sdio_disk, obj->data_name[ii], ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                              (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);
      }
    }
  }

  /* Save acquisition_info.json and device_config.json for the current acquisition*/
  unsigned long msg = FILEX_DCTRL_CMD_SAVE_STATUS;
  filex_dctrl_msg(obj, &msg);

  /* Unmount SD card when the acquisition is terminated */
  msg = FILEX_DCTRL_CMD_CLOSE;
  filex_dctrl_msg(obj, &msg);

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_post_data(IStream_t *_this, uint8_t id_stream, uint8_t *buf, uint32_t size)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;

  uint8_t *p_dst;
  uint32_t dst_idx, src_idx = 0;
  uint32_t dst_size, half_size;
  uint8_t mode;

  half_size = obj->sd_write_buffer_size[id_stream];

  if(half_size != 0)
  {
    /* double buffer */
    dst_size = half_size * 2;

    p_dst = obj->sd_write_buffer[id_stream];
    dst_idx = obj->sd_write_buffer_idx[id_stream];

    /* if the half or end buffer is not reached after the copy, use memcpy */
    uint32_t size_after_copy = dst_idx + size;

    if(size_after_copy < half_size) /* data to be copied won't exceed the first half of the dest buffer */
    {
      mode = 1; /* use memcpy*/
    }
    else if(size_after_copy < dst_size) /* data to be copied won't exceed the end of the buffer*/
    {
      if(dst_idx < half_size) /* data to be copied exceeds the first half of the dest buffer*/
      {
        mode = 0; /* bytewise copy */
      }
      else /* data to be copied won't exceed the end of the buffer*/
      {
        mode = 1; /* use memcpy*/
      }
    }
    else /* data to be copied exceeds the end of the buffer*/
    {
      mode = 0; /* bytewise copy */
    }

    if(mode == 0) /* bytewise copy */
    {
      /* byte per byte copy to SD buffer, automatic wrap */
      while(src_idx < size)
      {
        p_dst[dst_idx++] = buf[src_idx++];

        if(dst_idx >= dst_size)
        {
          dst_idx = 0;
          obj->byte_counter[id_stream] += (dst_size / 2) - 4;
          *((uint32_t*) &p_dst[dst_idx]) = obj->byte_counter[id_stream];
          dst_idx += 4;
        }
        else if(dst_idx == half_size)
        {
          obj->byte_counter[id_stream] += (dst_size / 2) - 4;
          *((uint32_t*) &p_dst[dst_idx]) = obj->byte_counter[id_stream];
          dst_idx += 4;
        }
      }
    }
    else
    {
      memcpy(&p_dst[dst_idx], &buf[src_idx], size);
      dst_idx += size;
    }

    if(obj->sd_write_buffer_idx[id_stream] < (dst_size / 2) && dst_idx >= (dst_size / 2)) /* first half full */
    {
      /* unlock write task */
      unsigned long msg = id_stream | FILEX_DCTRL_DATA_READY_MASK | FILEX_DCTRL_DATA_FIRST_HALF_MASK;
      if(filex_dctrl_msg(obj, &msg) != SYS_NO_ERROR_CODE)
      {
        res = SYS_TASK_QUEUE_FULL_ERROR_CODE;
      }
    }
    else if(dst_idx < obj->sd_write_buffer_idx[id_stream]) /* second half full */
    {
      unsigned long msg = id_stream | FILEX_DCTRL_DATA_READY_MASK | FILEX_DCTRL_DATA_SECOND_HALF_MASK;
      if(filex_dctrl_msg(obj, &msg) != SYS_NO_ERROR_CODE)
      {
        res = SYS_TASK_QUEUE_FULL_ERROR_CODE;
      }
    }

    obj->sd_write_buffer_idx[id_stream] = dst_idx;
  }

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size, const char *stream_name)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  obj->sd_write_buffer[id_stream] = NULL;
  obj->sd_write_buffer[id_stream] = (uint8_t*) SysAlloc(size * 2);

  if(obj->sd_write_buffer[id_stream] == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  obj->sd_write_buffer_size[id_stream] = size;

  obj->sd_write_buffer_idx[id_stream] = 4;
  obj->byte_counter[id_stream] = 0;

  sprintf(obj->data_name[id_stream], "%s%s", stream_name, ".dat");
  if(TX_SUCCESS != fx_file_create(&obj->sdio_disk, obj->data_name[id_stream]))
  {
    obj->fx_opened = FX_IO_ERROR;
    return SYS_BASE_ERROR_CODE;
  }

  if(TX_SUCCESS != fx_file_open(&obj->sdio_disk, &obj->file_dat[id_stream], obj->data_name[id_stream], FX_OPEN_FOR_WRITE))
  {
    obj->fx_opened = FX_IO_ERROR;
    return SYS_BASE_ERROR_CODE;
  }

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_set_mode(IStream_t *_this, IStreamMode_t mode)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;
  char *response_name;
  static char *serialized_cmd = 0;
  static uint32_t size = 0;

  if(mode == TRANSMIT)
  {
    ISerializeResponse(obj->cmd_parser, &response_name, &serialized_cmd, &size, 1);

    if(strncmp(response_name, "all", 3) == 0)
    {
      /*scrivi serialize_cmd nel file device_status.json*/
      filex_write_config_file(obj, serialized_cmd, size);
    }
    else if(strncmp(response_name, "acquisition_info", 16) == 0)
    {
      /*scrivi serialize_cmd nel file acquisition_info.json*/
      filex_write_acquisition_info(obj, serialized_cmd, size);
    }
    SysFree(serialized_cmd);
  }

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_dealloc(IStream_t *_this, uint8_t id_stream)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;

  if(obj->sd_write_buffer[id_stream] != NULL)
  {
    SysFree(obj->sd_write_buffer[id_stream]);
    obj->sd_write_buffer[id_stream] = NULL;
  }

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_set_parse_IF(IStream_t *_this, ICommandParse_t *ifn)
{
  assert_param(_this != NULL);
  assert_param(ifn != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t*) _this;

  obj->cmd_parser = ifn;
  return res;
}

sys_error_code_t filex_dctrl_msg(filex_dctrl_class_t *_this, unsigned long *msg)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  uint32_t message = *msg;

  // if(tx_queue_send(&_this->fx_app_queue, &message, TX_WAIT_FOREVER) != TX_SUCCESS)
  if(tx_queue_send(&_this->fx_app_queue, &message, TX_NO_WAIT) != TX_SUCCESS)
  {
    while(1);
  }

  return res;
}

sys_error_code_t filex_dctrl_write_ucf(filex_dctrl_class_t *_this, uint32_t compressed_ucf_size, const char *p_compressed_ucf)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t fx_create = 0;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  uint32_t ucf_file_size;
  char *p_ucf;
  uint32_t actual_size;

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

//  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, (ucf_data));
//  fflush(stdout);

  ucf_file_size = UCFP_UcfSize(compressed_ucf_size);
  p_ucf = SysAlloc(ucf_file_size);

  if(p_ucf == NULL)
  {
    return SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  /* Convert the compressed UCF into the Standard format  */
  if (UCFP_GetUcf(p_compressed_ucf, compressed_ucf_size, p_ucf, ucf_file_size, &actual_size) != SYS_NO_ERROR_CODE)
  {
    return SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  fx_create = fx_file_create(&_this->sdio_disk, TMP_UCF_FILE_NAME);
  if(fx_create == FX_ALREADY_CREATED)
  {
    fx_file_delete(&_this->sdio_disk, TMP_UCF_FILE_NAME);
    fx_create = fx_file_create(&_this->sdio_disk, TMP_UCF_FILE_NAME);
  }

  if(fx_create != TX_SUCCESS)
  {
    _this->fx_opened = FX_IO_ERROR;
    res = SYS_BASE_ERROR_CODE;
  }
  else
  {
    fx_file_open(&_this->sdio_disk, &_this->file_tmp, TMP_UCF_FILE_NAME, FX_OPEN_FOR_WRITE);
    fx_file_write(&_this->file_tmp, (uint8_t*) p_ucf, actual_size);
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, TMP_UCF_FILE_NAME, ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours, (uint32_t) sTime.Minutes,
                          (uint32_t) sTime.Seconds);
  }

  SysFree(p_ucf);

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDCardTask: %s written to root folder.\r\n", TMP_UCF_FILE_NAME));

  return res;
}

///* Private function definition */
///*******************************/
sys_error_code_t filex_write_config_file(filex_dctrl_class_t *_this, char *file, uint32_t size)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t fx_ret = FX_SUCCESS;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  fx_ret = fx_file_create(&_this->sdio_disk, DEVICE_JSON_FILE_NAME);

  if(fx_ret == FX_ALREADY_CREATED)
  {
    fx_file_delete(&_this->sdio_disk, DEVICE_JSON_FILE_NAME);
    fx_ret = fx_file_create(&_this->sdio_disk, DEVICE_JSON_FILE_NAME);
    if(fx_ret != FX_SUCCESS)
    {
      _this->fx_opened = FX_IO_ERROR;
      return SYS_BASE_ERROR_CODE;
    }
    fx_file_open(&_this->sdio_disk, &_this->file_tmp, DEVICE_JSON_FILE_NAME, FX_OPEN_FOR_WRITE);
    fx_file_write(&_this->file_tmp, file, size);
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, DEVICE_JSON_FILE_NAME, ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                          (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);
  }
  else if(fx_ret == FX_SUCCESS)
  {
    fx_file_open(&_this->sdio_disk, &_this->file_tmp, DEVICE_JSON_FILE_NAME, FX_OPEN_FOR_WRITE);
    fx_file_write(&_this->file_tmp, file, size);
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, DEVICE_JSON_FILE_NAME, ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                          (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);
  }
  else
  {
    _this->fx_opened = FX_IO_ERROR;
    return SYS_BASE_ERROR_CODE;
  }

  /* if ucf exists in root, copy tmp_ucf file from root into acquisition folder*/
  char root_ucf[32];
  sprintf(root_ucf, "%s%s", "../", TMP_UCF_FILE_NAME);

  fx_ret = fx_file_open(&_this->sdio_disk, &_this->file_tmp, root_ucf, FX_OPEN_FOR_READ);

  if(fx_ret == FX_SUCCESS)
  {
    unsigned long actual_bytes = 0;
    uint32_t ucf_file_size = (uint32_t)_this->file_tmp.fx_file_current_file_size;
    char *ucf_file_buffer = SysAlloc(ucf_file_size);
    fx_file_read(&_this->file_tmp, (uint8_t*) ucf_file_buffer, ucf_file_size, &actual_bytes);
    fx_file_close(&_this->file_tmp);

    fx_ret = fx_file_create(&_this->sdio_disk, MLC_UCF_FILE_NAME);
    if(fx_ret == FX_ALREADY_CREATED)
    {
      fx_file_delete(&_this->sdio_disk, MLC_UCF_FILE_NAME);
      fx_ret = fx_file_create(&_this->sdio_disk, MLC_UCF_FILE_NAME);
    }
    if(fx_ret != TX_SUCCESS)
    {
      _this->fx_opened = FX_IO_ERROR;
      return SYS_BASE_ERROR_CODE;
    }

    fx_file_open(&_this->sdio_disk, &_this->file_tmp, MLC_UCF_FILE_NAME, FX_OPEN_FOR_WRITE);
    fx_file_write(&_this->file_tmp, (uint8_t*) ucf_file_buffer, ucf_file_size);
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, MLC_UCF_FILE_NAME, ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                          (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);

    SysFree(ucf_file_buffer);
  }

  return res;
}

sys_error_code_t filex_write_acquisition_info(filex_dctrl_class_t *_this, char *acquisition_info, uint32_t size)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  if(TX_SUCCESS != fx_file_create(&_this->sdio_disk, ACQUISITION_INFO_FILE_NAME))
  {
    _this->fx_opened = FX_IO_ERROR;
    return SYS_BASE_ERROR_CODE;
  }

  fx_file_open(&_this->sdio_disk, &_this->file_tmp, ACQUISITION_INFO_FILE_NAME, FX_OPEN_FOR_WRITE);
  fx_file_write(&_this->file_tmp, acquisition_info, size);
  fx_media_flush(&_this->sdio_disk);
  fx_file_close(&_this->file_tmp);

  fx_file_date_time_set(&_this->sdio_disk, ACQUISITION_INFO_FILE_NAME, ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                        (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);

  return res;
}

void fx_thread_entry(unsigned long thread_input)
{
  uint32_t received_message;
  filex_dctrl_class_t *p_obj = (filex_dctrl_class_t*) thread_input;

  while(1)
  {
    /* Wait for a message on the queue. */
    if(TX_SUCCESS == tx_queue_receive(&p_obj->fx_app_queue, &received_message, TX_WAIT_FOREVER))
    {
      /* Check SD card status  */
      if(TX_SUCCESS == p_obj->fx_opened)
      {
        switch(received_message)
        {
          case FILEX_DCTRL_CMD_INIT: /* Check root folder content */
            filex_check_root_folder(p_obj);
            break;

          case FILEX_DCTRL_CMD_SAVE_STATUS:
          {
            char *componentJSON = NULL;
            uint32_t componentSize = 0;
            char *deviceConfig = NULL;
            uint32_t deviceConfigSize = 0;

            PnPLGetFilteredDeviceStatusJSON(&deviceConfig, &deviceConfigSize, skip_list, SKIP_LIST_SIZE, 1);
            filex_write_config_file(p_obj, deviceConfig, deviceConfigSize);
            SysFree(deviceConfig);

            PnPLGetComponentValue("acquisition_info", &componentJSON, &componentSize, 1);
            filex_write_acquisition_info(p_obj, componentJSON, componentSize);
            SysFree(componentJSON);

            break;
          }

          case FILEX_DCTRL_CMD_SET_DEFAULT_STATUS:
          {
            char *deviceConfig = NULL;
            uint32_t deviceConfigSize = 0;

            /*Save device_config in the root folder*/
            fx_directory_default_set(&p_obj->sdio_disk, FX_NULL);

            char uuid[37];
            PnPLGenerateAcquisitionUUID(uuid);
            PnPLGetFilteredDeviceStatusJSON(&deviceConfig, &deviceConfigSize, skip_list, SKIP_LIST_SIZE, 1);
            filex_write_config_file(p_obj, deviceConfig, deviceConfigSize);
            SysFree(deviceConfig);
            break;
          }

          case FILEX_DCTRL_CMD_CLOSE:
            filex_dctrl_vtblStream_disable((IStream_t*) p_obj);
            break;

          default: /*data ready: parse the mask*/
            filex_data_ready(p_obj, received_message);
            break;
        }
      }
    }
  }
}

void filex_data_ready(filex_dctrl_class_t *_this, uint32_t data_ready_mask)
{
  uint32_t sID = data_ready_mask & FILEX_DCTRL_DATA_SENSOR_ID_MASK;
  if(data_ready_mask & FILEX_DCTRL_DATA_FIRST_HALF_MASK) /* Data available on first half of the circular buffer */
  {
#ifdef ENABLE_THREADX_DBG_PIN
    BSP_DEBUG_PIN_On(CON34_PIN_22);
#endif
    if(FX_SUCCESS != fx_file_write(&_this->file_dat[sID], _this->sd_write_buffer[sID], _this->sd_write_buffer_size[sID]))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    }
#ifdef ENABLE_THREADX_DBG_PIN
    BSP_DEBUG_PIN_Off(CON34_PIN_22);
#endif
  }
  else /* Data available on second half of the circular buffer */
  {
#ifdef ENABLE_THREADX_DBG_PIN
    BSP_DEBUG_PIN_On(CON34_PIN_22);
#endif
    if(FX_SUCCESS != fx_file_write(&_this->file_dat[sID], _this->sd_write_buffer[sID] + _this->sd_write_buffer_size[sID], _this->sd_write_buffer_size[sID]))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    }
#ifdef ENABLE_THREADX_DBG_PIN
    BSP_DEBUG_PIN_Off(CON34_PIN_22);
#endif
  }
}

void filex_data_flush(filex_dctrl_class_t *_this, uint8_t sId)
{
  uint32_t buf_size = _this->sd_write_buffer_size[sId];
  if(_this->sd_write_buffer_idx[sId] > 0 && _this->sd_write_buffer_idx[sId] < (buf_size - 1))
  {
    /* flush from the beginning */
    if(FX_SUCCESS != fx_file_write(&_this->file_dat[sId], _this->sd_write_buffer[sId], _this->sd_write_buffer_idx[sId] + 1))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    }
  }
  else if(_this->sd_write_buffer_idx[sId] > (buf_size - 1) && _this->sd_write_buffer_idx[sId] < (2 * buf_size - 1))
  {
    /* flush from half buffer */
    if(FX_SUCCESS != fx_file_write(&_this->file_dat[sId], _this->sd_write_buffer[sId] + buf_size, _this->sd_write_buffer_idx[sId] + 1 - buf_size))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    }
  }
}

sys_error_code_t filex_dctrl_set_IIsm330dhcx_Mlc_IF(IStream_t *_this, IIsm330dhcx_Mlc_t *ifn)
{
  assert_param(_this != NULL);
  assert_param(ifn != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *p_obj = (filex_dctrl_class_t*) _this;

  p_obj->ism330dhcx_mlc = ifn;
  return res;
}

sys_error_code_t filex_dctrl_set_IIsm330is_Ispu_IF(IStream_t *_this, IIsm330is_Ispu_t *ifn)
{
  assert_param(_this != NULL);
  assert_param(ifn != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *p_obj = (filex_dctrl_class_t*) _this;

  p_obj->ism330is_ispu = ifn;
  return res;
}

static sys_error_code_t filex_check_root_folder(filex_dctrl_class_t *_this)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  char entry_name[50];
  const char *p_file_ext = NULL;
  char *p_json = NULL;
  char *p_ucf = NULL;
  char *p_cmd_json = NULL;
  uint32_t actual_ucf_size;
  char *p_compressed_ucf = 0;
  uint32_t status = FX_SUCCESS;
  unsigned long bytes_read;
  bool ucf_found = false;

  fx_directory_first_entry_find(&_this->sdio_disk, entry_name);
  while(fx_directory_next_entry_find(&_this->sdio_disk, entry_name) != FX_NO_MORE_ENTRIES)
  {
    /* Find and check file extension */
    p_file_ext = get_file_ext(entry_name);
    if(p_file_ext != NULL)
    {
      if(strncmp(p_file_ext, "json", 4) == 0) /* 'json' extension */
      {
        /* Check file name */
        if(strncmp(entry_name, DEVICE_JSON_FILE_NAME, sizeof(DEVICE_JSON_FILE_NAME)) == 0)
        {
          status = fx_file_open(&_this->sdio_disk, &_this->file_tmp, entry_name, FX_OPEN_FOR_READ);
          if(status == FX_SUCCESS)
          {
            /* Get file size to allocate the buffer */
            uint32_t size = (uint32_t)_this->file_tmp.fx_file_current_file_size;
            p_json = (char*) SysAlloc(size + 1); // +1 used for terminator char
            if(p_json == NULL)
            {
              SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
            }
            else
            {
              /* Read the whole content of the file */
              fx_file_seek(&_this->file_tmp, 0);
              fx_file_read(&_this->file_tmp, p_json, size, &bytes_read);
              fx_file_close(&_this->file_tmp);

              /* Ensure that the file ends with a terminator char to avoid parsing exceptions */
              p_json[size] = '\0'; //terminator

              /* allocate buffer for PnPL Command*/
              p_cmd_json = (char*) SysAlloc(sizeof(SET_STATUS_CMD) + size + sizeof(TERMINATOR));
              if(p_cmd_json == NULL)
              {
                SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
              }

              strcat(p_cmd_json, SET_STATUS_CMD);
              strcat(p_cmd_json, p_json);
              strcat(p_cmd_json, TERMINATOR);
            }
          }
        }
      }
      else if((strncmp(p_file_ext, "ucf", 3) == 0) && (!ucf_found)) /* first file with 'ucf' extension */
      {
        status = fx_file_open(&_this->sdio_disk, &_this->file_tmp, entry_name, FX_OPEN_FOR_READ);
        if(status == FX_SUCCESS)
        {
          ucf_found = true;
          /* Get file size to allocate the buffer */
          p_ucf = (char*) SysAlloc(_this->file_tmp.fx_file_current_file_size);
          if(p_ucf == NULL)
          {
            SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
          }
          else
          {
            uint32_t ucf_size, compressed_ucf_size;

            /* Read the whole content of the file */
            fx_file_seek(&_this->file_tmp, 0);
            fx_file_read(&_this->file_tmp, p_ucf, _this->file_tmp.fx_file_current_file_size, &bytes_read);
            ucf_size = _this->file_tmp.fx_file_current_file_size;
            fx_file_close(&_this->file_tmp);

            /* allocate buffer for compressed ucf */
            compressed_ucf_size = UCFP_CompressedUcfSize(ucf_size);
            p_compressed_ucf = (char*) SysAlloc(compressed_ucf_size);

            /* Convert ucf in compressed format */
            UCFP_GetCompressedUcf(p_ucf, ucf_size, p_compressed_ucf, compressed_ucf_size, &actual_ucf_size);

            SysFree(p_ucf);
          }
        }
      }
    }
  }
  if(p_cmd_json != NULL) /* First load the JSON config file found in SD card (if available) */
  {
    /* Build command*/
    IParseCommand(_this->cmd_parser, p_cmd_json, 0);
    SysFree(p_cmd_json);
    SysFree(p_json);
  }
  if(p_compressed_ucf != NULL) /* Then load the UCF configuration (if available) */
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDCardTask: UCF loaded from root folder.\r\n"));

    if(PnPLGetFWID() == FW_ID_DATALOG2_ISPU)
    {
      ism330is_ispu_load_file(_this->ism330is_ispu, p_compressed_ucf, actual_ucf_size, "", 0); //TODO
    }
    else
    {
      ism330dhcx_mlc_load_file(_this->ism330dhcx_mlc, p_compressed_ucf, actual_ucf_size);
    }
    SysFree(p_compressed_ucf);
    ucf_found = false;
  }
  automode_setup();

  return res;
}

static const char* get_file_ext(const char *filename)
{
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename)
  {
    return "";
  }
  return dot + 1;
}

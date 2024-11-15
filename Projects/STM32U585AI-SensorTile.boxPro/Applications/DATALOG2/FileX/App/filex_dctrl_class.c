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
#include "fx_stm32_config.h"
#include "stdio.h"
#include "rtc.h"

#include "services/sysdebug.h"
#include "services/sysmem.h"
#include "services/SUcfProtocol.h"

#include "PnPLCompManager.h"

#include "App_model.h"

#include "automode.h"

#include "DatalogAppTask.h"

#define SYS_DEBUGF(level, message)          SYS_DEBUGF3(SYS_DBG_SDC, level, message)
#define MAX_INSTANCES 1

#define SD_BUFFER_ITEMS   2U

#if defined(DEBUG) || defined (SYS_DEBUG)
#define sObj                                sFileXObj
#endif

/* The other files are located in fx_file[] from SENSOR_DAT_FILES position */
#define OTHER_FILES_ID                    (FILEX_DCTRL_DAT_FILES_COUNT - 1)

#define SKIP_LIST_SIZE            (1)

#define CHECK_STOP_SEND_QUEUE_PARAM(param)  ((param > 0) && (param < FILEX_DCTRL_DEFAULT_QUEUE_SIZE))

/* Private variable ------------------------------------------------------------*/

char *skip_list[SKIP_LIST_SIZE] =
{
  "acquisition_info"
};

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
  filex_dctrl_vtblStream_set_parse_IF
};

/* Private member function declaration */
/***************************************/
void fx_thread_entry(unsigned long thread_input);
void filex_data_ready(filex_dctrl_class_t *_this, uint32_t data_ready_mask);
void filex_data_flush(filex_dctrl_class_t *_this, uint8_t stream_id);
sys_error_code_t filex_write_config_file(filex_dctrl_class_t *_this, char *file, uint32_t size);
sys_error_code_t filex_write_acquisition_info(filex_dctrl_class_t *_this, char *acquisition_info, uint32_t size);

static sys_error_code_t filex_check_root_folder(filex_dctrl_class_t *_this);
static const char *get_file_ext(const char *filename);

/* Public API definition */
/*************************/
IStream_t *filex_dctrl_class_alloc(void)
{
  IStream_t *p = (IStream_t *) &sObj;
  p->vptr = &filex_dctrl_vtbl;
  return p;
}

/* IStream virtual functions definition */
/*******************************/
sys_error_code_t filex_dctrl_vtblStream_init(IStream_t *_this, uint8_t comm_interface_id, void *param)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  obj->comm_interface_id = comm_interface_id;

  /* Allocate FILEX_CFG_STACK_DEPTH. */
  obj->thread_memory_pointer = (UCHAR *) SysAlloc(FILEX_CFG_STACK_DEPTH);
  if (obj->thread_memory_pointer == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Create the main thread.  */
  tx_thread_create(&obj->fx_app_thread, "FileX App Thread", fx_thread_entry, (uint32_t)obj, obj->thread_memory_pointer,
                   FILEX_CFG_STACK_DEPTH,
                   FILEX_SEND_CFG_PRIORITY, FILEX_CFG_PREEMPTION_THRESHOLD, TX_NO_TIME_SLICE, TX_AUTO_START);

#ifdef ENABLE_THREADX_DBG_PIN
  obj->fx_app_thread.pxTaskTag = FILEX_TASK_CFG_TAG;
#endif

  /* Allocate the message queue. */
  obj->queue_memory_pointer = (UCHAR *) SysAlloc(FILEX_DCTRL_DEFAULT_QUEUE_SIZE * sizeof(uint32_t));
  if (obj->queue_memory_pointer == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }

  /* Create the message queue */
  tx_queue_create(&obj->fx_app_queue, "FileX App queue", TX_1_ULONG, obj->queue_memory_pointer,
                  FILEX_DCTRL_DEFAULT_QUEUE_SIZE * sizeof(uint32_t));

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
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  char return_entry_name[50];
  uint16_t tmp = -1;

  SD_DetectInit();

  if (SD_IsDetected() && TX_SUCCESS != obj->fx_opened)
  {
    /* Open the SD disk driver */
    obj->fx_opened = fx_media_open(&obj->sdio_disk, "STM32_SDIO_DISK", fx_stm32_sd_driver, 0, &obj->media_memory, sizeof(obj->media_memory));
  }

  if (TX_SUCCESS == obj->fx_opened)
  {
    obj->fx_stream_enabled = true;

    /* Scan the root folder... */
    while (fx_directory_next_entry_find(&obj->sdio_disk, return_entry_name) != FX_NO_MORE_ENTRIES)
    {
      int16_t nLenght = strlen(return_entry_name);
      for (int i = nLenght - 1; i >= 0; --i)
      {
        if (return_entry_name[i] == '.')
        {
          nLenght = i;
          break;
        }
      }
      /* and check if there are already some acquisition folders */
      if (strncmp(return_entry_name, FILEX_DCTRL_LOG_DIR_PREFIX, sizeof(FILEX_DCTRL_LOG_DIR_PREFIX) - 1) == 0)
      {
        tmp = strtol(&return_entry_name[sizeof(FILEX_DCTRL_LOG_DIR_PREFIX)], NULL, 10);
        if (tmp > obj->dir_n)
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
  return res;
}

sys_error_code_t filex_dctrl_vtblStream_disable(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  /* Close the SD disk driver */
  fx_media_close(&obj->sdio_disk);
  obj->fx_opened = FX_INVALID_STATE;
  obj->fx_stream_enabled = false;

  return res;
}

bool filex_dctrl_vtblStream_is_enabled(IStream_t *_this)
{
  assert_param(_this != NULL);
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  return obj->fx_stream_enabled;
}

sys_error_code_t filex_dctrl_vtblStream_deinit(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

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
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;
  uint32_t ret = FX_SUCCESS;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  /*Check directories on SD and setup the new acquisition folder*/
  fx_directory_default_set(&obj->sdio_disk, FX_NULL);

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  char *p_name = obj->dir_name;
  if (sDate.Year != 0)
  {
    char *temp_s = "";
    acquisition_info_get_start_time(&temp_s); /* "YYYY-MM-DDTHH:MM:SS.mmmZ" */

    memcpy(obj->year, temp_s, 4);         /* YYYY */
    memcpy(obj->month, &temp_s[5], 2);    /* MM   */
    memcpy(obj->day, &temp_s[8], 2);      /* DD   */
    memcpy(obj->hours, &temp_s[11], 2);   /* HH   */
    memcpy(obj->minutes, &temp_s[14], 2); /* MM   */
    memcpy(obj->seconds, &temp_s[17], 2); /* SS   */

    snprintf(p_name, 5, "%s", obj->year);
    snprintf(&p_name[4], 3, "%s", obj->month);
    snprintf(&p_name[6], 3, "%s", obj->day);
    snprintf(&p_name[8], 4, "_%s", obj->hours);
    snprintf(&p_name[11], 4, "_%s", obj->minutes);
    snprintf(&p_name[14], 4, "_%s", obj->seconds);
    /* p_name = YYYYMMDD_HH_MM_SS */
  }
  else
  {
    obj->dir_n++;
    sprintf(obj->dir_name, "%s%05d", FILEX_DCTRL_LOG_DIR_PREFIX, obj->dir_n);
  }

  ret = fx_directory_create(&obj->sdio_disk, obj->dir_name);
  if (TX_SUCCESS != ret)
  {
    res = SYS_UNDEFINED_ERROR_CODE;
    return res;
  }

  fx_file_date_time_set(&obj->sdio_disk, obj->dir_name, ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month,
                        (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                        (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);

  fx_directory_default_set(&obj->sdio_disk, obj->dir_name);

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_stop(IStream_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;
  uint8_t ii;
  uint32_t attributes_ptr;
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

  for (ii = 0; ii < FILEX_DCTRL_DAT_FILES_COUNT; ii++) /*Close all files*/
  {
    /*if file exists*/
    if (TX_SUCCESS == fx_file_attributes_read(&obj->sdio_disk, obj->file_dat_name[ii], (unsigned int *)&attributes_ptr))
    {
      if (attributes_ptr == FX_ARCHIVE)
      {
        /*flush the data buffer*/
        filex_data_flush(obj, ii);
        fx_media_flush(&obj->sdio_disk);
        /*close the file*/
        fx_file_close(&obj->file_dat[ii]);
        fx_file_date_time_set(&obj->sdio_disk, obj->file_dat_name[ii], ((uint32_t) sDate.Year + 2000), (uint32_t) sDate.Month,
                              (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
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
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  bool item_ready;
  CircularBufferDL2 *cbdl2 = obj->cbdl2[id_stream];

  /* Copy the stream data to the buffer item */
  if (CBDL2_FillCurrentItem(cbdl2, id_stream, buf, size, &item_ready) != SYS_NO_ERROR_CODE)
  {
    res = SYS_BASE_ERROR_CODE;
  }
  /* Check if the item is ready */
  if (item_ready == true)
  {
    /* unlock write task */
    unsigned long msg = (id_stream & FILEX_DCTRL_DATA_SENSOR_ID_MASK) | FILEX_DCTRL_DATA_READY_MASK;
    if (filex_dctrl_msg(obj, &msg) != SYS_NO_ERROR_CODE)
    {
      res = SYS_TASK_QUEUE_FULL_ERROR_CODE;
    }
  }

  return res;
}

sys_error_code_t filex_dctrl_vtblStream_alloc_resource(IStream_t *_this, uint8_t id_stream, uint32_t size,
                                                       const char *stream_name)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  /* Allocate a DL2 Circular Buffer with SD_BUFFER_ITEMS elements*/
  CircularBufferDL2 *cbdl2 = CBDL2_Alloc(SD_BUFFER_ITEMS);
  if (cbdl2 == NULL)
  {
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Allocate the buffer to be used assigned to the CBDL2 object */
  obj->sd_write_buffer[id_stream] = (uint8_t *) SysAlloc(size * SD_BUFFER_ITEMS);
  if (obj->sd_write_buffer[id_stream] == NULL)
  {
    CBDL2_Free(cbdl2);
    res = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(res);
    return res;
  }
  /* Initialize the Circular Buffer with the specified parameters */
  CBDL2_Init(cbdl2, obj->sd_write_buffer[id_stream], size, false);
  obj->cbdl2[id_stream] = cbdl2;

  sprintf(obj->file_dat_name[id_stream], "%s%s", stream_name, ".dat");
  if (TX_SUCCESS != fx_file_create(&obj->sdio_disk, obj->file_dat_name[id_stream]))
  {
    obj->fx_opened = FX_IO_ERROR;
    return SYS_BASE_ERROR_CODE;
  }

  if (TX_SUCCESS != fx_file_open(&obj->sdio_disk, &obj->file_dat[id_stream], obj->file_dat_name[id_stream],
                                 FX_OPEN_FOR_WRITE))
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
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;
  char *response_name;
  static char *serialized_cmd = 0;
  static uint32_t size = 0;

  if (mode == TRANSMIT)
  {
    ISerializeResponse(obj->cmd_parser, &response_name, &serialized_cmd, &size, 1);

    if (strncmp(response_name, "all", 3) == 0)
    {
      filex_write_config_file(obj, serialized_cmd, size);
    }
    else if (strncmp(response_name, "acquisition_info", 16) == 0)
    {
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
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  CBDL2_Free(obj->cbdl2[id_stream]);
  obj->cbdl2[id_stream] = NULL;

  if (obj->sd_write_buffer[id_stream] != NULL)
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
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;

  obj->cmd_parser = ifn;
  return res;
}

sys_error_code_t filex_dctrl_msg(filex_dctrl_class_t *_this, unsigned long *msg)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  uint32_t message = *msg;

  /* Check if the callback function is set */
  if (_this->filex_msg_queue_not_send_cb)
  {
    ULONG enqueued = 0;
    ULONG available_storage = 0;

    /** Get Queue info */
    tx_queue_info_get(&_this->fx_app_queue, NULL, &enqueued, &available_storage, NULL, NULL, NULL);

    if (available_storage <= _this->queue_available_storage_thr)
    {
      /* Notify application level through the callback */
      _this->filex_msg_queue_not_send_cb();
    }
    else
    {
      if (tx_queue_send(&_this->fx_app_queue, &message, TX_NO_WAIT) != TX_SUCCESS)
      {
        /* Handle the error condition if necessary */
        ;
      }
    }
  }

  else
  {
    /* Attempt to send the message to the queue */
    if (tx_queue_send(&_this->fx_app_queue, &message, TX_NO_WAIT) != TX_SUCCESS)
    {
      /* Handle the error condition if necessary */
      ;
    }
  }
  return res;
}

/**
  * @brief Configures the threshold for suspending message queueing.
  *
  * This function sets a threshold for the available storage in the message queue and associates
  * a callback function that will be invoked when the available storage falls below the set threshold.
  * The purpose of this is to prevent the queue from being overwhelmed by temporarily suspending
  * the enqueuing of new messages.
  *
  * @param _this A pointer to the instance of filex_dctrl_class_t that represents
  *              the message queue controller.
  * @param threshold_config A pointer to a filex_threshold_config_t structure that contains
  *                         the threshold value and the callback function.
  *
  * @retval SYS_NO_ERROR_CODE Configuration was successful and the threshold is set.
  * @retval SYS_SD_TASK_INPUT_ARG_ERROR_CODE The threshold parameter provided is invalid.
  * @retval SYS_SD_TASK_NULL_PTR_ARG_ERROR_CODE The callback function pointer provided is NULL.
  */
sys_error_code_t filex_dctrl_configure_stop_threshold(filex_dctrl_class_t *_this,
                                                      filex_threshold_config_t *threshold_config)
{
  assert_param(_this != NULL);
  assert_param(threshold_config != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* Validate the callback function pointer */
  if (NULL != threshold_config->filex_msg_queue_not_send_cb)
  {
    /* Validate the threshold parameter */
    if (CHECK_STOP_SEND_QUEUE_PARAM(threshold_config->queue_available_storage_thr))
    {
      _this->filex_msg_queue_not_send_cb = threshold_config->filex_msg_queue_not_send_cb;
      _this->queue_available_storage_thr = threshold_config->queue_available_storage_thr;
    }
    else
    {
      res = SYS_SD_TASK_INPUT_ARG_ERROR_CODE;
    }
  }
  else
  {
    res = SYS_SD_TASK_NULL_PTR_ARG_ERROR_CODE;
  }

  return res;
}

sys_error_code_t filex_dctrl_write_ucf(filex_dctrl_class_t *_this, uint32_t compressed_ucf_size,
                                       const char *p_compressed_ucf)
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

  ucf_file_size = UCFP_UcfSize(compressed_ucf_size);
  p_ucf = SysAlloc(ucf_file_size);

  if (p_ucf == NULL)
  {
    return SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  /* Convert the compressed UCF into the Standard format  */
  if (UCFP_GetUcf(p_compressed_ucf, compressed_ucf_size, p_ucf, ucf_file_size, &actual_size) != SYS_NO_ERROR_CODE)
  {
    return SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  fx_create = fx_file_create(&_this->sdio_disk, FILEX_DCTRL_TMP_UCF_FILE_NAME);
  if (fx_create == FX_ALREADY_CREATED)
  {
    fx_file_delete(&_this->sdio_disk, FILEX_DCTRL_TMP_UCF_FILE_NAME);
    fx_create = fx_file_create(&_this->sdio_disk, FILEX_DCTRL_TMP_UCF_FILE_NAME);
  }

  if (fx_create != TX_SUCCESS)
  {
    _this->fx_opened = FX_IO_ERROR;
    res = SYS_BASE_ERROR_CODE;
  }
  else
  {
    fx_file_open(&_this->sdio_disk, &_this->file_tmp, FILEX_DCTRL_TMP_UCF_FILE_NAME, FX_OPEN_FOR_WRITE);
    fx_file_write(&_this->file_tmp, (uint8_t *) p_ucf, actual_size);
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, FILEX_DCTRL_TMP_UCF_FILE_NAME, ((uint32_t) sDate.Year + 2000),
                          (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours, (uint32_t) sTime.Minutes,
                          (uint32_t) sTime.Seconds);
  }

  SysFree(p_ucf);

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDCardTask: %s written to root folder.\r\n", FILEX_DCTRL_TMP_UCF_FILE_NAME));

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

  fx_ret = fx_file_create(&_this->sdio_disk, FILEX_DCTRL_DEVICE_JSON_FILE_NAME);

  if (fx_ret == FX_ALREADY_CREATED)
  {
    fx_file_delete(&_this->sdio_disk, FILEX_DCTRL_DEVICE_JSON_FILE_NAME);
    fx_ret = fx_file_create(&_this->sdio_disk, FILEX_DCTRL_DEVICE_JSON_FILE_NAME);
    if (fx_ret != FX_SUCCESS)
    {
      _this->fx_opened = FX_IO_ERROR;
      return SYS_BASE_ERROR_CODE;
    }
    fx_file_open(&_this->sdio_disk, &_this->file_tmp, FILEX_DCTRL_DEVICE_JSON_FILE_NAME, FX_OPEN_FOR_WRITE);
    if (file[size - 1] == '\0')
    {
      fx_file_write(&_this->file_tmp, file, (size - 1));
    }
    else
    {
      fx_file_write(&_this->file_tmp, file, size);
    }
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, FILEX_DCTRL_DEVICE_JSON_FILE_NAME, ((uint32_t) sDate.Year + 2000),
                          (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                          (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);
  }
  else if (fx_ret == FX_SUCCESS)
  {
    fx_file_open(&_this->sdio_disk, &_this->file_tmp, FILEX_DCTRL_DEVICE_JSON_FILE_NAME, FX_OPEN_FOR_WRITE);
    if (file[size - 1] == '\0')
    {
      fx_file_write(&_this->file_tmp, file, (size - 1));
    }
    else
    {
      fx_file_write(&_this->file_tmp, file, size);
    }
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, FILEX_DCTRL_DEVICE_JSON_FILE_NAME, ((uint32_t) sDate.Year + 2000),
                          (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                          (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);
  }
  else
  {
    _this->fx_opened = FX_IO_ERROR;
    return SYS_BASE_ERROR_CODE;
  }

  /* if ucf exists in root, copy tmp_ucf file from root into acquisition folder*/
  char root_ucf[32];
  sprintf(root_ucf, "%s%s", "../", FILEX_DCTRL_TMP_UCF_FILE_NAME);

  fx_ret = fx_file_open(&_this->sdio_disk, &_this->file_tmp, root_ucf, FX_OPEN_FOR_READ);

  if (fx_ret == FX_SUCCESS)
  {
    unsigned long actual_bytes = 0;
    uint32_t ucf_file_size = (uint32_t)_this->file_tmp.fx_file_current_file_size;
    char *ucf_file_buffer = SysAlloc(ucf_file_size);
    fx_file_read(&_this->file_tmp, (uint8_t *) ucf_file_buffer, ucf_file_size, &actual_bytes);
    fx_file_close(&_this->file_tmp);

    fx_ret = fx_file_create(&_this->sdio_disk, FILEX_DCTRL_UCF_FILE_NAME);
    if (fx_ret == FX_ALREADY_CREATED)
    {
      fx_file_delete(&_this->sdio_disk, FILEX_DCTRL_UCF_FILE_NAME);
      fx_ret = fx_file_create(&_this->sdio_disk, FILEX_DCTRL_UCF_FILE_NAME);
    }
    if (fx_ret != TX_SUCCESS)
    {
      _this->fx_opened = FX_IO_ERROR;
      return SYS_BASE_ERROR_CODE;
    }

    fx_file_open(&_this->sdio_disk, &_this->file_tmp, FILEX_DCTRL_UCF_FILE_NAME, FX_OPEN_FOR_WRITE);
    fx_file_write(&_this->file_tmp, (uint8_t *) ucf_file_buffer, ucf_file_size);
    fx_media_flush(&_this->sdio_disk);
    fx_file_close(&_this->file_tmp);

    fx_file_date_time_set(&_this->sdio_disk, FILEX_DCTRL_UCF_FILE_NAME, ((uint32_t) sDate.Year + 2000),
                          (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
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

  if (TX_SUCCESS != fx_file_create(&_this->sdio_disk, FILEX_DCTRL_ACQUISITION_INFO_FILE_NAME))
  {
    _this->fx_opened = FX_IO_ERROR;
    return SYS_BASE_ERROR_CODE;
  }

  fx_file_open(&_this->sdio_disk, &_this->file_tmp, FILEX_DCTRL_ACQUISITION_INFO_FILE_NAME, FX_OPEN_FOR_WRITE);
  if (acquisition_info[size - 1] == '\0')
  {
    fx_file_write(&_this->file_tmp, acquisition_info, (size - 1));
  }
  else
  {
    fx_file_write(&_this->file_tmp, acquisition_info, size);
  }
  fx_media_flush(&_this->sdio_disk);
  fx_file_close(&_this->file_tmp);

  fx_file_date_time_set(&_this->sdio_disk, FILEX_DCTRL_ACQUISITION_INFO_FILE_NAME, ((uint32_t) sDate.Year + 2000),
                        (uint32_t) sDate.Month, (uint32_t) sDate.Date, (uint32_t) sTime.Hours,
                        (uint32_t) sTime.Minutes, (uint32_t) sTime.Seconds);

  return res;
}

void fx_thread_entry(unsigned long thread_input)
{
  uint32_t received_message;
  filex_dctrl_class_t *p_obj = (filex_dctrl_class_t *) thread_input;

  while (1)
  {
    /* Wait for a message on the queue. */
    if (TX_SUCCESS == tx_queue_receive(&p_obj->fx_app_queue, &received_message, TX_WAIT_FOREVER))
    {
      /* Check SD card status  */
      if (TX_SUCCESS == p_obj->fx_opened)
      {
        switch (received_message)
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
            filex_dctrl_vtblStream_disable((IStream_t *) p_obj);
            break;

          default: /*data ready: parse the mask*/
            filex_data_ready(p_obj, received_message);
            break;
        }
      }
    }
  }
}


FX_MEDIA *filex_dctrl_get_media(filex_dctrl_class_t *_this)
{
  assert_param(_this != NULL);
  return &_this->sdio_disk;
}


void filex_data_ready(filex_dctrl_class_t *_this, uint32_t data_ready_mask)
{
  uint32_t stream_id = data_ready_mask & FILEX_DCTRL_DATA_SENSOR_ID_MASK;
  CircularBufferDL2 *cbdl2 = _this->cbdl2[stream_id];
  CBItem *p_item;
  if (CB_GetReadyItemFromTail((CircularBuffer *) cbdl2, &p_item) == SYS_NO_ERROR_CODE)
  {
#ifdef ENABLE_THREADX_DBG_PIN
    BSP_DEBUG_PIN_On(CON34_PIN_22);
#endif
    if (FX_SUCCESS != fx_file_write(&_this->file_dat[stream_id], (uint8_t *) CB_GetItemData(p_item),
                                    CB_GetItemSize((CircularBuffer *) cbdl2)))
    {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    }
#ifdef ENABLE_THREADX_DBG_PIN
    BSP_DEBUG_PIN_Off(CON34_PIN_22);
#endif
    /* Release the buffer item and reset tx_state */
    CB_ReleaseItem((CircularBuffer *) cbdl2, p_item);
  }
}

void filex_data_flush(filex_dctrl_class_t *_this, uint8_t stream_id)
{
  CircularBufferDL2 *cbdl2 = _this->cbdl2[stream_id];
  if (cbdl2 != NULL)
  {
    CBItem *p_item;
    uint32_t item_size = CB_GetItemSize((CircularBuffer *)cbdl2);

    if (item_size != 0)
    {
      if (CB_GetReadyItemFromTail((CircularBuffer *) cbdl2, &p_item) == SYS_NO_ERROR_CODE)
      {
        if (FX_SUCCESS != fx_file_write(&_this->file_dat[stream_id], (uint8_t *) CB_GetItemData(p_item), item_size))
        {
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
        }
        /* Release the buffer item and reset tx_state */
        CB_ReleaseItem((CircularBuffer *)cbdl2, p_item);
      }

      uint32_t byte_counter = cbdl2->byte_counter;
      if (byte_counter > 0 && byte_counter < item_size)
      {
        if (FX_SUCCESS != fx_file_write(&_this->file_dat[stream_id], (uint8_t *) CB_GetItemData(cbdl2->p_current_item),
                                        byte_counter))
        {
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
        }
      }
    }
  }
}


static sys_error_code_t filex_check_root_folder(filex_dctrl_class_t *_this)
{
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  filex_dctrl_class_t *obj = (filex_dctrl_class_t *) _this;
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
  while (fx_directory_next_entry_find(&_this->sdio_disk, entry_name) != FX_NO_MORE_ENTRIES)
  {
    /* Find and check file extension */
    p_file_ext = get_file_ext(entry_name);
    if (p_file_ext != NULL)
    {
      if (strncmp(p_file_ext, "json", 4) == 0) /* 'json' extension */
      {
        /* Check file name */
        if (strncmp(entry_name, FILEX_DCTRL_DEVICE_JSON_FILE_NAME, sizeof(FILEX_DCTRL_DEVICE_JSON_FILE_NAME)) == 0)
        {
          status = fx_file_open(&_this->sdio_disk, &_this->file_tmp, entry_name, FX_OPEN_FOR_READ);
          if (status == FX_SUCCESS)
          {
            /* Get file size to allocate the buffer */
            uint32_t size = (uint32_t)_this->file_tmp.fx_file_current_file_size;
            p_json = (char *) SysAlloc(size + 1); // +1 used for terminator char
            if (p_json == NULL)
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
              p_cmd_json = (char *) SysAlloc(sizeof(FILEX_DCTRL_SET_STATUS_CMD) + size + sizeof(FILEX_DCTRL_TERMINATOR));
              if (p_cmd_json == NULL)
              {
                SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
              }

              strcat(p_cmd_json, FILEX_DCTRL_SET_STATUS_CMD);
              strcat(p_cmd_json, p_json);
              strcat(p_cmd_json, FILEX_DCTRL_TERMINATOR);
            }
          }
        }
      }
      else if ((strncmp(p_file_ext, "ucf", 3) == 0) && (!ucf_found)) /* first file with 'ucf' extension */
      {
        status = fx_file_open(&_this->sdio_disk, &_this->file_tmp, entry_name, FX_OPEN_FOR_READ);
        if (status == FX_SUCCESS)
        {
          ucf_found = true;
          /* Get file size to allocate the buffer */
          p_ucf = (char *) SysAlloc(_this->file_tmp.fx_file_current_file_size);
          if (p_ucf == NULL)
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
            p_compressed_ucf = (char *) SysAlloc(compressed_ucf_size);

            /* Convert ucf in compressed format */
            UCFP_GetCompressedUcf(p_ucf, ucf_size, p_compressed_ucf, compressed_ucf_size, &actual_ucf_size);

            SysFree(p_ucf);
          }
        }
      }
    }
  }
  if (p_cmd_json != NULL) /* First load the JSON config file found in SD card (if available) */
  {
    /* Build command*/
    IParseCommand(_this->cmd_parser, p_cmd_json, sObj.comm_interface_id);
    SysFree(p_cmd_json);
    SysFree(p_json);
  }
  if (p_compressed_ucf != NULL) /* Then load the UCF configuration (if available) */
  {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDCardTask: UCF loaded from root folder.\r\n"));

#ifdef UCF_SUPPORT
    DatalogAppTask_load_ucf(p_compressed_ucf, actual_ucf_size, "", 0);
#endif

    SysFree(p_compressed_ucf);
    ucf_found = false;
  }

#ifdef AUTOMODE_SUPPORT
  automode_setup();
#endif

  /* Unmount SD card when the reading from SD root procedure is terminated */
  unsigned long msg = FILEX_DCTRL_CMD_CLOSE;
  filex_dctrl_msg(obj, &msg);

  return res;
}

static const char *get_file_ext(const char *filename)
{
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
  {
    return "";
  }
  return dot + 1;
}

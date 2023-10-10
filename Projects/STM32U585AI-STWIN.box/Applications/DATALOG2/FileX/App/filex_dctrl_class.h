/**
  ******************************************************************************
  * @file    filex_dctrl_class.h
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

#ifndef FILEX_DCTRL_CLASS_H_
#define FILEX_DCTRL_CLASS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IStream.h"
#include "IStream_vtbl.h"
#include "ICommandParse.h"
#include "ICommandParse_vtbl.h"

#include "CircularBufferDL2.h"
#include "fx_api.h"
#include "fx_stm32_sd_driver.h"
#include "fx_user.h"
#include "fx_stm32_config.h"

#define FILEX_DCTRL_DEFAULT_QUEUE_SIZE          20U

#define FILEX_DCTRL_CMD_INIT                    (0x0010)
//#define FILEX_DCTRL_CMD_START                   (0x0020)
#define FILEX_DCTRL_CMD_SAVE_STATUS             (0x0030)
#define FILEX_DCTRL_CMD_SET_DEFAULT_STATUS      (0x0031)
#define FILEX_DCTRL_CMD_SET_STATUS              (0x0040)
#define FILEX_DCTRL_CMD_CLOSE                   (0x0050)

#define FILEX_DCTRL_DATA_READY_MASK             (0x4000)
#define FILEX_DCTRL_DATA_FIRST_HALF_MASK        (0x2000)
#define FILEX_DCTRL_DATA_SECOND_HALF_MASK       (0x0000)
#define FILEX_DCTRL_DATA_SENSOR_ID_MASK         (0x00FF)

#ifndef FILEX_DCTRL_DAT_FILES_COUNT
/* one .dat file for each sensor */
#define FILEX_DCTRL_DAT_FILES_COUNT             SM_MAX_SENSORS
#endif

#define FILEX_DCTRL_LOG_DIR_PREFIX              "DL2_"
#define FILEX_DCTRL_DEVICE_JSON_FILE_NAME       "device_config.json"
#define FILEX_DCTRL_ACQUISITION_INFO_FILE_NAME  "acquisition_info.json"
#define FILEX_DCTRL_TMP_UCF_FILE_NAME           "config.ucf.tmp"
#define FILEX_DCTRL_UCF_FILE_NAME               "config.ucf"
#define FILEX_DCTRL_SET_STATUS_CMD              "{\"update_device_status\":"
#define FILEX_DCTRL_TERMINATOR                  "}"


/**
  * Create  type name for _filex_dctrl_class_t.
  */
typedef struct _filex_dctrl_class_t filex_dctrl_class_t;

/**
  *  filex_dctrl_class_t internal structure.
  */
struct _filex_dctrl_class_t
{
  /**
    * Base class object.
    */
  IStream_t super;

  /**
    * Identification for this specific communication interface
    */
  uint8_t comm_interface_id;

  UCHAR *thread_memory_pointer;
  UCHAR *queue_memory_pointer;

  /*
   * Buffer for FileX FX_MEDIA sector cache.
   **/
  uint32_t media_memory[FX_STM32_SD_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)];

  /*
   * Define FileX global data structures.
   **/
  FX_MEDIA sdio_disk;
  FX_FILE file_dat[FILEX_DCTRL_DAT_FILES_COUNT];
  FX_FILE file_tmp;
  uint32_t fx_opened;
  /* Define ThreadX global data structures.  */
  TX_THREAD fx_app_thread;
  TX_QUEUE fx_app_queue;

  /* Directory name */
  char dir_name[sizeof(FILEX_DCTRL_LOG_DIR_PREFIX) + 9];
  /* Directory number */
  uint16_t dir_n;
  /* Dat file names */
  char file_dat_name[FILEX_DCTRL_DAT_FILES_COUNT][32];

  /* Data buffers sent to SD card*/
  CircularBufferDL2 *cbdl2[FILEX_DCTRL_DAT_FILES_COUNT];
  uint8_t *sd_write_buffer[FILEX_DCTRL_DAT_FILES_COUNT];

  /* Command */
  ICommandParse_t *cmd_parser;

  /**
    * HAL driver configuration parameters.
    */
  const void *mx_drv_cfg;
};

/** Public API declaration */
/***************************/

/**
  * Allocate an instance of filex_dctrl_class_t.
  * @return a pointer to the generic interface ::IStream_t if success,
  * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
  */
IStream_t *filex_dctrl_class_alloc(void);

sys_error_code_t filex_dctrl_set_ICommandParseIF(filex_dctrl_class_t *_this, ICommandParse_t *inf);

sys_error_code_t filex_dctrl_msg(filex_dctrl_class_t *_this, unsigned long *msg);

sys_error_code_t filex_dctrl_write_ucf(filex_dctrl_class_t *_this, uint32_t ucf_size, const char *ucf_data);


#ifdef __cplusplus
}
#endif

#endif /* FILEX_DCTRL_CLASS_H_ */

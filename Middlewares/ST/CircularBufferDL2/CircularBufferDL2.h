/**
 ******************************************************************************
 * @file    CircularBuffer.h
 * @author  SRA - MCD
 * @brief  Circular buffer implementation specialized for the producer /
 * consumer design pattern.
 * This class allocates and manage a set of user defined type items
 * (::CBItemData) in a circular way. The user get a new free item from the
 * head of the buffer (to produce its content), and a he get a ready item from
 * the tail (to consume its content).
 * This class is specialized for the producer /consumer design pattern.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef USER_INC_CIRCULARBUFFERDL2_H_
#define USER_INC_CIRCULARBUFFERDL2_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "services/systp.h"
#include "services/sysmem.h"
#include "services/syscs.h"
#include "services/CircularBuffer.h"

/**
 * Create a type name for _CircularBufferDL2
 */
typedef struct _CircularBufferDL2 CircularBufferDL2;


/**
 * ::CircularBufferExt internal state.
 */
struct _CircularBufferDL2
{
  /*
   * Pointer to the ::CircularBuffer
   */
  CircularBuffer cb;

  /**
   * ::CircularBuffer item that is currently in use (filling)
   */
  CBItem *p_current_item;

  /**
   * Index of the next free byte in current item.
   */
  uint32_t item_idx;

  /**
   * Total Bytes counter
   */
  uint32_t byte_counter;

  /**
   * If true, the first byte of each item is the channel number.
   * If false, the channel is skipped and the buffer starts with the ::byte_counter
   */
  bool add_channel_to_buffer;
};

// Public API declaration
// **********************

/**
 * Allocate an object of ::CircularBufferDL2 type. This allocator implement the singleton design pattern,
 * so there is only one instance of circular buffer.
 * A new allocated object must be initialized before use it.
 *
 * @param [IN] nItemCount specifies the maximum number of items that is possible to store in the buffer.
 * @return a pointer to new allocated circular buffer object.
 */
CircularBufferDL2* CBDL2_Alloc(uint16_t item_count);

/**
 * Deallocate the ::CircularBufferDL2 object.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBufferDL2 object.
 */
void CBDL2_Free(CircularBufferDL2 *_this);

/**
 * Initialize a circular buffer object for Datalog2. The application allocate the data buffer.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBufferDL2 object.
 * @param p_items_buffer [IN] specifies the memory buffer where the data are stored.
 * @param item_size [IN] specifies the size in byte of an item.
 * @return SYS_NO_ERROR_CODE
 */
uint16_t CBDL2_Init(CircularBufferDL2 *_this, void *p_items_buffer, uint32_t item_size, bool add_channel_to_buffer);

/**
 * Get the items size for the specified Circular Buffer.
 * @param _this [IN] specifies a pointer to a ::CircularBufferDL2 object.
 * @return the size of the items.
 */
uint16_t CBDL2_GetItemSize(CircularBufferDL2 *_this);

/**
 * Get a ready item from the tail of the buffer. A ready item can be consumed by the caller.
 * After the item is consumed the caller must call CBDL2_ReleaseItem() in order to free the item.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBufferDL2 object.
 * @param p_item [OUT] pointer to the ready circular buffer item object. If the operation fails the pointer will be set to NULL.
 * @return SYS_NO_ERROR_CODE if success, SYS_CB_NO_READY_ITEM_ERROR_CODE if there are not ready item in the tail of the buffer.
 */
uint16_t CBDL2_GetReadyItemFromTail(CircularBufferDL2 *_this, CBItem **p_item);

/**
 * Release an item. After an item has been consumed it must be released so it is marked as free,
 * and avoid the buffer to become full. If the item is new, that means it has been allocate but it is not ready yet,
 * the function fails.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBufferDL2 object.
 * @param p_item [IN] specifies a pointer to the item to be released.
 * @return SYS_NO_ERROR_CODE if success, SYS_CB_INVALID_ITEM_ERROR_CODE otherwise.
 */
uint16_t CBDL2_ReleaseItem(CircularBufferDL2 *_this, CBItem *p_item);

/**
 * Enqueue data on a the buffer of the ::CBItem that is currently in use. When the buffer is full
 * the function requests a new one (if available) and continue to copy the remaining data.
 * @param _this [IN] specifies a pointer to a ::CircularBufferDL2 object.
 * @param ch_number [IN]: channel number to be used
 * @param p_buf [IN]: pointer to data to be enqueued
 * @param size [IN]: length of data in bytes
 * @param p_ready [OUT]: set to true if the Item is ready after filling
 * @retval status
 */
uint32_t CBDL2_FillCurrentItem(CircularBufferDL2 *_this, uint8_t ch_number, uint8_t *p_buf, uint32_t size, bool *p_ready);

#ifdef __cplusplus
}
#endif

#endif /* USER_INC_CIRCULARBUFFERDL2_H_ */

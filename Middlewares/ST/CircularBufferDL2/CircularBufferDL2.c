/**
  ******************************************************************************
  * @file    CircularBufferDL2.c
  * @brief   Definition of the CircularBuffer class for Datalog2.
  * For more information look at the CircularBuffer.h file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "CircularBufferDL2.h"
#include <stdlib.h>


// Private functions declarations
// ******************************
static uint16_t GetNewItem(CircularBufferDL2 *_this, CircularBuffer *cb, uint32_t cb_item_size, uint8_t ch_number);

// Public API definition
// **********************

#define DL2_CHANNEL_SIZE  1U
#define DL2_COUNTER_SIZE  4U

CircularBufferDL2* CBDL2_Alloc(uint16_t item_count)
{
  CircularBufferDL2 *p_obj = (CircularBufferDL2 *)SysAlloc(sizeof(CircularBufferDL2));
  if(p_obj != NULL)
  {
    p_obj->cb.p_items = (CBItem *)SysAlloc(sizeof(CBItem) * item_count);

    if(p_obj->cb.p_items == NULL)
    {
      /* release the memory */
      SysFree(p_obj);
      p_obj = NULL;
    }
    else
    {
      p_obj->cb.item_count = item_count;
    }
  }
  return p_obj;
}

uint16_t CBDL2_Init(CircularBufferDL2 *_this, void *p_items_buffer, uint32_t item_size, bool add_channel_to_buffer)
{
  assert_param(_this != NULL);
  assert_param(p_items_buffer != NULL);

  uint16_t res = SYS_NO_ERROR_CODE;

  (void)CB_Init(&_this->cb, p_items_buffer, item_size);

  _this->item_idx = 0;
  _this->byte_counter = 0;
  _this->p_current_item = 0;
  _this->add_channel_to_buffer = add_channel_to_buffer;
  return res;
}

uint32_t CBDL2_FillCurrentItem(CircularBufferDL2 *_this, uint8_t ch_number, uint8_t *p_buf, uint32_t size, bool *p_ready)
{
  assert_param(_this != NULL);
  assert_param(p_buf != NULL);

  uint16_t res = SYS_NO_ERROR_CODE;
  CircularBuffer *cb = &_this->cb;
  CBItem *cb_item = _this->p_current_item;
  uint32_t cb_item_size = CB_GetItemSize(cb);
  uint32_t src_idx = 0;
  uint32_t dst_idx;
  uint8_t *p_dst;
  *p_ready = false;

  if(cb_item == NULL)
  {
    /* Get a new item and set the header */
    if(GetNewItem(_this, cb, cb_item_size, ch_number) != SYS_NO_ERROR_CODE)
    {
      return SYS_CB_NO_READY_ITEM_ERROR_CODE;
    }
  }
  cb_item = _this->p_current_item;
  p_dst = (uint8_t*) CB_GetItemData(cb_item);
  dst_idx = _this->item_idx;

  /* if there is enough space left in the item use memcpy */
  uint32_t size_after_copy = dst_idx + size;

  if(size_after_copy < cb_item_size) /* data to be copied won't exceed the size of the item */
  {
    (void) memcpy(&p_dst[dst_idx], p_buf, size);
    dst_idx += size;
  }
  else /* bytewise copy */
  {
    uint32_t partial_size = cb_item_size - dst_idx;
    (void) memcpy(&p_dst[dst_idx], p_buf, partial_size);
    src_idx += partial_size;

    /* Current item is full, set as ready so that it can be consumed */
    (void) CB_SetItemReady(cb, cb_item);
    *p_ready = true;

    /* Get a new item and set the header */
    if(GetNewItem(_this, cb, cb_item_size, ch_number) != SYS_NO_ERROR_CODE)
    {
      return SYS_CB_NO_READY_ITEM_ERROR_CODE;
    }
    cb_item = _this->p_current_item;
    p_dst = (uint8_t*) CB_GetItemData(cb_item);
    dst_idx = _this->item_idx;

    partial_size = size - partial_size;
    (void) memcpy(&p_dst[dst_idx], &p_buf[src_idx], partial_size);
    dst_idx += partial_size;
  }
  _this->item_idx = dst_idx;
  return res;
}

void CBDL2_Free(CircularBufferDL2 *_this)
{
  assert_param(_this != NULL);
  SysFree(_this->cb.p_items);
  SysFree(_this);
}

// Private functions definition
// ****************************

static uint16_t GetNewItem(CircularBufferDL2 *_this, CircularBuffer *cb, uint32_t cb_item_size, uint8_t ch_number)
{
  uint16_t res = SYS_NO_ERROR_CODE;

  if(CB_GetFreeItemFromHead(cb, &_this->p_current_item) != SYS_NO_ERROR_CODE)
  {
    res = SYS_CB_NO_READY_ITEM_ERROR_CODE;
    _this->item_idx = 0;
    _this->p_current_item = NULL;
    return res;
  }
  else
  {
    uint8_t *p_dst;
    p_dst = (uint8_t*) CB_GetItemData(_this->p_current_item);
    uint32_t *p_byte_counter_dst;
    uint8_t header_size;

    /* if required, write ch_number at the beginning of the buffer (first byte) */
    if(_this->add_channel_to_buffer)
    {
      p_dst[0] = ch_number;
      header_size = DL2_CHANNEL_SIZE + DL2_COUNTER_SIZE;
      p_byte_counter_dst = (uint32_t*) &p_dst[DL2_CHANNEL_SIZE];
    }
    else
    {
      header_size = DL2_COUNTER_SIZE;
      p_byte_counter_dst = (uint32_t*) p_dst;
    }

    /* increment the byte counter by adding the payload size (item - header) */
    _this->byte_counter += (cb_item_size - header_size);
    *p_byte_counter_dst = _this->byte_counter;

    /* move buffer index after header */
    _this->item_idx = header_size;
  }
  return res;
}

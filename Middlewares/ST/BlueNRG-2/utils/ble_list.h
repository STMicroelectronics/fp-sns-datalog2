/**
  ******************************************************************************
  * @file    ble_list.h
  * @author  AMS - HEA&RF BU
  * @brief   Header file for linked list library.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2012 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __BLE_LIST_H_
#define __BLE_LIST_H_

#include <stdint.h>

typedef struct _tListNode {
  struct _tListNode * next;
  struct _tListNode * prev;
} tListNode, *pListNode;

void list_init_head (tListNode * listHead);

uint8_t list_is_empty (tListNode * listHead);

void list_insert_head (tListNode * listHead, tListNode * node);

void list_insert_tail (tListNode * listHead, tListNode * node);

void list_remove_node (tListNode * node);

void list_remove_head (tListNode * listHead, tListNode ** node );

void list_remove_tail (tListNode * listHead, tListNode ** node );

void list_insert_node_after (tListNode * node, tListNode * ref_node);

void list_insert_node_before (tListNode * node, tListNode * ref_node);

int list_get_size (tListNode * listHead);

void list_get_next_node (tListNode * ref_node, tListNode ** node);

void list_get_prev_node (tListNode * ref_node, tListNode ** node);

#endif /* __BLE_LIST_H_ */

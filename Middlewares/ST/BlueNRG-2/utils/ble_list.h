/**
  ******************************************************************************
  * @file    ble_list.h
  * @author  AMS - HEA&RF-BU
  * @brief   Header file for linked list library.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct _list_node_t
{
  struct _list_node_t *next;
  struct _list_node_t *prev;
} list_node_t, *p_list_node_t;

void list_init_head(list_node_t *list_head);

uint8_t list_is_empty(list_node_t *list_head);

void list_insert_head(list_node_t *list_head, list_node_t *node);

void list_insert_tail(list_node_t *list_head, list_node_t *node);

void list_remove_node(list_node_t *node);

void list_remove_head(list_node_t *list_head, list_node_t **node);

void list_remove_tail(list_node_t *list_head, list_node_t **node);

void list_insert_node_after(list_node_t *node, list_node_t *ref_node);

void list_insert_node_before(list_node_t *node, list_node_t *ref_node);

int32_t list_get_size(list_node_t *list_head);

void list_get_next_node(list_node_t *ref_node, list_node_t **node);

void list_get_prev_node(list_node_t *ref_node, list_node_t **node);

#ifdef __cplusplus
}
#endif

#endif /* __BLE_LIST_H_ */

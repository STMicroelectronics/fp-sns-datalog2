/**
  ******************************************************************************
  * @file    ble_list.c
  * @author  AMS - HEA&RF-BU
  * @brief   Circular Linked List Implementation.
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
/******************************************************************************
  * Include Files
  ******************************************************************************/
#include "ble_list.h"
#include <stdio.h>

#include "ble_list_utils.h"

/******************************************************************************
  * Function Definitions
  ******************************************************************************/
void list_init_head(list_node_t *list_head)
{
  list_head->next = list_head;
  list_head->prev = list_head;
}

uint8_t list_is_empty(list_node_t *list_head)
{
  uint8_t return_value;

  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  if (list_head->next == list_head)
  {
    return_value = 1;
  }
  else
  {
    return_value = 0;
  }

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/

  return return_value;
}

void list_insert_head(list_node_t *list_head, list_node_t *node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  node->next = list_head->next;
  node->prev = list_head;
  list_head->next = node;
  (node->next)->prev = node;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

void list_insert_tail(list_node_t *list_head, list_node_t *node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  node->next = list_head;
  node->prev = list_head->prev;
  list_head->prev = node;
  (node->prev)->next = node;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

void list_remove_node(list_node_t *node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  (node->prev)->next = node->next;
  (node->next)->prev = node->prev;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

void list_remove_head(list_node_t *list_head, list_node_t **node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  *node = list_head->next;
  list_remove_node(list_head->next);
  (*node)->next = NULL;
  (*node)->prev = NULL;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

void list_remove_tail(list_node_t *list_head, list_node_t **node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  *node = list_head->prev;
  list_remove_node(list_head->prev);
  (*node)->next = NULL;
  (*node)->prev = NULL;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

void list_insert_node_after(list_node_t *node, list_node_t *ref_node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  node->next = ref_node->next;
  node->prev = ref_node;
  ref_node->next = node;
  (node->next)->prev = node;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

void list_insert_node_before(list_node_t *node, list_node_t *ref_node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  node->next = ref_node;
  node->prev = ref_node->prev;
  ref_node->prev = node;
  (node->prev)->next = node;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

int32_t list_get_size(list_node_t *list_head)
{
  int32_t size = 0;
  list_node_t *temp;

  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  temp = list_head->next;
  while (temp != list_head)
  {
    size++;
    temp = temp->next;
  }

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/

  return (size);
}

void list_get_next_node(list_node_t *ref_node, list_node_t **node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  *node = ref_node->next;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

void list_get_prev_node(list_node_t *ref_node, list_node_t **node)
{
  uint32_t uw_primask_bit;
  uw_primask_bit = __get_PRIMASK();  /**< backup PRIMASK bit */
  __disable_irq();                  /**< Disable all interrupts by setting PRIMASK bit on Cortex*/

  *node = ref_node->prev;

  __set_PRIMASK(uw_primask_bit);     /**< Restore PRIMASK bit*/
}

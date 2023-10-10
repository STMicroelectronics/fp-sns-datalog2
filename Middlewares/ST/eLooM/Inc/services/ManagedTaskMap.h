/**
  ******************************************************************************
  * @file    ManagedTaskMap.h
  * @author  SRA - GPM
  * @brief   This class implements a map between a managed task Instance and
  *           it's parameters.
  *
  *          The map is a collection of pairs (key, value) named element, where:
  *          - key is unique: a map cannot contains duplicate keys. Each key
  *            can map to at most one value.
  *          - the value is a combination of an ::AManagedTask plus an application
  *            specific parameter.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */
#ifndef INCLUDE_MANAGEDTASKMAP_H_
#define INCLUDE_MANAGEDTASKMAP_H_


#include "services/AManagedTask.h"


#ifdef __cplusplus
extern "C" {
#endif

#define MTM_NULL_KEY    (0U)  /**< reserved key value to specify an invalid key. */

/**
  * Check if an ::MTMapElement_t is empty. The element in the map are not allocated and released
  * dynamically, but rather the element are marked as empty.
  */
#define MTME_IS_EMPTY(p_element)  (((p_element)->value.p_mtask_obj == NULL) && ((p_element)->key == MTM_NULL_KEY))


/**
  * Specifies the value of the elements stored in the map.
  */
typedef struct _MTMapValue
{
  /**
    * Pointer to the managed task instance
    */
  AManagedTask *p_mtask_obj;

  /**
    * Generic pointer to the static parameters of the specific implementation
    */
  void *p_static_param;
} MTMapValue_t;

/**
  * Specifies the element of the map. It is the pair (key, value)
  */
typedef struct _MTMapElement
{
  /**
    * Unique key to identify the peripheral
    */
  uint32_t key;

  /**
    * Value linked to the key.
    */
  MTMapValue_t value;

} MTMapElement_t;

/**
  * This map is implemented as an array of `size` elements.
  */
typedef struct _MTMap
{
  /**
    * Pointer to the map instance
    */
  MTMapElement_t *p_elements;

  /**
    * Map size is the maximum number of elements
    */
  uint16_t size;

  /**
    * Specifies the number of element present in the map.
    */
  uint16_t element_count;
} MTMap_t;

/**
  * Initialize the Driver map.
  *
  * @param _this [IN] pointer to a valid ::MTMap_t object.
  * @param p_elements [IN] pointer to a valid ::MTMapElement_t array.
  * @param size [IN] number of elements in the array
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
static inline
sys_error_code_t MTMap_Init(MTMap_t *_this, MTMapElement_t *p_elements, uint16_t size)
{
  assert_param(_this != NULL);
  assert_param(p_elements != NULL);
  uint32_t i = 0U;

  _this->p_elements = p_elements;
  _this->size = size;
  _this->element_count = 0;

  /* initialize all elements to NULL */
  for (i = 0; i < size; i++)
  {
    p_elements[i].key = MTM_NULL_KEY;
    p_elements[i].value.p_mtask_obj = NULL;
    p_elements[i].value.p_static_param = NULL;
  }

  return SYS_NO_ERROR_CODE;
}

/**
  * Searches the map for an element with a the corresponding key
  *
  * @param _this [IN] pointer to a valid ::MTMap_t object.
  * @param key [IN] key to be searched.
  * @return The corresponding ::MTMapElement_t if found, NULL otherwise.
  */
static inline
MTMapValue_t *MTMap_FindByKey(MTMap_t *_this, uint32_t key)
{
  assert_param(_this != NULL);
  uint32_t i = 0U;

  while (_this->p_elements[i].key != key)
  {
    i++;
    if (i >= _this->size)
    {
      return NULL;
    }
  }
  return &_this->p_elements[i].value;
}

/**
  * Add a new pair (key, IDriver) to the map. The pair (key, IDriver) is namebd element.
  * If the key is already present to the map then the element is not added and the method return NULL.
  * If the element is already present in the map then the method return the element.
  *
  * @param _this [IN] pointer to a valid ::MTMap_t object.
  * @param key [IN] unique ID for the element.
  * @param [IN] p_instance specifies the ::IDriver instance of the element's value.
  * @return a pointer the the new added element (key, value) if success, or NULL if the element cannot be added into the map.
  */
static inline
MTMapElement_t *MTMap_AddElement(MTMap_t *_this, uint32_t key, AManagedTask *p_instance)
{
  assert_param(_this != NULL);
  uint32_t i = 0U;
  MTMapElement_t *p_free_element = NULL;

  while ((_this->p_elements[i].key != key) && (i < _this->size))
  {
    if ((p_free_element == NULL) && MTME_IS_EMPTY(&_this->p_elements[i]))
    {
      /* we found a free element in the map.*/
      p_free_element = &_this->p_elements[i];
    }

    i++;
  }

  if (i < _this->size)
  {
    /* the key is already present in the map*/
    if (_this->p_elements[i].value.p_mtask_obj == p_instance)
    {
      /* the element is present in the map, so we return it.*/
      p_free_element = &_this->p_elements[i];
    }
    else
    {
      /* the key is already present in the map, but it is mapped to a different IDriver instance, so we cannot add the new element.*/
      p_free_element = NULL;
    }
  }
  else
  {
    /* the key is not present in the map, so we try to add the new element*/
    if (p_free_element != NULL)
    {
      /* add the new element in the free position*/
      p_free_element->key = key;
      p_free_element->value.p_mtask_obj = p_instance;
      _this->element_count++;
    }
  }

  return p_free_element;
}

/**
  * Remove the element with the specific key from the map. The element is marked as removed, and
  * no memory de-allocation is done.
  *
  * @param _this [IN] pointer to a valid ::MTMap_t object.
  * @param key [IN] unique ID for the element.
  * @return true if the element as been removed from the map, false if the element is not in the map.
  */
static inline
bool MTMap_RemoveElement(MTMap_t *_this, uint32_t key)
{
  assert_param(_this != NULL);
  uint32_t i = 0U;
  bool res = true;

  while (res && (_this->p_elements[i].key != key))
  {
    if (++i >= _this->size)
    {
      res = false;
    }
  }

  if (res)
  {
    _this->p_elements[i].key = MTM_NULL_KEY;
    _this->p_elements[i].value.p_mtask_obj = NULL;
    _this->p_elements[i].value.p_static_param = NULL;
    _this->element_count--;
  }

  return res;
}

/**
  * Check if the map has been initialized. If a map is not initialized, any call to the map API has unknown effect.
  * To initialize a map use the method MTMap_Init().
  *
  * @param _this [IN] pointer to a valid ::MTMap_t object.
  * @return true if the map has been initialized, false otherwise.
  */
static inline
bool MTMap_IsInitialized(MTMap_t *_this)
{
  assert_param(_this != NULL);

  return _this->size != 0;
}

/**
  * Get the number of element (key, value) present in the map.
  *
  * @param _this [IN] pointer to a valid ::MTMap_t object.
  * @return the number of element (key, value) present in the map.
  */
static inline
bool MTMap_GetElementCount(const MTMap_t *const _this)
{
  assert_param(_this != NULL);

  return _this->element_count;
}


/* MTMapValue public API */
/*******************************/

static inline
void MTMapValue_SetStaticParam(MTMapValue_t *_this, void *p_static_param)
{
  assert_param(_this != NULL);

  _this->p_static_param = p_static_param;
}

static inline
void *MTMapValue_GetStaticParam(const MTMapValue_t *const _this)
{
  assert_param(_this != NULL);

  return _this->p_static_param;
}


/* Utility functions */
/*********************/

static inline
uint32_t MTMap_GetKeyForGPIO(GPIO_TypeDef *p_port, uint32_t pin)
{
  uint32_t port = (uint32_t)p_port;
  uint32_t position = 0U;
  uint32_t key = MTM_NULL_KEY;

  /*find the pin position*/
  uint32_t iocurrent = 0U;
  while ((iocurrent == 0U) && (position < 16))
  {
    iocurrent = (pin) & (1UL << position++);
  }

  if (iocurrent != 0)
  {
    key = port + position;
  }

  return key;
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_MANAGEDTASKMAP_H_ */

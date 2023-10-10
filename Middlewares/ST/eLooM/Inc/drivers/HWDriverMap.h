/**
  ******************************************************************************
  * @file    HWDriverMap.h
  * @author  SRA - GPM
  * @brief   This class implements a map between a driver Instance and it's
  *          parameters
  *
  *          The map is a collection of pairs (key, value) named element, where:
  *          - key is unique: a map cannot contains duplicate keys. each key
  *            can map to at most one value.
  *          - the value is a combination of an ::IDriver plus an application
  *            specific parameter s displayed in the following image
  *
  *            \anchor eloom_hwdrivermap_fig1 \image html 26_HWDriverMap_class.png "Fig.1 - class diagram" width=900px
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
#ifndef INCLUDE_HWDRIVERMAP_H_
#define INCLUDE_HWDRIVERMAP_H_


#include "drivers/IDriver.h"
#include "drivers/IDriver_vtbl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HDM_NULL_KEY    (0U)  /**< reserved key value to specify an invalid key. */

/**
  * Check if an ::HWDriverMapElement_t is empty. The element in the map are not allocated and released
  * dynamically, but rather the element are marked as empty.
  */
#define HDME_IS_EMPTY(p_element)  (((p_element)->value.p_driver_obj == NULL) && ((p_element)->key == HDM_NULL_KEY))


/**
  * Specifies the value of the elements stored in the map.
  */
typedef struct _HWDriverMapValue
{
  /**
    * Pointer to the driver instance
    */
  IDriver *p_driver_obj;

  /**
    * Generic pointer to the static parameters of the specific implementation
    */
  void *p_static_param;
} HWDriverMapValue_t;

/**
  * Specifies the element of the map. It is the pair (key, value)
  */
typedef struct _HWDriverMapElement
{
  /**
    * Unique key to identify the peripheral
    */
  uint32_t key;

  /**
    * Value linked to the key.
    */
  HWDriverMapValue_t value;

} HWDriverMapElement_t;

/**
  * This map is implemented as an array of `size` elements.
  */
typedef struct _HWDriverMap
{
  /**
    * Pointer to the map instance
    */
  HWDriverMapElement_t *p_elements;

  /**
    * Map size is the maximum number of elements
    */
  uint16_t size;

  /**
    * Specifies the number of element present in the map.
    */
  uint16_t element_count;
} HWDriverMap_t;

/**
  * Initialize the Driver map.
  *
  * @param _this [IN] pointer to a valid ::HWDriverMap_t object.
  * @param p_elements [IN] pointer to a valid ::HWDriverMapElement_t array.
  * @param size [IN] number of elements in the array
  * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
  */
static inline
sys_error_code_t HWDriverMap_Init(HWDriverMap_t *_this, HWDriverMapElement_t *p_elements, uint16_t size)
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
    p_elements[i].key = HDM_NULL_KEY;
    p_elements[i].value.p_driver_obj = NULL;
    p_elements[i].value.p_static_param = NULL;
  }

  return SYS_NO_ERROR_CODE;
}

/**
  * Searches the map for an element with a the corresponding key
  *
  * @param _this [IN] pointer to a valid ::HWDriverMap_t object.
  * @param key [IN] key to be searched.
  * @return The corresponding ::HWDriverMapElement_t if found, NULL otherwise.
  */
static inline
HWDriverMapValue_t *HWDriverMap_FindByKey(HWDriverMap_t *_this, uint32_t key)
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
  * @param _this [IN] pointer to a valid ::HWDriverMap_t object.
  * @param key [IN] unique ID for the element.
  * @param [IN] p_instance specifies the ::IDriver instance of the element's value.
  * @return a pointer the the new added element (key, value) if success, or NULL if the element cannot be added into the map.
  */
static inline
HWDriverMapElement_t *HWDriverMap_AddElement(HWDriverMap_t *_this, uint32_t key, IDriver *p_instance)
{
  assert_param(_this != NULL);
  uint32_t i = 0U;
  HWDriverMapElement_t *p_free_element = NULL;

  while ((_this->p_elements[i].key != key) && (i < _this->size))
  {
    if ((p_free_element == NULL) && HDME_IS_EMPTY(&_this->p_elements[i]))
    {
      /* we found a free element in the map.*/
      p_free_element = &_this->p_elements[i];
    }

    i++;
  }

  if (i < _this->size)
  {
    /* the key is already present in the map*/
    if (_this->p_elements[i].value.p_driver_obj == p_instance)
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
      p_free_element->value.p_driver_obj = p_instance;
      _this->element_count++;
    }
  }

  return p_free_element;
}

/**
  * Remove the element with the specific key from the map. The element is marked as removed, and
  * no memory de-allocation is done.
  *
  * @param _this [IN] pointer to a valid ::HWDriverMap_t object.
  * @param key [IN] unique ID for the element.
  * @return true if the element as been removed from the map, false if the element is not in the map.
  */
static inline
bool HWDriverMap_RemoveElement(HWDriverMap_t *_this, uint32_t key)
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
    _this->p_elements[i].key = HDM_NULL_KEY;
    _this->p_elements[i].value.p_driver_obj = NULL;
    _this->p_elements[i].value.p_static_param = NULL;
    _this->element_count--;
  }

  return res;
}

/**
  * Check if the map has been initialized. If a map is not initialized, any call to the map API has unknown effect.
  * To initialize a map use the method HWDriverMap_Init().
  *
  * @param _this [IN] pointer to a valid ::HWDriverMap_t object.
  * @return true if the map has been initialized, false otherwise.
  */
static inline
bool HWDriverMap_IsInitialized(HWDriverMap_t *_this)
{
  assert_param(_this != NULL);

  return _this->size != 0;
}

/**
  * Get the number of element (key, value) present in the map.
  *
  * @param _this [IN] pointer to a valid ::HWDriverMap_t object.
  * @return the number of element (key, value) present in the map.
  */
static inline
bool HWDriverMap_GetElementCount(const HWDriverMap_t *const _this)
{
  assert_param(_this != NULL);

  return _this->element_count;
}


/* HWDriverMapValue public API */
/*******************************/

static inline
void HWDriverMapValue_SetStaticParam(HWDriverMapValue_t *_this, void *p_static_param)
{
  assert_param(_this != NULL);

  _this->p_static_param = p_static_param;
}

static inline
void *HWDriverMapValue_GetStaticParam(const HWDriverMapValue_t *const _this)
{
  assert_param(_this != NULL);

  return _this->p_static_param;
}


/* Utility functions */
/*********************/

static inline
uint32_t HWDriverMap_GetKeyForGPIO(GPIO_TypeDef *p_port, uint32_t pin)
{
  uint32_t port = (uint32_t)p_port;
  uint32_t position = 0U;
  uint32_t key = HDM_NULL_KEY;

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

#endif /* INCLUDE_HWDRIVERMAP_H_ */

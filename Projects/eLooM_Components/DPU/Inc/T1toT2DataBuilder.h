/**
  ******************************************************************************
  * @file    T1toT2DataBuilder.h
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 1.0.0
  * @date    Jun 22, 2022
  *
  * @brief   Generic data builder that use an application defined function to
  *          convert from T1 to T2 type.
  *
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */
#ifndef DPU_T1TOT2DATABUILDER_H_
#define DPU_T1TOT2DATABUILDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IDataBuilder.h"
#include "IDataBuilder_vtbl.h"


/**
  * Create  type name for struct _T1toT2DataBuilder
  */
typedef struct _T1toT2DataBuilder T1toT2DataBuilder_t;

/**
  * Function used to convert a data from T1 to T2.
  * The data are passed as reference. The function convert a data of type T1
  * at address p_src_data, into a data of type T2, and store its value at address p_trg_data.
  *
  * @param p_src_data [IN] address of the T1 data to be converted
  * @param p_trg_data [OUT] address of the T2 data.
  */
typedef void (*Convert_f)(const uint8_t *p_src_data, uint8_t *p_trg_data);

/**
  * T1toT2DataBuilder_t internal state.
  */
struct _T1toT2DataBuilder
{
  /**
    * Base interface.
    */
  IDataBuilder_t super;

  /**
    * Index used to count how many elements in the target data have been built.
    */
  uint16_t index;

  /**
    * Store the data build context passed by the object that use the data build interface.
    */
  void *p_data_build_context;

  /**
    * Function to convert a data from T1 to T2.
    */
  Convert_f convert;
};


/* Public API declaration */
/**************************/

/**
  * Alloc an object from the system heap.
  *
  * @return a pointer to the new created object if success, NULL if an out of memory error occur.
  */
IDataBuilder_t *T1toT2DB_Alloc(void);

/**
  * Pseudo allocator to perform the basic object initialization (to assign the class virtual table to the new instance)
  * to an object instance allocate buy the application.
  *
  * @param _this [IN] object instance allocated by the application.
  * @return a pointer to the new created object if success, NULL if an out of memory error occur.
  */
IDataBuilder_t *T1toT2DB_AllocStatic(T1toT2DataBuilder_t *_this);

/**
  * Set the function used to convert a value from T1 to T2.
  *
  * @param _this [IN] specifies an object.
  * @param convert_f [IN] specifies a pointer to a conversion function.
  */
static inline
void T1toT2DB_SetConversoinFunc(T1toT2DataBuilder_t *_this, Convert_f convert_f);


/* Inline functions definition */
/*******************************/

static inline
void T1toT2DB_SetConversoinFunc(T1toT2DataBuilder_t *_this, Convert_f convert_f)
{
  assert_param(_this != NULL);
  _this->convert = convert_f;
}

#ifdef __cplusplus
}
#endif

#endif /* DPU_T1TOT2DATABUILDER_H_ */

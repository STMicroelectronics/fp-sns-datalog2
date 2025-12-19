/**
  ********************************************************************************
  * @file    AI_SC_DPU.c
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 1.0.0
  * @date    Oct 25, 2022
  *
  * @brief
  *
  * <ADD_FILE_DESCRIPTION>
  *
  ********************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ********************************************************************************
  */

#include "AI_SC_DPU.h"
#include "AI_SC_DPU_vtbl.h"
#include "services/AAI_Network.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"


#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_DPU, level, message)


/* Private Define */
/******************/
#define AI_SC_DPU_CHECK_MODEL_TOPOLOGY( input_dim, output_dim ) (((input_dim) <= MAX_NUM_OF_INPUT_BUFFER) && \
                                                                 ((output_dim) <= MAX_NUM_OF_OUTPUT_BUFFER))

/**
  * Class object declaration.
  */
typedef struct _AI_SC_DPUClass
{
  /**
    * IDPU2_t class virtual table.
    */
  IDPU2_vtbl vtbl;

} AI_SC_DPUClass_t;


/* Objects instance */
/********************/

/**
  * The class object.
  */
static const AI_SC_DPUClass_t sTheClass =
{
  /* class virtual table */
  {
    ADPU2_vtblAttachToDataSource,
    ADPU2_vtblDetachFromDataSource,
    ADPU2_vtblAttachToDPU,
    ADPU2_vtblDetachFromDPU,
    ADPU2_vtblDispatchEvents,
    ADPU2_vtblRegisterNotifyCallback,
    AI_SC_DPU_vtblProcess
  }
};


/* Private member function declaration */
/***************************************/
static sys_error_code_t AI_SC_DPU_ConfigureInData(IAI_Network_t *p_model, EMData_t *const p_in_emdata);
static sys_error_code_t AI_SC_DPU_ConfigureOutData(IAI_Network_t *p_model, EMData_t *const p_out_emdata);
static inline uint16_t AI_SC_DPU_AIBufferType2EMDataType(ai_buffer_format data_format);
static inline sys_error_code_t AI_SC_DPU_AllocAIOutBuffer(void **p_ai_output_buff, uint16_t ai_data_type,
                                                          uint8_t num_element);
static inline void AI_SC_DPU_AIOutBuffer2EMData(ai_buffer *const p_ai_output, EMData_t *const p_emdata_output,
                                                uint16_t ai_outputs_buffer_dim);

/* Inline function forward declaration */
/***************************************/


/* IDPU2 virtual functions definition */
/**************************************/

sys_error_code_t AI_SC_DPU_vtblProcess(IDPU2_t *_this, EMData_t in_data, EMData_t out_data)
{
  assert_param(_this != NULL);
  uint16_t ai_outputs_buffer_dim = 0u;
  AI_SC_DPU_t *p_obj = (AI_SC_DPU_t *) _this;
  AAI_Network_t *p_model_obj = (AAI_Network_t *)p_obj->p_network;
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* Get ai output buffer dimension from report structure */
  ai_outputs_buffer_dim = p_model_obj->report.n_outputs;

  /* Set ai buffer data with incoming EMData*/
  p_model_obj->ai_input.data = EMD_1dDataAt(&in_data, 0);

  /* Run model */
  IAI_Network_Run(p_obj->p_network, &p_model_obj->ai_input, p_model_obj->ai_output);

  /* Set EMData output */
  AI_SC_DPU_AIOutBuffer2EMData(p_model_obj->ai_output, &out_data, ai_outputs_buffer_dim);

  return res;
}


/* Public functions definition */
/*******************************/

IDPU2_t *AI_SC_DPU_Alloc()
{
  IDPU2_t *p_obj = (IDPU2_t *) SysAlloc(sizeof(AI_SC_DPU_t));

  if (p_obj != NULL)
  {
    p_obj->vptr = &sTheClass.vtbl;
  }

  return p_obj;
}

IDPU2_t *AI_SC_DPU_StaticAlloc(void *p_mem_block)
{
  AI_SC_DPU_t *p_obj = (AI_SC_DPU_t *)p_mem_block;

  if (p_obj != NULL)
  {
    ((IDPU2_t *)p_obj)->vptr = &sTheClass.vtbl;
  }

  return (IDPU2_t *)p_obj;
}

sys_error_code_t AI_SC_DPU_Init(AI_SC_DPU_t *_this, IAI_Network_t *p_network)
{
  assert_param(_this != NULL);
  assert_param(p_network != NULL);

  AAI_Network_t *p_model_Obj = (AAI_Network_t *)p_network;
  uint16_t num_of_inputs_buffer;
  uint16_t num_of_outputs_buffer;
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /* create and initialize the network*/
  _this->p_network = p_network;

  /* define the in_data and out_data for the DPU*/
  EMData_t in_data, out_data;

  /* Get input buffer pointer and dimension */
  num_of_inputs_buffer = p_model_Obj->report.n_inputs;

  /* Get output buffer pointer and dimension */
  num_of_outputs_buffer = p_model_Obj->report.n_outputs;

  /* Right now, only model with single input buffer and maximum 2 output buffer are managed.
   *
   * IMPORTANT:
   * In case of a model with two output buffer they will be interpreted in the following way:
   * first output buffer: it will contain the output class index
   * second output buffer: it will contain the output class probability
   */

  if (AI_SC_DPU_CHECK_MODEL_TOPOLOGY(num_of_inputs_buffer, num_of_outputs_buffer))
  {
    /* */
    (void) AI_SC_DPU_ConfigureInData(p_network, &in_data);
    (void) AI_SC_DPU_ConfigureOutData(p_network, &out_data);

    /* initialize the base class*/
    res = ADPU2_Init((ADPU2_t *) _this, in_data, out_data);
  }
  else
  {
    /* Error res*/
  }

  return res;
}

/* Private function definition */
/*******************************/
/**
  * Configure AI input buffer internal class param with the data coming from the generated model.
  * The AI input buffer will be used during the process phase.
  *
  * The function will return the expected input EMData configuration useful to initialize the DPU Class.
  *
  * @param p_model pointer to AI Supervised Classification Model
  * @param p_in_emdata [OUT] pointer to EMData type
  *
  * @return SYS_NO_ERROR_CODE in case of success, error otherwise.
  */
static sys_error_code_t AI_SC_DPU_ConfigureInData(IAI_Network_t *p_model, EMData_t *const p_in_emdata)
{
  assert_param(p_model != NULL);
  assert_param(p_in_emdata != NULL);

  uint32_t input_buffer_num_ele = 0u;
  uint16_t em_data_in_type = 0u;
  ai_buffer *p_ai_input_buffer = NULL;
  sys_error_code_t ret_val = SYS_IAIN_NULL_PTR_ERROR_CODE;

  AAI_Network_t *p_model_Obj = (AAI_Network_t *)p_model;

  /* Get input buffer pointer */
  p_ai_input_buffer = IAI_Network_InputsGet(p_model, NULL);

  if (NULL != p_ai_input_buffer)
  {
    /* Configure ai_buffer internal class param with the data coming from the model*/
    p_model_Obj->ai_input = p_ai_input_buffer[0];

    /* Get number of elements in the ai input buffer structure */
    input_buffer_num_ele = AI_BUFFER_SIZE(p_ai_input_buffer);

    /* Convert ai buffer data type into EMData type*/
    em_data_in_type = AI_SC_DPU_AIBufferType2EMDataType(AI_BUFFER_FORMAT(p_ai_input_buffer));

    /* Initialize AI DPU EMData input*/
    EMD_1dInit(p_in_emdata, NULL, em_data_in_type, input_buffer_num_ele);

    ret_val = SYS_NO_ERROR_CODE;
  }

  return ret_val;
}

/**
  * Configure AI outputs buffer internal class param with the data coming from the generated model.
  * The AI outputs buffer will be used during the process phase.
  *
  * The function will return the expected output EMData configuration useful to initialize the DPU Class.
  *
  * @param p_model pointer to AI Supervised Classification Model
  * @param p_out_emdata [OUT] pointer to EMData type
  *
  * @return SYS_NO_ERROR_CODE in case of success, error otherwise.
  */
static sys_error_code_t AI_SC_DPU_ConfigureOutData(IAI_Network_t *p_model, EMData_t *const p_out_emdata)
{
  assert_param(p_out_emdata != NULL);
  assert_param(p_model != NULL);

  uint8_t buffer_num_ele = 0u;
  uint8_t total_buffers_num_ele = 0u;
  ai_buffer *p_ai_output_buffer = NULL;
  uint16_t outputs_buffer_dim;
  uint16_t em_data_out_type;
  sys_error_code_t ret_val = SYS_IAIN_NULL_PTR_ERROR_CODE;

  AAI_Network_t *p_model_Obj = (AAI_Network_t *) p_model;

  /* Get output buffer pointer and dimension */
  p_ai_output_buffer = IAI_Network_OutputsGet(p_model, &outputs_buffer_dim);

  if (NULL != p_ai_output_buffer)
  {
    ai_buffer_format data_format;

    /*
     * Cycling on AI output buffer array to get the number of element for
     * each output buffer item and the data type
     */
    for (uint8_t out_buff_cnt = 0u; out_buff_cnt < outputs_buffer_dim; out_buff_cnt++)
    {
      buffer_num_ele = p_ai_output_buffer[out_buff_cnt].size;

      /* IMPORTATNT:
       * We are assuming that in case of multiple output, they have the same data output type
       */

      data_format = AI_BUFFER_FORMAT(&p_ai_output_buffer[out_buff_cnt]);

      em_data_out_type = AI_SC_DPU_AIBufferType2EMDataType(data_format);

      /* Set AI output buffer internal variable */
      p_model_Obj->ai_output[out_buff_cnt] = p_ai_output_buffer[out_buff_cnt];

      /* Alloc AI output buffer used during process phase */
      ret_val = AI_SC_DPU_AllocAIOutBuffer(&(p_model_Obj->ai_output[out_buff_cnt].data), AI_BUFFER_FMT_GET_TYPE(data_format), buffer_num_ele);

      total_buffers_num_ele += buffer_num_ele;
    }

    if (SYS_NO_ERROR_CODE == ret_val)
    {
      /* Multiple output model are mapped on mono-dimensional EMData */
      EMD_1dInit(p_out_emdata, NULL, em_data_out_type, total_buffers_num_ele);
    }

  }
  return ret_val;
}

/**
  * Convert AI buffer data type to expected DPU EMData type
  *
  * The function will receive an ai_buffer data format and will return the associated EMData type.
  *
  * @param data_format [IN] ai_buffer data format
  *
  * @return Converted EMData type
  */
static inline uint16_t AI_SC_DPU_AIBufferType2EMDataType(ai_buffer_format data_format)
{
  uint16_t em_data_type;
  uint32_t  ai_data_format_type = AI_BUFFER_FMT_GET_TYPE(data_format);

  switch (ai_data_format_type)
  {
    case AI_BUFFER_FMT_TYPE_FLOAT:
    {
      em_data_type = E_EM_FLOAT;
    }
    break;

    case AI_BUFFER_FMT_TYPE_BOOL:
    {
      em_data_type = E_EM_UINT8;
    }
    break;

    case AI_BUFFER_FMT_TYPE_Q:
    {
      if (AI_BUFFER_FMT_GET_SIGN(data_format))
      {
        em_data_type = E_EM_INT8;
      }
      else
      {
        em_data_type = E_EM_UINT8;
      }
    }
    break;

    default:
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AI_SC DPU: warning: unsopprted ai data format type. Default to float.\r\n"));

      em_data_type = E_EM_FLOAT;
      break;
  }

  return em_data_type;
}

/**
  * Alloc ai_output buffer data used during AI_SC_DPU process method
  *
  * @param ai_data_type [IN]
  * @param p_ai_output_buff [IN] pointer to ai_buffer
  * @param num_element [IN] buffer number of elements
  *
  * @return Converted EMData type
  */
static inline sys_error_code_t AI_SC_DPU_AllocAIOutBuffer(void **p_ai_output_buff, uint16_t ai_data_type,
                                                          uint8_t num_element)
{
  assert_param(p_ai_output_buff != NULL);
  sys_error_code_t ret_val = SYS_IAIN_NULL_PTR_ERROR_CODE;

  *p_ai_output_buff = NULL;

  switch (ai_data_type)
  {
    case AI_BUFFER_FMT_TYPE_BOOL:
    case AI_BUFFER_FMT_TYPE_Q:
    {
      *p_ai_output_buff = (void *)SysAlloc(sizeof(uint8_t) * num_element);
    }
    break;

    case AI_BUFFER_FMT_TYPE_FLOAT:
    {
      *p_ai_output_buff = (void *)SysAlloc(sizeof(float_t) * num_element);
    }
    break;

    default:
      break;
  }

  if (NULL != *p_ai_output_buff)
  {
    ret_val = SYS_NO_ERROR_CODE;
  }

  return ret_val;
}

/**
  * Convert output ai_buffer data into AI DPU EMData type
  *
  * @param p_ai_output [IN] pointer to output ai_buffer
  * @param p_emdata_output [OUT] pointer to EMData output
  * @param ai_outputs_buffer_dim [IN] number of ai_buffer output
  *
  */
static inline void AI_SC_DPU_AIOutBuffer2EMData(ai_buffer *const p_ai_output, EMData_t *const p_emdata_output,
                                                uint16_t ai_outputs_buffer_dim)
{
  assert_param(p_ai_output != NULL);
  assert_param(p_emdata_output != NULL);

  float_t *p_ai_model_output_0 = NULL;
  float_t *p_ai_model_output_1 = NULL;
  float_t *p_emdata_out_idx        = (float_t *)EMD_1dDataAt(p_emdata_output, 0);
  float_t *p_emdata_out_probabilty = (float_t *)EMD_1dDataAt(p_emdata_output, 1);

  p_ai_model_output_0 = (float_t *)p_ai_output[0].data;

  if (MAX_NUM_OF_OUTPUT_BUFFER == ai_outputs_buffer_dim)
  {
    p_ai_model_output_1 = (float_t *)p_ai_output[1].data;

    /* Set EMData Output*/
    *p_emdata_out_idx = *p_ai_model_output_0;
    *p_emdata_out_probabilty = p_ai_model_output_1[(uint16_t) * p_emdata_out_idx];
  }
  else
  {
    /*Search max output probability */
    float_t max_output_model_probability = *p_ai_model_output_0;
    uint16_t output_class_idx = 0u;

    for (uint16_t buff_ele_idx = 1u; buff_ele_idx < AI_BUFFER_SIZE(&p_ai_output[0]); buff_ele_idx++)
    {
      if (p_ai_model_output_0[buff_ele_idx] > max_output_model_probability)
      {
        max_output_model_probability = p_ai_model_output_0[buff_ele_idx];
        output_class_idx = buff_ele_idx;
      }
    }

    /* Set EMData Output */
    *p_emdata_out_idx = (float_t)(output_class_idx);
    *p_emdata_out_probabilty = max_output_model_probability;
  }
}


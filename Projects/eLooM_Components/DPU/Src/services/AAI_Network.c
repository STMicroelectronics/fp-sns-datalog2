/**
  ********************************************************************************
  * @file    AAI_Network.c
  * @author  STMicroelectronics - AIS - MCD Team
  * @version 1.0.0
  * @date    Oct 21, 2022
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

#include "services/AAI_Network.h"
#include "services/AAI_Network_vtbl.h"
#include "services/sysmem.h"
#include "services/sysdebug.h"
#include "services/syserror.h"


#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_AIN, level, message)


/**
  * Class object declaration.
  */
typedef struct _AAI_NetworkClass
{

} AAI_NetworkClass_t;


/* Objects instance */
/********************/


/* Private member function declaration */
/***************************************/

/**
  * Print the content of an ai_buffer into a file.
  * This method is not reentrant.
  *
  * @param _this [IN] specifies a pointer to a network object.
  * @param buffer [IN] specifies an buffer of type ai_buffer.
  * @param p_channel [IN] specifies the destination file for the report.
  */
static void AAI_Network_PrintBufferInfo(AAI_Network_t *_this, const ai_buffer *buffer, FILE *p_channel);


/* Inline function forward declaration */
/***************************************/

/**
  * Decode and print the AI data type into a file.
  * This method is not reentrant.
  *
  * @param _this [IN] specifies a pointer to a network object.
  * @param fmt [IN] specifies the AI data type.
  * @param p_channel  [IN] specifies the destination file for the report.
  */
static inline void AAI_Network_PrintDataType(AAI_Network_t *_this, const ai_buffer_format fmt, FILE *p_channel);


/* Public functions definition */
/*******************************/

sys_error_code_t AAI_Network_Bootstrap(AAI_Network_t *_this, const ai_handle *p_activaiton_buff,
                                       const ai_handle *p_weight_buff)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;
  ai_error ai_res;
  ai_bool ai_repo_res;

  /* Create an instance of the model */
  ai_res = IAI_Network_Create_and_Init((IAI_Network_t *) _this, p_activaiton_buff, p_weight_buff);
  if (ai_res.type != AI_ERROR_NONE)
  {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_IAIN_GENERIC_ERROR_CODE);
    res = SYS_IAIN_GENERIC_ERROR_CODE;

    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("AAIN: network error - type=%i, code=%i", ai_res.type, ai_res.code));
  }
  else
  {
    ai_repo_res = IAI_Network_GetReport((IAI_Network_t *) _this, &(_this->report));

    if (FALSE == ai_repo_res)
    {
      res = SYS_IAIN_GENERIC_ERROR_CODE;
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("AAIN: network report error /n/r"));
    }
  }

  return res;
}

sys_error_code_t AAI_Network_PrintNetworkInfo(AAI_Network_t *_this, FILE *p_channel)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  const ai_network_report *report = &_this->report;
  fprintf(p_channel, "Network informations...\r\n");
  fprintf(p_channel, " model name         : %s\r\n", report->model_name);
  fprintf(p_channel, " model signature    : %s\r\n", report->model_signature);
  fprintf(p_channel, " model datetime     : %s\r\n", report->model_datetime);
  fprintf(p_channel, " compile datetime   : %s\r\n", report->compile_datetime);
  fprintf(p_channel, " runtime version    : %d.%d.%d\r\n",
          report->runtime_version.major,
          report->runtime_version.minor,
          report->runtime_version.micro);
  if (report->tool_revision[0])
  {
    fprintf(p_channel, " Tool revision      : %s\r\n", (report->tool_revision[0]) ? report->tool_revision : "");
  }
  fprintf(p_channel, " tools version      : %d.%d.%d\r\n",
          report->tool_version.major,
          report->tool_version.minor,
          report->tool_version.micro);
  fprintf(p_channel, " complexity         : %lu MACC\r\n", (unsigned long)report->n_macc);
  fprintf(p_channel, " c-nodes            : %d\r\n", (int)report->n_nodes);

  fprintf(p_channel, " map_activations    : %d\r\n", report->map_activations.size);
  for (int idx = 0; idx < report->map_activations.size; idx++)
  {
    const ai_buffer *buffer = &report->map_activations.buffer[idx];
    fprintf(p_channel, "  [%d] ", idx);
    AAI_Network_PrintBufferInfo(_this, buffer, p_channel);
    fprintf(p_channel, "\r\n");
  }

  fprintf(p_channel, " n_inputs/n_outputs : %u/%u\r\n", report->n_inputs,
          report->n_outputs);

  for (int i = 0; i < report->n_inputs; i++)
  {
    // fprintf(p_channel, "  I[%d] %s\r\n", i, aiGetBufferDesc(&report->inputs[i]));
    fprintf(p_channel, "  I[%d] ", i);
    AAI_Network_PrintBufferInfo(_this, &report->inputs[i], p_channel);
    fprintf(p_channel, "\r\n");
  }

  for (int i = 0; i < report->n_outputs; i++)
  {
    //fprintf(p_channel, "  O[%d] %s\r\n", i, aiGetBufferDesc(&report->outputs[i]));
    fprintf(p_channel, "  O[%d] ", i);
    AAI_Network_PrintBufferInfo(_this, &report->outputs[i], p_channel);
    fprintf(p_channel, "\r\n");
  }

  return res;
}

sys_error_code_t AAI_Network_LogNetworkInfo(AAI_Network_t *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  return res;
}


/* Private function definition */
/*******************************/

static void AAI_Network_PrintBufferInfo(AAI_Network_t *_this, const ai_buffer *buffer, FILE *p_channel)
{
  assert_param(_this != NULL);
  assert_param(buffer != NULL);
  assert_param(p_channel != NULL);

  const ai_buffer_format fmt = buffer->format;

  /* shape + nb elem */
  fprintf(p_channel, "(%d,%d,%d,",
          (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_BATCH),
          (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_HEIGHT),
          (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_WIDTH));

  if (AI_BUFFER_SHAPE_SIZE(buffer) == 5)
  {
    fprintf(p_channel, "%d,%d)",
            (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_DEPTH),
            (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_CHANNEL));
  }
  else if (AI_BUFFER_SHAPE_SIZE(buffer) == 6)
  {
    fprintf(p_channel, "%d,%d,%d)",
            (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_DEPTH),
            (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_EXTENSION),
            (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_CHANNEL));
  }
  else
  {
    fprintf(p_channel, "%d)", (int)AI_BUFFER_SHAPE_ELEM(buffer, AI_SHAPE_CHANNEL));
  }

  fprintf(p_channel, "%d/", (int)AI_BUFFER_SIZE(buffer));

  /* type (+meta_data) */
  AAI_Network_PrintDataType(_this, fmt, p_channel);
  /* quantized info if available */
  if (AI_BUFFER_FMT_GET_TYPE(fmt) == AI_BUFFER_FMT_TYPE_Q)
  {
    if (AI_BUFFER_META_INFO_INTQ(buffer->meta_info))
    {
      ai_u16 s_ = AI_BUFFER_META_INFO_INTQ_GET_SIZE(buffer->meta_info);
      const int max_ = s_ > 4 ? 4 : s_;
      fprintf(p_channel, " %d:", s_);
      for (int idx = 0; idx < max_; idx++)
      {
        ai_float scale = AI_BUFFER_META_INFO_INTQ_GET_SCALE(buffer->meta_info, idx);
        int zero_point = AI_BUFFER_META_INFO_INTQ_GET_ZEROPOINT(buffer->meta_info, idx);
        fprintf(p_channel, "(%f,%d),", (double)scale, zero_point);
      }
      fprintf(p_channel, (s_ > max_) ? ".." : "");
    }
    else if (AI_BUFFER_FMT_GET_BITS(fmt) < 8)
    {
      /* lower of 8b format */
      fprintf(p_channel, " int32-%db", (int)AI_BUFFER_FMT_GET_BITS(fmt));
    }
    else
    {
      fprintf(p_channel, " Q%d.%d",
              (int)AI_BUFFER_FMT_GET_BITS(fmt) - ((int)AI_BUFFER_FMT_GET_FBITS(fmt) + (int)AI_BUFFER_FMT_GET_SIGN(fmt)),
              AI_BUFFER_FMT_GET_FBITS(fmt)
             );
    }
  }
  /* @ + size in bytes */
  if (buffer->data)
    fprintf(p_channel, " @0x%X/%d",
            (int)buffer->data,
            (int)AI_BUFFER_BYTE_SIZE(AI_BUFFER_SIZE(buffer), fmt)
           );
  else
    fprintf(p_channel, " (User Domain)/%d",
            (int)AI_BUFFER_BYTE_SIZE(AI_BUFFER_SIZE(buffer), fmt)
           );
}

/* Inline function definition */
/******************************/

static inline void AAI_Network_PrintDataType(AAI_Network_t *_this, const ai_buffer_format fmt, FILE *p_channel)
{
  assert_param(_this != NULL);
  assert_param(p_channel != NULL);

  if (AI_BUFFER_FMT_GET_TYPE(fmt) == AI_BUFFER_FMT_TYPE_FLOAT)
  {
    fprintf(p_channel, "float%d", (int)AI_BUFFER_FMT_GET_BITS(fmt));
  }
  else if (AI_BUFFER_FMT_GET_TYPE(fmt) == AI_BUFFER_FMT_TYPE_BOOL)
  {
    fprintf(p_channel, "bool%d", (int)AI_BUFFER_FMT_GET_BITS(fmt));
  }
  else     /* integer type */
  {
    fprintf(p_channel, "%s%d", AI_BUFFER_FMT_GET_SIGN(fmt) ? "i" : "u",
            (int)AI_BUFFER_FMT_GET_BITS(fmt));
  }
}

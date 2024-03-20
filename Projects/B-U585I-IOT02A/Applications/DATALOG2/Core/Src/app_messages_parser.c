/**
  ******************************************************************************
  * @file    app_messages_parser.c
  * @author  SRA
  *
  * @brief   Utility function for the application messages.
  *
  * define the API for the application messages.
  *
  *********************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *********************************************************************************
  */

#include "app_messages_parser.h"

uint16_t AppMsgGetSize(uint8_t message_id)
{
  uint16_t msg_size = 0;
  switch (message_id)
  {
    case APP_MESSAGE_ID_GENERIC:
      msg_size = sizeof(struct genericMsg_t);
      break;

    case APP_MESSAGE_ID_NEAI:
      msg_size = sizeof(struct NeaiMessage_t);
      break;

    case APP_REPORT_ID_FORCE_STEP:
      msg_size = sizeof(struct internalReportFE_t);
      break;

    case DPU_MESSAGE_ID_ATTACH_TO_DPU:
    case DPU_MESSAGE_ID_ATTACH_TO_DATA_SRC:
      msg_size = sizeof(struct DPU_MSG_Attach_t);
      break;

    case DPU_MESSAGE_ID_ADD_LISTENER:
    case DPU_MESSAGE_ID_REMOVE_LISTENER:
      msg_size = sizeof(struct DPU_MSG_AddRemoveListener_t);
      break;

    case DPU_MESSAGE_ID_SET_IN_BUFFER:
    case DPU_MESSAGE_ID_SET_OUT_BUFFER:
      msg_size = sizeof(struct DPU_MSG_SetBuffer_t);
      break;

    case DPU_MESSAGE_ID_CMD:
      msg_size = sizeof(struct DPU_MSG_Cmd_t);
      break;

    default:
      msg_size = 0;
  }

  return msg_size;
}

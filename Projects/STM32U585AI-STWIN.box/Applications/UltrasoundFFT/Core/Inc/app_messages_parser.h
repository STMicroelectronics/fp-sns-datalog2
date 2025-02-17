/**
  ******************************************************************************
  * @file    app_messages_parser.h
  * @author  SRA
  * @brief
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#ifndef APP_REPORT_PARSER_H_
#define APP_REPORT_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "services/syserror.h"

#define APP_REPORT_ID_INERTIAL_CMD                      0x01
#define APP_REPORT_ID_CLOUD_CMD                         0x02
#define APP_REPORT_ID_ACOUSTIC_CMD                      0x03
#define APP_REPORT_ID_BLE_CMD                           0x04
#define APP_MESSAGE_ID_NEAI                             0x10  /* Special ID used for the messages class of NanoEdgeAI task. */
#define APP_MESSAGE_ID_UTIL                             0x0B  /* Message ID used for the messages class of Utility task. */
#define APP_REPORT_ID_FORCE_STEP                        0xFE  /* Special ID used by the INIT task to force the execution of ManagedTaskEx step. */

typedef union _APPReport
{
  uint8_t msgId;

  //--------------------------------------------------------------------------------
  //  internalReport 11 (MCU) - Util task command
  //--------------------------------------------------------------------------------

  struct utilMessage_t
  {
    uint8_t msgId; /* Meport ID = 0x0B (11) */
    uint8_t nPadding; /* padding byte */
    uint16_t nCmdID; /* UitilityTask command ID */
    uint32_t nParam; /* optional command parameter. */
  } utilMessage;

  struct acousticDomainReport_t
  {
    uint8_t msgId; /**< Report ID = 0x01 (1) */
    uint16_t nCmdID;
  } acousticDomainReport;

  //--------------------------------------------------------------------------------
  //  internalReport (MCU)
  //--------------------------------------------------------------------------------

  struct internalReportFE_t
  {
    uint8_t msgId; /* Report ID = 0xFE */
    uint8_t nData; /* reserved. It can be ignored */
  } internalReportFE;

} APPReport;

/**
  * Get the size of the report with a specified ID
  *
  * @param nReportID [IN] specifies a report ID
  * @return the size of the report with the specified ID or SYS_INVALID_PARAMETER_ERROR_CODE
  */
uint16_t HidReportGetSize(uint8_t nReportID);

#ifdef __cplusplus
}
#endif

#endif /* APP_REPORT_PARSER_H_ */

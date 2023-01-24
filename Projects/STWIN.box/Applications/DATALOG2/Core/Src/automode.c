/**
 ******************************************************************************
 * @file    automode.c
 * @author  SRA
 * @brief
 *
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

#include "automode.h"
#include "App_model.h"
#include "DatalogAppTask.h"
#include "tx_api.h"

AutomodeModel_t automode_model;
TX_TIMER automode_timer;

/**
 * Callback function called when the software timer expires.
 *
 * @param timer [IN] specifies the handle of the expired timer.
 */
static void automode_timer_callback(ULONG timer);

/* Public functions declaration */
/*********************************/

uint8_t automode_setup(void)
{
  automode_get_enabled(&automode_model.enabled);

  if(automode_model.enabled)
  {
    automode_get_nof_acquisitions(&automode_model.nof_acquisitions);

    if(automode_model.nof_acquisitions >= 0)
    {
      if(automode_model.nof_acquisitions > 0)
      {
        /* automode is a sequence of nof_acquisitions*(datalog+idle) */
        automode_model.nof_acquisitions = 2 * automode_model.nof_acquisitions;
      }
      else if(automode_model.nof_acquisitions == 0)
      {
        /* nof_acquisitions = 0 --> infinite loop --> 2*maximum number of folder supported in SD card standalone mode
         * folder name = STWINBOX_xxxxx --> 100000 */
        automode_model.nof_acquisitions = 2 * 100000;
      }
      automode_get_start_delay_ms(&automode_model.start_delay_ms);
      automode_get_datalog_time_length(&automode_model.datalog_time_length);
      automode_get_idle_time_length(&automode_model.idle_time_length);

      /* If automode is enabled, setup timer to start/stop acquisitions automatically */
      tx_timer_create(&automode_timer, "automode_T", automode_timer_callback, (ULONG) TX_NULL, (automode_model.start_delay_ms), 0, TX_AUTO_ACTIVATE);
    }
    else
    {
      /* nof_acquisition is not a valid value --> stop automode */
      automode_set_enabled(false);
    }

    automode_get_start_delay_ms(&automode_model.start_delay_ms);
    automode_get_datalog_time_length(&automode_model.datalog_time_length);
    automode_get_idle_time_length(&automode_model.idle_time_length);

    /* If automode is enabled, setup timer to start/stop acquisitions automatically */
    tx_timer_create(&automode_timer, "automode_T", automode_timer_callback, (ULONG) TX_NULL, (automode_model.start_delay_ms), 0, TX_AUTO_ACTIVATE);
  }
  return 0;
}

uint8_t automode_forced_stop(void)
{
  tx_timer_deactivate(&automode_timer);
  tx_timer_delete(&automode_timer);
  return 0;
}

/* Private function definition */
/*******************************/

static void automode_timer_callback(ULONG timer)
{
  if(automode_model.nof_acquisitions > 0)
  {
    if((automode_model.nof_acquisitions % 2) == 0) /* even stage --> datalog */
    {
      tx_timer_change(&automode_timer, automode_model.datalog_time_length, 0);
      tx_timer_activate(&automode_timer);
    }
    else /* odd stage --> idle */
    {
      tx_timer_change(&automode_timer, automode_model.idle_time_length, 0);
      tx_timer_activate(&automode_timer);
    }
    DatalogAppTask_msg((ULONG) DT_USER_BUTTON);
    automode_model.nof_acquisitions--;
  }
  else
  {
    tx_timer_deactivate(&automode_timer);
  }
}


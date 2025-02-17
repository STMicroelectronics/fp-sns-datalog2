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

static uint8_t automode_setup_internal(void);

/* Public functions declaration */
/*********************************/
uint8_t automode_setup(void)
{
  uint8_t ret;
  if (automode_setup_internal() == 0)
  {
    /* If automode is enabled, setup timer to start/stop acquisitions automatically */
    ret = tx_timer_create(&automode_timer, "automode_T", automode_timer_callback, (ULONG) TX_NULL,
                          (automode_model.start_delay_s) * 1000, 0, TX_AUTO_ACTIVATE);
    automode_set_started(true, NULL);
  }
  else
  {
    ret = 1;
  }
  return ret;
}

uint8_t automode_setup_host(void)
{
  uint8_t ret;
  if (automode_setup_internal() == 0)
  {
    /* If automode is enabled, setup timer without activating it */
    ret = tx_timer_create(&automode_timer, "automode_T", automode_timer_callback, (ULONG) TX_NULL,
                          (automode_model.start_delay_s) * 1000, 0, TX_NO_ACTIVATE);
  }
  else
  {
    ret = 1;
  }
  return ret;
}

uint8_t automode_start(void)
{
  uint8_t ret;
  automode_get_enabled(&automode_model.enabled);
  if (automode_model.enabled)
  {
    ret = tx_timer_activate(&automode_timer);
    automode_set_started(true, NULL);
  }
  else
  {
    ret = 1;
  }
  return ret;
}

uint8_t automode_forced_stop(void)
{
  tx_timer_deactivate(&automode_timer);
  tx_timer_delete(&automode_timer);
  automode_set_started(false, NULL);
  return 0;
}

/* Private function definition */
/*******************************/
static uint8_t automode_setup_internal(void)
{
  uint8_t ret;

  automode_get_enabled(&automode_model.enabled);
  if (automode_model.enabled)
  {
    automode_get_nof_acquisitions(&automode_model.nof_acquisitions);

    if (automode_model.nof_acquisitions >= 0)
    {
      if (automode_model.nof_acquisitions > 0)
      {
        /* automode is a sequence of nof_acquisitions*(datalog+idle) */
        automode_model.nof_acquisitions = 2 * automode_model.nof_acquisitions;
      }
      else if (automode_model.nof_acquisitions == 0)
      {
        /* nof_acquisitions = 0 --> infinite loop --> 2*maximum number of folder supported in SD card standalone mode
         * folder name = STWINBOX_xxxxx --> 100000 */
        automode_model.nof_acquisitions = 2 * 100000;
      }
      automode_get_start_delay_s(&automode_model.start_delay_s);
      automode_get_logging_period_s(&automode_model.logging_period_s);
      automode_get_idle_period_s(&automode_model.idle_period_s);
    }
    else
    {
      /* nof_acquisition is not a valid value --> stop automode */
      automode_set_enabled(false, NULL);
    }
    automode_get_start_delay_s(&automode_model.start_delay_s);
    automode_get_logging_period_s(&automode_model.logging_period_s);
    automode_get_idle_period_s(&automode_model.idle_period_s);
    ret = 0;
  }
  else
  {
    ret = 1;
  }
  return ret;
}

static void automode_timer_callback(ULONG timer)
{
  if (automode_model.nof_acquisitions > 0)
  {
    if ((automode_model.nof_acquisitions % 2) == 0) /* even stage --> datalog */
    {
      tx_timer_change(&automode_timer, (automode_model.logging_period_s) * 1000, 0);
      tx_timer_activate(&automode_timer);
    }
    else /* odd stage --> idle */
    {
      tx_timer_change(&automode_timer, (automode_model.idle_period_s) * 1000, 0);
      tx_timer_activate(&automode_timer);
    }
    DatalogAppTask_msg((ULONG) DT_USER_BUTTON);
    automode_model.nof_acquisitions--;
  }
  else
  {
    tx_timer_deactivate(&automode_timer);
    automode_set_started(false, NULL);
  }
}


/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 *
 * This fine defines the main() function and few other functions to integrate
 * the low layer of the firmware with the HAL and the error management.
 * Normally a developer does not need to modify this file.
 * The main application entry points, instead, are defined in the file App.c
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "services/sysinit.h"


/**
 * The main() function is provided by the eLooM framework. It is not recommended to modify
 * this function. The entry points for the application are defined, instead, in the file App.c:
 * - SysLoadApplicationContext()
 * - SysOnStartApplication()
 * - SysGetPowerModeHelper()
 *
 * For more information look at the section **eLooM framework > System initialization** of the development documentation.
 *
 * @retval the application never returns.
 */
int main(void)
{
  /* System initialization. It is responsible of:
   * - the early MCU initialization (the minimum set of HW resources)
   * - create the INIT task, that is the first task running, and the one with the highest priority.
   */
  SysInit(FALSE);

  tx_kernel_enter();

  while(1);
}

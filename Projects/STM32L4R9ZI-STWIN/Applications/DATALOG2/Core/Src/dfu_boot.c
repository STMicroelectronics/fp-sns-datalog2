/**
  ******************************************************************************
  * @file    dfu_iboot.c
  * @author  STMicroelectronics
  * @version 2.0.0
  * @date    July 9, 2025
  *
  * @brief  IBoot interface implementation
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "dfu_boot.h"

/* Private define ------------------------------------------------------------*/
/**
  * Address of the flag in ram to check if the device is meant to boot in DFU mode
  * By default is set to the last word of the available RAM
  * Be careful to not use the last word of the RAM for other purposes
  * Example: in gcc linker script you should subtract 4 bytes to the _estack address
  *  -->  _estack = ORIGIN(RAM) + LENGTH(RAM) - 4;
  */
#define DFU_FLAG_ADDRESS                          0x2009FFFC
#define DFU_FLAG_VALUE                            0xDEADBEEF

/* Address of the STM32 System Memory Bootloader, it depends on STM32 part number */
#define BOOTLOADER_ADDRESS                        0x1FFF0000


/* Private function prototypes -----------------------------------------------*/
sys_error_code_t DfuBoot_Init(IBoot *_this);
boolean_t DfuBoot_CheckJumpTrigger(IBoot *_this);
uint32_t DfuBoot_GetAddress(IBoot *_this);
sys_error_code_t DfuBoot_OnJump(IBoot *_this, uint32_t nAppDress);

/* Private variables ---------------------------------------------------------*/
const IBoot_vtbl vptr =
{
  .Init = DfuBoot_Init,
  .CheckJumpTrigger = DfuBoot_CheckJumpTrigger,
  .GetJumpAddress = DfuBoot_GetAddress,
  .OnJump = DfuBoot_OnJump
};

IBoot DfuBoot =
{
  .vptr = &vptr
};

/* Private user code ---------------------------------------------------------*/
/**
  * Redefinition of SysGetBootIF to return the actual DfuBoot instance
  * This function is called by SysInit to get the boot interface.
  */
IBoot *SysGetBootIF(void)
{
  return &DfuBoot;
}

/**
  * @brief Set a flag in RAM to indicate that the device should boot into DFU mode
  * and reset the system to enter DFU mode.
  */
void DfuBoot_set_flag_and_reset(void)
{
  /* Set flag at the end of RAM to indicate that the device should boot into DFU mode */
  *((uint32_t *) DFU_FLAG_ADDRESS) = DFU_FLAG_VALUE;
  /* Reset the system to enter DFU mode */
  NVIC_SystemReset();
}

/* IBoot virtual functions definition */
/****************************************/

sys_error_code_t DfuBoot_Init(IBoot *_this)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  /*
   * Initialize the bootloader specific resources here if needed.
   * For now, we assume no specific initialization is required.
   */

  return res;
}

boolean_t DfuBoot_CheckJumpTrigger(IBoot *_this)
{
  assert_param(_this != NULL);
  boolean_t trigger = FALSE;

  /* Check the condition to enter DFU mode */
  if (*((uint32_t *) DFU_FLAG_ADDRESS) == DFU_FLAG_VALUE)
  {
    /* Clear flag */
    *((uint32_t *) DFU_FLAG_ADDRESS) = 0;
    trigger = TRUE;
  }

  return trigger;
}

uint32_t DfuBoot_GetAddress(IBoot *_this)
{
  assert_param(_this != NULL);

  return BOOTLOADER_ADDRESS;
}


sys_error_code_t DfuBoot_OnJump(IBoot *_this, uint32_t nAppDress)
{
  assert_param(_this != NULL);
  sys_error_code_t res = SYS_NO_ERROR_CODE;

  // Perform any necessary operations before jumping to the application.
  // This could include stopping peripherals, saving state, etc.

  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
  __enable_irq();
  HAL_RCC_DeInit();
  HAL_DeInit();
  SysTick->CTRL = SysTick->LOAD = SysTick->VAL = 0;
  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

  return res;
}


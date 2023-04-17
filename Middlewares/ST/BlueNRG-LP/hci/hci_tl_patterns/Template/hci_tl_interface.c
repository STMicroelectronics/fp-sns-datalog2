/**
  ******************************************************************************
  * @file    hci_tl_interface_template.c
  * @author  SRA Application Team
  * @brief   This file provides the implementation for all functions prototypes 
  *          for the STM32 BlueNRG HCI Transport Layer interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) ${year} STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Software License Agreement
  * SLA0055, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0055
  *
  ******************************************************************************
  */
#ifdef HCI_TL
#include "hci_tl.h"
#endif
#include "hci_tl_interface.h"
 
void hci_tl_lowlevel_init(void)
{
#ifdef HCI_TL
  tHciIO fops;  
  
  /* USER CODE BEGIN hci_tl_lowlevel_init 1 */
  /* Register IO bus services */
  
  /* USER CODE END hci_tl_lowlevel_init 1 */
  
  hci_register_io_bus(&fops);
  
  /* USER CODE BEGIN hci_tl_lowlevel_init 2 */
  /* Register event irq handler */
  
  /* USER CODE END hci_tl_lowlevel_init 2 */

#endif
  /* USER CODE BEGIN hci_tl_lowlevel_init 3 */
  
  /* USER CODE END hci_tl_lowlevel_init 3 */
}

void hci_tl_lowlevel_isr(void)
{
  /* USER CODE BEGIN hci_tl_lowlevel_isr */
  /* Call hci_notify_asynch_evt() */
  
  /* USER CODE END hci_tl_lowlevel_isr */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


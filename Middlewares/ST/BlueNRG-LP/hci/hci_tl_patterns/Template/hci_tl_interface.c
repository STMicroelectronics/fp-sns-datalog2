/**
  ******************************************************************************
  * @file    hci_tl_interface_template.c
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version 2.0.0
  * @date    25-September-2024
  * @brief   This file provides the implementation for all functions prototypes
  *          for the STM32 BlueNRG HCI Transport Layer interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifdef HCI_TL
#include "hci_tl.h"
#endif /* HCI_TL */

#include "hci_tl_interface.h"

void hci_tl_lowlevel_init(void)
{
#ifdef HCI_TL
  /* USER CODE BEGIN hci_tl_lowlevel_init  */

  /* Register event irq handler */

  /* USER CODE END hci_tl_lowlevel_init  */

#endif /* HCI_TL */
}

void hci_tl_lowlevel_isr(void)
{
  /* USER CODE BEGIN hci_tl_lowlevel_isr */
  /* Call hci_notify_asynch_evt() */

  /* USER CODE END hci_tl_lowlevel_isr */
}



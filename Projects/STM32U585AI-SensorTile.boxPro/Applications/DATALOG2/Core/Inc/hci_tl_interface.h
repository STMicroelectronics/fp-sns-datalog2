/**
  ******************************************************************************
  * @file    hci_tl_interface.h
  * @author  SRA
  * @brief   This file contains all the functions prototypes for the STM32
  *          BlueNRG HCI Transport Layer interface
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HCI_TL_INTERFACE_H
#define __HCI_TL_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "main.h"

/* Exported Defines ----------------------------------------------------------*/

#define HCI_TL_SPI_EXTI_IRQn  EXTI11_IRQn

#define HCI_TL_RST_PORT       BLE_RST_GPIO_Port
#define HCI_TL_RST_PIN        BLE_RST_Pin

/* Exported variables --------------------------------------------------------*/
extern EXTI_HandleTypeDef     hexti11;
#define H_EXTI_11 hexti11


/* Exported Functions --------------------------------------------------------*/
int32_t HCI_TL_SPI_Init(void *pConf);
int32_t HCI_TL_SPI_DeInit(void);
int32_t HCI_TL_SPI_Receive(uint8_t *buffer, uint16_t size);
int32_t HCI_TL_SPI_Send(uint8_t *buffer, uint16_t size);
int32_t HCI_TL_SPI_Reset(void);

int32_t IsDataAvailable(void);

/**
  * @brief  Register hci_tl_interface IO bus services
  *
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_init(void);

/**
  * @brief HCI Transport Layer Low Level Interrupt Service Routine
  *
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_isr(uint16_t nPin);

#ifdef __cplusplus
}
#endif
#endif /* __HCI_TL_INTERFACE_H */


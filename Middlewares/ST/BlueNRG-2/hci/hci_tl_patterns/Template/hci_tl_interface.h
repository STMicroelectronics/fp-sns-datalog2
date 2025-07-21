/**
  ******************************************************************************
  * @file    hci_tl_interface_template.h
  * @author  SRA Application Team
  * @brief   Header file for hci_tl_interface.c
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HCI_TL_INTERFACE_TEMPLATE_H
#define HCI_TL_INTERFACE_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @addtogroup LOW_LEVEL_INTERFACE LOW_LEVEL_INTERFACE
  * @{
  */

/**
  * @defgroup LL_HCI_TL_INTERFACE HCI_TL_INTERFACE
  * @{
  */

/**
  * @defgroup LL_HCI_TL_INTERFACE_TEMPLATE TEMPLATE
  * @{
  */

/* Includes ------------------------------------------------------------------*/

/* Exported Defines ----------------------------------------------------------*/

/* Exported Functions --------------------------------------------------------*/
/**
  * @defgroup LL_HCI_TL_INTERFACE_TEMPLATE_Functions Exported Functions
  * @{
  */
/**
  * @brief  Register event IRQ handlers.
  *         This function must be implemented by the user at the application level.
  *         E.g., in the following, it is provided an implementation example in the case of the SPI:
  *         @code
           void hci_tl_lowlevel_init(void)
           {
             //Register event irq handler
             hal_exti_handle_t *pEXTI = BLE_INT_exti_gethandle();
             HAL_EXTI_RegisterRisingCallback(pEXTI, &hci_tl_lowlevel_isr);
             HAL_EXTI_Start(pEXTI,HAL_EXTI_MODE_INTERRUPT);
           }
  *         @endcode
  *
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_init(void);

/**
  * @brief HCI Transport Layer Low Level Interrupt Service Routine.
  *        The Interrupt Service Routine must be called when the BlueNRG-MS
  *        reports a packet received or an event to the host through the
  *        BlueNRG-MS interrupt line.
  *        E.g. in case of data travelling over SPI
  *        @code
          void hci_tl_lowlevel_isr(void)
          {
            while(is_data_available())
            {
              hci_notify_asynch_evt(NULL);
            }
          }
  *        @endcode
  *        where is_data_available() checks the status of the SPI external interrupt pin
  *        @code
          static int32_t is_data_available(void)
          {
            return (HAL_GPIO_ReadPin(HCI_TL_SPI_EXTI_PORT, HCI_TL_SPI_EXTI_PIN) == GPIO_PIN_SET);
          }
  *        @endcode
  *
  * @param  None
  * @retval None
  */
void hci_tl_lowlevel_isr(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* HCI_TL_INTERFACE_TEMPLATE_H */

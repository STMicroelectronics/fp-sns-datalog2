/**
  ******************************************************************************
  * @file    hci_tl_interface_template.h
  * @author  SRA Application Team
  * @brief   Header file for hci_tl_interface.c
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
 * @brief  Register hci_tl_interface IO bus services and the IRQ handlers.
 *         This function must be implemented by the user at the application level.
 *         E.g., in the following, it is provided an implementation example in the case of the SPI:
 *         @code
           void hci_tl_lowlevel_init(void)
           {
             tHciIO fops;  
  
             //Register IO bus services 
             fops.Init    = HCI_TL_SPI_Init;
             fops.DeInit  = HCI_TL_SPI_DeInit;
             fops.Send    = HCI_TL_SPI_Send;
             fops.Receive = HCI_TL_SPI_Receive;
             fops.Reset   = HCI_TL_SPI_Reset;
             fops.GetTick = BSP_GetTick;
  
             hci_register_io_bus (&fops);
  
             //Register event irq handler 
             HAL_EXTI_GetHandle(&hexti0, EXTI_LINE_0);
             HAL_EXTI_RegisterCallback(&hexti0, HAL_EXTI_COMMON_CB_ID, hci_tl_lowlevel_isr);
             HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
             HAL_NVIC_EnableIRQ(EXTI0_IRQn);
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
            while(IsDataAvailable())
            {        
              hci_notify_asynch_evt(NULL);
            }
          }
 *        @endcode
 *        where IsDataAvailable() checks the status of the SPI external interrupt pin 
 *        @code
          static int32_t IsDataAvailable(void)
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
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


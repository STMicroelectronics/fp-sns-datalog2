/**
  ******************************************************************************
  * @file    STWIN_wifi.h
  * @author  SRA
  *
  *
  * @brief   This file contains the different wifi core resources definitions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

#ifndef __STWIN_WIFI_H_
#define __STWIN_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

// SPI Instance
#define WIFI_SPI_INSTANCE           SPI1
#define WIFI_SPI_CLK_ENABLE()       __SPI1_CLK_ENABLE()
#define WIFI_SPI_IRQn               SPI1_IRQn
#define WIFI_SPI_IRQHandler         SPI1_IRQHandler


// SPI Configuration
#define WIFI_SPI_MODE               SPI_MODE_MASTER
#define WIFI_SPI_DIRECTION          SPI_DIRECTION_2LINES
#define WIFI_SPI_DATASIZE           SPI_DATASIZE_16BIT
#define WIFI_SPI_CLKPOLARITY        SPI_POLARITY_LOW
#define WIFI_SPI_CLKPHASE           SPI_PHASE_1EDGE
#define WIFI_SPI_NSS                SPI_NSS_SOFT
#define WIFI_SPI_FIRSTBIT           SPI_FIRSTBIT_MSB
#define WIFI_SPI_TIMODE             SPI_TIMODE_DISABLED
#define WIFI_SPI_CRCPOLYNOMIAL      0
#define WIFI_SPI_BAUDRATEPRESCALER  SPI_BAUDRATEPRESCALER_8
#define WIFI_SPI_CRCCALCULATION     SPI_CRCCALCULATION_DISABLED

//  Reset Pin:
#define WIFI_SPI_RESET_PIN          GPIO_PIN_6
#define WIFI_SPI_RESET_PORT         GPIOC
#define WIFI_SPI_RESET_CLK_ENABLE() __GPIOC_CLK_ENABLE();

//  Data Ready Pin:
#define WIFI_DATA_READY_PIN           GPIO_PIN_11
#define WIFI_DATA_READY_PORT          GPIOE
#define WIFI_DATA_READY_CLK_ENABLE()  __GPIOE_CLK_ENABLE()

// SCLK:
#define WIFI_SPI_SCLK_PIN           GPIO_PIN_2
#define WIFI_SPI_SCLK_PORT          GPIOG
#define WIFI_SPI_SCLK_CLK_ENABLE()  __GPIOG_CLK_ENABLE()

// MISO (Master Input Slave Output):
#define WIFI_SPI_MISO_PIN           GPIO_PIN_3
#define WIFI_SPI_MISO_PORT          GPIOG
#define WIFI_SPI_MISO_CLK_ENABLE()  __GPIOG_CLK_ENABLE()

// MOSI (Master Output Slave Input):
#define WIFI_SPI_MOSI_PIN           GPIO_PIN_4
#define WIFI_SPI_MOSI_PORT          GPIOG
#define WIFI_SPI_MOSI_CLK_ENABLE()  __GPIOG_CLK_ENABLE()

// NSS/CSN/CS: PA.1
#define WIFI_SPI_CS_PIN             GPIO_PIN_3
#define WIFI_SPI_CS_PORT            GPIOF
#define WIFI_SPI_CS_CLK_ENABLE()    __GPIOF_CLK_ENABLE()

// BOOT
#define WIFI_BOOT_PIN             GPIO_PIN_12
#define WIFI_BOOT_PORT            GPIOF
#define WIFI_BOOT_CLK_ENABLE()    __GPIOF_CLK_ENABLE()



/* Exported functions ------------------------------------------------------- */
void  SPI_WIFI_ISR(void);
int32_t wifi_probe(void **ll_drv_obj);


#ifdef __cplusplus
}
#endif

#endif /* __STWIN_WIFI_H_ */




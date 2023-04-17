/**
  @page Beacon sample application for BlueNRG-LP Expansion Board and STM32 Nucleo Boards
  
  @verbatim
  ******************************************************************************
  * @file    Beacon/readme.txt
  * @author  SRA Application Team
  * @brief   This example which shows how to implement an iBeacon device.
  *          The communication is done using a STM32 Nucleo board and a Smartphone
  *          with BLE.
  ******************************************************************************
  *
  * Copyright (c) 2021 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Software License Agreement
  * SLA0055, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0055
  *
  ******************************************************************************
   @endverbatim

  
@par Example Description 

This application shows how to use the BlueNRG-LP STM32 expansion board to 
implement a Beacon device. 

------------------------------------
WARNING: When starting the project from Example Selector in STM32CubeMX and regenerating 
it from ioc file, you may face a build issue. To solve it, remove from the IDE project 
the file stm32l4xx_nucleo.c in Application/User virtual folder and delete from Src and 
Inc folders the files: stm32l4xx_nucleo.c, stm32l4xx_nucleo.h and stm32l4xx_nucleo_errno.h.
------------------------------------

A Beacon device is a smart Bluetooth Low Energy device that transmits a small data 
payload at regular intervals using Bluetooth advertising packets.
Beacons are used to mark important places and objects. Typically, a beacon is 
visible to a user's device from a range of a few meters, allowing for highly 
context-sensitive use cases.
To locate the beacon, it is necessary to have a scanner application running on a 
BLE-capable smartphone, such as "BLE Scanner" app.
The "BLE Scanner" app is available
- for Android at:
  https://play.google.com/store/apps/details?id=com.macdom.ble.blescanner
- for iOS at:
  https://apps.apple.com/it/app/ble-scanner-4-0/id1221763603

@par Keywords

BLE, Beacon, SPI

@par Directory contents

 - app_bluenrg_lp.c       Beacon initialization and applicative code
 
 - hci_tl_interface.c     Interface to the BlueNRG HCI Transport Layer 
 
 - main.c                 Main program body
 
 - stm32**xx_hal_msp.c    Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c         Source code for interrupt Service Routines

 - stm32**xx_nucleo.c     Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c     CMSIS Cortex-Mx Device Peripheral Access Layer
                          System Source File
  
@par Hardware and Software environment

  - This example runs on STM32 Nucleo devices with BlueNRG-LP STM32 expansion board
    (X-NUCLEO-XXXXXXX)
  - This example has been tested with STMicroelectronics:
    - NUCLEO-L476RG RevC board

ADDITIONAL_BOARD : X-NUCLEO-XXXXXXX https://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-xxxxxxx.html
ADDITIONAL_COMP : BlueNRG-LP https://www.st.com/content/st_com/en/products/wireless-connectivity/short-range/bluetooth-low-energy-application-processors/bluenrg-lp.html

@par How to use it? 

In order to make the program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.7.0).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.32.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V8.50.9).
 - Rebuild all files and load your image into target memory.
 - Run the example.
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

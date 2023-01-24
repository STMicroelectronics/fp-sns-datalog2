/**
  @page Central sample application for BlueNRG-2 Expansion Board and STM32 Nucleo Boards
  
  @verbatim
  ******************************************************************************
  * @file    Central/readme.txt  
  * @author  SRA Application Team
  * @brief   This example shows how to implement a BLE Central device able to 
  *          scan the network, to connect to a BLE device, to discover services 
  *          and characteristics and to update the characteristic properties.
  ******************************************************************************
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

  
@par Example Description 

This application shows how to implement a BLE Central device using an STM32 based board 
and a BlueNRG-2 module for Bluetooth Low Energy.
The BLE Central device is able to scan the BLE network, to connect to a BLE 
device (the peripheral), to discover its services and characteristics and to update
the characteristic properties enabling the data notifications, the reading and the 
writing.

The user can communicate with the BLE central device through a serial console, setting 
the Speed to 115200 baud/sec, where both the command menu and the peripheral information 
are printed.

This application requires the following linker settings:
 - CSTACK minimum size 0x600
 - HEAP minimum size 0x200

@par Keywords

BLE, Central, Master, SPI

@par Directory contents

 - app_bluenrg_2.c        Central initialization and applicative code
 
 - central.c              Central device init and state machine implementation
 
 - console.c              Manage the console for the log messages
 
 - hci_tl_interface.c     Interface to the BlueNRG HCI Transport Layer 
 
 - main.c                 Main program body
 
 - st_data_parser.c       Utilities for parsing data from ST BLE characteristics
 
 - stm32**xx_hal_msp.c    Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c         Source code for interrupt Service Routines

 - stm32**xx_nucleo.c     Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c     CMSIS Cortex-Mx Device Peripheral Access Layer
                          System Source File
  
@par Hardware and Software environment

  - This example runs on STM32 Nucleo devices with BlueNRG-2 STM32 expansion board
    (X-NUCLEO-BNRG2A1)
  - This example has been tested with STMicroelectronics:
    - NUCLEO-L476RG RevC board

ADDITIONAL_BOARD : X-NUCLEO-BNRG2A1 https://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-bnrg2a1.html
ADDITIONAL_COMP : BlueNRG-M2SP https://www.st.com/content/st_com/en/products/wireless-connectivity/short-range/bluetooth-low-energy-application-processors/bluenrg-m2.html
  
@par How to use it? 

In order to make the program work, you must do the following:
 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.9.0).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.32.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V9.20.1).
 - Rebuild all files and load your image into target memory.
 - Run the example.
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

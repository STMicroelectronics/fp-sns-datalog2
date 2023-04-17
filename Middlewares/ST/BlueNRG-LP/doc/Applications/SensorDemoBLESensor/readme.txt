/**
  @page SensorDemo_BLESensor-App sample application for BlueNRG-LP Expansion Board and STM32 Nucleo Boards
  
  @verbatim
  ******************************************************************************
  * @file    SensorDemo_BLESensor-App/readme.txt  
  * @author  SRA Application Team
  * @brief   This application contains an example which shows how to implement
  *          proprietary Bluetooth Low Energy profiles.
  *          The communication is done using a STM32 Nucleo board and a 
  *          Smartphone with BTLE.
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

This application shows how to implement a peripheral device tailored for 
interacting with the "ST BLE Sensor" app for Android/iOS devices.

------------------------------------
WARNING: When starting the project from Example Selector in STM32CubeMX and regenerating 
it from ioc file, you may face a build issue. To solve it, remove from the IDE project 
the file stm32l4xx_nucleo.c in Application/User virtual folder and delete from Src and 
Inc folders the files: stm32l4xx_nucleo.c, stm32l4xx_nucleo.h and stm32l4xx_nucleo_errno.h.
------------------------------------

The "ST BLE Sensor" app is freely available on both GooglePlay and iTunes
  - iTunes: https://itunes.apple.com/us/app/st-bluems/id993670214
  - GooglePlay: https://play.google.com/store/apps/details?id=com.st.bluems
The source code of the "ST BLE Sensor" app is available on GitHub at:
  - iOS: https://github.com/STMicroelectronics-CentralLabs/STBlueMS_iOS
  - Android: https://github.com/STMicroelectronics-CentralLabs/STBlueMS_Android

@note: NO SUPPORT WILL BE PROVIDED TO YOU BY STMICROELECTRONICS FOR ANY OF THE
ANDROID/iOS app INCLUDED IN OR REFERENCED BY THIS PACKAGE.

After establishing the connection between the STM32 board and the smartphone:
  - the emulated values of temperature and pressure are sent by the STM32 board to 
    the mobile device and are shown in the ENVIRONMENTAL tab;
  - the emulated sensor fusion data sent by the STM32 board to the mobile device 
    reflects into the cube rotation showed in the app's MEMS SENSOR FUSION tab
  - the plot of the emulated data (temperature, pressure, sensor fusion, 
    accelerometer, gyroscope and magnetometer), sent by the STM32 board, are shown in the 
	PLOT DATA tab;
  - in the RSSI & Battery tab the RSSI value is shown.
According to the value of the #define USE_BUTTON in file app_bluenrg_2.c, the 
environmental and the motion data can be sent automatically (with 1 sec period) 
or when the User Button is pressed.

The communication is done using a vendor specific profile.


@par Keywords

BLE, Peripheral, SPI

@par Directory contents

 - app_bluenrg_lp.c       SensorDemo_BLESensor-App initialization and applicative code
 
 - gatt_db.c              Functions for building GATT DB and handling GATT events
 
 - hci_tl_interface.c     Interface to the BlueNRG HCI Transport Layer 
 
 - main.c                 Main program body
 
 - sensor.c               Sensor init and state machine
 
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
 - Rebuild all files and load your image into target memory
 - Run the example.
 - Alternatively, you can download the pre-built *.bin in "Binary" folder
   included in the distributed package.


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

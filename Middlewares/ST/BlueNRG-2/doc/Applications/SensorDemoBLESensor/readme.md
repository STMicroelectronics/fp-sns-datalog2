
## <b>SensorDemo_BLESensor-App Application Description</b>
  
This application contains an example which shows how to implement proprietary Bluetooth Low Energy profiles.
The communication is done using a STM32 Nucleo board and a Smartphone with BTLE.
  
Example Description:

This application shows how to implement a peripheral device tailored for 
interacting with the "ST BLE Sensor" app for Android/iOS devices.

The "ST BLE Sensor" app is freely available on both GooglePlay and iTunes
  - iTunes: https://itunes.apple.com/us/app/st-bluems/id993670214
  - GooglePlay: https://play.google.com/store/apps/details?id=com.st.bluems
The source code of the "ST BLE Sensor" app is available on GitHub at:
  - iOS: https://github.com/STMicroelectronics-CentralLabs/STBlueMS_iOS
  - Android: https://github.com/STMicroelectronics-CentralLabs/STBlueMS_Android

__Note__: NO SUPPORT WILL BE PROVIDED TO YOU BY STMICROELECTRONICS FOR ANY OF THE
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

__VERY IMPORTANT NOTES__ for FOTA Feature (only for STM32L476RG):

 1) For the STM32L476RG MCU, this example support the Firmware-Over-The-Air (FOTA) 
    update using the ST BLE Sensor Android/iOS application (Version 3.0.0 or higher) 
 
 2) This example must run starting at address 0x08004000 in memory and works ONLY if the BootLoader 
    is saved at the beginning of the FLASH (address 0x08000000)
 	
 3) When generating a SensorDemo_BLESensorApp project for the STM32L476RG MCU with the STM32CubeMX, to 
    correctly run the FOTA feature, the following modifications to the code and the project are required 
	before building:

	3.1) In file Src/system_stm32l4xx.c, enable the #define USER_VECT_TAB_ADDRESS and set the VECT_TAB_OFFSET to 0x4000 (the default value is 0x00)
	     #define VECT_TAB_OFFSET  0x4000 /*!< Vector Table base offset field.
                                              This value must be a multiple of 0x200. */
											
    3.2) In your IDE, set the .intvec and ROM start addresses to 0x08004000, following the instructions below:
	     - EWARM
	       Project -> Options --> Linker --> Config --> Edit
            - Vector Table --> .intvec start 0x08004000
            - Memory Regions --> ROM: Start 0x08004000 - End 0x080FFFFF
         - MDK-ARM 
	       Project -> Options -> Target 
            - IROM1: 0x8004000
         - STM32CubeIDE
	       Open linker script file STM32L476RGTX_FLASH.ld and set the Flash origin address to:
           /* Memories definition */
           MEMORY
           {
             RAM    (xrw)    : ORIGIN = 0x20000000,   LENGTH = 96K
             RAM2    (xrw)    : ORIGIN = 0x10000000,   LENGTH = 32K
             FLASH    (rx)    : ORIGIN = 0x8004000,   LENGTH = 1024K
           }

 4) For debugging activity (Optimizations=None), increase the value of cstack (e.g. from 0x400 to 0x800)
    and heap (e.g. from 0x200 to 0x400)
	
 5) For each IDE (IAR/Keil uVision/STM32CubeIDE) a *.bat/*.sh script file (in folder GenSensorDemoBin_Script 
    contained in the archive Utilities\BootLoader\BootLoader.zip) must be used for running the following 
	operations on the STM32L476RG MCU:
    - Full Flash Erase
    - Load the BootLoader (in folder STM32L476RG_BL contained in the archive Utilities\BootLoader\BootLoader.zip) 
	  on the correct flash region
    - Load the Program (after the compilation) on the correct flash region (this could be used for a FOTA)
    - Dump back one single binary that contain BootLoader+Program that could be 
      flashed at the flash beginning (address 0x08000000) (this COULD BE NOT used for FOTA)
    - Reset the board
	To easily use these scripts, extract the BootLoader.zip archive in the root folder of your 
	SensorDemo_BLESensor-App sample application.
    Before launching the script for your IDE, open it and set the correct paths and filenames.

### <b>Keywords</b>

BLE, Peripheral, SPI

### <b>Directory contents</b>

 - app_bluenrg_2.c        SensorDemo_BLESensor-App initialization and applicative code
 
 - gatt_db.c              Functions for building GATT DB and handling GATT events
 
 - hci_tl_interface.c     Interface to the BlueNRG HCI Transport Layer 
 
 - main.c                 Main program body
 
 - OTA.c                  Over-the-Air Update API implementation
 
 - sensor.c               Sensor init and state machine
 
 - stm32**xx_hal_msp.c    Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c         Source code for interrupt Service Routines

 - stm32**xx_nucleo.c     Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c     CMSIS Cortex-Mx Device Peripheral Access Layer System Source File

 - target_platform.c      Get information on the target device memory
  
### <b>Hardware and Software environment</b>

  - This example runs on STM32 Nucleo devices with BlueNRG-2 STM32 expansion board
    (X-NUCLEO-BNRG2A1)
  - This example has been tested with STMicroelectronics:
    - NUCLEO-L476RG RevC board

ADDITIONAL_BOARD : X-NUCLEO-BNRG2A1 https://www.st.com/en/ecosystems/x-nucleo-bnrg2a1.html
ADDITIONAL_COMP : BlueNRG-M2SP https://www.st.com/en/wireless-connectivity/bluenrg-m2.html
  
### <b>How to use it?</b>

In order to make the program work, you must do the following:

 - WARNING: before opening the project with any toolchain be sure your folder
   installation path is not too in-depth since the toolchain may report errors
   after building.
   
 - Open STM32CubeIDE (this firmware has been successfully tested with Version 1.9.0).
   Alternatively you can use the Keil uVision toolchain (this firmware
   has been successfully tested with V5.32.0) or the IAR toolchain (this firmware has 
   been successfully tested with Embedded Workbench V9.20.1).
   
 - Rebuild all files and 
 
   - for STM32L476RG MCU: run the .bat/.sh script included in the IDE folder 
     (EWARM/MDK-ARM/STM32CubeIDE)
	 
   - for other STM32 MCU: load your image into target memory
   
 - Run the example.
 
 - Alternatively, you can download the pre-built .bin in "Binary" folder
   included in the distributed package (for STM32L476RG MCU, the _BL.bin file).

### <b>Author</b>

SRA Application Team

### <b>License</b>

Copyright (c) 2022 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.

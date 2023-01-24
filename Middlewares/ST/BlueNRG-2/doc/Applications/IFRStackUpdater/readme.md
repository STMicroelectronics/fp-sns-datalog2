
## <b>IFRStack_Updater Application Description</b>
  
This application is an example to be loaded in order to update the BlueNRG-2 fw stack to 
latest version or to change the IFR configuration.

Example Description:

This application should be used to update the BlueNRG-2 firmware stack to latest version 
or to change the IFR configuration.
To change the IFR configuration the #define BLUENRG_DEV_CONFIG_UPDATER must be defined.
The IFR parameters can be changed editing the file Middlewares\ST\BlueNRG-2\hci\bluenrg1_devConfig.c.

Note: Be sure you know what you are doing when modifying the IFR settings.
      This operation should be performed only by expert users. 

To update the BlueNRG-2 fw stack the #define BLUENRG_STACK_UPDATER must be
defined (see file app_bluenrg_2.h).
The FW image used by the application is contained in the FW_IMAGE array defined in 
file update_fw_image.c. In the same file the DTM SPI and FW versions are reported.

For some STM32 MCUs with low flash size, when the BLUENRG_STACK_UPDATER is defined, 
a linking error may be faced. 
For such STM32 MCUs, the DTM image can be updated using a serial terminal (e.g. HyperTerminal 
or TeraTerm) and its transfer feature based on the YMODEM.
To enable this configuration the #define BLUENRG_STACK_UPDATER_YMODEM must be defined 
in file app_bluenrg_2.h (defined by default).
In this configuration, if for instance the TeraTerm is used, after loading the binary file 
on the STM32:
 - open a connection 
   Speed: 115200, Data: 8 bit, Parity: None, Stop bits: 1, Flow control: none
   New-line: Receive=AUTO, Transmit=CR
 - go to File -> Transfer -> YMODEM -> Send
 - select the DTM SPI binary file to load from the PC's file system
   (e.g. the DTM_SPI_NOUPDATER.bin contained in the installation folder of the ST's BlueNRG GUI
   C:\Users\<username>\ST\BlueNRG GUI X.Y.Z\Firmware\BlueNRG2\DTM)

After loading the IFR/Stack Updater application on a STM32 Nucleo board equipped 
with a X-NUCLEO-BNRG2A1 expansion board: 
  - the LD2 LED on indicates that the updating process has started and is running
  - the slowly blinking LD2 LED (T = 3 secs) indicates that the updating process 
    has failed
  - the fast blinking LD2 LED (T = 0.5 secs) indicates that the updating process 
    has successfully terminated

This application project requires: 
  - CSTACK minimum size 0xD00
  - HEAP minimum size 0x200

### <b>Keywords</b>

BLE, SPI, YMODEM

### <b>Directory contents</b>

 - app_bluenrg_2.c        IFRStack_Updater initialization and applicative code
 
 - bluenrg_2_utils.c      Utilities for the BlueNRG-2 programming
 
 - hci_tl_interface.c     Interface to the BlueNRG HCI Transport Layer 
 
 - main.c                 Main program body
 
 - stm32**xx_hal_msp.c    Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c         Source code for interrupt Service Routines

 - stm32**xx_nucleo.c     Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c     CMSIS Cortex-Mx Device Peripheral Access Layer System Source File

 - update_fw_image.c      BlueNRG-2 FW image in hex format
 
 - ymodem.c               Utilities for the ymodem protocol management
  
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
   
 - Rebuild all files and load your image into target memory.
 
 - Run the example.
 
 - Alternatively, you can download the pre-built binaries in "Binary" 
   folder included in the distributed package.

### <b>Author</b>

SRA Application Team

### <b>License</b>

Copyright (c) 2022 STMicroelectronics.
All rights reserved.

This software is licensed under terms that can be found in the LICENSE file
in the root directory of this software component.
If no LICENSE file comes with this software, it is provided AS-IS.

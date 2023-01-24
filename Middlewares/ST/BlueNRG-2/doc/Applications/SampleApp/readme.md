
## <b>SampleApp Application Description</b>
  
This example shows how to create a client and server connection.

Example Description:

This application shows how to simply use the BLE Stack creating a client and server connection. 
It also provides the user with a complete example on how to perform an ATT MTU exchange procedure, 
in order the server and the client can agree on the supported max MTU.

To test this application two STM32 Nucleo boards with their respective X-NUCLEO-BNRG2A1 
STM32 expansion boards should be used. After flashing both the STM32 boards, one board 
configures itself as BLE Server-Peripheral device, while the other as BLE Client-Central 
device. 
After the connection between the two boards is established (signaled by the LD2 LED
blinking on the Client-Central device), pressing the USER button on one board, the
LD2 LED on the other one gets toggled and vice versa.
If you have only one STM32 Nucleo board, you can use the "BLE Scanner" app as BLE
Client-Central device.

The "BLE Scanner" app is available
- for Android at:
https://play.google.com/store/apps/details?id=com.macdom.ble.blescanner
- for iOS at:
https://apps.apple.com/it/app/ble-scanner-4-0/id1221763603

NOTE: In this example the CHAR VALUE length is set to 63 while the ATT MTU to 158.
For using greater values:
- increase the CHAR_VALUE_LENGTH up to 512 and the CLIENT_MAX_MTU_SIZE up to 247
  (file gatt_db.h)
- increase both the HCI_READ_PACKET_SIZE and HCI_MAX_PAYLOAD_SIZE to 256 
  (file bluenrg_conf.h)
- increase the CSTACK in the IDE project options (0xC00 is enough)

### <b>Keywords</b>

BLE, Master, Slave, Central, Peripheral, SPI

### <b>Directory contents</b>

 - app_bluenrg_2.c        SampleApp initialization and applicative code
 
 - gatt_db.c              Functions for building GATT DB and handling GATT events
 
 - hci_tl_interface.c     Interface to the BlueNRG HCI Transport Layer 
 
 - main.c                 Main program body
 
 - stm32**xx_hal_msp.c    Source code for MSP Initialization and de-Initialization

 - stm32**xx_it.c         Source code for interrupt Service Routines

 - stm32**xx_nucleo.c     Source file for the BSP Common driver 
						
 - stm32**xx_nucleo_bus.c Source file for the BSP BUS IO driver
 
 - system_stm32**xx.c     CMSIS Cortex-Mx Device Peripheral Access Layer System Source File
 
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

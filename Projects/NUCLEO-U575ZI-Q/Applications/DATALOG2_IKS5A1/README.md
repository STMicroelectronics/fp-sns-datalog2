## __DATALOG2_NUCLEO-U575ZI-Q Application__

The **FP-SNS-DATALOG2** function pack provides a comprehensive solution to save data from any combination of sensors and
microphones configured up to the maximum sampling rate.
 
Sensor data can be streamed to a PC via USB (WinUSBclass) using the companion host software (cli_example) provided
for Windows and Linux.

To read sensor data acquired using FP-SNS-DATALOG2, a few easy-to-use scripts in Python are provided
within the software package. The scripts have been successfully tested with Python 3.10.

FP-SNS-DATALOG2 firmware is based on application-level modules (**Sensor Manager**, **EMData**, …) that 
customer can reuse and extend easily to build up its custom application.
These application modules adopt state-of-the-art design patterns, and they support natively low-power modes.
To enable this solution, the function pack has been built on top of **eLooM**, an Embedded Light Object Oriented fraMework 
for STM32 applications properly designed for embedded low power applications powered by STM32.


### __Keywords__

Datalog, Predictive Maintenance, Condition Monitoring, Signal Processing, Industry 4.0, Sensor, Ultrasound


### __Hardware and Software environment__

  - This example runs on STMU5 Devices.

  - This example has been tested with STMicroelectronics NUCLEO-U575ZI-Q
    evaluation boards and can be easily tailored to any other supported
    device and development board. 


### __How to use it?__

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 
 - Rebuild all files and load your image into target memory
 
 - Run the example


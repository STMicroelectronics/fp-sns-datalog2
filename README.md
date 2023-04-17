# FP-SNS-DATALOG2 Firmware Package

![latest tag](https://img.shields.io/github/v/tag/STMicroelectronics/fp-sns-datalog2.svg?color=brightgreen)

The **FP-SNS-DATALOG2** function pack represents an evolution of FP-SNS-DATALOG1 and provides a comprehensive solution for saving
data from any combination of sensors and microphones configured up to the maximum sampling rate.

FP-SNS-DATALOG2 is based on application-level modules ((**Sensor Manager**, **EMData**, …) that a final user can reuse and extend easily
to build up its custom application. These application modules adopt state-of-the-art design patterns, and they support natively low-power modes.
To enable this solution, the function pack has been built on top of **eLooM**, an embedded Light object oriented fraMework for STM32 applications properly designed
for embedded low power applications powered by STM32. 

The FP-SNS-DATALOG2 application allows to store sensor data onto a microSD™ card (secure digital High capacity - SDHC) formatted with the FAT32 file
system, or stream to a PC via USB (WinUSB class) using the companion host software (cli_example) provided for Windows and Linux. It can also configure an
ISM330DHCX and LSM6DSV16X Machine Learning Core unit and the ISM330IS Intelligent Sensor Processing Unit, so to read the output of the selected algorithm.

This application allows downloading the selected configuration from a JSON file to the board as well as starting and controlling data acquisition. The same set
of commands can be operated from a host via Command Line Interface or via Bluetooth using the STBLESensor app which lets you manage the board and sensor configurations,
start/stop data acquisition on SD card and control data labeling. To read and process sensor data acquired using FP-SNS-DATALOG2, a few easy-to-use
scripts in Python are provided within the software package. The scripts have been successfully tested with Python 3.10.

The package also includes an example of ultrasound condition monitoring (**UltrasoundFFT**) for STEVAL-STWINBX1 that calculates the FFT of the IMP23ABSU analog microphone signal and streams
the result to a PC GUI via USB. The microphone sampling rate is set by default to 192 kHz whereas the microphone bandwidth is up to 80 kHz.

This firmware package includes Components Device Drivers, Board Support Package and example applications 
for the Sensortile.box PRO (STEVAL-MKBOXPRO) and the STWIN.box platform (STEVAL-STWINBX1).

![](_htmresc/FP-SNS-DATALOG2_Software_Architecture.jpg)

Here is the list of references to user documents:

- [DB4865](https://www.st.com/resource/en/data_brief/fp-sns-datalog2.pdf) : STM32Cube function pack for high speed datalogging and ultrasound processing
- [UM3106](https://www.st.com/resource/en/user_manual/um3106-getting-started-with-the-stm32cube-function-pack-for-the-stevalstwinbx1-evaluation-kit-for-high-speed-datalogging-and-ultrasound-processing-stmicroelectronics.pdf) : Getting started with the STM32Cube function pack for high speed datalogging and ultrasound processing
- [STM32Cube](https://www.st.com/stm32cube) : STM32Cube

## Known Limitations

- None

## Development Toolchains and Compilers

-   IAR Embedded Workbench for ARM (EWARM) toolchain V9.20.1
-   RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.37
-   STM32CubeIDE v1.12.0

## Supported Devices and Boards

- [STEVAL-MKBOXPRO](https://www.st.com/sensortileboxpro)
- [STEVAL-STWINBX1](https://www.st.com/stwinbox)

## Backward Compatibility

- None

## Dependencies

- None

# FP-AUD-AEC1 Firmware Package

![latest tag](https://img.shields.io/github/v/tag/STMicroelectronics/fp-aud-aec1.svg?color=brightgreen)

**FP-AUD-AEC1** STM32Cube Function Pack for Acoustic Echo Cancellation is a specific
example fully focused on Acoustic Echo Cancellation and provides an implementation 
of a USB smart speaker use case with microphone

The package includes the **STM32 AcousticEC library**,that provides an implementation for a real-time echo 
cancellation routine based on the well-known SPEEX implementation of the MDF 
algorithm. 

The firmware provides implementation example for **NUCLEO-F446RE** board or **NUCLEO-F746ZG** equipped with:

- **X-NUCLEO-CCA01M1**, an expansion board based on the **STA350BW** Sound Terminal® 2.1-channel high-efficiency digital audio output system.
- **X-NUCLEO-CCA02M2**, an evaluation board based on digital MEMS microphones, designed around STMicroelectronics MP34DT06J digital microphones.

![](_htmresc/FP-AUD-AEC1.png)

Here is the list of references to user documents:

- [DB4393](https://www.st.com/resource/en/data_brief/fp-aud-aec1.pdf) : STM32Cube Function Pack for Acoustic Echo Cancellation
- [UM2824](https://www.st.com/resource/en/user_manual/um2824-getting-started-with-the-stm32cube-function-pack-for-acoustic-echo-cancellation-stmicroelectronics.pdf) : Getting started with STM32Cube Function Pack for Acoustic Echo Cancellation
- [STM32Cube](https://www.st.com/stm32cube) : STM32Cube

## Known Limitations

- None

## Development Toolchains and Compilers

-   IAR Embedded Workbench for ARM (EWARM) toolchain V9.20.1
-   RealView Microcontroller Development Kit (MDK-ARM) toolchain V5.37.0
-   STM32CubeIDE Version 1.10.1

## Supported Devices and Boards

- STM32F446RE and STM32F746ZG devices
- [NUCLEO-F446RE](https://www.st.com/en/evaluation-tools/nucleo-f446re.html) Rev C
- [NUCLEO-F746ZG](https://www.st.com/en/evaluation-tools/nucleo-f746zg.html) Rev B
- [X-NUCLEO-CCA01M1](https://www.st.com/en/ecosystems/x-nucleo-cca01m1.html)
- [X-NUCLEO-CCA02M2](https://www.st.com/en/ecosystems/x-nucleo-cca02m2.html)

## Backward Compatibility

- To optimize memory footprint and MCU time consumption, v1.1.0 has been redesigned to acquire the audio signal 
from only 1 microphone.
To do so, a different HW configuration for X-NUCLEO-CCA02M2 is needed, as described in UM2824 (v1.1 and above).

## Dependencies

- None

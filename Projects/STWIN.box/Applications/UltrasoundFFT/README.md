## __UltrasoundFFT_STWIN.box Application__

The **FP-SNS-DATALOG2** function pack includes an example of ultrasound condition monitoring (UltrasoundFFT) that calculates the
FFT of the IMP23ABSU analog microphone signal and streams the result to a PC GUI via USB. The microphone
sampling rate is set by default to 192 kHz whereas the microphone bandwidth is up to 80 kHz.

FP-SNS-DATALOG2 firmware is based on application-level modules (**Sensor Manager**, **EMData**, …) that 
customer can reuse and extend easily to build up its custom application.
These application modules adopt state-of-the-art design patterns, and they support natively low-power modes.
To enable this solution, the function pack has been built on top of **eLooM**, an Embedded Light Object Oriented fraMework 
for STM32 applications properly designed for embedded low power applications powered by STM32.

This firmware package includes Components Device Drivers, Board Support Package and example applications 
for the STWIN.box platform (STEVAL-STWINBX1).  


### __Keywords__

Datalog, Predictive Maintenance, Condition Monitoring, Signal Processing, Industry 4.0, Sensor, Ultrasound


### __Hardware and Software environment__

  - This example runs on STMU5 Devices.

  - This example has been tested with STMicroelectronics STEVAL-STWINBX1
    evaluation boards and can be easily tailored to any other supported
    device and development board. 


### __How to use it?__

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


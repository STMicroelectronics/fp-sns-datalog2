## __cli_example__


The High-Speed DataLog firmware provides an easy way to save data from any combination 
of sensors and microphones configured at their maximum sampling rate.
Sensor data can be either stored on a micro SD Card, SDHC (Secure Digital High Capacity) 
formatted with the FAT32 file system, or streamed to a PC via USB (WinUSB class).

To save data via USB, a command line interface example as executable is available here.
It is available for Windows 32 and 64 bit, macOS, Linux and RaspberryPi 32 and 64 bit platforms.

USB_DataLog_Run.bat and USB_DataLog_Run.sh scripts provide a ready to use example.
They show how to launch and use the provided executables.
You can use the script as it is or modify it.
The application can receive parameters in input:

- timeout (-t)
- device configuration file (-f) 
- UCF file for Machine Learning Core or Intelligent Sensor Processing Unit (-u).

Run the application by double-clicking on the USB_DataLog_Run batch script.
The application starts, and the command line appears, showing information about the connected board. 
Press any key to start the datalogging. Application will stop automatically if a timeout was set
In any case, you can stop the data acquisition by pressing the ESC button
The application will create a YYYYMMDD_HH_MM_SS (i.e., 20200128_16_33_00) folder containing the raw data and the JSON configuration file.

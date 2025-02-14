## __DATALOG2_Utilities__


The ST High-Speed Datalog firmware provides an easy way to save data from any combination of 
sensors and microphones configured at their maximum sampling rate. Sensor data can be either 
stored on a microSD™ card, SDHC (Secure Digital High Capacity) formatted with the FAT32 file 
system, or streamed to a PC via USB (WinUSB class).

HSDPython SDK has been expanded and released as a separate software product. You can download 
it from [here](https://github.com/STMicroelectronics/stdatalog-pysdk). 
**[STDATALOG_PYSDK](https://github.com/STMicroelectronics/stdatalog-pysdk)** is a data-centric 
design and user-friendly Python SDK that can run with hardware boards that supply real-time data 
streams, empowering users with full control of the data acquisition process. It extends and 
substitutes the HSDPython SDK previously available inside the Utilities folder.

To save data via USB, a command-line interface example is available in “Utilities/cli_example”. 
USB_DataLog_Run.bat and USB_DataLog_Run.sh scripts provide a ready-to-use example.

In addition, the BoardReset folder contains a script useful to fully reset STWIN.box and 
SensorTile.box PRO. The script resets the board configuration to the default flash bank and newly 
flashes the DATALOG2 firmware.

WiFi_module_upgrade contains the binary and the step-by-step procedure to upgrade the WiFi module 
firmware. The module must be upgraded to run the DATALOG2 example properly on STWIN.box.

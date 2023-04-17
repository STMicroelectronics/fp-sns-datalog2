## __DATALOG2_Utilities__


The High-Speed DataLog firmware provides an easy way to save data from any combination 
of sensors and microphones configured at their maximum sampling rate.
Sensor data can be either stored on a micro SD Card, SDHC (Secure Digital High Capacity) 
formatted with the FAT32 file system, or streamed to a PC via USB (WinUSB class).

To save data via USB, a command line interface example is available in 
“Utilities/cli_example”.
USB_DataLog_Run.bat and USB_DataLog_Run.sh scripts provide a ready to use example.

Together with HSDatalog application, inside the Utilities folder, Python 
scripts are available to automatically read and plot the data saved by the application. 
The scripts have been successfully tested with Python 3.10. 

Python_SDK folder contains also a "how to use" Jupyter Notebook, with easy-to-use examples
to show the main functionalities of our HSDatalog Python SDK.

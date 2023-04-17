## __How to install HSDPython_SDK in Linux environment__

The HSDPython_SDK provides installer scripts that can be used to properly install the SDK and all the required dependencies.
To install it, follow the above procedure: 

- Open a terminal from HSD_SDK_dev folder

- Create a venv:

	- **python3 -m venv .venv**
	- **source .venv/bin/activate**
	
- Install useful packages (if not present in your system):

	- **sudo apt-get install libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0**
	- **sudo apt-get install libxcb-cursor-dev**
	- **sudo apt-get install dos2unix**
	
- Install Python requirements. 

	- If you are behind a proxy server: **python3 -m pip install -r requirements.txt --proxy=http://usr_name:password@proxyserver_name:port**
		- if you have special characters in your password you will have to replace them with their correspondent hex representation. E.g. & --> %26
	- Else: **python3 -m pip install -r requirements.txt**

- **dos2unix 30-hsdatalog.rules linux_USB_config_setup.sh linux_USB_config_removal.sh**

- **chmod 777 linux_USB_config_setup.sh linux_USB_config_removal.sh**

- Launch **./linux_USB_config_setup.sh**

- Reboot to be sure to reload USB udev rules

- Reopen a terminal from HSD_SDK_dev folder

- Activate the .venv virtual environment created before 

- Launch the GUI: **python3 hsdatalog_GUI.py**


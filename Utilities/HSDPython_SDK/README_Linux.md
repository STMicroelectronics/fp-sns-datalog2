## __How to install HSDPython_SDK in Linux environment__

The HSDPython_SDK provides installer scripts that can be used to properly install the SDK and all the required dependencies.
The SDK has been developed using Python 3.10.
To properly use it, Python 3.10 or above must be already installed on the machine before proceeding with the following procedure: 

- Open a terminal from HSDPython_SDK folder

- The SDK has been built in a Windows environment. To avoid any possible issues while executing the script in Linux environment, we suggest you to use dos2unix to reformat properly the files. To do so, execute the following commands:

	- **sudo apt-get install dos2unix**
	- **dos2unix HSDPython_SDK_install.sh HSDPython_SDK_install_noGUI.sh HSDPython_SDK_uninstall.sh**
	- **chmod 777 HSDPython_SDK_install.sh HSDPython_SDK_install_noGUI.sh HSDPython_SDK_uninstall.sh**
	
- Install the SDK by launching **./HSDPython_SDK_install.sh**. The script will install all the wheels with the required dependencies and will create and enable a virtual environment.

	- If you are behind a proxy server you must launch the installer script in this way: **./HSDPython_SDK_install.sh http://usr_name:password@proxyserver_name:port**
	- If you have special characters in your password you will have to replace them with their correspondent hex representation. E.g. '&' --> %26
	
- Reboot to be sure to reload USB udev rules

- Reopen a terminal from HSDPython_SDK folder

- Activate the .venv venv automatically created from the install script by executing **source .venv/bin/activate**

- **cd examples**

- Launch the preferred script: i.e.: to launch the GUI type **python3 hsdatalog_GUI.py**


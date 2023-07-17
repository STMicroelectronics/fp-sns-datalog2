## __How to install HSDPython_SDK in Windows environment__

The HSDPython_SDK provides installer scripts that can be used to properly install the SDK and all the required dependencies.
If you are in a Linux environment, see README_Linux file.

The SDK has been developed using Python 3.10.
To properly use it, Python 3.10 or above must be already installed on the machine before proceeding with the following procedure: 

- Open a terminal from HSDPython_SDK folder
	
- Install the SDK by launching **./HSDPython_SDK_install.bat**. The script will install all the wheels with the required dependencies and will create and enable a virtual environment.

	- If you are behind a proxy server you must launch the installer script in this way: **./HSDPython_SDK_install.bat http://usr_name:password@proxyserver_name:port**
	- If you have special characters in your password you will have to replace them with their correspondent hex representation. E.g. '&' --> %26

- Activate the .venv venv automatically created from the install script by executing **.venv/scripts/activate**

- **cd examples**

- Launch the preferred script: i.e.: to launch the GUI type **python3 hsdatalog_GUI.py**


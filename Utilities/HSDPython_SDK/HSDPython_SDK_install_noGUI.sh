#!/bin/bash 
#This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
echo "HSDPython_SDK(no GUI version) Installer"
echo ""
echo "[NOTE]: If you are using a proxy launch this script passing your proxy string as a parameter."
echo "-- e.g.: HSDPython_SDK_install.bat http://usr_name:password@proxyserver_name:port"
echo ""

# Section 0: Linux Prerequisites
echo "==================================================="
echo "check and install prerequisites"
echo "==================================================="
sudo apt-get install libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0
sudo apt-get install libxcb-cursor-dev
sudo apt-get install python3-venv

# Section 1: Python3 Virtual environment Creation and Activation
echo "==================================================="
echo "Create and Activate a Python3 Virtual Environment"
echo "==================================================="
python3 -m venv .venv
source .venv/bin/activate

# Section 2: st_pnpl
echo "==================================================="
echo "installing st_pnpl..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install st_pnpl/dist/st_pnpl-1.2.1-py3-none-any.whl
else
	python3 -m pip install st_pnpl/dist/st_pnpl-1.2.1-py3-none-any.whl --proxy=$1
fi
# Section 3: st_hsdatalog (NO GUI version)
echo "==================================================="
echo "installing st_hsdatalog (NO GUI version)..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install st_hsdatalog/dist/noGUI/st_hsdatalog-2.2.1-py3-none-any.whl
else
	python3 -m pip install st_hsdatalog/dist/noGUI/st_hsdatalog-2.2.1-py3-none-any.whl --proxy=$1
fi
# Section 4: asciimatics needed from hsdatalog_cli.py example script
echo "==================================================="
echo "installing asciimatics..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install asciimatics
else
	python3 -m pip install asciimatics --proxy=$1
fi
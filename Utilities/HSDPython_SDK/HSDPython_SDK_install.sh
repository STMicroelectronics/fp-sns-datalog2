#!/bin/bash 
#This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
echo "HSDPython_SDK Installer"
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
	python3 -m pip install st_pnpl/dist/st_pnpl-1.1.0-py3-none-any.whl
else
	python3 -m pip install st_pnpl/dist/st_pnpl-1.1.0-py3-none-any.whl --proxy=$1
fi
# Section 3: st_dtdl_gui
echo "==================================================="
echo "installing st_dtdl_gui..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install st_dtdl_gui/dist/st_dtdl_gui-1.1.0-py3-none-any.whl
else
	python3 -m pip install st_dtdl_gui/dist/st_dtdl_gui-1.1.0-py3-none-any.whl --proxy=$1
fi
# Section 4: st_hsdatalog (GUI version)
echo "==================================================="
echo "installing st_hsdatalog (GUI version)..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install st_hsdatalog/dist/GUI/st_hsdatalog-2.1.0-py3-none-any.whl
else
	python3 -m pip install st_hsdatalog/dist/GUI/st_hsdatalog-2.1.0-py3-none-any.whl --proxy=$1
fi
# Section 5: asciimatics needed from hsdatalog_cli.py example script
echo "==================================================="
echo "installing asciimatics..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install asciimatics
else
	python3 -m pip install asciimatics --proxy=$1
fi
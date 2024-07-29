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

if [[ "$(uname -m)" == "armv7l" ]]; then
echo "Raspberry Pi doesn't support GUI. Please use HSDPython_SDK_install_noGUI.sh"
fi

sudo apt-get install libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0
sudo apt-get install libxcb-cursor-dev

# Section 1: st_pnpl
echo "==================================================="
echo "installing st_pnpl..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install st_pnpl/dist/st_pnpl-2.3.0-py3-none-any.whl
else
	python3 -m pip install st_pnpl/dist/st_pnpl-2.3.0-py3-none-any.whl --proxy=$1
fi
# Section 2: st_dtdl_gui
echo "==================================================="
echo "installing st_dtdl_gui..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install st_dtdl_gui/dist/st_dtdl_gui-2.3.0-py3-none-any.whl
else
	python3 -m pip install st_dtdl_gui/dist/st_dtdl_gui-2.3.0-py3-none-any.whl --proxy=$1
fi
# Section 3: st_hsdatalog (GUI version)
echo "==================================================="
echo "installing st_hsdatalog (GUI version)..."
echo "==================================================="
if [ -z $1 ]
then
	python3 -m pip install st_hsdatalog/dist/GUI/st_hsdatalog-3.3.0-py3-none-any.whl
else
	python3 -m pip install st_hsdatalog/dist/GUI/st_hsdatalog-3.3.0-py3-none-any.whl --proxy=$1
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

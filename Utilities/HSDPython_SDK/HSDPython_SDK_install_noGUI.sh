#!/bin/bash 
#This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
echo "HSDPython_SDK(no GUI version) Installer"
echo ""
echo "[NOTE]: If you are using a proxy launch this script passing your proxy string as a parameter."
echo "-- e.g.: HSDPython_SDK_install.bat http://usr_name:password@proxyserver_name:port#"
echo ""

# Section 1: st_pnpl
echo "==========================================="
echo "installing st_pnpl..."
echo "==========================================="
if [ -z $1 ]
then
	python -m pip install st_pnpl/dist/st_pnpl-1.0.0-py3-none-any.whl
else
	python -m pip install st_pnpl/dist/st_pnpl-1.0.0-py3-none-any.whl --proxy=$1
fi
# Section 2: st_dtdl_gui
echo "==========================================="
echo "installing st_dtdl_gui..."
echo "==========================================="
if [ -z $1 ]
then
	python -m pip install st_dtdl_gui/dist/st_dtdl_gui-1.0.0-py3-none-any.whl
else
	python -m pip install st_dtdl_gui/dist/st_dtdl_gui-1.0.0-py3-none-any.whl --proxy=$1
fi
# Section 3: st_hsdatalog (NO GUI version)
echo "==========================================="
echo "installing st_hsdatalog (NO GUI version)..."
echo "==========================================="
if [ -z $1 ]
then
	python -m pip install st_hsdatalog/dist/noGUI/st_hsdatalog-2.0.0-py3-none-any.whl
else
	python -m pip install st_hsdatalog/dist/noGUI/st_hsdatalog-2.0.0-py3-none-any.whl --proxy=$1
fi
# Section 4: asciimatics needed from hsdatalog_cli.py example script
echo "==========================================="
echo "installing asciimatics..."
echo "==========================================="
if [ -z $1 ]
then
	python -m pip install asciimatics
else
	python -m pip install asciimatics --proxy=$1
fi
echo "==========================================="
echo "HSDPython_SDK installed correctly"
echo "==========================================="
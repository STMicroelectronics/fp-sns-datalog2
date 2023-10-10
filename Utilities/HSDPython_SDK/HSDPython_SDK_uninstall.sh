#!/bin/bash 
#This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
echo "HSDPython_SDK Uninstaller"

# Section 1: st_pnpl
echo "==========================================="
echo "uninstalling st_pnpl..."
echo "==========================================="
python3 -m pip uninstall st_pnpl
# Section 2: st_dtdl_gui
echo "==========================================="
echo "uninstalling st_dtdl_gui..."
echo "==========================================="
python3 -m pip uninstall st_dtdl_gui
echo "==========================================="
echo "uninstalling st_hsdatalog"
echo "==========================================="
python3 -m pip uninstall st_hsdatalog
# Section 4: asciimatics needed from hsdatalog_cli.py example script
echo "==========================================="
echo "uninstalling asciimatics..."
echo "==========================================="
python3 -m pip uninstall asciimatics
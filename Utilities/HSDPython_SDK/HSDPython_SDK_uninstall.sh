#!/bin/bash 
#This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
echo "HSDPython_SDK Uninstaller"

# Section 1: st_pnpl
echo "==========================================="
echo "uninstalling st_pnpl..."
echo "==========================================="
python3 -m pip uninstall st_pnpl/dist/st_pnpl-1.1.0-py3-none-any.whl
# Section 2: st_dtdl_gui
echo "==========================================="
echo "uninstalling st_dtdl_gui..."
echo "==========================================="
python3 -m pip uninstall st_dtdl_gui/dist/st_dtdl_gui-1.1.0-py3-none-any.whl
# Section 3.1: st_hsdatalog (GUI version)
echo "==========================================="
echo "uninstalling st_hsdatalog (GUI version)..."
echo "==========================================="
python3 -m pip uninstall st_hsdatalog/dist/GUI/st_hsdatalog-2.1.0-py3-none-any.whl
# Section 3.2: st_hsdatalog (NO GUI version)
echo "==========================================="
echo "uninstalling st_hsdatalog (NO GUI version)..."
echo "==========================================="
python3 -m pip uninstall st_hsdatalog/dist/noGUI/st_hsdatalog-2.1.0-py3-none-any.whl
# Section 4: asciimatics needed from hsdatalog_cli.py example script
echo "==========================================="
echo "uninstalling asciimatics..."
echo "==========================================="
python3 -m pip uninstall asciimatics
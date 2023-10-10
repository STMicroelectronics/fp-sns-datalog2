@ECHO OFF 
:: This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
ECHO HSDPython_SDK Unistaller
:: Section 1: st_pnpl
ECHO ===========================================
ECHO uninstalling st_pnpl...
ECHO ===========================================
python -m pip uninstall st_pnpl
:: Section 2: st_dtdl_gui
ECHO ===========================================
ECHO uninstalling st_dtdl_gui...
ECHO ===========================================
python -m pip uninstall st_dtdl_gui
:: Section 3: st_hsdatalog
ECHO ===========================================
ECHO uninstalling st_hsdatalog
ECHO ===========================================
python -m pip uninstall st_hsdatalog
:: Section 4: asciimatics needed from hsdatalog_cli.py example script
ECHO ===========================================
ECHO uninstalling asciimatics...
ECHO ===========================================
python -m pip uninstall asciimatics
PAUSE
@ECHO OFF 
:: This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
ECHO HSDPython_SDK Unistaller
:: Section 1: st_pnpl
ECHO ===========================================
ECHO uninstalling st_pnpl...
ECHO ===========================================
python -m pip uninstall st_pnpl\dist\st_pnpl-1.1.0-py3-none-any.whl
:: Section 2: st_dtdl_gui
ECHO ===========================================
ECHO uninstalling st_dtdl_gui...
ECHO ===========================================
python -m pip uninstall st_dtdl_gui\dist\st_dtdl_gui-1.1.0-py3-none-any.whl
:: Section 3.1: st_hsdatalog (GUI version)
ECHO ===========================================
ECHO uninstalling st_hsdatalog (GUI version)...
ECHO ===========================================
python -m pip uninstall st_hsdatalog\dist\GUI\st_hsdatalog-2.1.0-py3-none-any.whl
:: Section 3.2: st_hsdatalog (NO GUI version)
ECHO ===========================================
ECHO uninstalling st_hsdatalog (NO GUI version)...
ECHO ===========================================
python -m pip uninstall st_hsdatalog\dist\noGUI\st_hsdatalog-2.1.0-py3-none-any.whl
:: Section 4: asciimatics needed from hsdatalog_cli.py example script
ECHO ===========================================
ECHO uninstalling asciimatics...
ECHO ===========================================
python -m pip uninstall asciimatics
PAUSE
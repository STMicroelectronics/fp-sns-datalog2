@ECHO OFF
:: This batch file is used to install all Python packages needed to use the HSDPython_SDK and its examples
ECHO HSDPython_SDK Installer
ECHO.
ECHO [NOTE]: If you are using a proxy launch this script passing your proxy string as a parameter.
ECHO -- e.g.: HSDPython_SDK_install.bat http://usr_name:password@proxyserver_name:port#
ECHO.
set proxy_string=%1
:: Section 1: st_pnpl
ECHO ===========================================
ECHO installing st_pnpl...
ECHO ===========================================
if "%proxy_string%"=="" (python -m pip install st_pnpl\dist\st_pnpl-1.1.0-py3-none-any.whl) else (python -m pip install st_pnpl\dist\st_pnpl-1.1.0-py3-none-any.whl --proxy %proxy_string%) 
:: Section 2: st_dtdl_gui
ECHO ===========================================
ECHO installing st_dtdl_gui...
ECHO ===========================================
if "%proxy_string%"=="" (python -m pip install st_dtdl_gui\dist\st_dtdl_gui-1.1.0-py3-none-any.whl) else (python -m pip install st_dtdl_gui\dist\st_dtdl_gui-1.1.0-py3-none-any.whl --proxy %proxy_string%) 
:: Section 3: st_hsdatalog (GUI version)
ECHO ===========================================
ECHO installing st_hsdatalog (GUI version)...
ECHO ===========================================
if "%proxy_string%"=="" (python -m pip install st_hsdatalog\dist\GUI\st_hsdatalog-2.1.0-py3-none-any.whl) else (python -m pip install st_hsdatalog\dist\GUI\st_hsdatalog-2.1.0-py3-none-any.whl --proxy %proxy_string%) 
:: Section 4: asciimatics needed from hsdatalog_cli.py example script
ECHO ===========================================
ECHO installing asciimatics...
ECHO ===========================================
if "%proxy_string%"=="" (python -m pip install asciimatics) else (python -m pip install asciimatics --proxy %proxy_string%) 
PAUSE
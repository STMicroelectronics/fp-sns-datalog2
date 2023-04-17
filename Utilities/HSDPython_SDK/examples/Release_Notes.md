---
pagetitle: Release Notes for HSDatalog Python SDK 
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-4}

<center> 
# Release Notes for <mark>HSDatalog Python SDK</mark> 
Copyright &copy; 2023 STMicroelectronics
    
[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com){.logo}
</center>


# Purpose

The High-Speed DataLog firmware provides an easy way to save data from any combination 
of sensors and microphones configured at their maximum sampling rate.
Sensor data can be either stored on a micro SD Card, SDHC (Secure Digital High Capacity) 
formatted with the FAT32 file system, or streamed to a PC via USB (WinUSB class).

This folder contains different Python scripts and classes that can be used to handle
the datasets obtained by the HSDatalog firmware.

:::

::: {.col-sm-12 .col-lg-8}
# Update History

::: {.collapse}
<input type="checkbox" id="collapse-section10" checked aria-hidden="true">
<label for="collapse-section10" aria-hidden="true">v2.1.0 / 24-Mar-23</label>
<div>

## Main Changes

### Maintenance release and product update

- Refactored HSDPython_SDK Install/uninstall batch scripts
- Fixed setup issues in Linux environment
- st_pnpl local USB catalog updated with new supported devices (Sensortile.box PRO)
- Fixed hsdatalog_cli.py acquisitions output path selection
- Fixed algorithm plot widget used by Ultrasound FFT GUI
- Fixed sensor plot widget for low ODR and samples_per_ts=0
- Fixed convert_dat_to_unico method to manage also gyro data and filter MLC sensor type
- Updated HSDv2 DLL v3.1.0
- Packages refactoring focusing on GUI modules inheritance for Custom GUI creation.
- New st_dtdl_gui Widgets: PlotsArrayWidget, AnalogGaugeWidget, LevelWidget
- New Plots hierarchy
- Plot settings section:
	- Wav converter/player for mic sensors
	- FFT filtering zones and peak reveal
	- ISPU output format spec file upload
- New GUI Examples to manage AI outputs and Motor Control telemetries
- Solved pandas v2.0.0 compatibility issue
- PySide6 version fixed to 6.4.3 (pyqtgraph issues in PySide6 v6.5.0)

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section9" aria-hidden="true">
<label for="collapse-section9" aria-hidden="true">v2.0.1 / 14-Feb-23</label>
<div>

## Main Changes

### Patch release

- Fixed toNanoedge converter and offline plot data for dummy data
- Updated device models

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section8" aria-hidden="true">
<label for="collapse-section8" aria-hidden="true">v2.0.0 / 30-Sep-22</label>
<div>

## Main Changes

### Maintenance release and product update

- Added support for both DATALOG1 and DATALOG2

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section7" aria-hidden="true">
<label for="collapse-section7" aria-hidden="true">v1.4.0 / 8-Jul-2022</label>
<div>

## Main Changes

### Maintenance release and product update

- Added support to SensorTile.box and LSM6DSOX
- Bug fixed in plot function: 
  - Fix bug when SamplesPerTS=0
  - Solved issue when plotting labelled data
- Fixed acquisition stop/restart in cli example
- Renamed reference to cartesiam - use NanoEdge term instead of Cartesiam
- Reviewed Error/Log management
- Updated dll (for Windows, Linux and Raspberry pi 3B)
- Updated license schema

## Dependencies

- All the python modules required by the SDK are shown in **requirements.txt** file.
  - If you are on a Linux or Raspberry machine: 
    - Install manually the following libraries through the **sudo apt-get install** command:
      - libjpeg-dev
      - libbz2-dev
      - libatlas-base-dev
      - python3-tk
    - Then launch the **sudo pip install -r requirements.txt** command  
  - On a Windows machine, just launch the **pip install -r requirements.txt** command
- In Linux or Raspberry environment, launch the scripts as root through **sudo** command

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section6" aria-hidden="true">
<label for="collapse-section6" aria-hidden="true">v1.3.0 / 5-Nov-2021</label>
<div>

## Main Changes

### Maintenance release

- **Updated requirements.txt for Linux and Raspberry pi 3B compatibility**
- **Bug fixed in plot function: avoid time shifting for sensors whose samples_per_ts is 1**
- Reviewed Error/Log management
- Automatically setup wav file channels number in hsdatalog_to_wav.py according to the n dimensions of the considered subsensor
- AcquisitionInfo Model Updated (start/end acquisition json keys updated)
- Updated dll (for Windows, Linux and Raspberry pi 3B)

## Dependencies

- All the python modules required by the SDK are shown in **requirements.txt** file.
  - If you are on a Linux or Raspberry machine: 
    - Install manually the following libraries through the **sudo apt-get install** command:
      - libjpeg-dev
      - libbz2-dev
      - libatlas-base-dev
      - python3-tk
    - Then launch the **sudo pip install -r requirements.txt** command  
  - On a Windows machine, just launch the **pip install -r requirements.txt** command
- In Linux or Raspberry environment, launch the scripts as root through **sudo** command

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section5" aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">v1.2.0 / 25-Jun-2021</label>
<div>

## Main Changes

### Maintenance release and product update

- **Updated requirements.txt - import missing**
- **Hotfix in nb_hsdatalog_communication notebook**
- **Removed MIC sensor_type constraints in hsdatalog_to_wav.py (conversion to .wav is available for all sensors)**
- **Redesigned HSDLink com_manager definition and code cleaning**
- Added missing commands in HSDLink and fixed missing import
- Fixed wrong condition for warning message in cartesiam converter function in HSD_utils
- Updated file_manager and DeviceConfig model in HSD
- Bug fixed in HSD plot function 


</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">v1.1.0 / 29-Jan-2021</label>
<div>

## Main Changes

### Maintenance release and product update

- **Updated core module and code cleaning**
- Add BSD 3 Clause license disclaimer
- Fixed update device function in command manager (add tagConfig params)
- Issue solved in set label HSDCmd
- **Added new wav and to_unico converters**
- **Updated hsdatalog_check_fake_data.py and hsdatalog_cli.py**


</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">v1.0.0 / 13-Nov-2020</label>
<div>

## Main Changes

### First official release


</div>
:::

:::
:::

<footer class="sticky">
::: {.columns}
::: {.column width="95%"}
For complete documentation,
visit: [www.st.com](https://www.st.com/en/embedded-software/fp-sns-datalog2.html)
:::
::: {.column width="5%"}
<abbr title="Based on template cx566953 version 2.0">Info</abbr>
:::
:::
</footer>

# *****************************************************************************
#  * @file    hsdatalog_data_segmentation_GUI.py
#  * @author  SRA
# ******************************************************************************
# * @attention
# *
# * Copyright (c) 2022 STMicroelectronics.
# * All rights reserved.
# *
# * This software is licensed under terms that can be found in the LICENSE file
# * in the root directory of this software component.
# * If no LICENSE file comes with this software, it is provided AS-IS.
# *
# *
# ******************************************************************************
#

import sys
import subprocess

from PySide6.QtWidgets import QApplication
from PySide6 import QtCore

import st_hsdatalog.HSD_utils.logger as logger
log = logger.setup_applevel_logger(is_debug = False)

log.info("Checking additional required packages...")

# List of required packages
required_packages = [
    "skopt",
    "mango",
    "scipy",
    "pywt"
]

# Check for missing packages
missing_packages = {}
for package in required_packages:
    try:
        __import__(package)
    except ImportError:
        if package == "skopt":
            missing_packages[package] = "scikit-optimize"
        elif package == "mango":
            missing_packages[package] = "arm-mango"
        elif package == "scipy":
            missing_packages[package] = "scipy"
        elif package == "pywt":
            missing_packages[package] = "PyWavelets"

# Notify user of missing packages
if missing_packages:
    log.warning("The following required packages are missing:")
    for package in missing_packages:
        log.warning(f" - ({package}) {missing_packages[package]}")
    
    user_input = input("Do you want to install the missing packages? (yes/no): ").strip().lower()
    if user_input in ["yes", "y"]:
        for package in missing_packages:
            version = input(f"Enter the version for {missing_packages[package]} (or press Enter to install the latest version): ").strip()
            if version:
                subprocess.check_call([sys.executable, "-m", "pip", "install", f"{missing_packages[package]}=={version}"])
            else:
                subprocess.check_call([sys.executable, "-m", "pip", "install", missing_packages[package]])
        log.info("All required packages are installed.")
    else:
        log.error("Please install the missing packages and try again.")
        sys.exit(1)
else:
    log.info("All required packages are installed.")

from Assisted_Segmentation_GUI.gui.views import AssistedSegmentationWindow

def main():
    QApplication.setAttribute(QtCore.Qt.AA_ShareOpenGLContexts)
    app = QApplication(sys.argv)
    mainWindow = AssistedSegmentationWindow()
    mainWindow.setWindowTitle("Assisted Segmentation GUI")
    mainWindow.showMaximized()
    app.setAttribute(QtCore.Qt.AA_Use96Dpi)
    app.exec()

if __name__ == "__main__":
    main()

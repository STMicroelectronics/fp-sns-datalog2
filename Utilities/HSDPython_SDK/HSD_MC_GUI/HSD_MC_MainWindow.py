# *****************************************************************************
#  * @file    MainWindow.py
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

from HSD_MC_GUI.HSD_MC_Controller import HSD_MC_Controller
from HSD_MC_GUI.HSD_MC_DeviceConfigPage import HSD_MC_DeviceConfigPage
from st_dtdl_gui.STDTDL_MainWindow import STDTDL_MainWindow

class HSD_MC_MainWindow(STDTDL_MainWindow):
    
    def __init__(self, app, parent=None):
        super().__init__(app, HSD_MC_Controller(parent), parent)
        self.device_conf_page = HSD_MC_DeviceConfigPage(self.configuration_widget, self.controller)
        
        self.setWindowTitle("HSDatalog2_MC")
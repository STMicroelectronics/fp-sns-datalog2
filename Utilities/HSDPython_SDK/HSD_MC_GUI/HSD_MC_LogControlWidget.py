
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

from PySide6.QtCore import Slot
from st_hsdatalog.HSD_GUI.Widgets.HSDLogControlWidget import HSDLogControlWidget
from st_dtdl_gui.UI.styles import STDTDL_PushButton

import st_hsdatalog.HSD_utils.logger as logger
log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

class HSD_MC_LogControlWidget(HSDLogControlWidget):
    def __init__(self, controller, comp_contents, comp_name="log_controller", comp_display_name ="Log Controller" ,comp_sem_type="other", c_id=0, parent=None):
        super().__init__(controller, comp_contents, comp_name, comp_display_name, comp_sem_type, c_id, parent)
        
        self.controller.sig_is_motor_started.connect(self.s_is_motor_started)
    
    @Slot(bool, int)
    def s_is_motor_started(self, status:bool, motor_id:int):        
        #NOTE nex dev: motor_id check vs self.motor_id
        if status == True and self.controller.is_logging:
            self.log_start_button.setEnabled(False)
        elif status == False and self.controller.is_logging:
            self.log_start_button.setEnabled(True)
    
    @Slot(bool)
    def s_is_logging(self, status:bool, interface:int):
        super().s_is_logging(status, interface)
        if status:
            if self.controller.is_motor_started:
                self.log_start_button.setEnabled(False)
    

        
   

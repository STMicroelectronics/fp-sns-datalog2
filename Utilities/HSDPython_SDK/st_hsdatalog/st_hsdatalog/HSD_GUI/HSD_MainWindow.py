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

import st_dtdl_gui.UI.images #NOTE don't delete this! it is used from resource_filename (@row 35..38)
from st_dtdl_gui.STDTDL_MainWindow import STDTDL_MainWindow

from st_hsdatalog.HSD_GUI.HSD_DeviceConfigPage import HSD_DeviceConfigPage
from st_hsdatalog.HSD_GUI.HSD_Controller import HSD_Controller

from pkg_resources import resource_filename
motor_normal_img_path = resource_filename('st_dtdl_gui.UI.images', 'Motor_Normal_Class.png')
motor_anomaly_img_path = resource_filename('st_dtdl_gui.UI.images', 'Motor_Anomaly_Class.png')
motor_vibration_img_path = resource_filename('st_dtdl_gui.UI.images', 'Motor_Vibration_Class.png')
motor_magnet_img_path = resource_filename('st_dtdl_gui.UI.images', 'Motor_Magnet_Class.png')
motor_belt_img_path = resource_filename('st_dtdl_gui.UI.images', 'Motor_Belt_Class.png')
ispu_logo_img_path = resource_filename('st_dtdl_gui.UI.images', 'ISPU.png')
nanoedge_ispu_logo_img_path = resource_filename('st_dtdl_gui.UI.images', 'Nanoedge_ISPU.png')
nanoedge_stm32_logo_img_path = resource_filename('st_dtdl_gui.UI.images', 'Nanoedge_STM32.png')
ai_output_img_path = resource_filename('st_dtdl_gui.UI.images', 'AI_Output.png')

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class HSD_MainWindow(STDTDL_MainWindow):
    
    def __init__(self, app, controller = HSD_Controller(None), parent=None):
        super().__init__(app, controller, parent)
        
        # TODO: Next version --> Hotplug events notification support
        # self.controller.sig_usb_hotplug.connect(self.s_usb_hotplug_event)
        # TODO: Next version --> Hotplug events notification support
        
        self.device_conf_page = HSD_DeviceConfigPage(self.configuration_widget, self.controller)
        
        self.supported_out_class_dict = {
            "Motor_Normal_class" : ("Normal" , motor_normal_img_path),
            "Motor_Anomaly_class" : ("Anomaly" , motor_anomaly_img_path),
            "Motor_Vibration_class" : ("Vibration" , motor_vibration_img_path),
            "Motor_Magnet_class" : ("Magnet" , motor_magnet_img_path),
            "Motor_Belt_class" : ("Belt" , motor_belt_img_path),
            "ISPU" : ("ISPU", ispu_logo_img_path) #NOTE inserted here for ISPU CES 2023 demo purposes
        }
        self.anomaly_classes = {}
        self.out_classes = {}

        self.supported_ai_tools_dict = {
            "ISPU" : ("ISPU", ispu_logo_img_path),
            "Nanoedge_ISPU" : ("Nanoedge on ISPU", nanoedge_ispu_logo_img_path),
            "Nanoedge_STM32" : ("Nanoedge on STM32", nanoedge_stm32_logo_img_path)
        }
        self.ai_anomaly_tool = {}
        self.ai_classifier_tool = {}
        
        self.setWindowTitle("HSDatalog2")

    def setAIAnomalyImages(self, anomaly_images:list):
        for n in anomaly_images:
            if n in self.supported_out_class_dict:
                out_c_name = self.supported_out_class_dict[n][0]
                out_c_img = self.supported_out_class_dict[n][1]
                self.anomaly_classes[out_c_name] = out_c_img 
            else:
                self.anomaly_classes[n] = ai_output_img_path
        self.controller.set_anomaly_classes(self.anomaly_classes)
        
    def setAIClassifierImages(self, class_names:list):
        for n in class_names:
            if n in self.supported_out_class_dict:
                out_c_name = self.supported_out_class_dict[n][0]
                out_c_img = self.supported_out_class_dict[n][1]
                self.out_classes[out_c_name] = out_c_img 
            else:
                self.out_classes[n] = ai_output_img_path
        self.controller.set_output_classes(self.out_classes)
    
    def setAIAnomalyTool(self, tool_name:str):
        if tool_name in self.supported_ai_tools_dict:
            ai_anomaly_tool_name = self.supported_ai_tools_dict[tool_name][0]
            ai_anomaly_tool_img = self.supported_ai_tools_dict[tool_name][1]
            self.ai_anomaly_tool[ai_anomaly_tool_name] = ai_anomaly_tool_img
            self.controller.set_ai_anomaly_tool(self.ai_anomaly_tool)

    def setAIClassifierTool(self, tool_name:str):
        if tool_name in self.supported_ai_tools_dict:
            ai_classifier_tool_name = self.supported_ai_tools_dict[tool_name][0]
            ai_classifier_tool_img = self.supported_ai_tools_dict[tool_name][1]
            self.ai_classifier_tool[ai_classifier_tool_name] = ai_classifier_tool_img
            self.controller.set_ai_classifier_tool(self.ai_classifier_tool)

    def getOutputClassDict(self):
        return self.out_classes

    def closeEvent(self, event):
        self.controller.stop_log()
        event.accept()

    # TODO: Next version --> Hotplug events notification support
    # @Slot(bool)
    # def s_usb_hotplug_event(self, status):
    #     print("HSDv2 USB Device Plugged") if status else print("HSDv2 USB Device Unplugged")
    #     # if status == False:
    #     #     self.page_manager.setCurrentWidget(self.connection_page)
    #     #     self.menu_btn_device_conf.setVisible(False)
    # TODO: Next version --> Hotplug events notification support
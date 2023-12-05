# *****************************************************************************
#  * @file    DeviceConfigPage.py
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

from abc import abstractmethod
from PySide6.QtWidgets import QFrame, QWidget, QLabel, QScrollArea
from PySide6.QtGui import QPixmap
from PySide6.QtCore import Slot, Qt
from st_dtdl_gui.Utils.DataClass import ActuatorPlotParams, AlgorithmPlotParams, SensorPlotParams
from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
from st_dtdl_gui.STDTDL_Controller import ComponentType

import st_dtdl_gui.UI.images #NOTE don't delete this! it is used from resource_filename (@row 35)

from pkg_resources import resource_filename

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class STDTDL_DeviceConfigPage():
    def __init__(self, page_widget, controller):
        self.controller = controller
        
        self.controller.sig_component_config_widget_width_updated.connect(self.s_update_comp_config_width)

        self.controller.sig_component_found.connect(self.s_component_found)
        self.controller.sig_sensor_component_found.connect(self.s_sensor_component_found)
        self.controller.sig_algorithm_component_found.connect(self.s_algorithm_component_found)
        self.controller.sig_actuator_component_found.connect(self.s_actuator_component_found)
        self.controller.sig_dtm_loading_completed.connect(self.add_st_logo_to_device_config_widget_layout)

        self.controller.sig_sensor_component_updated.connect(self.s_sensor_component_updated)
        self.controller.sig_algorithm_component_updated.connect(self.s_algorithm_component_updated)
        self.controller.sig_actuator_component_updated.connect(self.s_actuator_component_updated)

        self.controller.sig_component_removed.connect(self.s_component_removed)
        
        self.controller.sig_logging.connect(self.s_is_logging)
        self.controller.sig_detecting.connect(self.s_is_detecting)

        self.page_widget = page_widget

        # layout_device_config
        self.main_layout = page_widget.findChild(QFrame, "frame_device_config")
        self.widget_header = self.main_layout.findChild(QWidget, "widget_header")
        self.log_control_widget = None
        
        self.scrollArea_device_config = self.main_layout.findChild(QScrollArea, "scrollArea_device_config")
        self.widget_special_componenents = self.main_layout.findChild(QWidget,"widget_special_components")
        self.device_config_widget = self.main_layout.findChild(QWidget,"widget_device_config")
        self.logging_message = QLabel(self.controller.get_log_msg())
        self.logging_message.setContentsMargins(12,6,12,6)
        self.logging_message.hide()
        self.device_config_widget.layout().addWidget(self.logging_message)
        self.error_message = QLabel("")
        self.error_message.setStyleSheet("color: #FF5050;")
        self.error_message.setContentsMargins(12,6,12,6)
        self.error_message.hide()
        self.device_config_widget.layout().addWidget(self.error_message)
        self.plots_widget = self.main_layout.findChild(QWidget,"widget_plots")

        self.comp_id = 0
        
        self.st_logo_img_path = resource_filename('st_dtdl_gui.UI.images', 'st_logo.png')

    def remove_comp_widget(self, name):
        self.controller.remove_component_config_widget(name)
        
    def get_nof_components(self):
        if self.controller.is_com_ok():
            return len(self.controller.get_device_status()["devices"][self.controller.device_id]["components"])
        return 0
    
    @Slot(int)
    def s_update_comp_config_width(self, width):
        self.scrollArea_device_config.setMinimumWidth(width+6)
        self.scrollArea_device_config.setMaximumWidth(width+6)
        self.scrollArea_device_config.widget().setFixedWidth(width)
    
    def add_st_logo_to_device_config_widget_layout(self):
        st_logo_image = QLabel()
        st_logo_pixmap = QPixmap(self.st_logo_img_path)
        st_logo_image.setPixmap(st_logo_pixmap)
        st_logo_image.setAlignment(Qt.AlignmentFlag.AlignCenter)
        st_logo_image.setContentsMargins(0,12,0,0)
        self.device_config_widget.layout().addWidget(st_logo_image)
    
    @Slot(str, dict)
    def s_component_found(self, comp_name, comp_interface):
        comp_id = list(self.controller.components_dtdl.keys()).index(comp_name)
        comp_display_name = comp_interface.display_name if isinstance(comp_interface.display_name, str) else comp_interface.display_name.en
        if comp_name == "applications_stblesensor":
            pass
        else:
            comp_config_widget = ComponentWidget(self.controller, comp_name, comp_display_name, "", comp_interface.contents, comp_id, self.device_config_widget)
            self.controller.add_component_config_widget(comp_config_widget)
            self.device_config_widget.layout().addWidget(comp_config_widget)
            self.controller.fill_component_status(comp_name)
    
    @Slot(str, dict)
    def s_sensor_component_found(self, comp_name, comp_interface):
        #create a ComponentWidget
        comp_display_name = comp_interface.display_name if isinstance(comp_interface.display_name, str) else comp_interface.display_name.en
        sensor_config_widget = ComponentWidget(self.controller, comp_name, comp_display_name, ComponentType.SENSOR, comp_interface.contents, self.comp_id, self.device_config_widget)
        self.comp_id += 1
        self.controller.add_component_config_widget(sensor_config_widget)
        self.device_config_widget.layout().addWidget(sensor_config_widget)
        
        self.controller.fill_component_status(comp_name)

    @Slot(str, dict)
    def s_algorithm_component_found(self, comp_name, comp_interface):
        #create a ComponentWidget
        comp_display_name = comp_interface.display_name if isinstance(comp_interface.display_name, str) else comp_interface.display_name.en
        alg_config_widget = ComponentWidget(self.controller, comp_name, comp_display_name, ComponentType.ALGORITHM, comp_interface.contents, self.comp_id, self.device_config_widget)
        self.comp_id += 1
        self.controller.add_component_config_widget(alg_config_widget)
        self.device_config_widget.layout().addWidget(alg_config_widget)
        
        self.controller.fill_component_status(comp_name)       

    @Slot(str, dict)
    def s_actuator_component_found(self, comp_name, comp_interface):
        #create a ComponentWidget
        comp_display_name = comp_interface.display_name if isinstance(comp_interface.display_name, str) else comp_interface.display_name.en
        act_config_widget = ComponentWidget(self.controller, comp_name, comp_display_name, ComponentType.ACTUATOR, comp_interface.contents, self.comp_id, self.device_config_widget)
        self.comp_id += 1
        self.controller.add_component_config_widget(act_config_widget)
        self.device_config_widget.layout().addWidget(act_config_widget)
        
        self.controller.fill_component_status(comp_name)  

    @Slot(str)
    def s_component_removed(self, comp_name):
        self.remove_comp_widget(comp_name)
    
    @Slot(str, SensorPlotParams)
    @abstractmethod
    def s_sensor_component_updated(self, comp_name, plot_params:SensorPlotParams):
        pass

    @Slot(str, AlgorithmPlotParams)
    @abstractmethod
    def s_algorithm_component_updated(self, comp_name, plot_params:AlgorithmPlotParams):
        pass

    @Slot(str, ActuatorPlotParams)
    @abstractmethod
    def s_actuator_component_updated(self, comp_name, plot_params:ActuatorPlotParams):
        pass
        
    def set_error_message(self, status, message):
        if status:
            self.error_message.setText(message)
            self.error_message.show()
        else:
            self.error_message.hide()
    
    def endisable_logging_message(self, status):
        if self.controller.get_log_msg() != "":
            self.logging_message.setText(self.controller.get_log_msg())
            self.logging_message.show() if status else self.logging_message.hide()
            
    def endisable_detecting_message(self, status):
        if self.controller.get_detect_msg() != "":
            self.detect_message.setText(self.controller.get_detect_msg())
            self.detect_message.show() if status else self.detect_message.hide()
        
    def endisable_component_config(self, status, c_to_avoid):
        for w in self.device_config_widget.findChildren(ComponentWidget):
            if w.comp_name not in c_to_avoid:
                w.contents_widget.setEnabled(not status)
                if status:
                    style_split = w.frame_component_config.styleSheet().split(';')
                    style_split[-1] = "\ncolor: rgb(100, 100, 100)"
                    w.frame_component_config.setStyleSheet(';'.join(style_split))
                else:
                    style_split = w.frame_component_config.styleSheet().split(';')
                    style_split[-1] = "\ncolor: rgb(210, 210, 210)"
                    w.frame_component_config.setStyleSheet(';'.join(style_split))
    
    @abstractmethod
    def add_header_widget(self, widget):
        pass
    
    @Slot(bool)
    def s_is_logging(self, status:bool, interface:int):
        self.endisable_logging_message(status)
        
    @Slot(bool)
    def s_is_detecting(self, status:bool):
        self.endisable_detecting_message(status)

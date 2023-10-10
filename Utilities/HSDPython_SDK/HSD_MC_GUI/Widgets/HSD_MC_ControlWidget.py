
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

import os

from st_dtdl_gui.UI.styles import STDTDL_PushButton

from PySide6.QtCore import Slot
from PySide6.QtWidgets import QFrame, QPushButton, QSlider, QLabel, QTextEdit
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import HSD_MC_GUI

from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
import st_hsdatalog.HSD_utils.logger as logger
log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

class HSD_MC_ControlWidget(ComponentWidget):
    def __init__(self, controller, comp_contents, comp_name="motor_controller", comp_display_name = "Motor Controller" ,comp_sem_type="other", c_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)

        self.controller.sig_is_motor_started.connect(self.s_is_motor_started)
        
        self.motor_speed = 1440
        self.motor_id = 0
        self.is_motor_connected = False

        # clear all widgets in contents_widget layout (contents)
        for i in reversed(range(self.contents_widget.layout().count())):
            self.contents_widget.layout().itemAt(i).widget().deleteLater()

        self.setWindowTitle(comp_display_name)

        QPyDesignerCustomWidgetCollection.registerCustomWidget(HSD_MC_ControlWidget, module="MotorControlWidget")
        loader = QUiLoader()
        motor_control_widget = loader.load(os.path.join(os.path.dirname(HSD_MC_GUI.__file__),"UI","motor_control_widget.ui"))
        frame_contents = motor_control_widget.frame_motor_control.findChild(QFrame,"frame_contents")
        
        # Start/Stop Motor PushButton
        self.motor_start_button = frame_contents.findChild(QPushButton,"start_button")
        self.motor_start_button.clicked.connect(self.clicked_start_motor_button)
        self.motor_start_button.setEnabled(True)

        # Ack Fault Motor
        self.ack_fault_motor_button = frame_contents.findChild(QPushButton,"ack_button");
        self.ack_fault_motor_button.clicked.connect(self.clicked_ack_fault_button)
        self.ack_fault_motor_button.setEnabled(True)
        
        # Motor Speed Frame
        self.frame_set_speed = frame_contents.findChild(QFrame,"frame_set_speed")
        self.frame_set_speed.setEnabled(True)
        ## Motor Speed Slider
        self.speed_slider = frame_contents.findChild(QSlider,"speed_slider")
        self.speed_slider.sliderReleased.connect(self.motor_slider_released)
        self.speed_slider.valueChanged.connect(self.motor_slider_value_changed)
        self.speed_slider.setValue(self.motor_speed);
        self.speed_slider.setEnabled(False)
        ## Motor Speed Value
        self.speed_value = frame_contents.findChild(QTextEdit,"speed_value")
        self.speed_value.setText(str(self.motor_speed))
        self.speed_value.setEnabled(False)
        self.speed_value.setReadOnly(True)

        self.layout().setContentsMargins(0,0,0,0)
        # self.contents_widget.layout().setContentsMargins(15,0,15,0)
        self.contents_widget.layout().setContentsMargins(9,0,9,0)
        self.contents_widget.layout().addWidget(motor_control_widget.frame_motor_control)
        self.contents_widget.setVisible(True)
    
    
    @Slot(bool, int)
    def clicked_connect_motor_button(self):
        if not self.is_motor_connected:
            self.controller.connect_motor(self.motor_id)
        else:
            self.controller.disconnect_motor(self.motor_id)
    @Slot(bool, int)
    def clicked_start_motor_button(self): 
        if not self.controller.is_motor_started:
            self.controller.start_motor(self.motor_id)
        else:
            self.controller.stop_motor(self.motor_id)
    
    @Slot(bool, int)
    def clicked_ack_fault_button(self):
        self.controller.ack_fault(self.motor_id)
    
    @Slot(bool, int)
    def s_is_motor_connected(self, status:bool, motor_id:int):   
        #NOTE nex dev: motor_id check vs self.motor_id
        if status:
            self.motor_start_button.setEnabled(True)
            self.is_motor_connected = True
            self.frame_set_speed.setEnabled(True)
        else:
            self.motor_start_button.setEnabled(False)
            self.frame_set_speed.setEnabled(False)
            self.is_motor_connected = False
    
    @Slot(bool, int)
    def s_is_motor_started(self, status:bool, motor_id:int):        
        #NOTE nex dev: motor_id check vs self.motor_id
        if status:
            self.motor_start_button.setText("Stop")
            self.motor_start_button.setStyleSheet(STDTDL_PushButton.red)
            self.speed_slider.setEnabled(True)
            self.speed_value.setEnabled(True)
            self.controller.is_motor_started = True
        else:
            self.motor_start_button.setText("Start")
            self.motor_start_button.setStyleSheet(STDTDL_PushButton.green)
            self.speed_slider.setEnabled(False)
            self.speed_value.setEnabled(False)
            self.controller.is_motor_started = False
            
    def motor_slider_released(self):
        self.controller.set_motor_speed(self.speed_slider.value())
        
    def motor_slider_value_changed(self):
        self.speed_value.setText(str(int(self.speed_slider.value())))
        
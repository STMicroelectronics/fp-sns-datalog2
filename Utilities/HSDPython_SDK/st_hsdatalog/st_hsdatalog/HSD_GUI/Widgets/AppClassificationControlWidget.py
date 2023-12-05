
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

from PySide6.QtCore import Qt, Slot, QTimer
from st_pnpl.PnPLCmd import PnPLCMDManager

from PySide6.QtCore import Slot
from PySide6.QtWidgets import QFrame, QPushButton, QProgressBar, QSpinBox
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
import st_hsdatalog
import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class AppClassificationControlWidget(ComponentWidget):
    def __init__(self, controller, comp_contents, comp_name="log_controller", comp_display_name = "App Controller" ,comp_sem_type="other", c_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)
        self.is_detecting = False

        # clear all widgets in contents_widget layout (contents)
        for i in reversed(range(self.contents_widget.layout().count())):
            self.contents_widget.layout().itemAt(i).widget().deleteLater()

        self.setWindowTitle(comp_display_name)

        QPyDesignerCustomWidgetCollection.registerCustomWidget(AppClassificationControlWidget, module="AppClassificationControlWidget")
        loader = QUiLoader()
        log_control_widget = loader.load(os.path.join(os.path.dirname(st_hsdatalog.__file__),"HSD_GUI","UI","app_classification_control.ui"))
        frame_contents = log_control_widget.frame_log_control.findChild(QFrame,"frame_contents")

        # self.interface_combobox = frame_contents.findChild(QComboBox,"interface_combobox")

        # Start/Stop Learning PushButton
        self.start_learning_button = frame_contents.findChild(QPushButton,"start_learning")
        self.start_learning_button.clicked.connect(self.clicked_start_learning_button)

        # Start/Stop Detection
        self.start_detection_button = frame_contents.findChild(QPushButton,"start_detection")
        self.start_detection_button.clicked.connect(self.clicked_start_detection_button)
        self.start_detection_button.setEnabled(False)
        
        # Learning Time SpinBox
        self.learning_time_spinBox = log_control_widget.frame_log_control.findChild(QSpinBox,"learning_time_spinBox")
        self.learning_time_spinBox.setMaximum(600)
        self.learning_time_spinBox.setValue(15)

        # Learning progress Bar
        self.learning_progress_bar = log_control_widget.frame_log_control.findChild(QProgressBar,"learning_progress_bar")
        # self.learning_progress_bar.resize(100,92)
        self.learning_progres_time_ms = 100
        self.timer = QTimer() 
        self.timer.setTimerType(Qt.PreciseTimer)
        self.timer.timeout.connect(self.update_learning_progress_bar)
        self.learning_progress_bar_val = 0
        
        self.layout().setContentsMargins(0,0,0,0)
        self.contents_widget.layout().setContentsMargins(15,0,15,0)
        self.contents_widget.layout().addWidget(log_control_widget.frame_log_control)
        self.contents_widget.setVisible(True)
            
    @Slot()
    def clicked_start_detection_button(self):
        if not self.is_detecting:
            self.controller.start_detect()
            self.start_learning_button.setEnabled(False)
        else:
            self.controller.update_component_status("acquisition_info")
            self.controller.stop_detect()
            self.start_learning_button.setEnabled(True)
    
    @Slot()
    def clicked_start_learning_button(self):
        start_learning_msg = PnPLCMDManager.create_command_cmd("log_controller", "start_learning")
        self.controller.send_command(start_learning_msg)
        self.start_detection_button.setEnabled(False)
        self.learning_progress_bar_val = 0
        self.start_learning_button.setEnabled(False)
        self.learning_progres_time_ms = self.learning_time_spinBox.value() * 10
        self.timer.start(self.learning_progres_time_ms)
        self.start_learning_button.setText("Learning ...")

    @Slot(bool)
    def s_is_detecting(self, status:bool):        
        if status:
            self.start_detection_button.setText("Stop Detection")
            self.is_detecting = True
            self.controller.start_plots()
        else:
            self.start_detection_button.setText("Start Detection")
            self.is_detecting = False
            self.controller.stop_plots()

    def update_learning_progress_bar(self):
         self.learning_progress_bar_val += 1
         self.learning_progress_bar.setValue(self.learning_progress_bar_val)
         if self.learning_progress_bar_val == 100:   
             self.timer.stop()
             stop_learning_smg = PnPLCMDManager.create_command_cmd("log_controller", "stop_learning")
             self.controller.send_command(stop_learning_smg)
             self.start_learning_button.setText("Start Learning")
             self.start_learning_button.setEnabled(False)
             self.start_detection_button.setEnabled(True)
             self.controller.start_detect()
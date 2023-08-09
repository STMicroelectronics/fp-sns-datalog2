
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

from PySide6.QtCore import Slot
from PySide6.QtWidgets import QFileDialog, QFrame, QLineEdit, QVBoxLayout, QWidget, QPushButton
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import st_dtdl_gui

class DeviceTemplateLoadingWidget(QWidget):
    def __init__(self, controller, parent=None):
        super().__init__(parent)
        self.controller = controller
        
        self.s_is_dt_loaded = False
        self.setWindowTitle("Device Template")

        QPyDesignerCustomWidgetCollection.registerCustomWidget(DeviceTemplateLoadingWidget, module="DeviceTemplateLoadingWidget")
        loader = QUiLoader()
        dt_load_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","device_template_load_widget.ui"), parent)
        contents_widget = dt_load_widget.frame_dt_load.findChild(QFrame,"frame_contents")
        self.dt_value = contents_widget.findChild(QLineEdit,"lineEdit_dt_path_value")
        self.browse_dt_button = contents_widget.findChild(QPushButton,"pushButton_browse_in_file")
        self.browse_dt_button.clicked.connect(self.clicked_browse_dt_button)
        
        #Main layout
        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        main_layout.addWidget(dt_load_widget)

    @Slot()
    def clicked_browse_dt_button(self):
        json_filter = "JSON Device Template files (*.json *.JSON)"
        filepath = QFileDialog.getOpenFileName(filter=json_filter)
        self.input_file_path = filepath[0]
        self.dt_value.setText(self.input_file_path)
        self.controller.load_local_device_template(self.input_file_path)
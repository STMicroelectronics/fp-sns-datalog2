
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

from dataclasses import dataclass
from functools import partial
import time
import os

from PySide6.QtWidgets import QLabel, QRadioButton, QPushButton, QLineEdit, QVBoxLayout, QWidget, QHBoxLayout, QComboBox, QFrame, QFileDialog, QApplication
from PySide6.QtGui import QDoubleValidator, QIntValidator
from PySide6.QtCore import QTimer
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection
from st_pnpl.PnPLCmd import PnPLCMDManager
import st_dtdl_gui
from st_dtdl_gui.UI.styles import STDTDL_PushButton
from st_dtdl_gui.Widgets.LoadingWindow import LoadingWindow
from st_dtdl_gui.Utils.DataClass import TypeEnum

class CommandField():
    def __init__(self, f_name, f_type, f_label, f_value):
        self.f_name = f_name
        self.f_type = f_type
        self.f_label = f_label
        self.f_value = f_value

@dataclass 
class MLC_CmdValues:
    mlc_ucf:str = "mlc_ucf"

@dataclass
class ISPU_CmdValues:
    ispu_ucf = "ispu_ucf"
    ispu_json = "ispu_json"

class CommandWidget(QWidget):
    def __init__(self, controller, comp_name, comp_sem_type, request_name, command_name, command_fields, command_label=None, parent=None):
        super().__init__(parent)
        self.app = QApplication.instance()
        self.controller = controller
        self.comp_name = comp_name
        self.comp_sem_type = comp_sem_type
        self.request_name = request_name
        self.command_name = command_name
        self.command_fields = command_fields
        self.values = dict()
        self.labels = dict()
        self.loaded_file_path = {}
        self.loaded_file_value = {}
        self.file_id_list = []

        QPyDesignerCustomWidgetCollection.registerCustomWidget(CommandWidget, module="CommandWidget")
        loader = QUiLoader()
        command_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","send_command_widget.ui"), parent)
        command_title_frame = command_widget.frame_component_config.findChild(QFrame,"frame_title")
        command_fields_widget = command_widget.frame_component_config.findChild(QFrame,"frame_contents")
        command_send_button = command_widget.findChild(QPushButton,"pushButton")
        command_send_button.clicked.connect(partial(self.clicked_send_command_button, self, self.file_id_list))
        
        if command_label is not None:
            command_title_label = command_title_frame.findChild(QLabel,"label_title")
            command_title_label.setText(command_label)

        layout = QVBoxLayout(self)
        command_fields_frame = QFrame()
        if "load_file" not in request_name:
            for f in self.command_fields:
                self.labels[f.f_name] = QLabel(f.f_label)
                self.labels[f.f_name].setFixedWidth(150)
                if f.f_type == TypeEnum.STRING.value:
                    self.values[f.f_name] = QLineEdit(f.f_value)
                elif f.f_type == TypeEnum.DOUBLE.value:
                    self.validator = QDoubleValidator()
                    self.values[f.f_name] = QLineEdit(f.f_value)
                    self.values[f.f_name].setValidator(self.validator)
                elif f.f_type == TypeEnum.INTEGER.value:
                    self.validator = QIntValidator(0,1000,self)
                    self.values[f.f_name] = QLineEdit(f.f_value)
                    self.values[f.f_name].setValidator(self.validator)
                elif f.f_type == TypeEnum.BOOLEAN.value:
                    self.values[f.f_name] = QRadioButton(f.f_value)
                elif f.f_type == TypeEnum.ENUM.value:
                    if not f.f_name in self.values:
                        self.values[f.f_name] = QComboBox()
                    self.values[f.f_name].addItem(f.f_label)
                else:
                    self.values[f.f_name] = QLineEdit("UNKNOWN")

                self.values[f.f_name].setFixedWidth(200)
        
            for l in self.labels:
                in_layout = QHBoxLayout()
                in_layout.addWidget(self.labels[l])
                in_layout.addWidget(self.values[l])
                layout.addLayout(in_layout)
        else:
            for i in range(0, len(command_fields), 2):
                browse_file_button = QPushButton("Browse...")
                browse_file_button.setFixedHeight(30)
                browse_file_button.setStyleSheet(STDTDL_PushButton.valid)
                browse_file_button.adjustSize()
                if "_mlc" in comp_name:
                    ext_filter = "UCF Configuration files (*.ucf *.UCF)"
                    file_id = MLC_CmdValues.mlc_ucf
                if "_ispu" in comp_name:
                    if "ucf" in command_fields[i].f_name:
                        file_id = ISPU_CmdValues.ispu_ucf
                    elif "output" in command_fields[i].f_name:
                        file_id = ISPU_CmdValues.ispu_json                
                if "ucf" in command_fields[i].f_label.lower():
                    ext_filter = "UCF Configuration files (*.ucf *.UCF)"
                elif "json" in command_fields[i].f_label.lower():
                    ext_filter = "JSON files (*.json *.JSON)"
                
                self.file_id_list.append(file_id)
                file_desc = " ".join(command_fields[i].f_label.split(' ')[0:-1])
                loaded_file_label = QLabel(file_desc)
                loaded_file_label.setFixedHeight(30)
                self.loaded_file_value[file_id] = QLineEdit()
                self.loaded_file_value[file_id].setFixedHeight(30)
                self.loaded_file_value[file_id].setContentsMargins(9,0,9,0)
                self.loaded_file_value[file_id].setReadOnly(True)
                
                browse_file_button.clicked.connect(partial(self.clicked_browse_file_button, self, file_id, ext_filter))
                in_layout = QHBoxLayout()
                in_layout.addWidget(loaded_file_label)
                in_layout.addWidget(self.loaded_file_value[file_id])
                in_layout.addWidget(browse_file_button)
                in_layout.setContentsMargins(6,6,6,6)
                layout.addLayout(in_layout)

        if len(command_fields)/2 > 1:
            layout.setSpacing(12)
            layout.setContentsMargins(0,0,0,12)
        else:
            layout.setContentsMargins(0,0,0,24)

        command_fields_frame.setLayout(layout)
        command_fields_widget.layout().addWidget(command_fields_frame)
            
        self.setContentsMargins(26, 9, 9, 9)
            
        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        main_layout.addWidget(command_widget)
    
    def __load_ucf_timer_done(self, ai_sensor_name, widget, loading_window):
        #update all components. UCF can modify other sensors (of the same Component) configuration
        for cn in list(self.controller.components_dtdl.keys()):
            if ai_sensor_name in cn:
                self.controller.update_component_status(cn, widget.comp_sem_type)
        loading_window.loadingDone()
                
    def clicked_send_command_button(self, widget, file_id_list):
        message_fields = dict()
        if widget.request_name == "load_file":
            if "_mlc" in self.comp_name:
                self.controller.upload_mlc_ucf_file(self.comp_name, self.loaded_file_path[MLC_CmdValues.mlc_ucf])
                loading_window = LoadingWindow("Loading...", "UCF Configuration file Loading", self)
                QTimer.singleShot(5000, lambda: self.__load_ucf_timer_done(self.comp_name.split('_')[0], widget, loading_window))
                return
            elif "_ispu" in self.comp_name:
                loading_window = LoadingWindow("Loading...", "UCF Configuration file Loading", self)
                self.controller.upload_ispu_ucf_file(self.comp_name, self.loaded_file_path[ISPU_CmdValues.ispu_ucf], self.loaded_file_path[ISPU_CmdValues.ispu_json])
                QTimer.singleShot(5000, lambda: self.__load_ucf_timer_done(self.comp_name.split('_')[0], widget, loading_window))
                return
            else:
                self.controller.upload_file(self.comp_name, self.loaded_file_path)
        else:
            for f in widget.command_fields:
                if f.f_type == TypeEnum.STRING.value:
                    message_fields[f.f_name] = widget.values[f.f_name].text()
                elif f.f_type == TypeEnum.DOUBLE.value:
                    message_fields[f.f_name] = float(widget.values[f.f_name].text())
                elif  f.f_type == TypeEnum.INTEGER.value:
                    message_fields[f.f_name] = int(widget.values[f.f_name].text())
                elif f.f_type == TypeEnum.BOOLEAN.value:
                    message_fields[f.f_name] = widget.values[f.f_name].isChecked()
                elif f.f_type == TypeEnum.ENUM.value:
                    if type(f.f_value) == int:
                        message_fields[f.f_name] = widget.values[f.f_name].currentIndex()
                    else:
                        message_fields[f.f_name] = widget.values[f.f_name].itemText(widget.values[f.f_name].currentIndex())
            
            json_string = PnPLCMDManager.create_command_cmd(widget.comp_name, widget.request_name, self.command_name, message_fields)
            self.controller.send_command(json_string)
        
        self.controller.update_component_status(widget.comp_name, widget.comp_sem_type)
        
    def clicked_browse_file_button(self, widget, file_id, filter):
        if "_mlc" in self.comp_name:
            ucf_filter = "UCF Configuration files (*.ucf *.UCF)"
            filepath = QFileDialog.getOpenFileName(filter=ucf_filter)
            self.loaded_file_path[file_id] = filepath[0]
            self.loaded_file_value[file_id].setText(self.loaded_file_path[file_id])
        elif "_ispu" in self.comp_name:
            ext_filter = filter
            filepath = QFileDialog.getOpenFileName(filter=ext_filter)
            self.loaded_file_path[file_id] = filepath[0]
            self.loaded_file_value[file_id].setText(self.loaded_file_path[file_id])
        else:
            filepath = QFileDialog.getOpenFileName()
            self.loaded_file_path[file_id] = filepath[0]
            self.loaded_file_value[file_id].setText(self.loaded_file_path[file_id])
            self.controller.upload_file(self.loaded_file_path[file_id])
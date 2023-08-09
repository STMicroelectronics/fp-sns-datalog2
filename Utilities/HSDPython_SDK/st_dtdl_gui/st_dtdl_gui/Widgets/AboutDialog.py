
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

from PySide6.QtWidgets import QDialog, QVBoxLayout, QWidget, QLabel
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import st_dtdl_gui

class AboutDialog(QDialog):
    def __init__(self, controller, app_title, app_credits, app_version, parent=None):
        super().__init__(parent)
        self.controller = controller
        
        self.setWindowTitle("About")
        
        QPyDesignerCustomWidgetCollection.registerCustomWidget(AboutDialog, module="AboutDialog")
        loader = QUiLoader()
        about_dialog = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","about_dialog.ui"), parent)
        main_dialog_widget = about_dialog.findChild(QWidget,"main_dialog_widget")
        
        about_dialog_title = about_dialog.findChild(QLabel, "about_dialog_title")
        about_dialog_title.setText(app_title)
        label_credits = about_dialog.findChild(QLabel, "label_credits")
        label_credits.setText(app_credits)
        label_version = about_dialog.findChild(QLabel, "label_version")
        label_version.setText(app_version)
        
        #Main layout
        main_layout = QVBoxLayout()
        main_layout.setContentsMargins(0,0,0,0)
        main_dialog_widget.setStyleSheet("background-color: rgb(44, 49, 60); color: rgb(210, 210, 210);")
        self.setLayout(main_layout)
        main_layout.addWidget(main_dialog_widget)
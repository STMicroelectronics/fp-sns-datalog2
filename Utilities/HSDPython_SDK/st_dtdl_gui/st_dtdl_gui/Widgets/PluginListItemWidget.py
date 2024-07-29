
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

from PySide6.QtWidgets import QLabel, QPushButton, QVBoxLayout, QWidget, QFrame
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import st_dtdl_gui

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class PluginListItemWidget(QWidget):
    def __init__(self, plugin_name, plugin_item, parent=None):
        
        super().__init__(parent)
        self.parent = parent
        
        self.item = plugin_item
        self.plugin_name = plugin_name
        self.setWindowTitle(plugin_name)
        
        QPyDesignerCustomWidgetCollection.registerCustomWidget(PluginListItemWidget, module="PluginListItemWidget")
        loader = QUiLoader()
        plugin_item_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","plugin_list_item_widget.ui"), parent)
        # self.frame_plugin:QFrame = plugin_item_widget.frame_acquisition
        self.frame_plugin_name = plugin_item_widget.findChild(QFrame,"frame_plugin_name")
        self.label_plugin_name = self.frame_plugin_name.findChild(QPushButton,"label_plugin_name")
        self.label_plugin_name.setText(self.plugin_name)
        self.label_plugin_name.clicked.connect(self.clicked_plugin_name)
        self.label_plugin_notes = self.frame_plugin_name.findChild(QLabel,"label_plugin_notes")

        #Main layout
        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        main_layout.addWidget(plugin_item_widget)
        self.shrinked_size = self.sizeHint()
        
    def clicked_plugin_name(self):
        self.parent.setCurrentItem(self.item)
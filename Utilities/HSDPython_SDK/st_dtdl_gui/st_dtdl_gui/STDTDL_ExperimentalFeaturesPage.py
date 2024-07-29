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

import os
from PySide6.QtWidgets import QFrame, QLineEdit, QPushButton, QListWidget, QListWidgetItem, QFileDialog, QCheckBox, QGroupBox
from PySide6.QtCore import QDir
import st_dtdl_gui.UI.images #NOTE don't delete this! it is used from resource_filename (@row 35)

from pkg_resources import resource_filename

from st_dtdl_gui.Widgets.PluginListItemWidget import PluginListItemWidget
hsd2_folder_icon_path = resource_filename('st_dtdl_gui.UI.icons', 'baseline_folder_open_white_18dp.png')

from st_dtdl_gui.Widgets.AcqListItemWidget import AcqListItemWidget
from st_hsdatalog.HSD.HSDatalog import HSDatalog
import st_hsdatalog.HSD_utils.logger as logger
from st_pnpl.PnPLCmd import PnPLCMDManager
log = logger.get_logger(__name__)

class STDTDL_ExperimentalFeaturesPage():
    def __init__(self, page_widget, controller):
        self.controller = controller
        self.selected_acquisitions = []

        # self.controller.sig_user_login_done.connect(self.s_user_login_cb)


        self.page_widget = page_widget
        self.main_layout = page_widget.findChild(QFrame, "frame_experimental_features")

        # Data Toolkit settings frame
        self.dt_frame_content = page_widget.findChild(QFrame, "dt_frame_content")
        self.dt_frame_content.setEnabled(False)
        self.dt_plugins_folder_button = self.dt_frame_content.findChild(QPushButton, "dt_plugins_folder_button")
        self.dt_plugins_folder_button.clicked.connect(self.select_dt_plugins_folder)
        self.dt_plugin_folder_lineEdit:QLineEdit = self.dt_frame_content.findChild(QLineEdit, "dt_plugins_folder_lineEdit")
        self.dt_enabled_checkBox = page_widget.findChild(QCheckBox, "dt_enabled_checkBox")
        self.dt_enabled_checkBox.toggled.connect(self.dt_enable_button_toggled)
        self.dt_plugin_listWidget = page_widget.findChild(QListWidget, "dt_plugin_listWidget")

        # layout_device_config
        self.acq_upload_main_layout = page_widget.findChild(QFrame, "acq_upload_frame")
        self.acq_upload_main_layout.setEnabled(False)
        self.acq_upload_main_layout.setVisible(False)

        self.login_button = page_widget.findChild(QPushButton, "login_button")
        self.login_button.clicked.connect(self.show_login_dialog)

        self.groupBox_base_acquisition_selection = page_widget.findChild(QFrame, "groupBox_base_acquisition_selection")
        self.base_acq_folder_button = page_widget.findChild(QPushButton, "base_acq_folder_button")
        self.base_acq_folder_button.clicked.connect(self.select_base_acquisitions_folder)
        self.base_acq_folder_textEdit:QLineEdit = page_widget.findChild(QLineEdit, "base_acq_folder_textEdit")
        self.groupBox_base_acquisition_selection.setEnabled(False)

        self.groupBox_acquisitions_list = page_widget.findChild(QGroupBox, "groupBox_acquisitions_list")
        self.acquisitions_listWidget:QListWidget = page_widget.findChild(QListWidget, "acquisitions_listWidget")
        self.acquisitions_listWidget.itemSelectionChanged.connect(self.acquisition_selected)
        self.groupBox_acquisitions_list.setEnabled(False)

        self.groupBox_upload_settings = page_widget.findChild(QGroupBox, "groupBox_upload_settings")
        self.upload_acquisition_button = page_widget.findChild(QPushButton, "upload_acquisition_button")
        self.upload_acquisition_button.clicked.connect(self.upload_acquisitions)
        self.groupBox_upload_settings.setEnabled(False)

    def select_dt_plugins_folder(self):
        # Open a dialog to select a directory
        folder_path = QFileDialog.getExistingDirectory(None, 'Select Folder')
        if folder_path:
            self.dt_plugin_listWidget.clear()
            self.dt_plugin_folder_lineEdit.setText(folder_path)
            self.controller.set_dt_plugins_folder(folder_path)
            # List all .py files in the specified data toolkit plugins directory
            files = os.listdir(folder_path)
            py_files = [f for f in files if os.path.isfile(os.path.join(folder_path, f)) and f.endswith('.py') and f != "__init__.py"]
            # Remove the .py extension
            plugin_modules = [os.path.splitext(f)[0] for f in py_files]
            for pm in plugin_modules:
                item = QListWidgetItem(self.dt_plugin_listWidget)
                custom_widget = PluginListItemWidget(pm, item, self.dt_plugin_listWidget)                    
                self.dt_plugin_listWidget.addItem(item)
                self.dt_plugin_listWidget.setItemWidget(item, custom_widget)
                item.setSizeHint(custom_widget.sizeHint())
            self.dt_enable_button_toggled(True)

    def dt_enable_button_toggled(self, status):
        self.dt_frame_content.setEnabled(status)
        if status:
            if self.dt_plugin_folder_lineEdit.text() != "":
                self.controller.set_dt_plugins_folder(self.dt_plugin_folder_lineEdit.text())
                self.controller.create_data_pipeline()
        else:
            self.controller.set_dt_plugins_folder(None)
            self.controller.destroy_data_pipeline()

    def show_login_dialog(self):
        print("Login button clicked")
        self.groupBox_base_acquisition_selection.setEnabled(True)
    
    def select_base_acquisitions_folder(self):
        # Open a dialog to select a directory
        folder_path = QFileDialog.getExistingDirectory(None, 'Select Folder')
        if folder_path:
            self.base_acq_folder_textEdit.setText(folder_path)
            
            self.groupBox_acquisitions_list.setEnabled(True)
            # Clear the list widget
            self.acquisitions_listWidget.clear()

            # Get the list of folders in the selected directory
            dir = QDir(folder_path)
            dir.setFilter(QDir.Dirs | QDir.NoDotAndDotDot)
            folder_list = dir.entryList()

            # hsd_factory = HSDatalog()
            
            # Add folders to the list widget with icons
            for folder in folder_list:
                hsd_version = HSDatalog.validate_hsd_folder(os.path.join(folder_path, folder))
                if hsd_version != HSDatalog.HSDVersion.INVALID:
                    # self.hsd= hsd_factory.create_hsd(os.path.join(folder_path, folder))
                    # print(HSDatalog.present_sensor_list(self.hsd))
                    item = QListWidgetItem(self.acquisitions_listWidget)
                    # custom_widget = AcqListItemWidget(self.controller, folder, HSDatalog.get_sensor_list(self.hsd))
                    custom_widget = AcqListItemWidget(self.controller, hsd_version, folder_path, folder, item, self.acquisitions_listWidget)                    
                    self.acquisitions_listWidget.addItem(item)
                    self.acquisitions_listWidget.setItemWidget(item, custom_widget)
                    item.setSizeHint(custom_widget.sizeHint())
                    # item = QListWidgetItem(QIcon(QPixmap(hsd2_folder_icon_path)), folder)
                    # self.acquisitions_listWidget.addItem(item)
                else:
                    log.warning(f"Acquisition {folder}")

    def upload_acquisitions(self):
        for acq_item in self.acquisitions_listWidget.selectedItems():
            print(acq_item.listWidget().itemWidget(acq_item).acq_folder_path)

    def acquisition_selected(self):
        self.selected_items = self.acquisitions_listWidget.selectedItems()
        print(self.selected_items)
        self.groupBox_upload_settings.setEnabled(True)
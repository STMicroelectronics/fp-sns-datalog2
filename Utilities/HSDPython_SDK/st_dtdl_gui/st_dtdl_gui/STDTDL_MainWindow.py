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

import sys
import os
from datetime import datetime

from PySide6.QtWidgets import QMainWindow, QStackedWidget, QTextEdit, QLabel, QPushButton, QWidget, QFrame
from PySide6.QtCore import Slot

from st_dtdl_gui.UI.styles import STDTDL_MenuButton
from st_dtdl_gui.UI.Ui_MainWindow import Ui_MainWindow
from st_dtdl_gui.Widgets.LoadingWindow import LoadingWindow
from st_dtdl_gui.Widgets.ConnectionWidget import ConnectionWidget
from st_dtdl_gui.Widgets.AboutDialog import AboutDialog
from st_dtdl_gui.Widgets.ToggleButton import ToggleButton

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class STDTDL_MainWindow(QMainWindow):
    
    def __init__(self, app, controller, parent=None):
        super(STDTDL_MainWindow, self).__init__(parent)
        
        self.app = app

        self.controller = controller
        self.controller.sig_device_connected.connect(self.s_device_connected)
        self.controller.sig_dtm_loading_started.connect(self.s_dtm_loading_started)
        self.controller.sig_dtm_loading_completed.connect(self.s_dtm_loaded)

        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        
        self.app_title = "ST DTDL GUI"
        self.app_credit = "created by ST"
        self.app_version = ""
        self.ui.label_app_title.setText(self.app_title)
        self.ui.label_credits.setText(self.app_credit)
        self.ui.label_version.setText(self.app_version)
        
        self.loading_window = None
        
        self.setWindowTitle(self.app_title)

        # Find the widgets in the xml file
        # Main stacked widget (Application page manager)
        self.page_manager = self.findChild(QStackedWidget, "stacked_widget")
        
        # Connection page
        self.connection_page = self.findChild(QWidget, "page_connection")
        frame_log_file_options = self.findChild(QFrame, "frame_log_file_options")
        app_log_file_label = QLabel("Enable application log file")
        app_log_file_label.setStyleSheet("font: 700 10pt \"Segoe UI\";")
        frame_log_file_options.layout().addWidget(app_log_file_label)
        app_log_file_toggle_button = ToggleButton()
        frame_log_file_options.layout().addWidget(app_log_file_toggle_button)
        app_log_file_toggle_button.toggled.connect(self.app_log_file_button_toggled)

        self.connection_widget = ConnectionWidget(self.controller,self)
        self.connection_page.layout().addWidget(self.connection_widget)

        self.connection_page.layout().addStretch()
        
        # Device Components Configuration page
        self.configuration_widget = self.findChild(QWidget, "page_device_config") #change name in .ui file
        # for plots processEvent
        self.controller.set_Qt_app(self.app)
        self.device_conf_page = None

        self.widget_device_config = self.findChild(QFrame, "widget_device_config")
        self.widget_plots = self.findChild(QWidget, "widget_plots")
        self.widget_header = self.findChild(QWidget, "widget_header")

        # Application Log file display page
        self.show_log_file_page = self.findChild(QWidget, "page_app_log_file")
        self.log_file_text_edit:QTextEdit = self.show_log_file_page.findChild(QTextEdit, "log_file_textEdit")
        self.log_file_text_title:QLabel = self.show_log_file_page.findChild(QLabel, "log_file_title")

        # self.show_log_file_page.layout().addStretch()

        # Set the first displayed [Connection] Page
        self.page_manager.setCurrentWidget(self.connection_page)

        # Left Menu Items (page navigation menu)
        self.menu_btn_connection = self.findChild(QPushButton, "menu_btn_connection")
        self.menu_btn_connection.clicked.connect(self.clicked_menu_connection)
        self.menu_btn_device_conf = self.findChild(QPushButton, "menu_btn_device_conf")
        self.menu_btn_device_conf.clicked.connect(self.clicked_menu_device_conf)
        self.menu_btn_about = self.findChild(QPushButton, "menu_btn_about")
        self.menu_btn_about.clicked.connect(self.clicked_menu_about)
        self.menu_btn_show_log_file = self.findChild(QPushButton, "menu_btn_show_log_file")
        self.menu_btn_show_log_file.clicked.connect(self.clicked_menu_btn_show_log_file)

        # Hide Configuration menu button (it will be unhided when a device will be connected) 
        self.menu_btn_device_conf.setVisible(False)
        # Hide Application file viewer menu button (it will be unhided if The user wants to save the application log file) 
        self.menu_btn_show_log_file.setVisible(False)
    
    def quit(self):
        sys.exit(0)

    def clicked_menu_connection(self):
        self.page_manager.setCurrentWidget(self.connection_page)
        self.menu_btn_device_conf.setStyleSheet(STDTDL_MenuButton.unselected_device_conf_stylesheet)        
    
    def clicked_menu_device_conf(self):
        self.page_manager.setCurrentWidget(self.device_conf_page.page_widget)
        self.menu_btn_device_conf.setStyleSheet(STDTDL_MenuButton.selected_device_conf_stylesheet)
        if self.loading_window is not None:
            self.loading_window.loadingDone()

    def clicked_menu_about(self):
        print("WARNING - About screen will be available soon.")
        dlg = AboutDialog(self, self.app_title, self.app_credit, self.app_version)
        dlg.exec_()

    def clicked_menu_btn_show_log_file(self):
        self.page_manager.setCurrentWidget(self.show_log_file_page)
        self.menu_btn_device_conf.setStyleSheet(STDTDL_MenuButton.unselected_device_conf_stylesheet)
        for handler in log.parent.handlers:
            if hasattr(handler, "baseFilename"):
                log_file_name = getattr(handler, 'baseFilename')
                self.log_file_text_title.setText("Log File: {}".format(os.path.basename(log_file_name)))
                # print(f"writing log to {log_file_name}")
                log_text=open(log_file_name).read()
                self.log_file_text_edit.setText(log_text)

    def closeEvent(self, event):
        event.accept()
    
    def setAppTitle(self, title:str):
        self.ui.label_app_title.setText(title)
    
    def setAppCredits(self, credits:str):
        self.ui.label_credits.setText(credits)
        
    def setAppVersion(self, version:str):
        self.ui.label_version.setText(version)
    
    def setLogMsg(self, log_msg:str):
        self.controller.set_log_msg(log_msg)
        
    def setComponentsConfigWidth(self, width):
        self.controller.set_component_config_width(width)

    @Slot(bool)
    def s_device_connected(self, status):
        if status:
            self.menu_btn_device_conf.setVisible(True)
            self.menu_btn_connection.setStyleSheet(STDTDL_MenuButton.connected_stylesheet)
        else:
            self.menu_btn_device_conf.setVisible(False)
            self.menu_btn_connection.setStyleSheet(STDTDL_MenuButton.unconnected_stylesheet)
            
            for i in reversed(range(self.widget_header.layout().count())): 
                self.widget_header.layout().itemAt(i).widget().setParent(None)
            
            for i in reversed(range(self.widget_device_config.layout().count())): 
                self.widget_device_config.layout().itemAt(i).widget().setParent(None)

            for i in reversed(range(self.widget_plots.layout().count())): 
                self.widget_plots.layout().itemAt(i).widget().setParent(None)
    
    @Slot()                
    def s_dtm_loading_started(self):       
        self.loading_window = LoadingWindow("Loading...", "Device Template Model Loading", self.page_manager)
        self.app.processEvents()
    
    @Slot()
    def s_dtm_loaded(self):
        self.clicked_menu_device_conf()

    @Slot()
    def app_log_file_button_toggled(self, status):
        self.menu_btn_show_log_file.setVisible(status)
        logger.setup_applevel_logger(is_debug=status, file_name= "{}_app_debug.log".format(datetime.today().strftime('%Y%m%d_%H_%M_%S')))
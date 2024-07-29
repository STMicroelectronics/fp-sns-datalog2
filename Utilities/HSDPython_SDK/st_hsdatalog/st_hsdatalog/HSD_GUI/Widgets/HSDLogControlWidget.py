
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

from datetime import datetime
import os
import math
import threading
import time

from st_dtdl_gui.UI.styles import STDTDL_Label, STDTDL_PushButton
from st_dtdl_gui.Widgets.LoadingWindow import StaticLoadingWindow

import matplotlib.pyplot as plt

from PySide6.QtCore import Slot, Qt
from PySide6.QtWidgets import QFrame, QSpinBox, QComboBox, QPushButton, QCheckBox, QFileDialog, QGroupBox, QRadioButton, QLabel, QLineEdit
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
import st_hsdatalog
from st_hsdatalog.HSD.HSDatalog import HSDatalog
from st_hsdatalog.HSD_GUI.HSD_Controller import AutomodeStatus
import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class HSDLogControlWidget(ComponentWidget):
    
    def __init__(self, controller, comp_contents, comp_name="log_controller", comp_display_name = "Log Controller" ,comp_sem_type="other", c_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)
        
        self.controller.sig_autologging_is_stopping.connect(self.s_is_autologging_stopping)
        self.controller.sig_offline_plots_completed.connect(self.s_offline_plots_completed)
        self.controller.sig_lock_start_button.connect(self.s_lock_start_button)
        
        self.app = self.controller.qt_app
        self.is_waiting_to_start = False
        self.is_logging = False
        self.parent_widget = parent
        self.hsd = None

        self.curr_start_log_button_statue = True

        self.sd_mounted_label = QLabel()
        # clear all widgets in contents_widget layout (contents)
        for i in reversed(range(self.contents_widget.layout().count())):
            self.contents_widget.layout().itemAt(i).widget().deleteLater()

        self.setWindowTitle(comp_display_name)

        QPyDesignerCustomWidgetCollection.registerCustomWidget(HSDLogControlWidget, module="LogControlWidget")
        loader = QUiLoader()
        log_control_widget = loader.load(os.path.join(os.path.dirname(st_hsdatalog.__file__),"HSD_GUI","UI","log_control_widget.ui"))
        frame_contents = log_control_widget.frame_log_control.findChild(QFrame,"frame_contents")
        # Plots Time Window
        self.time_spinbox = frame_contents.findChild(QSpinBox,"spinBox_time_window")
        self.time_spinbox.valueChanged.connect(self.plot_window_time_change)
        self.time_spinbox.setValue(30)
        # Start/Stop Log PushButton
        self.log_start_button = frame_contents.findChild(QPushButton,"start_button")
        self.log_start_button.clicked.connect(self.clicked_start_log_button)

        self.config_errors_label = frame_contents.findChild(QLabel,"config_errors_label")
        self.config_errors_label.setVisible(False)
        
        #TODO generic logging interfaces management (update SD card status message only if SD card interface is supported)
        # # SD Mounted Label
        self.frame_sub_log_info = log_control_widget.frame_log_control.findChild(QFrame,"frame_sub_log_info")
        self.sd_mounted_label.setContentsMargins(6,0,0,0)
        self.frame_sub_log_info.layout().addWidget(self.sd_mounted_label)
        self.frame_sub_log_info.setEnabled(False)
        self.frame_sub_log_info.setVisible(False)
        
        # # Available interfaces Refresh button        
        self.sd_mounted = self.controller.get_sd_mounted_status()
        # # self.sd_mounted.setFixedHeight(0)
        # # self.sd_mounted_label.setText("SD Mounted") if self.sd_mounted else self.sd_mounted_label.setText("SD NOT Mounted")
        # # self.__update_sd_status_label()
        self.update_sd_status_label()
        
        self.refresh_sd_button = frame_contents.findChild(QPushButton,"refresh_sd_button")
        self.refresh_sd_button.clicked.connect(self.clicked_refresh_button)
        self.refresh_sd_button.setEnabled(False)
        self.refresh_sd_button.setVisible(False)
        #TODO end todo

        self.save_files_checkbox = frame_contents.findChild(QCheckBox,"save_files_checkbox")
        self.save_files_checkbox.stateChanged.connect(self.checkBox_save_files_checked)

        # Log Interfaces ComboBox
        # self.interface_combobox = frame_contents.findChild(QComboBox,"interface_combobox")
        # ==============================================================================================================
        #NOTE uncomment the following instructions when FW support start logging on different interfaces (e.g. SD card)
        # self.log_interfaces_enums = [c for c in comp_contents if c.name == "start_log"][0].request.schema.enum_values
        # for v in self.log_interfaces_enums:
        #     if v.name == "sd_card":
        #         if self.sd_mounted:
        #             self.interface_combobox.addItem(v.display_name if isinstance(v.display_name,str) else v.display_name.en)
        #     else:
        #         self.interface_combobox.addItem(v.display_name if isinstance(v.display_name,str) else v.display_name.en)
        # self.interface_combobox.setCurrentIndex(self.interface_combobox.count() - 1)
        # self.interface_combobox.addItem("USB")
        # ==============================================================================================================

        # Save Config PushButton
        self.save_config_button = log_control_widget.frame_log_control.findChild(QPushButton,"save_conf_button")
        self.save_config_button.clicked.connect(self.clicked_save_config_button)
        # Load Config PushButton
        self.load_config_button = log_control_widget.frame_log_control.findChild(QPushButton,"load_conf_button")
        self.load_config_button.clicked.connect(self.clicked_load_config_button)
        # Offline plot settings GroupBox
        self.groupBox_offline_plot = frame_contents.findChild(QGroupBox, "groupBox_offline_plot")
        self.groupBox_offline_plot.setVisible(False)
        # Online plot settings GroupBox
        self.groupBox_online_plot = frame_contents.findChild(QGroupBox, "groupBox_online_plot")
        self.groupBox_online_plot.setVisible(True)
        # Offline plot settings GroupBox
        self.checkBox_offline = frame_contents.findChild(QCheckBox, "checkBox_offline")
        self.checkBox_offline.stateChanged.connect(self.checkBox_offline_checked)
        # Online plot settings GroupBox
        self.checkBox_online = frame_contents.findChild(QCheckBox, "checkBox_online")
        self.checkBox_online.stateChanged.connect(self.checkBox_online_checked)
        
        self.acq_folder_button = frame_contents.findChild(QPushButton, "acq_folder_button")
        self.acq_folder_button.clicked.connect(self.browse_acq_folder_clicked)
        self.acq_folder_lineEdit = frame_contents.findChild(QLineEdit, "acq_folder_lineEdit")
        self.acq_folder = os.getcwd()
        self.acq_folder_lineEdit.setText(self.acq_folder)
        self.subfolder_checkbox = frame_contents.findChild(QCheckBox,"subfolder_checkbox")
        self.subfolder_checkbox.stateChanged.connect(self.checkBox_subfolder_checked)
        self.sub_folder = True
        self.subfolder_checkbox.setChecked(True)

        self.ds_component_names_combo = frame_contents.findChild(QComboBox,"sensor_names_combo")

        self.spectrum_radio = frame_contents.findChild(QRadioButton,"spectrum_radio")
        self.spectrum_flag = False
        self.spectrum_radio.toggled.connect(self.toggled_spectrum_flag)
        self.spectrum_radio.setEnabled(False)
        self.spectrum_radio.setVisible(False)
        
        self.debug_radio = frame_contents.findChild(QRadioButton,"debug_radio")
        self.debug_flag = False
        self.debug_radio.toggled.connect(self.toggled_debug_flag)
        self.debug_radio.setEnabled(False)
        self.debug_radio.setVisible(False)
        
        self.st_spinbox = frame_contents.findChild(QSpinBox,"st_spinbox")
        self.s_start = 0
        self.st_spinbox.setValue(self.s_start)
        self.et_spinbox = frame_contents.findChild(QSpinBox,"et_spinbox")
        self.s_end = -1
        self.et_spinbox.setValue(self.s_end)
        
        self.tags_label_combo = frame_contents.findChild(QComboBox,"tags_label_combo")
        self.tags_label_combo.setEnabled(False)
        self.tags_label_combo.setVisible(False)
        
        self.sub_plots_radio = frame_contents.findChild(QRadioButton,"sub_plots_radio")
        self.sub_plots_flag = False
        self.sub_plots_radio.toggled.connect(self.toggled_sub_plots_flag)
        self.sub_plots_radio.setEnabled(False)
        self.sub_plots_radio.setVisible(False)
        
        self.raw_data_radio = frame_contents.findChild(QRadioButton,"raw_data_radio")
        self.raw_data_flag = False
        self.raw_data_radio.toggled.connect(self.toggled_raw_data_flag)
        self.raw_data_radio.setEnabled(False)        
        self.raw_data_radio.setVisible(False)
        
        self.offline_plot_button = frame_contents.findChild(QPushButton,"offline_plot_button")
        self.offline_plot_button.clicked.connect(self.clicked_offline_plot_button)
        # Save Config Dialog
        loader = QUiLoader()
        self.save_config_dialog = loader.load(os.path.join(os.path.dirname(st_hsdatalog.__file__),"HSD_GUI","UI","save_config_dialog.ui"), self)
        
        self.save_config_dialog.setFixedSize(360,200)
        self.save_config_dialog.setWindowTitle("Save Configuration")
        self.pc_checkbox = self.save_config_dialog.dialog_frame.findChild(QCheckBox,"pc_checkbox")
        self.sd_checkbox = self.save_config_dialog.dialog_frame.findChild(QCheckBox,"sd_checkbox")
        # Save PushButton
        self.save_button = self.save_config_dialog.dialog_frame.findChild(QPushButton,"save_button")
        self.save_button.clicked.connect(self.clicked_save_button)
        # Cancel PushButton
        self.cancel_button = self.save_config_dialog.dialog_frame.findChild(QPushButton,"cancel_button")
        self.cancel_button.clicked.connect(self.clicked_cancel_button)
        
        self.layout().setContentsMargins(0,0,0,0)
        self.contents_widget.layout().setContentsMargins(9,0,9,0)
        self.contents_widget.layout().addWidget(log_control_widget.frame_log_control)
        self.contents_widget.setVisible(True)
    
    def update_sd_status_label(self):
        self.sd_mounted = self.controller.get_sd_mounted_status()#hsd_link.get_boolean_property(0,"log_controller","sd_mounted")
        if self.sd_mounted:
            self.sd_mounted_label.setText("SD Mounted")
            self.sd_mounted_label.setStyleSheet(STDTDL_Label.valid)
        else:
            self.sd_mounted_label.setText("SD NOT Mounted")
            self.sd_mounted_label.setStyleSheet(STDTDL_Label.invalid)
    @Slot()
    def clicked_refresh_button(self):
        # ==============================================================================================================
        #NOTE uncomment the following instructions when FW support start logging on different interfaces (e.g. SD card)
        # self.interface_combobox.clear()
        # for v in self.log_interfaces_enums:
        #     if v.name == "sd_card":
        #         if self.sd_mounted:
        #             self.interface_combobox.addItem(v.display_name if isinstance(v.display_name,str) else v.display_name.en)
        #             log.info("Log interfaces Combobox refreshed, SD Card detected")
        #         else:
        #             log.info("Log interfaces Combobox refreshed, No SD Card detected")
        #     else:
        #         self.interface_combobox.addItem(v.display_name if isinstance(v.display_name,str) else v.display_name.en)
        # self.interface_combobox.setCurrentIndex(self.interface_combobox.count() - 1)
        # ==============================================================================================================
        
        self.update_sd_status_label()
        # self.sd_mounted = self.controller.hsd_link.get_boolean_property(0,"log_controller","sd_mounted")
        # if self.sd_mounted:
        #     self.sd_mounted_label.setText("SD Mounted")
        #     self.sd_mounted_label.setStyleSheet("color: #DAF0E2;")
        # else:
        #     self.sd_mounted_label.setText("SD NOT Mounted")
        #     self.sd_mounted_label.setStyleSheet("color: #FFC7E6;")
        # self.sd_mounted_label.setText("SD Mounted") if self.sd_mounted else self.sd_mounted_label.setText("SD NOT Mounted")
    
    @Slot()
    def clicked_save_config_button(self):
        if self.sd_mounted:
           self.sd_checkbox.setVisible(True)
        else:
            self.sd_checkbox.setVisible(False)
        self.save_config_dialog.exec_()
        
    @Slot()
    def clicked_save_button(self):
        self.controller.save_config(self.pc_checkbox.isChecked(), self.sd_checkbox.isChecked())
        self.save_config_dialog.close()
    
    @Slot()
    def clicked_cancel_button(self):
        self.save_config_dialog.close()
        
    @Slot()
    def toggled_spectrum_flag(self, status):
        self.spectrum_flag = status

    @Slot()
    def toggled_debug_flag(self, status):
        self.debug_flag = status
        
    @Slot()
    def toggled_sub_plots_flag(self, status):
        self.sub_plots_flag = status
        
    @Slot()
    def toggled_raw_data_flag(self, status):
        self.raw_data_flag = status
        
    @Slot()
    def clicked_offline_plot_button(self):
        self.loading_window = StaticLoadingWindow("Plot ongoing...", "Acquired data extraction. Please wait...", self.parent_widget)
        self.app.processEvents()
        
        cb_sensor_value = self.ds_component_names_combo.currentText()
        tag_label = self.tags_label_combo.currentText()
        self.s_start = self.st_spinbox.value()
        self.s_end = self.et_spinbox.value()
        self.controller.do_offline_plots(cb_sensor_value, tag_label, self.s_start, self.s_end, self.active_sensor_list, self.active_algorithm_list, self.debug_flag, self.sub_plots_flag, self.raw_data_flag, self.active_actuator_list, self.spectrum_flag)
    
    @Slot()
    def s_lock_start_button(self, status, msg):
        if status != self.curr_start_log_button_statue:
            if status:
                self.log_start_button.setStyleSheet(STDTDL_PushButton.invalid)
            else:
                self.log_start_button.setStyleSheet(STDTDL_PushButton.green)
            self.config_errors_label.setVisible(status)
            self.config_errors_label.setText(msg)
            self.log_start_button.setEnabled(not status)
            self.curr_start_log_button_statue = status

    @Slot()
    def s_offline_plots_completed(self):
        self.loading_window.loadingDone()
        plt.show()

    @Slot()
    def s_is_autologging_stopping(self, status):
        self.log_start_button.setEnabled(not status)
        
    @Slot()
    def clicked_load_config_button(self):
        fname = QFileDialog.getOpenFileName(None, "Load a Device Configuration file", "device_config", "JSON (*.json)")
        self.controller.load_config(fname[0])

    @Slot()
    def checkBox_offline_checked(self, state):
        if state == Qt.Unchecked.value:
            self.groupBox_offline_plot.setVisible(False)
        elif state == Qt.Checked.value:
            self.groupBox_offline_plot.setVisible(True)

    @Slot()
    def checkBox_online_checked(self, state):
        if state == Qt.Unchecked.value:
            self.groupBox_online_plot.setVisible(False)
        elif state == Qt.Checked.value:
            self.groupBox_online_plot.setVisible(True)
    
    @Slot()
    def browse_acq_folder_clicked(self):
        options = QFileDialog.Options()
        options |= QFileDialog.Option.ShowDirsOnly
        folder = QFileDialog.getExistingDirectory(self, "Select Folder", options=options)
        if folder:
            self.acq_folder_lineEdit.setText(folder)
            self.acq_folder = folder

    @Slot()
    def checkBox_save_files_checked(self, state):
        if state == Qt.Unchecked.value:
            self.controller.set_save_files_flag(False)
        elif state == Qt.Checked.value:
            self.controller.set_save_files_flag(True)

    @Slot()
    def checkBox_subfolder_checked(self, state):
        if state == Qt.Unchecked.value:
            self.sub_folder = False
        elif state == Qt.Checked.value:
            self.sub_folder = True

            
    @Slot()
    def clicked_start_log_button(self):
        # ==============================================================================================================
        #NOTE uncomment the following instructions when FW support start logging on different interfaces (e.g. SD card)
        # if self.sd_mounted == False:
        #     interface = 1
        # else:
        #     interface = self.interface_combobox.currentIndex()
        interface = 1
        # ============================================================================================================== 
        if not self.is_logging:
            if not self.is_waiting_to_start:
                if self.controller.is_automode_enabled():
                    automode_status = self.controller.get_automode_status()
                    if automode_status == AutomodeStatus.AUTOMODE_UNSTARTED:
                        auto_settings = self.controller.get_automode_settings()
                        n = auto_settings[0] # Number of times to execute the timer
                        m = auto_settings[1] # Wait M seconds before the first start
                        x = auto_settings[2] # Execute for X seconds
                        y = auto_settings[3] # Wait Y seconds before the next execution
                        if m != 0:
                            self.save_files_checkbox.setEnabled(False)
                            self.log_start_button.setText("Stop Log")
                            self.log_start_button.setStyleSheet(STDTDL_PushButton.red)
                        self.automode_timer, self.stop_automode_timer = self.run_timer(n, m, x, y)
                    elif automode_status == AutomodeStatus.AUTOMODE_IDLE:
                        self.save_files_checkbox.setEnabled(True)
                        self.log_start_button.setText("Start Log")
                        self.log_start_button.setStyleSheet(STDTDL_PushButton.green)
                        self.stop_automode_timer()
                        self.controller.stop_idle_auto_log()
                        self.controller.set_automode_status(AutomodeStatus.AUTOMODE_UNSTARTED)
                    else: #automode_status == AutomodeStatus.AUTOMODE_LOGGING:
                        self.save_files_checkbox.setEnabled(True)
                        self.log_start_button.setText("Start Log")
                        self.log_start_button.setStyleSheet(STDTDL_PushButton.green)
                        self.stop_automode_timer()
                        self.controller.set_automode_status(AutomodeStatus.AUTOMODE_UNSTARTED)
                else:
                    self.acq_folder = self.acq_folder_lineEdit.text()
                    self.save_files_checkbox.setEnabled(False)
                    self.controller.start_log(interface, self.acq_folder, self.sub_folder)
            else:
                if self.controller.is_automode_enabled():
                    self.stop_automode_timer()
                    self.controller.stop_auto_log(1)
                    self.controller.stop_waiting_auto_log()
                    self.save_files_checkbox.setEnabled(True)
                    self.log_start_button.setText("Start Log")
                    self.log_start_button.setStyleSheet(STDTDL_PushButton.green)
                    self.is_waiting_to_start = False
        else:
            self.controller.update_component_status("acquisition_info")
            if self.controller.is_automode_enabled():
                self.stop_automode_timer()
                self.controller.set_automode_status(AutomodeStatus.AUTOMODE_UNSTARTED)
            self.save_files_checkbox.setEnabled(True)
            self.controller.stop_log(interface)

    @Slot(bool)
    def s_is_logging(self, status:bool, interface:int):        
        if status:
            self.log_start_button.setText("Stop Log")
            self.log_start_button.setStyleSheet(STDTDL_PushButton.red)
            self.is_logging = True
            if interface == 1:
                self.controller.start_plots()
            
            self.groupBox_offline_plot.setEnabled(False)
            self.offline_plot_button.setEnabled(False)
            self.st_spinbox.setEnabled(False)
            self.et_spinbox.setEnabled(False)
            self.time_spinbox.setEnabled(True)
        else:
            self.is_logging = False
            if interface == 1:
                self.controller.stop_plots()
            
            automode_status = self.controller.get_automode_status()
            if automode_status == AutomodeStatus.AUTOMODE_UNSTARTED:
                self.log_start_button.setText("Start Log")
                self.log_start_button.setStyleSheet(STDTDL_PushButton.green)

            if self.controller.get_save_files_flag():
                self.groupBox_offline_plot.setEnabled(True)
                self.offline_plot_button.setEnabled(True)
                self.st_spinbox.setEnabled(True)
                self.et_spinbox.setEnabled(True)

                acquisition_folder = self.controller.get_acquisition_folder()
                
                hsd_factory = HSDatalog()
                self.hsd= hsd_factory.create_hsd(acquisition_folder)
                
                self.active_sensor_list = self.hsd.get_sensor_list(only_active=True)
                self.active_algorithm_list = self.hsd.get_algorithm_list(only_active=True)
                self.active_actuator_list = self.hsd.get_actuator_list(only_active=True)
                self.ds_component_names_combo.clear()
                self.ds_component_names_combo.addItem("all")
                for s in self.active_sensor_list:
                    self.ds_component_names_combo.addItem(list(s.keys())[0])
                for a in self.active_algorithm_list:
                    self.ds_component_names_combo.addItem(list(a.keys())[0])
                for act in self.active_actuator_list:
                    self.ds_component_names_combo.addItem(list(act.keys())[0])
                self.ds_component_names_combo.setCurrentIndex(0)
                
                tags_label_list = self.hsd.get_acquisition_label_classes()
                self.tags_label_combo.clear()
                self.tags_label_combo.addItem("None")
                if tags_label_list is not None:
                    for t in tags_label_list:
                        self.tags_label_combo.addItem(t)
                    self.tags_label_combo.setCurrentIndex(0)
                
                acq_info_model = self.hsd.get_acquisition_info()
                start_time = acq_info_model["start_time"]
                end_time = acq_info_model["end_time"]            
                st_date = datetime.strptime(start_time, "%Y-%m-%dT%H:%M:%S.%fZ")
                et_date = datetime.strptime(end_time, "%Y-%m-%dT%H:%M:%S.%fZ")
                acq_duration_in_sec = (et_date - st_date).total_seconds()
                self.et_spinbox.setMaximum(math.ceil(acq_duration_in_sec))
            
    
    @Slot()
    def plot_window_time_change(self):
        self.controller.plot_window_changed(self.time_spinbox.value())

    def saveSettings(self, settings):
         settings.setValue("logcontrolwidget/timewindow", self.time_spinbox.value())

    def restoreSettings(self, settings):
         try:
             self.time_spinbox.setValue(int(settings.value("logcontrolwidget/timewindow")))
         except:
             pass
    
    # import threading
    def run_timer(self, n, m, x, y):
        # tim = threading.Event()
        def timer_thread():
            nonlocal stop_flag
            self.is_waiting_to_start = True
            self.controller.start_waiting_auto_log()
            # tim.wait(m)
            time.sleep(m)  # Wait M seconds before the first start
            self.is_waiting_to_start = False
            self.controller.stop_waiting_auto_log()
            if n != 0:
                for i in range(n):
                    if stop_flag:
                        break
                    self.controller.set_automode_status(AutomodeStatus.AUTOMODE_LOGGING)
                    self.controller.start_auto_log(1, self.acq_folder, True)
                    time.sleep(x)  # Wait X seconds
                    if stop_flag:
                        break
                    if i < n-1:
                        self.controller.set_automode_status(AutomodeStatus.AUTOMODE_IDLE)
                        self.controller.stop_auto_log(1)
                        self.log_start_button.setText("Stop Log")
                        self.log_start_button.setStyleSheet(STDTDL_PushButton.red)
                        self.controller.update_component_status("acquisition_info")
                        self.controller.start_idle_auto_log()
                        time.sleep(y)  # Wait Y seconds before the next execution
                        self.controller.stop_idle_auto_log()
                    else:
                        self.controller.set_automode_status(AutomodeStatus.AUTOMODE_UNSTARTED)
                        # self.controller.set_automode_enabled(False)
                        self.controller.stop_log(1)
                        self.controller.update_component_status("acquisition_info")
            else:
                while True:
                    if stop_flag:
                        break
                    self.controller.set_automode_status(AutomodeStatus.AUTOMODE_LOGGING)
                    self.controller.start_auto_log(1, self.acq_folder, True)
                    time.sleep(x)  # Wait X seconds
                    if stop_flag:
                        break
                    self.controller.set_automode_status(AutomodeStatus.AUTOMODE_IDLE)
                    self.controller.stop_auto_log(1)
                    self.log_start_button.setText("Stop Log")
                    self.log_start_button.setStyleSheet(STDTDL_PushButton.red)
                    self.controller.update_component_status("acquisition_info")
                    self.controller.start_idle_auto_log()
                    time.sleep(y)  # Wait Y seconds before the next execution
                    self.controller.stop_idle_auto_log()
                    
        stop_flag = False
        
        def stop_timer():
            nonlocal stop_flag
            stop_flag = True

        timer_thread = threading.Thread(target=timer_thread)
        timer_thread.start()
        return timer_thread, stop_timer

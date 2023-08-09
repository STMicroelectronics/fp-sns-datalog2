import os
from functools import partial

from PySide6.QtCore import Qt, QTime, QTimer, Slot
from PySide6.QtWidgets import QFrame, QSpinBox, QLabel
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection
from st_dtdl_gui.Widgets.PropertyWidget import PropertyWidget
from st_dtdl_gui.Widgets.ToggleButton import ToggleButton

import st_hsdatalog
from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget

class HSDAutoModeWidget(ComponentWidget):
    
    def __init__(self, controller, comp_contents, comp_name = "automode", comp_display_name = "Automatic Mode", comp_sem_type = "other", c_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)

        self.controller.sig_is_auto_started.connect(self.is_auto_started)

        self.app = self.controller.qt_app
        self.is_logging = False
        self.parent_widget = parent

        # clear all widgets in contents_widget layout (contents)
        for i in reversed(range(self.contents_widget.layout().count())):
            self.contents_widget.layout().itemAt(i).widget().deleteLater()
        
        self.setWindowTitle(comp_display_name)

        self.elapsed_time = 0
        self.digital_clock_timer = QTimer()
        self.digital_clock_timer.timeout.connect(self.update_time)
        
        QPyDesignerCustomWidgetCollection.registerCustomWidget(HSDAutoModeWidget, module="AutoModeWidget")
        loader = QUiLoader()
        auto_mode_widget = loader.load(os.path.join(os.path.dirname(st_hsdatalog.__file__),"HSD_GUI","UI","auto_mode_widget.ui"))
        self.frame_contents = auto_mode_widget.frame_auto_mode.findChild(QFrame,"frame_contents")

        frame_enabled = self.frame_contents.findChild(QFrame,"frame_enabled")
        toggle_button = ToggleButton()
        frame_enabled.layout().addWidget(toggle_button)
        toggle_button.toggled.connect(self.toggle_button_toggled)

        nof_acq_value = self.frame_contents.findChild(QSpinBox,"nof_acq_value")
        idle_period_value = self.frame_contents.findChild(QSpinBox,"idle_period_value")
        log_period_value = self.frame_contents.findChild(QSpinBox,"log_period_value")
        start_delay_value = self.frame_contents.findChild(QSpinBox,"start_delay_value")

        for pw in self.property_widgets:
            if pw == "enabled":
                self.property_widgets[pw].value = toggle_button
                self.assign_callbacks(self.property_widgets[pw], "boolean")
            elif pw == "nof_acquisitions":
                self.property_widgets[pw].value = nof_acq_value
                self.assign_callbacks(self.property_widgets[pw], "integer")
            elif pw == "start_delay_s" or pw == "start_delay_ms":
                self.property_widgets[pw].value = start_delay_value
                self.assign_callbacks(self.property_widgets[pw], "integer")
            elif pw == "logging_period_s" or pw == "datalog_time_length":
                self.property_widgets[pw].value = log_period_value
                self.assign_callbacks(self.property_widgets[pw], "integer")
            elif pw == "idle_period_s" or pw == "idle_time_length":
                self.property_widgets[pw].value = idle_period_value
                self.assign_callbacks(self.property_widgets[pw], "integer")

        self.layout().setContentsMargins(0,0,0,0)
        self.contents_widget.layout().setContentsMargins(9,0,9,0)
        self.contents_widget.layout().addWidget(auto_mode_widget.frame_auto_mode)
        self.contents_widget.setVisible(True)
    
    @Slot(bool)
    def s_is_logging(self, status:bool, interface:int):
        self.frame_contents.setEnabled(not status)
        if status and self.property_widgets["enabled"].value.checkState() == Qt.CheckState.Checked:
            self.title_frame.setStyleSheet("background-color: rgb(51, 71, 51);")
        else:
            self.title_frame.setStyleSheet("background-color: rgb(39, 44, 54);")
            self.digital_clock_timer.stop()
            self.title_label.setText("Automatic Mode")

    def toggle_button_toggled(self, status):
        # print(status)
        self.controller.set_automode_status(status)
    
    def format_time(self, seconds):
        minutes, seconds = divmod(seconds, 60)
        hours, minutes = divmod(minutes, 60)
        return f"{hours:02d}:{minutes:02d}:{seconds:02d}"
    
    def update_time(self):
        if self.is_logging:
            self.elapsed_time += 1
            time_str = self.format_time(self.elapsed_time)
            self.title_label.setText("Automatic Mode ---> LOGGING: {}".format(time_str))
        else:
            self.title_label.setText("Automatic Mode ---> IDLE")
    
    def is_auto_started(self, status):
        self.elapsed_time = 0
        self.is_logging = status
        self.digital_clock_timer.start(1000)

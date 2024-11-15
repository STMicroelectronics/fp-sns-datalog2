
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

from PySide6.QtCore import Qt
from PySide6.QtGui import QPixmap

from functools import partial
from st_dtdl_gui.UI.styles import STDTDL_LineEdit
from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
from st_dtdl_gui.Widgets.PropertyWidget import PropertyWidget
import st_dtdl_gui.UI.icons #NOTE don't delete this! it is used from resource_filename (@row 35..38)
from pkg_resources import resource_filename

info_img_path_valid = resource_filename('st_dtdl_gui.UI.icons', 'outline_info_white_18dp.png')
info_img_path_invalid = resource_filename('st_dtdl_gui.UI.icons', 'info_18dp_FF0000.svg')
class HSDComponentWidget(ComponentWidget):
    def __init__(self, controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id = 0, parent=None):
        if comp_name == "DeviceInformation":
            [c for c in comp_contents if c.name == "totalStorage"][0].display_name = "Total Storage [GB]"
            [c for c in comp_contents if c.name == "totalMemory"][0].display_name = "Total Memory [KB]"
        
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)

class HSDALSComponentWidget(HSDComponentWidget):
    def __init__(self, controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)
        
        self.exposure_time_widget:PropertyWidget = None
        self.intermeasurement_time_widget:PropertyWidget = None
        self.default_tooltip_text = ""

        self.initialize_widgets()

    def initialize_widgets(self):
        for w_key, widget in self.property_widgets.items():
            if w_key == "exposure_time":
                self.exposure_time_widget = widget
                self.exposure_time_widget.value.textChanged.connect(self.times_value_changed)
            elif w_key == "intermeasurement_time":
                self.default_tooltip_text = widget.icon.toolTip()
                self.intermeasurement_time_widget = widget
                self.intermeasurement_time_widget.value.textChanged.connect(self.times_value_changed)

    def times_value_changed(self, value):
        exposure_time_str = self.exposure_time_widget.value.text()
        intermeasurement_time_str = self.intermeasurement_time_widget.value.text()
        if exposure_time_str == "" or intermeasurement_time_str == "":
            return
        exposure_time = float(exposure_time_str)
        intermeasurement_time = float(intermeasurement_time_str)
        if intermeasurement_time <= (exposure_time / 1000) + 6:
            self.intermeasurement_time_widget.value.setStyleSheet(STDTDL_LineEdit.invalid)
            self.intermeasurement_time_widget.icon.setToolTip("<span style='color:red;'>If IM &le; Texp + 6ms, then it is ignored.</span>")
            pixmap = QPixmap(info_img_path_invalid)
        else:
            self.intermeasurement_time_widget.value.setStyleSheet(STDTDL_LineEdit.valid)
            self.intermeasurement_time_widget.icon.setToolTip(self.default_tooltip_text)
            pixmap = QPixmap(info_img_path_valid)
        self.intermeasurement_time_widget.icon.setPixmap(pixmap)
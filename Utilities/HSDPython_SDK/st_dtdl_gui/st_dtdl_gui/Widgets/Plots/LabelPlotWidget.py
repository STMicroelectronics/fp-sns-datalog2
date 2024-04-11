 
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
from PySide6.QtWidgets import QLabel
import st_dtdl_gui
from PySide6.QtCore import Slot
from st_dtdl_gui.Utils.DataClass import PlotLevelParams

from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

class LabelPlotWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, plot_params:PlotLevelParams, p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent)

        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.layout().itemAt(i).widget().setParent(None)

        QPyDesignerCustomWidgetCollection.registerCustomWidget(PlotWidget, module="PlotWidget")
        loader = QUiLoader()
        self.plot_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","plot_label_widget.ui"), parent)
        
        self.plot_params = plot_params
        self.min_val = plot_params.min_val
        self.max_val = plot_params.max_val
        self.value = None

        self.label_widget = self.plot_widget.label
        self.label_widget.setText(comp_display_name)
        self.value_widget = self.plot_widget.frame_value.findChild(QLabel,"value")
        self.value_widget.setText("")
        self.value_widget.setFixedWidth(100)
        self.unit_widget = self.plot_widget.frame_value.findChild(QLabel,"unit")
        self.unit_widget.setText("[{}]".format(self.plot_params.unit))

        self.timer_interval_ms = self.timer_interval*700

        self.layout().addWidget(self.plot_widget)

    @Slot(bool)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1 or interface == 3:
            if_str = "USB" if interface == 1 else "Serial"
            print(f"Sensor {self.comp_name} is logging via {if_str}: {status}")
            if status:
                #self.update_plot_characteristics(self.plot_params)
                # component_status = self.controller.components_status.get(self.comp_name)
                # sensor_category = component_status.get("sensor_category")
                # if sensor_category is not None:
                    # if sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_MEMS.value:
                    #     self.update_plot_characteristics(SensorMemsPlotParams(self.comp_name, True, self.odr, self.n_curves, "", self.time_window))
                    # elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_AUDIO.value:
                    #     self.update_plot_characteristics(SensorAudioPlotParams(self.comp_name, True, self.odr, self.n_curves, "", self.time_window))
                    # elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
                    #     self.update_plot_characteristics(SensorMemsPlotParams(self.comp_name, True, self.n_curves, "", self.time_window))
                    # elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                    #     pass
                    # elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_CAMERA.value:
                    #     pass
                    # else:
                    #     self.update_plot_characteristics(SensorMemsPlotParams(self.comp_name, True, self.odr, self.n_curves, "", self.time_window))
                # self.update_plot_characteristics(LinesPlotParams(self.comp_name, True, self.odr, self.n_curves, "", self.time_window))
                self.timer.start(self.timer_interval_ms)
            else:
                self.value = None
                self.timer.stop()
                
        else: # interface == 0
            print("Component {} is logging on SD Card: {}".format(self.comp_name,status))
    
    def update_plot(self):
        if self.value is not None:
            self.value_widget.setText(str(self.value[0]))
        self.update()
    
    def add_data(self, data):
        self.value = data[0]

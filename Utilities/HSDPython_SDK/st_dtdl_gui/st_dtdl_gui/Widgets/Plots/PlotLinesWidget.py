
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

import numpy as np
from collections import deque

from PySide6.QtCore import Slot

import pyqtgraph as pg
from st_dtdl_gui.Utils.DataClass import LinesPlotParams, SensorCameraPlotParams, SensorLightPlotParams, SensorMemsPlotParams, SensorAudioPlotParams, SensorPlotParams, SensorRangingPlotParams

from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from st_pnpl.DTDL import dtdl_utils

class PlotLinesWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id = 0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, plot_params.unit)
        
        self.controller.sig_plot_window_time_updated.connect(self.s_time_window_updated)
        
        # self.unit = plot_params.unit
        self.plot_params = plot_params
        self.plot_t_interval_size = int(self.plot_len/(plot_params.time_window / self.timer_interval))     
        
        self.lines_colors = ['#e6007e', '#a4c238', '#3cb4e6', '#ef4f4f', '#46b28e', '#e8ce0e', '#60b562', '#f99e20', '#41b3ba']
        self.graph_curves = dict()
        
        self.one_t_interval_resampled = dict()
        self._data = dict() # dict of queues
        self.y_queue = dict() # dict of queues
        
        self.update_plot_characteristics(plot_params)
        # if isinstance(plot_params, SensorMemsPlotParams):
        #     self.update_plot_characteristics(SensorMemsPlotParams(self.comp_name, True, plot_params.odr, plot_params.dimension, "", time_window))
        # elif isinstance(plot_params, SensorAudioPlotParams):
        #     self.update_plot_characteristics(SensorAudioPlotParams(self.comp_name, True, plot_params.odr, plot_params.dimension, "", time_window))
        # elif isinstance(plot_params, SensorRangingPlotParams):
        #     self.update_plot_characteristics(SensorRangingPlotParams(self.comp_name, True, plot_params.dimension, "", time_window))
        # elif isinstance(plot_params, SensorLightPlotParams):
        #     pass
        # elif isinstance(plot_params, SensorCameraPlotParams):
        #     pass
        # else:
        #     self.update_plot_characteristics(SensorMemsPlotParams(self.comp_name, True, plot_params.odr, plot_params.dimension, "", time_window))

    def update_plot_characteristics(self, plot_params:LinesPlotParams):
        self.plot_params = plot_params
        
        # if isinstance(plot_params, SensorMemsPlotParams) or isinstance(plot_params, SensorAudioPlotParams):
        #     self.odr = plot_params.odr
        # self.n_curves = plot_params.dimension
        # self.time_window = plot_params.time_window

        for i in range(self.plot_params.dimension):
           self.one_t_interval_resampled[i] = np.zeros(self.plot_t_interval_size)
        
        self.x_data = np.linspace(-(plot_params.time_window), 0, self.plot_len)
        for i in range(self.plot_params.dimension):
            self._data[i] = deque(maxlen=200000)
            self.y_queue[i] = deque(maxlen=self.plot_len)
            self.y_queue[i].extend(np.zeros(self.plot_len))
            if len(self.graph_curves) < self.plot_params.dimension:
                self.graph_curves[i] = self.graph_widget.plot()
                self.graph_curves[i] = pg.PlotDataItem(pen=({'color': self.lines_colors[i - (len(self.lines_colors)* int(i / len(self.lines_colors)))], 'width': 1}), skipFiniteCheck=True, ignoreBounds=True)
                self.graph_widget.addItem(self.graph_curves[i])
            
        self.app_qt.processEvents()
        
        self.plot_t_interval_size = int(self.plot_len/(plot_params.time_window / self.timer_interval))
            
    @Slot(float)
    def s_time_window_updated(self, new_time_w):
        # component_status = self.controller.components_status.get(self.comp_name)
        # sensor_category = component_status.get("sensor_category")
        self.plot_params.time_window = new_time_w
        self.update_plot_characteristics(self.plot_params)
        
        # if sensor_category is not None:
        #     if sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_MEMS.value:
        #         odr = component_status.get("odr")
        #         self.update_plot_characteristics(SensorMemsPlotParams(self.comp_name, True, odr, self.n_curves, "", new_time_w))
        #     elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_AUDIO.value:
        #         odr = component_status.get("odr")
        #         self.update_plot_characteristics(SensorAudioPlotParams(self.comp_name, True, odr, self.n_curves, "", new_time_w))
        #     elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
        #         self.update_plot_characteristics(SensorRangingPlotParams(self.comp_name, True, self.n_curves, "", new_time_w))
        #     elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
        #         pass
        #     elif sensor_category == dtdl_utils.SensorCategoryEnum.ISENSOR_CLASS_CAMERA.value:
        #         pass
        # else:
        #     odr = component_status.get("odr", 1)
        #     self.update_plot_characteristics(SensorMemsPlotParams(self.comp_name, True, odr, self.n_curves, "", new_time_w))

    @Slot(bool)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1:
            print("Component {} is logging via USB: {}".format(self.comp_name,status))
            if status:
                self.update_plot_characteristics(self.plot_params)
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
                self.timer.stop()
                
        else: # interface == 0
            print("Component {} is logging on SD Card: {}".format(self.comp_name,status))
            
    @Slot(bool)
    def s_is_detecting(self, status: bool):
        self.s_is_logging(status, 1)

    def reset(self):
        pass
    
    def resample_linear1D(self, original, targetLen):
        original = np.array(original, dtype=float)
        index_arr = np.linspace(0, len(original)-1, num=targetLen, dtype=float)
        index_floor = np.array(index_arr, dtype=int) #Round down
        index_ceil = index_floor + 1
        index_rem = index_arr - index_floor #Remain
        
        val1 = original[index_floor]
        val2 = original[index_ceil % len(original)]
        interp = val1 * (1.0-index_rem) + val2 * index_rem
        assert(len(interp) == targetLen)
        return interp
    
    def update_plot(self):
        self.x_data = self.x_data + self.timer_interval
        # for i in range(self.n_curves):
        for i in range(self.plot_params.dimension):
            if len(self._data[i]) > 0: # If data queue is not empty
                # Extract all data from the queue (pop)
                one_reduced_t_interval = [self._data[i].popleft() for _i in range(len(self._data[i]))]
                # Resample extracted raw data to have the same plot_timer_interval size (plot len / (time window / times interval(sec)))
                self.one_t_interval_resampled[i] = self.resample_linear1D(one_reduced_t_interval, self.plot_t_interval_size)
                # Put resampled data into the y data queue
                self.y_queue[i].extend(self.one_t_interval_resampled[i])
            else: #data queue is empty
                self.y_queue[i].extend(self.one_t_interval_resampled[i])
            # set extracted resampled data into the plot curve (for each axis) [x and y will have the same len = (plot len / (time window / times interval(sec)))
            self.graph_curves[i].setData(x=self.x_data,y=np.array(self.y_queue[i]))
        self.app_qt.processEvents()

    def add_data(self, data):
        for i in range(self.plot_params.dimension):
            self._data[i].extend(data[i])
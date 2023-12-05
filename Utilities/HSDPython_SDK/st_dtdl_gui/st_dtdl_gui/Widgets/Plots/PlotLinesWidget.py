
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
from st_dtdl_gui.Utils.DataClass import LinesPlotParams

from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget

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
        self.current_x = 0

        self.update_plot_characteristics(plot_params)

    def update_plot_characteristics(self, plot_params:LinesPlotParams):
        self.plot_params = plot_params

        for i in range(self.plot_params.dimension):
           self.one_t_interval_resampled[i] = np.zeros(self.plot_t_interval_size)
        
        self.x_data = np.linspace(-(plot_params.time_window) + self.current_x, self.current_x, self.plot_len)
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
        self.plot_params.time_window = new_time_w
        self.update_plot_characteristics(self.plot_params)

    @Slot(bool)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1 or interface == 3:
            print("Component {} is logging via USB: {}".format(self.comp_name,status))
            if status:
                self.current_x = 0
                self.update_plot_characteristics(self.plot_params)
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
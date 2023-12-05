 
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

from PySide6.QtCore import Slot, QSize

import pyqtgraph as pg

from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget

class PlotBarWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, n_bars, y0, y1, bar_width = 1, unit= "", p_id = 0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, unit)
        
        self.y0 = y0
        self.y1 = y1
        
        self.n_bars = n_bars
        self.bar_width = bar_width
        
        self._data = dict() # dict of queues
        self._data[0] = deque(maxlen=200000)
        
        # create list for y-axis
        y1 = np.zeros(n_bars)
 
        # create horizontal list i.e x-axis
        if n_bars == 1:
            self.x = np.arange(0 , 1, dtype=int)
        else:
            self.x = np.arange(0 ,n_bars,  dtype=int)
        
        self.bargraph = pg.BarGraphItem(x = self.x, height = y1, width = self.bar_width, brush ='#a4c238', pen='#1B1D23')
        
        self.graph_widget.setYRange(self.y0, self.y1, padding=0)
        
        # add item to plot window
        # adding bargraph item to the plot window
        self.graph_widget.addItem( self.bargraph)
        
        self.graph_widget.getPlotItem().layout.setContentsMargins(10, 3, 3, 3)
        self.graph_widget.getPlotItem().setMenuEnabled(False) #Disable right click menu in plots
        self.graph_widget.setMinimumSize(QSize(300, 150))
        
        styles = {'color':'#d2d2d2', 'font-size':'12px'}
        self.graph_widget.setLabel('left', self.left_label, **styles)
        
        self.timer_interval_ms = self.timer_interval*700

    def update_plot_characteristics(self, plot_params):
        pass

    @Slot(bool, int)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1 or interface == 3:
            print("Component {} is logging via USB: {}".format(self.comp_name,status))
            if status:
                self.buffering_timer_counter = 0
                self.timer.start(self.timer_interval_ms)
            else:
                self.timer.stop()
        else: # interface == 0
            print("Component {} is logging on SD Card: {}".format(self.comp_name,status))
            
    @Slot(bool)
    def s_is_detecting(self, status:bool):
        self.s_is_logging(status, 1)
    
    def update_plot(self):
        if self.buffering_timer_counter == 0:
            if len(self._data[0]) > 0: 
                # Extract all data from the queue (pop)    
                one_reduced_t_interval = [self._data[0].popleft() for _i in range(len(self._data[0]))]
                y_value = np.array(one_reduced_t_interval)
                y_array_mean = np.mean(y_value, axis = 0)
                self.bargraph.setOpts(x = self.x, height = y_array_mean)
            self.app_qt.processEvents()
        else:
            # Increment the buffering counter (skip a plot timer interval to bufferize data from sensors)
            self.buffering_timer_counter += 1

    def add_data(self, data):
        self._data[0].append(data[0])

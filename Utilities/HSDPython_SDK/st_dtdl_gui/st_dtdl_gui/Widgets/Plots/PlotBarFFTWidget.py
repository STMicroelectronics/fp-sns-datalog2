 
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

import math
import numpy as np

from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QPushButton
import pyqtgraph as pg
from pyqtgraph import mkPen
from st_dtdl_gui.UI.styles import STDTDL_PushButton
from st_dtdl_gui.Widgets.Plots.PlotBarWidget import PlotBarWidget
    
class CustomLinearRegionItem(pg.LinearRegionItem):
    def __init__(self, values=(0, 1), orientation='vertical', brush=None, pen=None,
                 hoverBrush=None, hoverPen=None, movable=True, bounds=None, 
                 span=(0, 1), swapMode='sort', clipItem=None):
        super().__init__(values, orientation, brush, pen, hoverBrush, hoverPen, movable, bounds, span, swapMode, clipItem)
    
    def mouseDragEvent(self, ev): #Unmovable Rects
        pass

class PlotBarFFTWidget(PlotBarWidget):  
    def __init__(self, controller, comp_name, fft_len, fft_input_freq_hz, comp_display_name ="", unit= "dB", fft_bottom_val = -150, fft_top_val = -60, p_id = 0, parent=None):
        # Set FFT Specific parameters
        self.fft_len = fft_len
        self.fft_input_freq_hz = fft_input_freq_hz
        self.fft_width = (self.fft_input_freq_hz/2)/self.fft_len
        self.fft_bottom_val = fft_bottom_val
        self.fft_top_val = fft_top_val

        super().__init__(controller, comp_name, comp_display_name, n_bars=fft_len, y0=fft_bottom_val, y1=fft_top_val, bar_width=self.fft_width, unit=unit, p_id=p_id , parent=parent)
        
        self.pushButton_plot_settings.setVisible(True)
        self.pushButton_plot_settings.clicked.connect(self.clicked_plot_settings_button)
        self.is_settings_displayed = False
        
        self.pushButton_fft_close_settings = self.fft_settings_frame.findChild(QPushButton, "pushButton_fft_close_settings")
        self.pushButton_fft_close_settings.clicked.connect(self.clicked_fft_close_settings)
        
        self.fft_settings_frame.setVisible(False)
        self.fft_peak_pushButton = self.fft_settings_frame.findChild(QPushButton, "pushButton_fft_peak")
        self.fft_peak_status = True
        self.fft_peak_label_shown = True 
        self.fft_peak_pushButton.setStyleSheet(STDTDL_PushButton.green)
        self.fft_peak_pushButton.clicked.connect(self.clicked_fft_peak_button)
        
        self.fft_filters_pushButton = self.fft_settings_frame.findChild(QPushButton, "pushButton_fft_filter")
        self.fft_filters_status = False
        self.fft_filters_pushButton.setStyleSheet(STDTDL_PushButton.red)
        self.fft_filters_pushButton.clicked.connect(self.clicked_fft_filter_button)
        
        self.x_values = np.arange(0, self.fft_width*(self.fft_len) ,self.fft_width,  dtype=int)
        self.y_values = None
        self.min_filtered_x = 0
        self.max_filtered_x = len(self.x_values)
        
        self.bargraph.setY(fft_bottom_val)
        self.graph_widget.setYRange(fft_bottom_val, fft_top_val, padding=0)
        self.graph_widget.setXRange(self.x_values[0], self.x_values[-1], padding=0)
        
        peak_line_pen = mkPen(color="#BBBBBB", width=2, style=Qt.DashLine)
        self.peak_h_line = pg.InfiniteLine(angle=0, movable=False, pen=peak_line_pen)
        self.peak_h_line.label = pg.InfLineLabel(self.peak_h_line, text="")
        self.peak_h_line.label.setMovable(True)
        self.graph_widget.addItem(self.peak_h_line, ignoreBounds=True)
        self.peak_v_line = pg.InfiniteLine(angle=90, movable=False, pen=peak_line_pen)
        self.graph_widget.addItem(self.peak_v_line, ignoreBounds=True)
        self.__show_hide_peak_reveal(self.fft_peak_status)
        
        filter_lines_pen = mkPen(color="#105955", width=3)
        filter_lines_hover_pen = mkPen(color="#20b2aa", width=9)
        self.low_pass_filter_region = CustomLinearRegionItem(values=(0, self.x_values[-1]*0.05), 
                                                             orientation=pg.LinearRegionItem.Vertical,
                                                             pen=filter_lines_pen,
                                                             hoverPen=filter_lines_hover_pen,
                                                             brush=(16, 17, 21, 200),
                                                             hoverBrush=(27, 29, 35, 125),
                                                             bounds=[0, int(self.x_values[-1]/2)])
        self.low_pass_filter_region.setZValue(1)
        self.graph_widget.addItem(self.low_pass_filter_region)
        
        self.high_pass_filter_region = CustomLinearRegionItem(values=(self.x_values[-1] - (self.x_values[-1]*0.05),
                                                              self.x_values[-1]),
                                                              orientation=pg.LinearRegionItem.Vertical,
                                                              pen=filter_lines_pen,
                                                              hoverPen=filter_lines_hover_pen,
                                                              brush=(16, 17, 21, 200),
                                                              hoverBrush=(27, 29, 35, 125),
                                                              bounds=[int(self.x_values[-1]/2), self.x_values[-1]])
        self.high_pass_filter_region.setZValue(1)
        self.graph_widget.addItem(self.high_pass_filter_region)
    
        self.magnitude_filter_region = CustomLinearRegionItem(values=(self.fft_bottom_val, self.fft_bottom_val*0.95),
                                                              orientation=pg.LinearRegionItem.Horizontal,
                                                              pen=filter_lines_pen,
                                                              hoverPen=filter_lines_hover_pen,
                                                              brush=(16, 17, 21, 200),
                                                              hoverBrush=(27, 29, 35, 125),
                                                              bounds=[self.fft_bottom_val, self.fft_top_val])
        self.magnitude_filter_region.setZValue(1)
        self.graph_widget.addItem(self.magnitude_filter_region)
        
        self.__show_hide_filter_regions(self.fft_filters_status)
        
        self.graph_widget.setMouseEnabled(x=False, y=False)
        self.graph_widget.hideButtons()
    
    def update_peak_line(self, y_values):
        
        if self.fft_filters_status:
            left_limit = self.low_pass_filter_region.lines[1].getXPos()
            right_limit = self.high_pass_filter_region.lines[0].getXPos()
        
            mag_filter_br = self.magnitude_filter_region.viewRect()
            mag_filter_rng = self.magnitude_filter_region.getRegion()
            mag_filter_br.setTop(mag_filter_rng[0])
            mag_filter_br.setBottom(mag_filter_rng[1])

            mag_filter_br.setRight(right_limit)
            mag_filter_br.setLeft(left_limit)
        
            self.min_filtered_x = math.floor(left_limit/self.fft_width)
            self.max_filtered_x = math.ceil(right_limit/self.fft_width)
        
            x_filtered_y_values = y_values[self.min_filtered_x:self.max_filtered_x]
            self.y_filter_height = self.magnitude_filter_region.lines[1].getYPos() - self.fft_bottom_val
            y_filtered_y_values = x_filtered_y_values[x_filtered_y_values > self.y_filter_height]
        else:
            y_filtered_y_values = y_values
        
        if self.fft_peak_status:
            if len(y_filtered_y_values) > 0:
                max_value = max(y_filtered_y_values)
                max_idx, = np.where(np.isclose(y_values, max_value)) # floating-point
                self.peak_v_line.setPos(self.x_values[max_idx][0])
                self.peak_h_line.setPos(max_value + self.fft_bottom_val)
                self.peak_h_line.label.setHtml("<p><strong><span style=\"font-size:18.0pt; color: #46b28e;\">{} Hz</span></strong></p>".format(self.x_values[max_idx][0]))
            else:
                self.peak_h_line.label.setHtml("<span style=color: transparent;\"></span>")
        else:
            self.peak_h_line.label.setHtml("<span style=color: transparent;\"></span>")
    
    def update_plot(self):
        if self.buffering_timer_counter == 0:
            if len(self._data[0]) > 0: 
                # Extract all data from the queue (pop)    
                one_reduced_t_interval = [self._data[0].popleft() for _i in range(len(self._data[0]))]
                y_raw_values = np.array(one_reduced_t_interval)
                y_array_mean = np.mean(y_raw_values, axis = 0)
                # Power spectral density
                y_raw_values = np.square(y_array_mean)
                y_raw_values =  y_raw_values / (self.fft_len * self.fft_input_freq_hz)
                if y_raw_values.any():
                    y_raw_values =  10 * np.log10(y_raw_values)
                    self.y_values = y_raw_values - self.fft_bottom_val
                    self.bargraph.setOpts(x = self.x_values, height = self.y_values, brush ='#a4c238', pen='#1B1D23')
                else:
                    self.bargraph.setOpts(brush=(0,0,0,0), pen=(0,0,0,0))
                
                if self.y_values is not None:
                    if not self.fft_peak_label_shown:
                        self.__show_hide_peak_reveal(True)
                        self.fft_peak_label_shown = True
                    self.update_peak_line(self.y_values)
                if not any(y_raw_values):
                    if self.fft_peak_label_shown:
                        self.__show_hide_peak_reveal(False)
                        self.fft_peak_label_shown = False
            self.app_qt.processEvents()
        else:
            # Increment the buffering counter (skip a plot timer interval to bufferize data from sensors)
            self.buffering_timer_counter += 1
    
    def __show_hide_peak_reveal(self, status):
        self.peak_h_line.setVisible(status)
        self.peak_v_line.setVisible(status)
        # if status:
        #     self.peak_h_line.setVisible(True)
        #     self.peak_v_line.setVisible(True)
        # else:
        #     self.peak_h_line.setVisible(False)
        #     self.peak_v_line.setVisible(False)
    
    def __show_hide_filter_regions(self, status):
        self.low_pass_filter_region.setVisible(status)
        self.high_pass_filter_region.setVisible(status)
        self.magnitude_filter_region.setVisible(status)
        # if status:
        #     self.low_pass_filter_region.setVisible(True)
        #     self.high_pass_filter_region.setVisible(True)
        #     self.magnitude_filter_region.setVisible(True)
        # else:
        #     self.low_pass_filter_region.setVisible(False)
        #     self.high_pass_filter_region.setVisible(False)
        #     self.magnitude_filter_region.setVisible(False)
            
    @Slot()
    def clicked_plot_settings_button(self):
        self.is_settings_displayed = not self.is_settings_displayed
        if self.is_settings_displayed:
            self.fft_settings_frame.setVisible(True)
        else:
            self.fft_settings_frame.setVisible(False)
            
    @Slot()
    def clicked_fft_close_settings(self):
        self.is_settings_displayed = False
        self.fft_settings_frame.setVisible(False)
        
    @Slot()
    def clicked_fft_peak_button(self):
        self.fft_peak_status = not self.fft_peak_status
        self.__show_hide_peak_reveal(self.fft_peak_status)
        if self.fft_peak_status:
            self.fft_peak_pushButton.setStyleSheet(STDTDL_PushButton.green)
        else:
            self.fft_peak_pushButton.setStyleSheet(STDTDL_PushButton.red)
    
    @Slot()
    def clicked_fft_filter_button(self):
        self.fft_filters_status = not self.fft_filters_status
        self.__show_hide_filter_regions(self.fft_filters_status)
        if self.fft_filters_status:
            self.fft_filters_pushButton.setStyleSheet(STDTDL_PushButton.green)
        else:
            self.fft_filters_pushButton.setStyleSheet(STDTDL_PushButton.red)
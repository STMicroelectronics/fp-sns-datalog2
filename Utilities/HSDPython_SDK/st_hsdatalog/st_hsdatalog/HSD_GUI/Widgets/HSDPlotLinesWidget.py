
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


from collections import deque
from PySide6.QtCore import Slot
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QPushButton, QFileDialog

import numpy as np
import pyqtgraph as pg
import struct

import st_dtdl_gui.UI.icons #do not remove this import. It is used by pkg_resources
from st_dtdl_gui.Utils.DataClass import PlotParams, SensorISPUPlotParams
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotLabel
from st_dtdl_gui.Widgets.Plots.PlotLinesWavWidget import PlotLinesWavWidget

from pkg_resources import resource_filename

from st_hsdatalog.HSD.utils.type_conversion import TypeConversion

ispu_out_fmt_ok_status_path = resource_filename('st_dtdl_gui.UI.icons', 'outline_done_outline_white_18dp.png')
ispu_out_fmt_ko_status_path = resource_filename('st_dtdl_gui.UI.icons', 'outline_close_white_36dp.png')

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class HSDPlotLinesWidget(PlotLinesWavWidget):    
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id=0, parent=None):
        self.ispu_output_format = None

        super().__init__(controller, comp_name, comp_display_name, plot_params, p_id, parent)
        self.controller.sig_tag_done.connect(self.s_tag_done)
        self.controller.sig_ispu_ucf_loaded.connect(self.s_ispu_ucf_loaded)
                
        self.active_tags = dict()
        self.tag_lines = []
        
        self.out_fmt_valid = None
        self.plot_params = plot_params

        #Show Output format description file loading frame
        if "_ispu" in comp_name:
            self.plot_params = SensorISPUPlotParams(comp_name, plot_params.enabled, plot_params.dimension, None, plot_params.time_window)
            self.is_out_fmt_displayed = True
            self.pushButton_plot_settings.setVisible(True)
            self.load_output_fmt_frame.setVisible(True)
            self.out_fmt_valid = False
            load_output_fmt_pushButton = self.load_output_fmt_frame.findChild(QPushButton, "pushButton_load_out_fmt")
            load_output_fmt_pushButton.clicked.connect(self.clicked_load_out_fmt_button)
            self.out_fmt_status = self.load_output_fmt_frame.findChild(QPushButton, "out_fmt_status")
            icon  = QPixmap(ispu_out_fmt_ko_status_path)
            self.out_fmt_status.setIcon(icon)
            self.load_output_fmt_frame.layout().addWidget(PlotLabel("OUT Format"))
            self.pushButton_close_settings = self.load_output_fmt_frame.findChild(QPushButton, "pushButton_close_settings")
            self.pushButton_close_settings.clicked.connect(self.clicked_out_fmt_plot_settings_button)
            self.pushButton_plot_settings.clicked.connect(self.clicked_out_fmt_plot_settings_button)
        
        for gc_id in self.graph_curves:
            if "_mic" in self.plot_params.comp_name:
                self.legend.addItem(self.graph_curves[gc_id], "Waveform")
            elif "_temp" in self.plot_params.comp_name:
                self.legend.addItem(self.graph_curves[gc_id], "Temperature")
            elif "_pres" in self.plot_params.comp_name:
                self.legend.addItem(self.graph_curves[gc_id], "Pressure")
            elif "_mlc" in self.plot_params.comp_name:
                self.legend.addItem(self.graph_curves[gc_id], "reg_{}".format(gc_id))
            elif "_ispu" in self.plot_params.comp_name:
                pass #TODO
            else:
                self.legend.addItem(self.graph_curves[gc_id], "x" if gc_id == 0 else ("y" if gc_id == 1 else "z"))
        
    def __load_ispu_ucf(self, filepath):
        self.controller.load_ispu_ucf_file(filepath)
    
    def __load_ispu_out_fmt(self, filepath):
        self.out_fmt_valid = self.controller.load_ispu_output_fmt_file(filepath)
        if self.out_fmt_valid:
            icon  = QPixmap(ispu_out_fmt_ok_status_path)
            self.ispu_output_format = self.controller.ispu_output_format["output"]
            self.ispu_out_bytes_cnt = []
            self.ispu_out_fmt_char = []
            for of in self.ispu_output_format:
                self.ispu_out_bytes_cnt.append(self.controller.get_out_fmt_byte_count(of["type"]))
                self.ispu_out_fmt_char.append(self.controller.get_out_fmt_char(of["type"]))
            self.n_curves = len(self.ispu_output_format)
        else:
            icon  = QPixmap(ispu_out_fmt_ko_status_path)
        self.out_fmt_status.setIcon(icon)
        
    @Slot()
    def clicked_load_out_fmt_button(self):
        json_filter = "JSON Output format description Files (*.json *.JSON)"
        filepath = QFileDialog.getOpenFileName(filter=json_filter)
        self.__load_ispu_out_fmt(filepath[0])
        if self.ispu_output_format is not None:
            for id in range(len(self.legend.items)):
                if id != 0:
                    self.legend.removeItem(self.graph_curves[id])#
                    self.legend.layout.removeAt(id)
            self.legend.addItem(pg.PlotDataItem(pen=pg.mkPen(0,0,0,0)),"")
            for i, of in enumerate(self.ispu_output_format):
                self.legend.addItem(self.graph_curves[i], of.get("name",""))
    
    @Slot()
    def clicked_out_fmt_plot_settings_button(self):
        self.is_out_fmt_displayed = not self.is_out_fmt_displayed
        if self.is_out_fmt_displayed:
            self.load_output_fmt_frame.setVisible(True)
        else:
            self.load_output_fmt_frame.setVisible(False)
    
    @Slot()
    def s_tag_done(self, status, tag_label:str):
        if status:
            if not tag_label in self.active_tags or self.active_tags[tag_label] == False:
                self.active_tags[tag_label] = True
                pen=pg.mkPen(color='#00FF00', width=1)
                tag_line = pg.InfiniteLine(pos = self.x_data[-1], angle=90, movable=False, pen=pen, label=tag_label + " ON")
                self.graph_widget.addItem(tag_line, ignoreBounds=True)
                self.tag_lines.append(tag_line)
        else:
            if not tag_label in self.active_tags or self.active_tags[tag_label] == True:
                self.active_tags[tag_label] = False
                pen=pg.mkPen(color='#FF0000', width=1)
                tag_line = pg.InfiniteLine(pos = self.x_data[-1], angle=90, movable=False, pen=pen, label=tag_label + " OFF")
                self.graph_widget.addItem(tag_line, ignoreBounds=True)
                self.tag_lines.append(tag_line)
                
    @Slot(str,str)
    def s_ispu_ucf_loaded(self, ucf_path, output_json_fpath):
        if "_ispu" in self.comp_name:
            self.__load_ispu_ucf(ucf_path)
            self.__load_ispu_out_fmt(output_json_fpath)
            if self.ispu_output_format is not None:
                for id in range(len(self.legend.items)):
                    if id != 0:
                        self.legend.removeItem(self.graph_curves[id])#
                        self.legend.layout.removeAt(id)
                self.legend.addItem(pg.PlotDataItem(pen=pg.mkPen(0,0,0,0)),"")
                for i, of in enumerate(self.ispu_output_format):
                    self.legend.addItem(self.graph_curves[i], of.get("name",""))
            

    def __clean_tag_lines(self):
        for t in self.tag_lines:
            self.graph_widget.removeItem(t)
    
    @Slot(bool, int) #Override PlotLinesWavWidget s_is_logging
    def s_is_logging(self, status: bool, interface: int):
        if not "_mic" in self.comp_name:# or "_acc" in self.comp_name:
            if interface == 1:
                print("Sensor {} is logging via USB: {}".format(self.comp_name,status))
                if status:
                    self.__clean_tag_lines()
                    if "_ispu" in self.comp_name:
                        enabled = self.plot_params.enabled
                        time_window = self.plot_params.time_window
                        if self.out_fmt_valid:
                            for iof in self.ispu_output_format:
                                data_format = TypeConversion.get_format_char(iof["type"])
                                data_byte_len = TypeConversion.check_type_length(iof["type"])
                                iof["data_format"] = data_format
                                iof["data_byte_len"] = data_byte_len
                            self.plot_params = SensorISPUPlotParams(self.comp_name, enabled, len(self.ispu_output_format), self.ispu_output_format, time_window)
                            self.update_plot_characteristics(self.plot_params)
                            self.timer.start(self.timer_interval_ms)
                        else:
                            log.error("Missing ISPU JSON Output format descriptor.")
                            # self.plot_params = SensorISPUPlotParams(self.comp_name, enabled, self.plot_params.dimension, None, time_window)
                    else:
                        self.update_plot_characteristics(self.plot_params)
                        self.timer.start(self.timer_interval_ms)
                else:
                    self.timer.stop()
            else: # interface == 0
                print("Sensor {} is logging on SD Card: {}".format(self.comp_name,status))
        else:
            if interface == 1:
                print("Sensor {} is logging via USB: {}".format(self.comp_name,status))
                if status:
                    self.__clean_tag_lines()
            super().s_is_logging(status, interface)
    
    def update_plot_characteristics(self, plot_params:PlotParams):
        self.plot_params = plot_params

        if isinstance(plot_params, SensorISPUPlotParams):
            if plot_params.out_fmt is not None:
                dimensions = len(plot_params.out_fmt)
            else:
                dimensions = 0
        else:
            dimensions = self.plot_params.dimension
        
        for i in range(dimensions):
            self.one_t_interval_resampled[i] = np.zeros(self.plot_t_interval_size)
        
        self.x_data = np.linspace(-(plot_params.time_window), 0, self.plot_len)
        for i in range(dimensions):
            self._data[i] = deque(maxlen=200000)
            self.y_queue[i] = deque(maxlen=self.plot_len)
            self.y_queue[i].extend(np.zeros(self.plot_len))
            if len(self.graph_curves) < dimensions:
                self.graph_curves[i] = self.graph_widget.plot()
                self.graph_curves[i] = pg.PlotDataItem(pen=({'color': self.lines_colors[i - (len(self.lines_colors)* int(i / len(self.lines_colors)))], 'width': 1}), skipFiniteCheck=True, ignoreBounds=True)
                self.graph_widget.addItem(self.graph_curves[i])
            
        self.app_qt.processEvents()
        
        self.plot_t_interval_size = int(self.plot_len/(plot_params.time_window / self.timer_interval))
            
    @Slot(bool)
    def s_is_detecting(self, status:bool):
        self.s_is_logging(status, 1)

    def add_data(self, data):
        if "_ispu" in self.comp_name:
            if self.plot_params.out_fmt is not None:
                data_idx = 0
                for i, of in enumerate(self.plot_params.out_fmt):
                    ax_len = of["data_byte_len"]
                    ax_value_bytes = np.array(data[0][data_idx:data_idx+ax_len],dtype='int8').tobytes()#np.concatenate(list(data.values())[data_idx:data_idx+ax_len]).tolist()
                    ax_value = struct.unpack("=" + of["data_format"], ax_value_bytes)
                    self._data[i].extend(ax_value)
                    data_idx += ax_len
        else:
            super().add_data(data)

    
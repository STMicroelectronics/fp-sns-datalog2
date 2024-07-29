
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

import time
from PySide6.QtCore import Slot, Qt

import pyqtgraph as pg
from PySide6.QtGui import QColor, QBrush

from st_dtdl_gui.STDTDL_Controller import ComponentType
from st_dtdl_gui.Utils.DataClass import PlotPAmbientParams, PlotPMotionParams, PlotPObjectParams, PlotPPresenceParams, PlotParams
from st_hsdatalog.HSD_GUI.Widgets.HSDPlotLinesWidget import HSDPlotLinesWidget

class HSDPlotLinesTMOSWidget(HSDPlotLinesWidget):
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, plot_params, p_id, parent)
        
        self.legend.clear()

        self.controller.sig_tag_done.connect(self.s_tag_done)
        
        self.controller.sig_tmos_presence_detected.connect(self.s_tmos_data_tag_done)
        self.controller.sig_tmos_motion_detected.connect(self.s_tmos_data_tag_done)

        
        self.data_flag = False
        self.data_flag_regions = {}
        self.lines_params = []
        self.curr_status = None
        self.nof_lines = 4

        self.legend = self.graph_widget.addLegend()
        brush = QBrush(QColor(255, 255, 255, 15))
        self.legend.setBrush(brush)
        
        self.redraw_plot(plot_params)

    def redraw_plot(self, plot_params):
        self.plot_params = plot_params
        self.lines_params = []
        if isinstance(self.plot_params, PlotPAmbientParams):
            self.lines_params.append("Tambient (raw)")
        elif isinstance(self.plot_params, PlotPObjectParams):
            self.graph_curves[0].setVisible(True)
            self.lines_params.append("Tobject (raw)")
            if plot_params.embedded_compensation:
                self.graph_curves[1].setVisible(True)
                self.lines_params.append("Tobject (emb_comp)")
            else:
                self.graph_curves[1].setVisible(False)
            if plot_params.software_compensation:
                self.graph_curves[2].setVisible(True)
                self.graph_curves[3].setVisible(True)
                self.lines_params.append("Tobject (sw_comp)")
                self.lines_params.append("Tobject_change (sw_comp)")
            else:
                self.graph_curves[2].setVisible(False)
                self.graph_curves[3].setVisible(False)
        elif isinstance(self.plot_params, PlotPPresenceParams):
            self.lines_params.append("Tpresence")
        elif isinstance(self.plot_params, PlotPMotionParams):
            self.lines_params.append("Tmotion")
            
        self.legend.clear()
        
        if isinstance(self.plot_params, PlotPObjectParams):
            self.legend.addItem(self.graph_curves[0], self.lines_params[0])
            if plot_params.embedded_compensation: #T
                self.legend.addItem(self.graph_curves[1], self.lines_params[1])
                if plot_params.software_compensation: #T
                    self.legend.addItem(self.graph_curves[2], self.lines_params[2])
                    self.legend.addItem(self.graph_curves[3], self.lines_params[3])
            else: #F
                if plot_params.software_compensation: #T
                    self.legend.addItem(self.graph_curves[2], self.lines_params[1])
                    self.legend.addItem(self.graph_curves[3], self.lines_params[2])
        else:
            for i, lp in enumerate(self.lines_params):
                self.legend.addItem(self.graph_curves[i], lp)

    @Slot()
    def s_tmos_data_tag_done(self, status, tag_label:str, comp_name:str):
        if comp_name == self.comp_name:
            if isinstance(self.plot_params, PlotPPresenceParams) and tag_label == "Presence" or \
                isinstance(self.plot_params, PlotPPresenceParams) and tag_label == "Presence (SW comp)" or \
                isinstance(self.plot_params, PlotPMotionParams) and tag_label == "Motion" or \
                isinstance(self.plot_params, PlotPMotionParams) and tag_label == "Motion (SW comp)":
                if tag_label == "Presence" or tag_label == "Motion":
                    colors = ['#00FF00', '#FF0000']
                    texts = ["Start", "End"]
                else:
                    colors = ['#00FFFF', '#FF8000']
                    texts = ["Start (SW comp)", "End (SW comp)"]
                
                if status:
                    if not tag_label in self.active_tags or self.active_tags[tag_label] == False:
                        self.active_tags[tag_label] = True
                        pen=pg.mkPen(color=colors[0], width=3, style=Qt.PenStyle.DotLine)
                        tag_line = pg.InfiniteLine(pos = self.x_data[-1], angle=90, movable=False, pen=pen)
                        label = pg.TextItem(text=texts[0].format(tag_label), angle=-90)
                        label.setAnchor((1, 0))  # Position label above the line
                        label.setParentItem(tag_line)
                        self.graph_widget.addItem(tag_line, ignoreBounds=True)
                        self.tag_lines.append(tag_line)
                else:
                    if not tag_label in self.active_tags or self.active_tags[tag_label] == True:
                        self.active_tags[tag_label] = False
                        pen=pg.mkPen(color=colors[1], width=3, style=Qt.PenStyle.DotLine)
                        tag_line = pg.InfiniteLine(pos = self.x_data[-1], angle=90, movable=False, pen=pen)
                        label = pg.TextItem(text=texts[1], angle=-90)
                        label.setAnchor((1, 0))  # Position label above the line
                        label.setParentItem(tag_line)
                        self.graph_widget.addItem(tag_line, ignoreBounds=True)
                        self.tag_lines.append(tag_line)

    def add_data(self, data):
        super().add_data(data)
        if isinstance(self.plot_params, PlotPPresenceParams):
            if bool(data[1].any()) == True:
                self.controller.sig_tmos_presence_detected.emit(True, "Presence", self.comp_name)
            else:
                self.controller.sig_tmos_presence_detected.emit(False, "Presence", self.comp_name)
            if self.plot_params.software_compensation:
                if bool(data[2].any()) == True:
                    self.controller.sig_tmos_presence_detected.emit(True, "Presence (SW comp)", self.comp_name)
                else:
                    self.controller.sig_tmos_presence_detected.emit(False, "Presence (SW comp)", self.comp_name)
        elif isinstance(self.plot_params, PlotPMotionParams):
            if bool(data[1].any()) == True:
                self.controller.sig_tmos_motion_detected.emit(True, "Motion", self.comp_name)
            else:
                self.controller.sig_tmos_motion_detected.emit(False, "Motion", self.comp_name)
            if self.plot_params.software_compensation:
                if bool(data[2].any()) == True:
                    self.controller.sig_tmos_motion_detected.emit(True, "Motion (SW comp)", self.comp_name)
                else:
                    self.controller.sig_tmos_motion_detected.emit(False, "Motion (SW comp)", self.comp_name)
    
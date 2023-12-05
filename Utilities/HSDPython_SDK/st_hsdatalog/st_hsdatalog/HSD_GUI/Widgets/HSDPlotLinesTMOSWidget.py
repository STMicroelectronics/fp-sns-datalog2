
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


from PySide6.QtCore import Slot, Qt

import pyqtgraph as pg

from st_dtdl_gui.Utils.DataClass import PlotPMotionParams, PlotPPresenceParams
from st_hsdatalog.HSD_GUI.Widgets.HSDPlotLinesWidget import HSDPlotLinesWidget

class HSDPlotLinesTMOSWidget(HSDPlotLinesWidget):
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, plot_params, p_id, parent)
        
        self.controller.sig_tag_done.connect(self.s_tag_done)
        
        self.controller.sig_tmos_presence_detected.connect(self.s_tmos_data_tag_done)
        self.controller.sig_tmos_motion_detected.connect(self.s_tmos_data_tag_done)
        
        self.prev_data_flag = False
        self.data_flag = False
        self.data_flag_regions = {}

    @Slot()
    def s_tmos_data_tag_done(self, status, tag_label:str):
        if isinstance(self.plot_params, PlotPPresenceParams) and tag_label == "Presence" or \
            isinstance(self.plot_params, PlotPMotionParams) and tag_label == "Motion":
            if status:
                if not tag_label in self.active_tags or self.active_tags[tag_label] == False:
                    self.active_tags[tag_label] = True
                    pen=pg.mkPen(color='#00FF00', width=2, style=Qt.PenStyle.DotLine)
                    tag_line = pg.InfiniteLine(pos = self.x_data[-1], angle=90, movable=False, pen=pen)
                    label = pg.TextItem(text="Start".format(tag_label), angle=-90)
                    label.setAnchor((1, 0))  # Position label above the line
                    label.setParentItem(tag_line)
                    self.graph_widget.addItem(tag_line, ignoreBounds=True)
                    self.tag_lines.append(tag_line)
            else:
                if not tag_label in self.active_tags or self.active_tags[tag_label] == True:
                    self.active_tags[tag_label] = False
                    pen=pg.mkPen(color='#FF0000', width=2, style=Qt.PenStyle.DotLine)
                    tag_line = pg.InfiniteLine(pos = self.x_data[-1], angle=90, movable=False, pen=pen)
                    label = pg.TextItem(text="End", angle=-90)
                    label.setAnchor((1, 0))  # Position label above the line
                    label.setParentItem(tag_line)
                    self.graph_widget.addItem(tag_line, ignoreBounds=True)
                    self.tag_lines.append(tag_line)

    def add_data(self, data):
        super().add_data(data)
        if isinstance(self.plot_params, PlotPPresenceParams):
            if bool(data[1].any()) != self.data_flag:
                self.data_flag = not self.data_flag
                self.controller.sig_tmos_presence_detected.emit(self.data_flag, "Presence")
        elif isinstance(self.plot_params, PlotPMotionParams):
            if bool(data[1].any()) != self.data_flag:
                self.data_flag = not self.data_flag
                self.controller.sig_tmos_motion_detected.emit(self.data_flag, "Motion")

    
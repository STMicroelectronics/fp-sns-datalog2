
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

from PySide6.QtCore import Slot
from PySide6.QtWidgets import QFrame, QHBoxLayout

from st_dtdl_gui.Utils.DataClass import PlotParams
from st_dtdl_gui.Widgets.Plots.PlotHeatmapWidget import PlotHeatmapWidget
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget

class HSDPlotToFWidget(PlotWidget):    
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent)
        self.active_tags = dict()
        self.plot_params = plot_params
        self.heatmaps = {}
        self.RESOLUTION_4x4 = 0
        self.RESOLUTION_8x8 = 1
        
        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.contents_frame.layout().itemAt(i).widget().setParent(None)

        heatmaps_shape = (4,4) if plot_params.resolution == self.RESOLUTION_4x4 else (8,8) #0 = 4x4, 1 = 8x8

        self.t1_out = PlotHeatmapWidget(controller, comp_name, comp_display_name, heatmaps_shape, plot_label= "Target 1", p_id = p_id, parent=self)
        # self.t2_out = PlotHeatmapWidget(controller, comp_name, comp_display_name, heatmaps_shape, plot_label= "Target 2", p_id = p_id, parent=self)
        self.heatmaps["target1"] = self.t1_out
        # self.heatmaps["target2"] = self.t2_out

        heatmaps_frame = QFrame()
        wdg_layout = QHBoxLayout()
        wdg_layout.addWidget(self.t1_out)
        # wdg_layout.addWidget(self.t2_out)
        heatmaps_frame.setLayout(wdg_layout)
        self.contents_frame.layout().addWidget(heatmaps_frame)
    
    @Slot(bool, int) #Override PlotLinesWavWidget s_is_logging
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1:
            print("Sensor {} is logging via USB: {}".format(self.comp_name, status))
        super().s_is_logging(status, interface)
    
    def update_plot_characteristics(self, plot_params:PlotParams):
        heatmaps_shape = (4,4) if plot_params.resolution == self.RESOLUTION_4x4 else (8,8)
        self.t1_out.update_plot_characteristics(heatmaps_shape)
        # self.t2_out.update_plot_characteristics(heatmaps_shape)

    def add_data(self, data):
        start_t1_dist_id = 4
        # start_t2_dist_id = 7
        #extract targets matrices
        t1_data = data[0][start_t1_dist_id::8]
        t1_status_mask = data[0][start_t1_dist_id-1::8]
        # t2_data = data[0][start_t2_dist_id::8]
        #then:
        self.heatmaps["target1"].add_data((t1_data,t1_status_mask))
        # self.heatmaps["target2"].add_data(t2_data)

    
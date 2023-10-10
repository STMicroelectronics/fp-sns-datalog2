 
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

from st_dtdl_gui.Utils.DataClass import MCTelemetriesPlotParams
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from HSD_MC_GUI.Widgets.HSD_MC_FastTelemetriesPlotLinesWidget import HSD_MC_FastTelemetriesPlotLinesWidget

class HSD_MC_FastTelemetriesPlotWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, plot_params, time_window, p_id = 0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, "")
        
        self.plot_t_interval_size = int(self.plot_len/(time_window / self.timer_interval))
        
        self.graph_curves = dict()
        
        self.one_t_interval_resampled = dict()

        self.graph_widget.deleteLater()
        self.n_curves = 1
        self.plots_params = plot_params

        self.graph_widgets = {}

        for i, p in enumerate(plot_params.plots_params_dict):
            p_enabled = plot_params.plots_params_dict[p].enabled

            if p_enabled:
                unit = self.plots_params.plots_params_dict[p].unit
                self.plots_params.plots_params_dict[p].unit = "{} [{}]".format(p,unit)
                pw = HSD_MC_FastTelemetriesPlotLinesWidget(self.controller,
                                     self.plots_params.plots_params_dict[p].comp_name,
                                     p,
                                     plot_params.plots_params_dict[p],
                                     i,
                                     self
                                     )
                self.graph_widgets[p] = pw
                self.contents_frame.layout().addWidget(self.graph_widgets[p].graph_widget)
                self.contents_frame.layout().setSpacing(6)

            self.graph_widgets[p].graph_widget.setVisible(p_enabled)
        #self.update_plot_characteristics(plot_params)

    def update_plot_characteristics(self, plot_params: MCTelemetriesPlotParams):
        self.plots_params = plot_params
        for i, p in enumerate(plot_params.plots_params_dict):
            p_enabled = plot_params.plots_params_dict[p].enabled
            self.graph_widgets[p].graph_widget.setVisible(p_enabled)
            
        self.app_qt.processEvents()
        
    @Slot(bool)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1:
            print("Component {} is logging via USB: {}".format(self.comp_name,status))
            if status:
                #Get number of enabled fast telemetries
                self.ft_enabled_list = [ ft for ft in self.plots_params.plots_params_dict if self.plots_params.plots_params_dict[ft].enabled]
                self.update_plot_characteristics(self.plots_params)
            else:
                self.ft_enabled_list = []

    def add_data(self, data):
        for i, ft_enabled_name in enumerate(self.ft_enabled_list):
            self.graph_widgets[ft_enabled_name].add_data([data[0][i::len(self.ft_enabled_list)]])

    def get_num_enabled_fast_tele(self):
        enabled_cnt = 0
        for i, p in enumerate(self.plots_params.plots_params_dict):
            if self.plots_params.plots_params_dict[p].enabled:
                enabled_cnt += 1
        return enabled_cnt

    


 
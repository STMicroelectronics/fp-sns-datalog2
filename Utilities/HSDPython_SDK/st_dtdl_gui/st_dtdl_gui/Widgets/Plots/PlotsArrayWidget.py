 
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
import os

from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QPalette
from PySide6.QtWidgets import QFrame, QLabel, QPushButton, QVBoxLayout, QHBoxLayout, QSizePolicy, QWidget, QListWidget, QListView, QListWidgetItem, QAbstractItemView
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import st_dtdl_gui
from st_dtdl_gui.Widgets.Plots.AnalogGaugeWidget import AnalogGaugeWidget
from st_dtdl_gui.Widgets.Plots.LabelPlotWidget import LabelPlotWidget
from st_dtdl_gui.Widgets.Plots.LevelPlotWidget import LevelPlotWidget
from st_dtdl_gui.Widgets.Plots.PlotLinesWidget import PlotLinesWidget
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget, PlotLabel
from st_dtdl_gui.Utils.DataClass import PlotLabelParams, PlotLevelParams, PlotGaugeParams, PlotCheckBoxParams
from st_dtdl_gui.Widgets.Plots.CheckBoxListWidget import CheckBoxListWidget

class PlotsArrayWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, out_plots, p_id=0, parent=None, left_label=None, orientation="h"):
        """AI is creating summary for __init__
        Args:
            controller ([type]): [description]
            comp_name ([type]): [description]
            comp_display_name ([type]): [description]
            out_plots (dict): a dict (plot name, plot type)
            p_id (int, optional): [description]. Defaults to 0.
            parent ([type], optional): [description]. Defaults to None.
        """
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, left_label)
        
        # self.setMinimumHeight(610)
        self.orientation = orientation
        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.layout().itemAt(i).widget().setParent(None)
        
        self._data = dict() # dict of queues
        self._data[0] = deque(maxlen=200000)

        self.out_plots = out_plots
        self.out_plots_widget = {}
        
        # New Customized Graphic layout
        QPyDesignerCustomWidgetCollection.registerCustomWidget(PlotsArrayWidget, module="PlotsArrayWidget")
        self.loader = QUiLoader()
        self.plot_widget = self.loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","plots_array_output.ui"), parent)
        self.title_frame = self.plot_widget.findChild(QFrame,"frame_title")
        contents_frame = self.plot_widget.findChild(QFrame,"frame_contents")
        
        if self.orientation == "h":
            # self.sub_plot_list = FlowContainer(self)
            self.sub_plot_list = QHBoxLayout(self)
            contents_frame.setLayout(self.sub_plot_list)
            # contents_frame.layout().addWidget(self.sub_plot_list)
        else:
            self.sub_plot_list = QVBoxLayout(self)
            contents_frame.setLayout(self.sub_plot_list)
        
        pushButton_pop_out = self.title_frame.findChild(QPushButton, "pushButton_pop_out")
        pushButton_pop_out.clicked.connect(self.clicked_pop_out_button)
        
        title_label = PlotLabel("{}".format(self.comp_display_name))
        title_label.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        self.title_frame.layout().addWidget(title_label)
        
        sp_id = 0
        for pw in self.out_plots:
            sub_plot = SubPlotFrame(pw, sp_id, self)
            contents_frame.layout().addWidget(sub_plot)
            sp_id += 1
        self.timer_interval_ms = self.timer_interval*700
        self.layout().addWidget(self.plot_widget)
    
    @Slot(bool)
    def s_is_detecting(self, status: bool):
        if status:
            self.buffering_timer_counter = 0
            self.timer.start(self.timer_interval_ms)
        else:
            self.timer.stop()

    def add_data(self, data):
        if "slow_mc" in self.comp_name:
            data_idx = 0
            for p_id in range(len(self.out_plots)):
                if self.out_plots[list(self.out_plots.keys())[p_id]].enabled:
                    sub_plot = self.out_plots_widget[list(self.out_plots.keys())[p_id]]
                    sub_plot.add_data(data[data_idx])
                    data_idx = data_idx + 1
        
        elif "fast_mc" in self.comp_name:
            enabled_cnt = 0
            for p_id in range(len(self.out_plots)):
                if self.out_plots[list(self.out_plots.keys())[p_id]].enabled:
                    enabled_cnt = enabled_cnt+1
            
            current_enabled_tele =0
            for p_id in range(len(self.out_plots)):
                if self.out_plots[list(self.out_plots.keys())[p_id]].enabled:
                    sub_plot = self.out_plots_widget[list(self.out_plots.keys())[p_id]]
                    sub_plot.add_data(data[0][current_enabled_tele::enabled_cnt])
                    current_enabled_tele = current_enabled_tele+1
                    

    
    def update_plot_characteristics(self, plot_params):
        pass
   
class SubPlotFrame(QWidget):
    def __init__(self, pw, sp_id, parent_wdgt:PlotsArrayWidget):
        super().__init__()
        self.parent_wdgt = parent_wdgt
        plot_params = self.parent_wdgt.out_plots[pw]
        sub_plot_ui_widget = self.parent_wdgt.loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","sub_plot.ui"), self.parent_wdgt)
        sub_plot_frame_contents = sub_plot_ui_widget.findChild(QFrame,"frame_contents")
        sub_plot_name = sub_plot_frame_contents.findChild(QLabel,"sub_plot_name")
        sub_plot_name.setStyleSheet("color: #6e778d; font-size: 20px;")
        sub_plot_name.setText(pw)
        sub_plot_widget = sub_plot_frame_contents.findChild(QWidget,"sub_plot_widget")
        comp_name = self.parent_wdgt.comp_name
        comp_display_name = self.parent_wdgt.comp_display_name
        if isinstance(plot_params, PlotGaugeParams):   
            sub_plot = AnalogGaugeWidget(self.parent_wdgt.controller, self.parent_wdgt.comp_name, self.parent_wdgt.comp_display_name, plot_params.min_val, plot_params.max_val, plot_params.unit, sp_id, self.parent_wdgt)
            sub_plot.setMinimumSize(200,200)
        elif isinstance(plot_params, PlotLabelParams):
            sub_plot = LabelPlotWidget(self.parent_wdgt.controller, comp_name, pw, plot_params, sp_id, self.parent_wdgt)
            sub_plot_name.setVisible(False)
        elif isinstance(plot_params, PlotLevelParams):
            sub_plot = LevelPlotWidget(self.parent_wdgt.controller, comp_name, comp_display_name, plot_params.min_val, plot_params.max_val, 1, plot_params.unit, sp_id, self.parent_wdgt)
            sub_plot.setFixedSize(200, 200)
            #hide x axis
            sub_plot.graph_widget.getAxis('bottom').setVisible(False)
        elif isinstance(plot_params, PlotCheckBoxParams):
            sub_plot = CheckBoxListWidget(self.parent_wdgt.controller, comp_name, comp_display_name, plot_params.labels, sp_id)
            sub_plot.setFixedSize(200, 200)
        else:
            sub_plot = PlotLinesWidget(self.parent_wdgt.controller, self.parent_wdgt.comp_name, self.parent_wdgt.comp_display_name, 1, "", 30, 1, sp_id, self.parent_wdgt)
            sub_plot.setFixedSize(1200, 200)
        sub_plot_widget.layout().addWidget(sub_plot)
        self.parent_wdgt.out_plots_widget[pw] = sub_plot

        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(1)
        layout.addWidget(sub_plot_ui_widget)
        sub_plot_widget.setContentsMargins(0, 0, 0, 0)
        # self.setFixedSize(layout.sizeHint())


class FlowContainer(QListWidget):
    def __init__(self, parent_wdgt):
        super().__init__()
        self.parent_wdgt = parent_wdgt
        self.viewport().setBackgroundRole(QPalette.Window)
        self.setFlow(QListView.Flow.LeftToRight)
        self.setWrapping(True)
        # prevent user repositioning
        self.setMovement(QListView.Movement.Static)
        self.setResizeMode(QListView.ResizeMode.Adjust)        
        self.setHorizontalScrollMode(QAbstractItemView.ScrollMode.ScrollPerPixel)
        self.setVerticalScrollMode(QAbstractItemView.ScrollMode.ScrollPerPixel)
        self.setSpacing(4)
        self.setItemAlignment(Qt.AlignmentFlag.AlignCenter)

    def addSubPlot(self, sub_plot, sp_id, parent_wdgt):
        item = QListWidgetItem(sub_plot)
        item.setFlags(item.flags() & ~(Qt.ItemIsSelectable|Qt.ItemIsEnabled))
        self.addItem(item)
        frame = SubPlotFrame(sub_plot, sp_id, parent_wdgt)
        item.setSizeHint(frame.sizeHint())
        self.setItemWidget(item, frame)
        frame.setStyleSheet("background-color: rgb(39, 44, 54);")
        
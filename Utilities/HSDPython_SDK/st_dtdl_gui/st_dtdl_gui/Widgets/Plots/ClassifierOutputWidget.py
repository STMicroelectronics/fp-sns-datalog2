 
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

from PySide6.QtCore import Qt, QPoint, Slot
from PySide6.QtGui import QPainter, QPixmap
from PySide6.QtWidgets import QFrame, QLabel, QPushButton, QSizePolicy
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import st_dtdl_gui
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotLabel

class ClassifierOutputWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, out_classes, ai_tool = None, with_signal=False, with_confidence=True, p_id=0, parent=None, left_label=None):
        """AI is creating summary for __init__
        Args:
            controller ([type]): [description]
            comp_name ([type]): [description]
            comp_display_name ([type]): [description]
            out_classes (dict): a dict (class name, image_path)
            p_id (int, optional): [description]. Defaults to 0.
            parent ([type], optional): [description]. Defaults to None.
        """
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, left_label)
        
        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.layout().itemAt(i).widget().setParent(None)
    
        self._data = dict() # dict of queues
        self._data[0] = deque(maxlen=200000)
        
        self.parent_widget = parent

        self.out_classes = out_classes
        self.ai_tool = ai_tool
        self.output_class_widget = {}
        self.output_class_pixmaps = {}
        
        self.with_signal = with_signal
        self.with_confidence = with_confidence

        self.is_settings_displayed = True
        self.timer_interval = 1
        self.is_plotting_out = True
        
        # New Customized Graphic layout
        QPyDesignerCustomWidgetCollection.registerCustomWidget(ClassifierOutputWidget, module="ClassifierOutputWidget")
        loader = QUiLoader()
        self.plot_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","classifier_output_widget.ui"), parent)
        title_frame = self.plot_widget.findChild(QFrame,"frame_title")
        self.frame_powered_by_ai =  self.plot_widget.findChild(QFrame,"frame_powered_by_ai")
        self.contents_frame = self.plot_widget.findChild(QFrame,"frame_contents")
        frame_output_classes = self.plot_widget.findChild(QFrame,"frame_output_classes")
        self.pushButton_plot_settings = title_frame.findChild(QPushButton, "pushButton_plot_settings")
        pushButton_pop_out = title_frame.findChild(QPushButton, "pushButton_pop_out")

        pushButton_pop_out.clicked.connect(self.clicked_pop_out_button)

        self.out_info = self.contents_frame.findChild(QFrame,"out_class_info_frame")

        self.out_class_signal_frame = self.out_info.findChild(QFrame, "out_class_signal_frame")
        self.class_signal_value = self.out_info.findChild(QLabel,"out_class_signal_value")
        if not with_signal:
            self.out_class_signal_frame.setVisible(False)
            if not with_confidence:
                self.out_info.setVisible(False)
            else:
                self.out_info.setVisible(True)

        self.out_class_confidence_frame = self.out_info.findChild(QFrame, "out_class_confidence_frame")
        self.class_confidence_value = self.out_info.findChild(QLabel,"out_class_confidence_value")
        if not with_confidence:
            self.out_class_confidence_frame.setVisible(False)
            if not with_signal:
                self.out_info.setVisible(False)
            else:
                self.out_info.setVisible(True)

        if self.ai_tool is not None:
            self.pushButton_close_settings = self.frame_powered_by_ai.findChild(QPushButton, "pushButton_close_settings")
            self.pushButton_close_settings.clicked.connect(self.clicked_close_settings)

            self.pushButton_plot_settings.setVisible(True)
            self.pushButton_plot_settings.clicked.connect(self.clicked_plot_settings_button)

            ai_tool_name = list(ai_tool.keys())[0]
            ai_tool_name_label = self.frame_powered_by_ai.findChild(QLabel,"ai_tool_name")
            ai_tool_name_label.setText(ai_tool_name)
            self.ai_tool_category_label = self.frame_powered_by_ai.findChild(QLabel,"ai_tool_category")
            self.ai_tool_category_label.setText("Classification")
            ai_tool_pixmap = QPixmap(self.ai_tool[ai_tool_name])
            ai_tool_image:QLabel = self.frame_powered_by_ai.findChild(QLabel,"ai_tool_image")
            ai_tool_image.setPixmap(ai_tool_pixmap)
        else:
            self.frame_powered_by_ai.setVisible(False)
            
        title_label = PlotLabel("{}".format(self.comp_display_name))
        title_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        title_frame.layout().addWidget(title_label)
        
        for output_class in self.out_classes:
            self.output_class_widget[output_class] = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","output_class_widget.ui"), parent)
            class_name = self.output_class_widget[output_class].findChild(QLabel,"out_class_name")
            class_name.setText(output_class)
            if output_class != "ISPU": #NOTE Done for ISPU CES2023 Demo purposes
                self.output_class_widget[output_class].out_class_name.setStyleSheet("color: #383D48; font-size: 20px;")
            else:
                self.output_class_widget[output_class].out_class_name.setStyleSheet("color: #3cb4e6; font-size: 20px;")
            class_pixmap = QPixmap(self.out_classes[output_class])
            class_image:QLabel = self.output_class_widget[output_class].findChild(QLabel,"out_class_image")
            enabled_pixmap = class_pixmap
            disabled_pixmap = self.setOpacity(class_pixmap, 0.1)
            self.output_class_pixmaps[output_class] = (enabled_pixmap, disabled_pixmap)
            if output_class != "ISPU": #NOTE Done for ISPU CES2023 Demo purposes
                class_image.setPixmap(disabled_pixmap)
            else:
                class_image.setPixmap(enabled_pixmap)
                
            frame_output_classes.layout().addWidget(self.output_class_widget[output_class])

        self.timer_interval_ms = self.timer_interval*700
        self.layout().addWidget(self.plot_widget)
        
    def setOpacity(self, pixmap:QPixmap, perc):
        new_pix = QPixmap(pixmap.size())
        new_pix.fill(Qt.transparent)
        painter = QPainter(new_pix)
        painter.setOpacity(perc)
        painter.drawPixmap(QPoint(), pixmap)
        painter.end()
        return new_pix
    
    def update_plot_characteristics(self, plot_params):
        pass
    
    @Slot(bool)
    def s_is_detecting(self, status: bool):
        if status:
            if not self.timer.isActive():
                self.timer.start(self.timer_interval_ms)
        else:
            if self.timer.isActive():
                self.timer.stop()
            
    @Slot()
    def s_is_logging(self, status: bool):
        if status:
            if not self.timer.isActive():
                self.timer.start(self.timer_interval_ms)
        else:
            if self.timer.isActive():
                self.timer.stop()
        
    
    def update_plot(self):
        if len(self._data[0]) > 0: 
            if not self.is_plotting_out:
                self.is_plotting_out = True
            # Extract all data from the queue (pop)    
            one_reduced_t_interval = [self._data[0].popleft() for _i in range(len(self._data[0]))]
            ort = one_reduced_t_interval[0][0]
            if self.with_confidence:
                confidence = one_reduced_t_interval[0][1]
                self.class_confidence_value.setText(str(round((confidence * 100.0), 2)) + " %")
            class_id = int(ort)
            class_names = list(self.output_class_widget.keys())
            class_name = class_names[class_id]
            for cn in class_names:
                if cn == class_name:
                    self.output_class_widget[cn].out_class_image.setPixmap(self.output_class_pixmaps[cn][0])
                    self.output_class_widget[cn].setEnabled(True)
                    self.output_class_widget[cn].findChild(QLabel,"out_class_name").setStyleSheet("color: #a4c238; font-size: 30px;")
                else:
                    self.output_class_widget[cn].out_class_image.setPixmap(self.output_class_pixmaps[cn][1])
                    self.output_class_widget[cn].setEnabled(False)
                    self.output_class_widget[cn].findChild(QLabel,"out_class_name").setStyleSheet("color: #383D48; font-size: 20px;")
        else:
            if self.is_plotting_out:
                for cn in list(self.output_class_widget.keys()):
                    self.output_class_widget[cn].out_class_image.setPixmap(self.output_class_pixmaps[cn][1])
                    self.output_class_widget[cn].setEnabled(False)
                    self.output_class_widget[cn].findChild(QLabel,"out_class_name").setStyleSheet("color: #383D48; font-size: 20px;")
                self.is_plotting_out = False
                if self.with_confidence:
                    self.class_confidence_value.setText("--- %")
        self.app_qt.processEvents()

    def add_data(self, data):
        self._data[0].append(data[0])

    @Slot()
    def clicked_plot_settings_button(self):
        self.is_settings_displayed = not self.is_settings_displayed
        if self.is_settings_displayed:
            self.frame_powered_by_ai.setVisible(True)
        else:
            self.frame_powered_by_ai.setVisible(False)

    @Slot()
    def clicked_close_settings(self):
        self.is_settings_displayed = False
        self.frame_powered_by_ai.setVisible(False)

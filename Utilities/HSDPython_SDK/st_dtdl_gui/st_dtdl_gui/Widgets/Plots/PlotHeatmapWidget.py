 
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
from functools import partial

from PySide6.QtCore import Slot, Qt, QTimer, QPoint
from PySide6.QtGui import QColor, QIcon, QIntValidator, QPainter, QPen, QBrush
from PySide6.QtWidgets import QHBoxLayout, QVBoxLayout, QFrame, QPushButton, QLineEdit, QButtonGroup, QLabel, QGridLayout

import pyqtgraph as pg
from st_dtdl_gui.UI.styles import STDTDL_Chip, STDTDL_LineEdit, STDTDL_PushButton
from st_dtdl_gui.Utils import UIUtils
from st_dtdl_gui.Widgets.Plots.PlotWidget import CustomPGPlotWidget, PlotWidget

from PySide6.QtCore import Signal

from pkg_resources import resource_filename
rot_up = resource_filename('st_dtdl_gui.UI.icons', 'outline_keyboard_arrow_up_white_18dp.png')
rot_down = resource_filename('st_dtdl_gui.UI.icons', 'outline_keyboard_arrow_down_white_18dp.png')
flip = resource_filename('st_dtdl_gui.UI.icons', 'outline_autorenew_white_18dp.png')
flip_flipped = resource_filename('st_dtdl_gui.UI.icons', 'outline_autorenew_white_flipped_18dp.png')

roi_colors_rgba = [ [182, 206, 95, 128],
                    [98, 195, 235, 128],
                    [235, 50, 151, 128],
                    [106, 193, 164, 128]]

roi_qcolors = [QColor('#B6CE5F'),
               QColor('#62C3EB'),
               QColor('#EB3297'),
               QColor('#6AC1A4')]

MIN_DIST = 0
MAX_DIST = 2000
ROI_NUMBER = 4

class CustomHeatmapPlotWidget(CustomPGPlotWidget):
    def __init__(self, parent=None, background='default', plotItem=None, **kargs):
        super().__init__(parent, background, plotItem, **kargs)

    def mouseMoveEvent(self, ev):
        pass

class Chip(QPushButton):
    
    def __init__(self, text, color, parent=None):
        super().__init__(text, parent)
        self.color = color
        self.setFlat(True)
        self.setCursor(Qt.CursorShape.PointingHandCursor)
        self.setCheckable(True)
        self.setFixedHeight(30)
        self.setStyleSheet(STDTDL_Chip.color(color))
        self._timer = QTimer(self)
        self._timer.timeout.connect(self._on_timeout)
        self._color = QColor(255, 0, 0)
        self._alpha = 0
        self._direction = 1

    def start_flash(self):
        self._timer.start(10)

    def stop_flash(self):
        self._timer.stop()
        self._alpha = 0
        self.update()

    def enterEvent(self, event):
        super().enterEvent(event)

    def leaveEvent(self, event):
        super().leaveEvent(event)

    def paintEvent(self, event):
        super().paintEvent(event)
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setPen(QPen(Qt.PenStyle.NoPen))
        brush = QBrush(self.color)
        # painter.setBrush(brush)
        # painter.drawRoundedRect(self.rect(), 12, 12)
        brush.setColor(QColor(self._color.red(), self._color.green(), self._color.blue(), self._alpha))
        painter.setBrush(brush)
        painter.drawRoundedRect(self.rect(), 12, 12)
        painter.setPen(QPen(Qt.black))
        painter.drawText(self.rect(), Qt.AlignmentFlag.AlignCenter, self.text())

    def _on_timeout(self):
        self._alpha += self._direction * 10
        if self._alpha > 255:
            self._alpha = 255
            self._direction = -1
        elif self._alpha < 0:
            self._alpha = 0
            self._direction = 1
        self.update()

class ROISettingsWidget(QFrame):

    sig_selected_roi = Signal(int)
    sig_data_rotation = Signal(int)
    sig_data_flip = Signal()
    sig_roi_threshold_set = Signal(int,int)#roi_id,th_value
    sig_presence_threshold_set = Signal(int)#th_value

    def __init__(self):
        super().__init__()
        
        # Create a vertical layout for the widget
        layout = QVBoxLayout()

        layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setFixedWidth(150)

        # Create a button group to manage the radio buttons
        self.button_group = QButtonGroup()

        # Set the layout for the widget
        self.setLayout(layout)

        self.rotation_id = 0
        self.flipped_status = False
        self.rois = {}
        self.setStyleSheet("QFrame { border: transparent; background:#272c36}")

        # Create button widgets to adjust the rotation value
        self.rot_left_button = QPushButton()
        self.rot_right_button = QPushButton()
        self.rot_left_button.setStyleSheet(STDTDL_PushButton.valid)
        self.rot_right_button.setStyleSheet(STDTDL_PushButton.valid)
        self.rot_left_button.setIcon(QIcon(rot_down))
        self.rot_right_button.setIcon(QIcon(rot_up))
        self.rot_left_button.clicked.connect(lambda:self.data_rotation(-1))
        self.rot_right_button.clicked.connect(lambda:self.data_rotation(+1))

        # Create a label widget to display the value
        self.rot_label = QLabel("0°")
        self.rot_label.setStyleSheet("font:700")
        self.rot_label.setAlignment(Qt.AlignCenter)

        rot_label_title = QLabel("Data Rotation:")
        flip_label_title = QLabel("Data Flip:")
        roi_label_title = QLabel("ROI settings:")

        rotation_layout = QHBoxLayout()
        rotation_layout.addWidget(self.rot_left_button)
        rotation_layout.addWidget(self.rot_label)
        rotation_layout.addWidget(self.rot_right_button)

        self.flip_label = QLabel("Normal")
        self.flip_label.setStyleSheet("font:700")

        flip_layout = QHBoxLayout()
        self.flip_button = QPushButton()
        self.flip_button.setStyleSheet(STDTDL_PushButton.valid)
        self.flip_button.setIcon(QIcon(flip))
        self.flip_button.clicked.connect(self.data_flip)
        flip_layout.addWidget(self.flip_button)
        flip_layout.addWidget(self.flip_label)

        self.layout().addWidget(rot_label_title)
        self.layout().insertLayout(1, rotation_layout)
        self.layout().addWidget(flip_label_title)
        self.layout().insertLayout(3, flip_layout)
        

        global_thresh_layout = QVBoxLayout()
        global_thresh_label = QLabel("Global threshold:")
        global_thresh_layout.addWidget(global_thresh_label)
        self.global_thresh_value = QLineEdit()
        self.global_thresh_value.setText(str(MIN_DIST))
        self.global_thresh_value.setFixedSize(60,30)
        self.global_thresh_value.setStyleSheet(STDTDL_LineEdit.valid)
        self.global_thresh_value.setAlignment(Qt.AlignmentFlag.AlignCenter)
        int_validator = QIntValidator()
        int_validator.setRange(MIN_DIST, MAX_DIST)
        self.global_thresh_value.setToolTip("min: {}, max: {}".format(MIN_DIST, MAX_DIST))
        self.global_thresh_value.setValidator(int_validator)
        self.global_thresh_value.textChanged.connect(partial(UIUtils.validate_value, self.global_thresh_value))
        self.global_thresh_value.editingFinished.connect(lambda: self.presence_threshold_set(self.global_thresh_value))
        global_thresh_layout.addWidget(self.global_thresh_value)
        self.layout().insertLayout(4, global_thresh_layout)
        
        self.layout().addWidget(roi_label_title)
        rois_layout = QGridLayout()
        roi_col_label = QLabel("region")
        roi_col_label.setStyleSheet("color:#666666;")
        roi_col_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        rois_layout.addWidget(roi_col_label,0,0)
        thresh_col_label = QLabel("threshold")
        thresh_col_label.setStyleSheet("color:#666666;")
        thresh_col_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        rois_layout.addWidget(thresh_col_label,0,1)

        for i in range(0,ROI_NUMBER):
            self.add_roi_chip(rois_layout,i,6)

        self.layout().insertLayout(6, rois_layout)

    def data_rotation(self, inc_dec):
        self.rotation_id += inc_dec
        r_id = self.rotation_id % 4
        if r_id == 0 or r_id == 4:
            self.rot_label.setText("0°")
        elif r_id == 1:
            self.rot_label.setText("90°")
        elif r_id == 2:
            self.rot_label.setText("180°")
        elif r_id == 3:
            self.rot_label.setText("270°")
        self.sig_data_rotation.emit(r_id)
    
    def data_flip(self):
        self.flipped_status = not self.flipped_status
        self.flip_label.setText("Flipped") if self.flipped_status else self.flip_label.setText("Normal")
        self.flip_button.setIcon(QIcon(flip_flipped) if self.flipped_status else QIcon(flip))
        self.sig_data_flip.emit()

    def add_roi_chip(self, rois_layout:QGridLayout, roi_id, start_offset = 0):
        # Create a new horizontal layout for the radio button and "X" button
        # chip_layout = QHBoxLayout()

        # Create a new radio button and add it to the layout
        roi_chip = Chip("ROI {}".format(roi_id), roi_qcolors[roi_id])
        roi_chip.setFixedSize(50,30)
        roi_chip.toggled.connect(lambda: self.roi_radio_clicked(roi_id))
        # chip_layout.addWidget(roi_chip)
        
        roi_threshold_lineedit = QLineEdit()
        roi_threshold_lineedit.setText(str(MIN_DIST))
        roi_threshold_lineedit.setFixedSize(60,30)
        roi_threshold_lineedit.setStyleSheet(STDTDL_LineEdit.valid)
        roi_threshold_lineedit.setAlignment(Qt.AlignmentFlag.AlignCenter)
        int_validator = QIntValidator()
        int_validator.setRange(MIN_DIST, MAX_DIST)
        roi_threshold_lineedit.setToolTip("min: {}, max: {}".format(MIN_DIST, MAX_DIST))
        roi_threshold_lineedit.setValidator(int_validator)
        roi_threshold_lineedit.textChanged.connect(partial(UIUtils.validate_value, roi_threshold_lineedit))
        roi_threshold_lineedit.editingFinished.connect(lambda: self.roi_threshold_set(roi_id, roi_threshold_lineedit))
        # chip_layout.addWidget(roi_threshold_lineedit)

        # Add the radio button to the button group
        self.button_group.addButton(roi_chip)

        self.rois[roi_id] = roi_chip        
        if len(self.rois) == 1:
            roi_chip.toggle()
        
        # self.layout().setSpacing(12)
        # self.layout().insertLayout(roi_id+start_offset, chip_layout)
        rois_layout.addWidget(roi_chip, roi_id+1, 0)
        rois_layout.addWidget(roi_threshold_lineedit, roi_id+1, 1)
    
    def get_rois_chips(self):
        return self.rois

    def roi_radio_clicked(self, roi_id):
        self.sig_selected_roi.emit(roi_id)

    def roi_threshold_set(self, roi_id, threshold_lineedit):
        self.sig_roi_threshold_set.emit(roi_id, int(threshold_lineedit.text()))
    
    def presence_threshold_set(self, threshold_lineedit):
        self.sig_presence_threshold_set.emit(int(threshold_lineedit.text()))

class PlotHeatmapWidget(PlotWidget):
    
    sig_threshold_exceded = Signal(int,int,int)# roi_id, current_value, threshold_value

    def __init__(self, controller, comp_name, comp_display_name, heatmap_shape, plot_label= "", p_id = 0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, plot_label)

        self.plot_label = plot_label
        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        while self.layout().count():
            item = self.layout().takeAt(0)
            widget = item.widget()
            if widget:
                widget.deleteLater()

        main_layout = QHBoxLayout()
        main_frame = QFrame()
        main_frame.setStyleSheet("QFrame { border-radius: 5px; border: 2px solid rgb(27, 29, 35);}")
        main_frame.setLayout(main_layout)
        
        self.graph_widget = CustomHeatmapPlotWidget(parent = self)        
        
        self.heatmap_shape = heatmap_shape
        self.heatmap_rotation = 0
        self.heatmap_is_flipped = False
        self.roi_thresolds = {i:MIN_DIST for i in range(ROI_NUMBER)}
        self.presence_threshold = MIN_DIST
        
        self.data = np.zeros(shape=(self.heatmap_shape), dtype='i')
        # self.data = np.random.uniform(0, 4000, size=self.heatmap_shape)#debug
        # self.data = np.array([[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]])
        self.validity_mask = np.zeros(shape=(self.heatmap_shape), dtype='i')
        
        self.zones = PlotHeatmapWidget.create_matrix(self.heatmap_shape[0])
        self.rois = {i: {} for i in range(ROI_NUMBER)}
        self.underthresh = {}
        self.is_roi_flashing = {i:False for i in range(ROI_NUMBER)}
        
        self.selected_roi_id = 0
        self.a = 0
        
        self.heatmap_img = pg.ImageItem()
        self.heatmap_img.setImage(self.data, levels=[MIN_DIST, MAX_DIST])

        old_plot_item = self.graph_widget.getPlotItem()
        self.graph_widget.removeItem(old_plot_item)
        self.graph_widget.getPlotItem().addItem(self.heatmap_img)

        self.graph_widget.getPlotItem().layout.setContentsMargins(10, 3, 3, 10)
        self.graph_widget.getPlotItem().setMenuEnabled(False) #Disable right click menu in plots
        self.graph_widget.getPlotItem().showGrid(True,True)
        # self.graph_widget.setFixedSize(QSize(250, 270))
        
        styles = {'color':'#d2d2d2', 'font-size':'12px'}
        self.graph_widget.setLabel('bottom', self.left_label, **styles)
        self.graph_widget.setBackground('#1b1d23')
        
        self.timer_interval_ms = self.timer_interval*700

        self.rois_layout = QVBoxLayout()
        self.rois_frame = ROISettingsWidget()
        self.rois_frame.sig_data_rotation.connect(self.data_rotation_callback)
        self.rois_frame.sig_selected_roi.connect(self.set_selected_roi_id)
        self.rois_frame.sig_data_flip.connect(self.data_flip_callback)
        self.rois_frame.sig_roi_threshold_set.connect(self.roi_threshold_set_callback)
        self.rois_frame.sig_presence_threshold_set.connect(self.presence_threshold_set_callback)
        self.roi_chips = self.rois_frame.rois
        # self.roi_animations = {}

        self.plot_layout = QHBoxLayout()
        self.plot_frame = QFrame()
        self.plot_frame.setStyleSheet("QFrame { border: transparent;}")
        self.plot_frame.setContentsMargins(0,0,0,0)
        self.plot_frame.setLayout(self.plot_layout)
        self.plot_layout.addWidget(self.graph_widget)

        main_layout.addWidget(self.rois_frame)
        main_layout.addWidget(self.plot_frame)
        self.layout().addWidget(main_frame)

        # Add text items for each pixel
        self.text_items = []
        self.fill_with_text_items()

        # Add a mouse click event handler to the imageItem
        self.heatmap_img.mouseClickEvent = self.image_item_clicked
        self.heatmap_img.getViewBox().setAspectLocked(True)

    def fill_with_text_items(self):
        #clean existing text items, if any
        for ti in self.text_items:
            for w in ti:
                self.graph_widget.removeItem(w)
                w.deleteLater()
        self.text_items = []
        
        #add new text items
        for i in range(self.heatmap_shape[0]):
            row = []
            for j in range(self.heatmap_shape[1]):
                text_item = pg.TextItem(text= str(self.data[i, j]), color=(200, 200, 200))
                text_item.setPos(j, i + 0.5)
                self.graph_widget.addItem(text_item)
                row.append(text_item)
            self.text_items.append(row)

    def update_plot_characteristics(self, heatmap_shape):
        self.heatmap_shape = heatmap_shape
        self.zones = PlotHeatmapWidget.create_matrix(self.heatmap_shape[0])
        self.data = np.zeros(shape=(self.heatmap_shape),dtype='i')
        self.heatmap_img.setImage(self.data, levels=[MIN_DIST, MAX_DIST])
        self.graph_widget.getPlotItem()._updateView()
        # Add text items for each pixel
        self.fill_with_text_items()
        self.app_qt.processEvents()

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

    def update_plot(self):
        self.heatmap_img.setImage(self.data, levels=[MIN_DIST, MAX_DIST])
        #TODO send signal only if ...
        self.controller.sig_tof_presence_detected.emit(np.any(np.less(self.data ,self.presence_threshold)),"tof_presence")

        for i in range(self.heatmap_shape[0]):
            for j in range(self.heatmap_shape[1]):
                curr_data = self.data[i][j]
                curr_valid_mask = self.validity_mask[i][j]
                if curr_data > MAX_DIST:
                    self.text_items[j][i].setText("X")
                    curr_valid_mask = -1
                    self.text_items[j][i].setColor("#ee6055")
                else:                    
                    self.text_items[j][i].setText(str(curr_data))
                if curr_valid_mask == 5:
                    self.text_items[j][i].setColor("#6a994e")
                elif curr_valid_mask == 6 or curr_valid_mask == 9:
                    self.text_items[j][i].setColor("#ffcf56")
                else:
                    self.text_items[j][i].setColor("#ee6055")
                for k in range(ROI_NUMBER):
                    if len(self.rois[k].keys()) == 0:
                        self.underthresh[k] = []
                    elif (i,j) in self.rois[k].keys():
                        if curr_data < self.roi_thresolds[k]:
                            if (i,j) not in self.underthresh[k]:
                                self.underthresh[k].append((i,j))
                        else:
                            if (i,j) in self.underthresh[k]:
                                self.underthresh[k].remove((i,j))
        
        for x in range(ROI_NUMBER):
            if x in self.underthresh and self.underthresh[x] != []:
                 if not self.is_roi_flashing[x]: 
                    roi_chip = self.rois_frame.rois[x]
                    self.is_roi_flashing[x] = True
                    roi_chip.start_flash()
                    self.controller.sig_tof_presence_detected_in_roi.emit(True,x+1,"Target {}".format(x+1))
            else:
                if self.is_roi_flashing[x]: 
                    roi_chip = self.rois_frame.rois[x]
                    self.is_roi_flashing[x] = False
                    roi_chip.stop_flash()
                    self.controller.sig_tof_presence_detected_in_roi.emit(False,x+1,"Target {}".format(x+1))
                        
        
    def add_data(self, data):
        if len(data[0]) == self.heatmap_shape[0]*self.heatmap_shape[1]:
            self.data = data[0].reshape(self.heatmap_shape)
            self.data = np.rot90(self.data, k=self.heatmap_rotation % 4)
            if self.heatmap_is_flipped:
                self.data = np.flip(self.data)
        
        if len(data[1]) == self.heatmap_shape[0]*self.heatmap_shape[1]:
            self.validity_mask = data[1].reshape(self.heatmap_shape)
            self.validity_mask = np.rot90(self.validity_mask, k=self.heatmap_rotation % 4)
            if self.heatmap_is_flipped:
                self.validity_mask = np.flip(self.validity_mask)

    @staticmethod
    def create_matrix(size):
        # Create a matrix of False values
        matrix = [[False for col in range(size)] for row in range(size)]
        # Create an empty dictionary
        coord_dict = {}
        # Iterate over the rows and columns of the matrix
        for row in range(size):
            for col in range(size):
                # Create a dictionary entry for each tuple of coordinates
                coord_dict[(row, col)] = matrix[row][col]
        # Return the dictionary
        return coord_dict
    
    def set_selected_roi_id(self, roi_id):
        self.selected_roi_id = roi_id
    
    def data_rotation_callback(self, rot_id):
        self.heatmap_rotation = rot_id

    def data_flip_callback(self):
        self.heatmap_is_flipped = not self.heatmap_is_flipped

    def roi_threshold_set_callback(self, roi_id, roi_threshold):
        # print("roi_id: {}, roi_threshold: {}".format(roi_id, roi_threshold))
        self.roi_thresolds[roi_id] = roi_threshold
    
    def presence_threshold_set_callback(self, threshold):
        self.presence_threshold = threshold

    def image_item_clicked(self, event):
        # Get the mouse click position in image coordinates
        pos = self.heatmap_img.mapFromScene(event.scenePos())
        x, y = int(pos.x()), int(pos.y())

        # Get the pixel value at the clicked position
        pixel_value = self.data[y, x]

        print(f"Clicked on pixel ({x}, {y}) with value {pixel_value}")

        if self.zones[(x,y)] == False:
            if (x,y) not in self.rois[self.selected_roi_id]:
                mask = pg.ImageItem()
                mask.setImage(np.ones((1, 1, 4)) * np.array(roi_colors_rgba[self.selected_roi_id]))
                mask.setPos(QPoint(x,y))
                self.rois[self.selected_roi_id][(x,y)] = mask
            self.graph_widget.addItem(self.rois[self.selected_roi_id][(x,y)])
            self.zones[(x,y)] = True
        else:
            self.graph_widget.removeItem(self.rois[self.selected_roi_id][(x,y)])
            del self.rois[self.selected_roi_id][(x,y)]
            self.zones[(x,y)] = False
            # self.app.processEvents()

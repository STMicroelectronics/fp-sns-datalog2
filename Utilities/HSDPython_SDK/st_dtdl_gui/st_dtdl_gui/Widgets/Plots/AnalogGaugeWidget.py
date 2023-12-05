
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

from PySide6.QtWidgets import QLabel, QVBoxLayout
from PySide6.QtGui import QPolygon, QPolygonF, QColor, QPen, QFont, QPainter, QFontMetrics, QConicalGradient
from PySide6.QtCore import Qt, QPoint, QPointF, Signal, Slot

from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget

   
class AnalogGaugeInnerWidget(PlotWidget):

    valueChanged = Signal(int)

    def __init__(self, controller, comp_name, comp_display_name, min_value, max_value, left_label=None, p_id=0, parent=None):#TODO! use plotParams
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, left_label)

        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.layout().itemAt(i).widget().setParent(None)

        self.needle_color = QColor(150, 150, 150, 255)
        self.scale_values_color = QColor(255, 255, 255, 255)
        self.display_value_color = QColor(255, 255, 255, 255)
        self.center_color = QColor(27, 29, 35, 255)
        self.markers_pen = QPen(self.center_color)

        self.needle = []
        self.min_value = min_value
        self.max_value = max_value
        self.value = self.min_value

        self.gauge_color_outer_radius_factor = 1 #TODO
        self.gauge_color_inner_radius_factor = 0.95 #TODO

        self.scale_angle_start_value = 135
        self.scale_angle_size = 270

        self.scale_values_count = 10
        self.scale_sub_values_count = 5

        self.l_data = self.min_value

        self.scale_polygon_colors = [[.00, Qt.red],
                                     [.1, Qt.yellow],
                                     [.15, Qt.green],
                                     [1, Qt.transparent]]

        self.scale_values_font = "Roboto"
        self.scale_values_fontsize = 8
        self.value_font = "Roboto"
        self.value_fontsize = 15
        self.component_label_fontsize = 21
        
        self.text_radius_factor = 0.7 #TODO
    
    @Slot(bool, int)
    def s_is_logging(self, status: bool, interface: int):
        if status:
            self.buffering_timer_counter = 0
            self.timer.start(self.timer_interval_ms)
        else:
            self.timer.stop()
    
    def update_plot(self):
        self.value = self.l_data
        self.update()
    
    def add_data(self, data):
        if data > self.max_value:
            data = self.max_value
        self.l_data = data
    
    def set_scale_method(self):
        self.wdgt_width = self.width() if self.width() <= self.height() else self.height()
        self.needle.append(QPolygon([
            QPoint(3, 27),
            QPoint(-3, 27),
            QPoint(-2, - self.wdgt_width / 2*0.8),
            QPoint(0, - self.wdgt_width / 2*0.8 - 6),
            QPoint(2, - self.wdgt_width / 2*0.8)
        ]))
    
    def create_pie(self, start, lenght, out_rad, in_rad):
        polygon_pie = QPolygonF()
        x = y = 0
        lenght = int(round((lenght / (self.max_value - self.min_value)) * (self.value - self.min_value)))
        # Outer circle line
        for i in range(lenght+1):
            t = i + start
            x = out_rad * math.cos(math.radians(t))
            y = out_rad * math.sin(math.radians(t))
            polygon_pie.append(QPointF(x, y))
        # Inner circle line
        for i in range(lenght+1):
            t = (lenght - i) + start
            x = in_rad * math.cos(math.radians(t))
            y = in_rad * math.sin(math.radians(t))
            polygon_pie.append(QPointF(x, y))
        # Outer line closure
        polygon_pie.append(QPointF(x, y))
        return polygon_pie

    def draw_polygon(self, outline_pen_with=0):
        painter_filled_polygon = QPainter(self)
        painter_filled_polygon.setRenderHint(QPainter.Antialiasing)
        # Center the coords origin with the widget
        painter_filled_polygon.translate(self.width() / 2, self.height() / 2)
        painter_filled_polygon.setPen(Qt.NoPen)
        self.markers_pen.setWidth(outline_pen_with)
        if outline_pen_with > 0:
            painter_filled_polygon.setPen(self.markers_pen)
        colored_scale_polygon = self.create_pie(
            self.scale_angle_start_value,
            self.scale_angle_size,
            ((self.wdgt_width / 2) - (self.markers_pen.width() / 2)) * self.gauge_color_outer_radius_factor,
            (((self.wdgt_width / 2) - (self.markers_pen.width() / 2)) * self.gauge_color_inner_radius_factor))
        grad = QConicalGradient(QPointF(0, 0), - self.scale_angle_size - self.scale_angle_start_value - 1)

        for eachcolor in self.scale_polygon_colors:
            grad.setColorAt(eachcolor[0], eachcolor[1])
        painter_filled_polygon.setBrush(grad)
        painter_filled_polygon.drawPolygon(colored_scale_polygon)

    def draw_large_markers(self):
        my_painter = QPainter(self)
        my_painter.setRenderHint(QPainter.Antialiasing)
        my_painter.translate(self.width() / 2, self.height() / 2)
        
        self.markers_pen.setWidth(2)
        my_painter.setPen(self.markers_pen)

        my_painter.rotate(self.scale_angle_start_value)
        steps_size = (float(self.scale_angle_size) / float(self.scale_values_count))
        scale_line_outer_start = self.wdgt_width/2
        scale_line_lenght = (self.wdgt_width / 2) - (self.wdgt_width / 20)
        for i in range(self.scale_values_count+1):
            my_painter.drawLine(scale_line_lenght, 0, scale_line_outer_start, 0)
            my_painter.rotate(steps_size)

    def draw_markers_values_text(self):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.translate(self.width() / 2, self.height() / 2)
        
        font = QFont(self.scale_values_font, self.scale_values_fontsize)
        fm = QFontMetrics(font)

        pen_shadow = QPen()

        pen_shadow.setBrush(self.scale_values_color)
        painter.setPen(pen_shadow)

        text_radius_factor = 0.8
        text_radius = self.wdgt_width/2 * text_radius_factor

        scale_per_div = int((self.max_value - self.min_value) / self.scale_values_count)

        angle_distance = (float(self.scale_angle_size) / float(self.scale_values_count))
        for i in range(self.scale_values_count + 1):
            text = str(int(self.min_value + scale_per_div * i))
            w = fm.boundingRect(text).width() + 1
            h = fm.height()
            painter.setFont(QFont(self.scale_values_font, self.scale_values_fontsize))
            angle = angle_distance * i + float(self.scale_angle_start_value)
            x = text_radius * math.cos(math.radians(angle))
            y = text_radius * math.sin(math.radians(angle))
            text = [x - int(w/2), y - int(h/2), int(w), int(h), Qt.AlignCenter, text]
            painter.drawText(text[0], text[1], text[2], text[3], text[4], text[5])

    def draw_small_markers(self):
        my_painter = QPainter(self)
        my_painter.setRenderHint(QPainter.Antialiasing)
        my_painter.translate(self.width() / 2, self.height() / 2)

        my_painter.setPen(QColor(0, 0, 0, 255))
        my_painter.rotate(self.scale_angle_start_value)
        steps_size = (float(self.scale_angle_size) / float(self.scale_values_count * self.scale_sub_values_count))
        scale_line_outer_start = self.wdgt_width/2
        scale_line_lenght = (self.wdgt_width / 2) - (self.wdgt_width / 40)
        for i in range((self.scale_values_count * self.scale_sub_values_count)+1):
            my_painter.drawLine(scale_line_lenght, 0, scale_line_outer_start, 0)
            my_painter.rotate(steps_size)

    def draw_values_text(self):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        painter.translate(self.width() / 2, self.height() / 2)
        font = QFont(self.value_font, self.value_fontsize)
        fm = QFontMetrics(font)

        pen_shadow = QPen()

        pen_shadow.setBrush(self.display_value_color)
        painter.setPen(pen_shadow)

        text_radius = self.wdgt_width / 2 * self.text_radius_factor
        text = str(int(self.value))
        w = fm.boundingRect(text).width() + 1
        h = fm.height()
        painter.setFont(QFont(self.value_font, self.value_fontsize))

        angle_end = float(self.scale_angle_start_value + self.scale_angle_size - 360)
        angle = (angle_end - self.scale_angle_start_value) / 2 + self.scale_angle_start_value

        x = text_radius * math.cos(math.radians(angle))
        y = text_radius * math.sin(math.radians(angle))
        text = [x - int(w/2), y - int(h/2), int(w), int(h), Qt.AlignCenter, text]
        painter.drawText(text[0], text[1], text[2], text[3], text[4], text[5])

    def draw_center_point(self, diameter=30):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        painter.translate(self.width() / 2, self.height() / 2)
        painter.setPen(Qt.NoPen)
        painter.setBrush(self.center_color)
        painter.drawEllipse(int(-diameter / 2), int(-diameter / 2), int(diameter), int(diameter))

    def draw_needle(self):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.translate(self.width() / 2, self.height() / 2)
        painter.setPen(Qt.NoPen)
        painter.setBrush(self.needle_color)
        painter.rotate(((self.value - self.min_value) * self.scale_angle_size /
                        (self.max_value - self.min_value)) + 90 + self.scale_angle_start_value)

        painter.drawConvexPolygon(self.needle[0])

    def resizeEvent(self, event):
        self.set_scale_method()

    def paintEvent(self, event):
        # colored pie area
        self.draw_polygon()

        # draw scale marker lines
        self.draw_small_markers()
        self.draw_large_markers()

        # draw scale marker value text
        self.draw_markers_values_text()

        # Display Value
        self.draw_values_text()

        # draw needle
        self.draw_needle()

        # Draw Center Point
        self.draw_center_point(diameter=(self.wdgt_width / 6))

class AnalogGaugeWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, min_value, max_value, left_label=None, p_id=0, parent=None):#TODO! use plotParams
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, left_label)

        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.layout().itemAt(i).widget().setParent(None)

        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        self.pw = AnalogGaugeInnerWidget(controller, comp_name, comp_display_name, min_value, max_value, left_label, p_id, parent)
        self.pw.setMinimumSize(200,200)
        self.layout().addWidget(self.pw)
        pw_label = QLabel(comp_display_name)
        pw_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        pw_label_style = """
            border: transparent;
            font: 700 10pt "Segoe UI";
            color: rgb(220, 220, 220);
        """
        pw_label.setStyleSheet(pw_label_style)
        self.layout().addWidget(pw_label)

    def add_data(self, data):
        self.pw.add_data(data)
import math
import random


from PySide6.QtWidgets import QWidget, QCheckBox
from PySide6.QtGui import QPolygon, QPolygonF, QColor, QPen, QFont, QPainter, QFontMetrics, QConicalGradient
from PySide6.QtCore import Qt, QTimer, QPoint, QPointF, QRect, QSize, QObject, Signal, Slot

from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget

class CheckBoxListWidget(PlotWidget):

    valueChanged = Signal(int)

    def __init__(self, controller, comp_name, comp_display_name, labels_list, left_label=None, p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, left_label)

        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.layout().itemAt(i).widget().setParent(None)
        
        self.checkBoxList = []
        self.l_data = 0
        self.prev_data = 0
        self.labels_list = labels_list


        for a in range(len(self.labels_list)):
            self.checkBoxList.append(QCheckBox(str(self.labels_list[a])))
            self.checkBoxList[a].setEnabled(False)
            self.layout().addWidget(self.checkBoxList[a])        
        # self.setMinimumSize(200,200)
    
    @Slot(bool, int)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1:
            print("Component {} is logging via USB: {}".format(self.comp_name,status))
            if status:
                self.buffering_timer_counter = 0
                self.timer.start(self.timer_interval_ms)
            else:
                self.timer.stop()
        else: # interface == 0
            print("Component {} is logging on SD Card: {}".format(self.comp_name,status))
    
    def update_plot(self):
        if self.l_data != 0:
            if self.l_data != self.prev_data:
                data = self.l_data -1
                self.checkBoxList[int(data)].setChecked(True)
        else:
            if self.l_data != self.prev_data:
                for idx in range(len(self.labels_list)):
                    self.checkBoxList[int(idx)].setChecked(False)
        self.prev_data = self.l_data
        self.update()
        # pass #TODO
    
    def add_data(self, data):
        self.l_data = data[0]

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

from PySide6.QtWidgets import QCheckBox, QVBoxLayout
from PySide6.QtCore import Signal, Slot

from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget

class CheckBoxListWidget(PlotWidget):

    valueChanged = Signal(int)

    def __init__(self, controller, comp_name, comp_display_name, labels_list, left_label=None, p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, left_label)

        # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
        for i in reversed(range(self.layout().count())): 
            self.layout().itemAt(i).widget().setParent(None)
        
        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        self.layout().setSpacing(3)

        self.checkBoxList = []
        self.l_data = 0
        self.prev_data = -1
        self.labels_list = labels_list

        main_layout = QVBoxLayout()
        for a in range(len(self.labels_list)):
            self.checkBoxList.append(QCheckBox(str(self.labels_list[a])))
            self.checkBoxList[a].setEnabled(False)
            self.layout().addWidget(self.checkBoxList[a])        
    
    @Slot(bool, int)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1 or interface == 3:
            if_str = "USB" if interface == 1 else "Serial"
            print(f"Sensor {self.comp_name} is logging via {if_str}: {status}")
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
                data = self.l_data
                self.checkBoxList[int(data)].setChecked(True)
                self.checkBoxList[0].setChecked(False)
        else:
            if self.l_data != self.prev_data:
                for idx in range(len(self.labels_list)):
                    self.checkBoxList[int(idx)].setChecked(False)
                self.checkBoxList[0].setChecked(True)
        self.prev_data = self.l_data
        self.update()
    
    def add_data(self, data):
        self.l_data = data[0]
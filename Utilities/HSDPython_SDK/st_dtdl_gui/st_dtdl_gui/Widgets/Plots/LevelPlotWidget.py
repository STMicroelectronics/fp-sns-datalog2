 
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


from PySide6.QtWidgets import QLabel
from PySide6.QtCore import QSize

from st_dtdl_gui.Widgets.Plots.PlotBarWidget import PlotBarWidget

class LevelPlotWidget(PlotBarWidget):
    def __init__(self, controller, comp_name, comp_display_name, y0, y1, bar_width=1, unit="", p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, 1, y0, y1, bar_width, unit, p_id, parent)

        label = QLabel(unit)
        width = label.fontMetrics().boundingRect(label.text()).height() #height (90 deg rotation)
        self.graph_widget.getAxis("left").setWidth(width + 20)
        self.graph_widget.setMouseEnabled(x=False, y=False)
        self.graph_widget.hideButtons()
        self.title_frame.setVisible(False)
        self.graph_widget.setMinimumSize(QSize(120, 180))
        self.legend.clear()

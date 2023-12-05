import pyqtgraph as pg
from PySide6.QtGui import QColor, QBrush
from st_dtdl_gui.Utils.DataClass import SensorLightPlotParams
from st_dtdl_gui.Widgets.Plots.PlotLinesWavWidget import PlotLinesWavWidget
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from st_hsdatalog.HSD_GUI.Widgets.HSDPlotLinesWidget import HSDPlotLinesWidget


class HSDPlotALSWidget(PlotLinesWavWidget):    
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, plot_params, p_id, parent)
        
        self.lines_params = {0:{"color":"#FF0000", "label":"Red"},
                             1:{"color":"#666666", "label":"Visible"},
                             2:{"color":"#0000FF", "label":"Blue"},
                             3:{"color":"#00FF00", "label":"Green"},
                             4:{"color":"#FF00FF", "label":"IR"},
                             5:{"color":"#FFFFFF", "label":"Clear"}}
        
        brush = QBrush(QColor(255, 255, 255, 15))
        self.legend.setBrush(brush)

        for gc_id in self.graph_curves:
            self.graph_curves[gc_id].setPen(({'color': self.lines_params[gc_id]["color"], 'width': 1}))
            self.legend.addItem(self.graph_curves[gc_id], self.lines_params[gc_id]["label"])
        
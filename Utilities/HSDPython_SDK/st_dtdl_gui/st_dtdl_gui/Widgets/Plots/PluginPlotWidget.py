from PySide6.QtWidgets import QLabel, QFrame
from PySide6.QtGui import QPixmap
from st_dtdl_gui.STDTDL_Controller import STDTDL_Controller
from st_dtdl_gui.UI.styles import STDTDL_PushButton
from st_dtdl_gui.Utils.DataClass import LinesPlotParams, PlotLabelParams, PlotHeatMapParams
from st_dtdl_gui.Widgets.Plots.LabelPlotWidget import LabelPlotWidget
from st_dtdl_gui.Widgets.Plots.PlotLinesWidget import PlotLinesWidget
from st_dtdl_gui.Widgets.Plots.PlotHeatmapWidget import PlotHeatmapWidget
from enum import Enum

import st_dtdl_gui.UI.icons 
from pkg_resources import resource_filename
plugin_img_path = resource_filename('st_dtdl_gui.UI.icons', 'power_18dp_E8EAED.svg')

class PluginPlotType(Enum):
    """
    Enumeration for different types of plots.
    @var LINE: Represents a line plot.
    """
    LINE = "Line"
    LABEL = "Label"
    HEATMAP = "Heatmap"

class PluginPlotWidget:
    
    class LinesWidget(PlotLinesWidget):
        
        def __init__(self, plot_name, dimension, unit="", p_id=0, parent=None):
            self.controller = STDTDL_Controller()
            plot_params = LinesPlotParams(plot_name, True, dimension, unit)
            super().__init__(self.controller, plot_name, plot_name, plot_params, p_id, parent)
            PluginPlotWidget._add_plugin_icon(self.title_frame)
    class LabelWidget(LabelPlotWidget):
        
        def __init__(self, plot_name, dimension, unit="", p_id=0, parent=None):
            self.controller = STDTDL_Controller()
            plot_params = PlotLabelParams(plot_name, True, dimension, unit)
            super().__init__(self.controller, plot_name, plot_name, plot_params, p_id, parent)
            PluginPlotWidget._add_plugin_icon(self.title_frame)

    class HeatmapWidget(PlotHeatmapWidget):
        
        def __init__(self, plot_name, heatmap_shape, unit="", p_id=0, parent=None):
            self.controller = STDTDL_Controller()
            super().__init__(self.controller, plot_name, plot_name, heatmap_shape, unit, p_id, parent)
            PluginPlotWidget._add_plugin_icon(self.title_frame)

    @staticmethod
    def _add_plugin_icon(title_frame:QFrame):
        pixmap = QPixmap(plugin_img_path)
        icon_label = QLabel()
        icon_label.setPixmap(pixmap)
        style_title = STDTDL_PushButton.valid + "\n QFrame { background-color: rgb(45, 87, 87);}"
        title_frame.setStyleSheet(style_title)
        title_frame.layout().addWidget(icon_label)

    @staticmethod
    def create_plot(plot_name, plot_type: PluginPlotType, dimension, unit=""):
        """
        This method creates a plot widget based on the provided plot parameters. The plot type must be an instance of PluginPlotType.
        Currently, the supported plot types are PluginPlotType.LINE and PluginPlotType.LABEL.

        @param plot_name: The name of the plot.
        @type plot_name: str

        @param plot_type: The type of the plot. Must be an instance of PluginPlotType.
        @type plot_type: PluginPlotType

        @param dimension: The dimensions of the plot (e.g., number of lines)
        @type dimension: Any (Specify the exact type if known)

        @param unit: The unit of measurement for the plot dimensions. Default is an empty string.
        @type unit: str, optional

        @return: A plot widget instance if the plot type is valid, otherwise None.
        @rtype: PlotLinesWidget or None

        @note: 
        - For line plots, the input type is expected to be `LinesPlotParams`.
        - If an invalid plot type is provided, the method will print an error message and return None.
        """
        plot_widget = None

        if plot_type == PluginPlotType.LINE:
            plot_widget = PluginPlotWidget.LinesWidget(plot_name, dimension, unit)
        elif plot_type == PluginPlotType.LABEL:
            plot_widget = PluginPlotWidget.LabelWidget(plot_name, dimension, unit)
        elif plot_type == PluginPlotType.HEATMAP:
            heatmap_shape = (dimension, dimension)
            plot_widget = PluginPlotWidget.HeatmapWidget(plot_name, heatmap_shape, unit)
        else:
            print("Invalid plot parameters")
        
        return plot_widget



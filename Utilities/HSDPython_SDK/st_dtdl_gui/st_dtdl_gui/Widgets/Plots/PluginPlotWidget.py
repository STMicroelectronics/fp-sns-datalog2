from st_dtdl_gui.STDTDL_Controller import STDTDL_Controller
from st_dtdl_gui.Utils.DataClass import LinesPlotParams, SensorAudioPlotParams, SensorMemsPlotParams
from st_dtdl_gui.Widgets.Plots.PlotLinesWidget import PlotLinesWidget


# class PluginPlotWidget(PlotWidget):
#     def __init__(self, comp_name, comp_display_name, plot_params, p_id = 0, parent=None):
#         super().__init__(None, comp_name, comp_display_name, p_id, parent, plot_params.unit)

class PluginPlotWidget:
    
    # def __init__(self, comp_name, comp_display_name, plot_params, p_id=0, parent=None):
    #     controller = STDTDL_Controller()
    #     super().__init__(controller, comp_name, comp_display_name, plot_params, p_id, parent)

    def create_plot(self, comp_name, comp_display_name, plot_params, p_id=0, parent=None):
        """
        Create a plot widget based on the plot parameters.
        lines: input type: .....
        scatter: input type: .....
        """
        controller = STDTDL_Controller()

        if isinstance(plot_params, LinesPlotParams) or isinstance(plot_params, SensorMemsPlotParams) or isinstance(plot_params, SensorAudioPlotParams):
            return PlotLinesWidget(controller, comp_name, comp_display_name, plot_params, p_id, parent)
        else:
            print("Invalid plot parameters")
            return None



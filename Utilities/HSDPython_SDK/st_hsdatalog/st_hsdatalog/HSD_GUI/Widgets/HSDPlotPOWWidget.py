from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from st_hsdatalog.HSD_GUI.Widgets.HSDPlotLinesWidget import HSDPlotLinesWidget

class HSDPlotPOWWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id = 0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, "")
        
        self.graph_curves = dict()
        self.one_t_interval_resampled = dict()

        self.graph_widget.deleteLater()

        self.plots_params = plot_params

        self.graph_widgets = {}
        
        for i, p in enumerate(plot_params.plots_params_dict):
            unit = self.plots_params.plots_params_dict[p].unit
            self.plots_params.plots_params_dict[p].unit = f"{p} [{unit}]"
            pw = HSDPlotLinesWidget(self.controller, self.plots_params.plots_params_dict[p].comp_name, p, plot_params.plots_params_dict[p], i, self)
            self.graph_widgets[p] = pw

            # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
            for i in reversed(range(pw.layout().count())): 
                pw.layout().itemAt(i).widget().setParent(None)

            self.contents_frame.layout().addWidget(self.graph_widgets[p].graph_widget)
            self.contents_frame.layout().setSpacing(6)

        self.update_plot_characteristics(plot_params)

    def update_plot_characteristics(self, plot_params):
        self.plots_params = plot_params
        for p in plot_params.plots_params_dict:
            p_enabled = plot_params.plots_params_dict[p].enabled
            self.graph_widgets[p].graph_widget.setVisible(p_enabled)
        
        if self.app_qt is not None:
            self.app_qt.processEvents()
    
    def update_plot(self):
        super().update_plot()

    def add_data(self, data):
        self.graph_widgets["Voltage"].add_data([data[0]])
        self.graph_widgets["Voltage(VShunt)"].add_data([data[1]])
        self.graph_widgets["Current"].add_data([data[2]])
        self.graph_widgets["Power"].add_data([data[3]])
from st_dtdl_gui.Utils.DataClass import SensorPresenscePlotParams
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from st_hsdatalog.HSD_GUI.Widgets.HSDPlotLinesTMOSWidget import HSDPlotLinesTMOSWidget

class HSDPlotTMOSWidget(PlotWidget):
    def __init__(self, controller, comp_name, comp_display_name, plot_params, p_id = 0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, "")
        
        self.graph_curves = dict()
        self.one_t_interval_resampled = dict()

        self.graph_widget.deleteLater()

        self.plots_params = plot_params

        self.graph_widgets = {}
        
        for i, p in enumerate(plot_params.plots_params_dict):
            unit = self.plots_params.plots_params_dict[p].unit
            self.plots_params.plots_params_dict[p].unit = "{}".format(p,unit)
            pw = HSDPlotLinesTMOSWidget(self.controller, self.plots_params.plots_params_dict[p].comp_name, p, plot_params.plots_params_dict[p], i, self)
            self.graph_widgets[p] = pw

            # Clear PlotWidget inherited graphic elements (mantaining all attributes, functions and signals)
            for i in reversed(range(pw.layout().count())): 
                pw.layout().itemAt(i).widget().setParent(None)

            # lri = pg.LinearRegionItem([interval[0], interval[1]], brush=pg.mkBrush(color=colors[i], alpha=0.4))
            # lri.setMovable(False)
            # lri.setAcceptedMouseButtons(pg.QtCore.Qt.NoButton)
            # self.plot_widget.addItem(lri)

            self.contents_frame.layout().addWidget(self.graph_widgets[p].graph_widget)
            self.contents_frame.layout().setSpacing(6)

        self.update_plot_characteristics(plot_params)

    def update_plot_characteristics(self, plot_params: SensorPresenscePlotParams):
        self.plots_params = plot_params
        for p in plot_params.plots_params_dict:
            p_enabled = plot_params.plots_params_dict[p].enabled
            self.graph_widgets[p].graph_widget.setVisible(p_enabled)

        self.app_qt.processEvents()
        
    # @Slot(bool)
    def s_is_logging(self, status: bool, interface: int):
        if interface == 1 or interface == 3:
            print("Component {} is logging via USB: {}".format(self.comp_name,status))
            if status:
                #Get number of enabled fast telemetries
                self.ft_enabled_list = [ ft for ft in self.plots_params.plots_params_dict if self.plots_params.plots_params_dict[ft].enabled]
                self.update_plot_characteristics(self.plots_params)
            else:
                self.ft_enabled_list = []
    
    def update_plot(self):
        super().update_plot()

    def add_data(self, data):
        amb = [data[0]] # Tambient (raw)
        self.graph_widgets["Ambient"].add_data(amb)
        obj = [data[1], data[2], data[7], data[8]] # Tobject (raw) | Tobject (emb_comp) | Tobject (sw_comp) | Tobject_change (sw_comp)
        self.graph_widgets["Object"].add_data(obj)
        pres = [data[3], data[4], data[10]] # Tpresence | Presence flag | Presence flag (sw_comp)
        self.graph_widgets["Presence"].add_data(pres)
        mot = [data[5], data[6], data[9]] # Tmotion | Motion flag | Motion flag (sw_comp)
        self.graph_widgets["Motion"].add_data(mot)
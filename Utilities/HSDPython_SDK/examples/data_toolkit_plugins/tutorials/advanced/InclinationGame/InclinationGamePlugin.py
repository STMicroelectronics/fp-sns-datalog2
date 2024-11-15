from collections import deque
import pyqtgraph as pg
from PySide6.QtCore import QSize
from st_dtdl_gui.Widgets.Plots.PlotWidget import PlotWidget
from st_hsdatalog.HSD_datatoolkit.HSD_DataToolkit_Pipeline import HSD_Plugin
from st_dtdl_gui.STDTDL_Controller import STDTDL_Controller
from PySide6.QtWidgets import QGraphicsRectItem

import numpy as np

class PlotScatterWidget(PlotWidget):
    def __init__(self, comp_name, comp_display_name, y0, y1, unit="", p_id=0, parent=None):
        """
        Initialize the PlotScatterWidget.

        :param comp_name: The name of the component.
        :param comp_display_name: The display name of the component.
        :param y0: Minimum value for the y-axis.
        :param y1: Maximum value for the y-axis.
        :param unit: Unit of measurement for the y-axis values (default is an empty string).
        :param p_id: Plot ID (default is 0).
        :param parent: Parent widget (default is None).
        """
        controller = STDTDL_Controller()
        super().__init__(controller, comp_name, comp_display_name, p_id, parent, unit)
        
        self.y0 = y0
        self.y1 = y1
        self.thr = 0.8
        
        self._data = dict()  # dict of queues
        self._data[0] = deque(maxlen=200000)
        
        self.scatter = pg.ScatterPlotItem(x= [0], y=[0], pen=pg.mkPen(None), brush=pg.mkBrush('#3cb4e6'), size=20)
        
        self.graph_widget.setYRange(self.y0, self.y1, padding=0)
        self.graph_widget.setXRange(self.y0, self.y1, padding=0)

        # Add a rectangular figure
        self.rect = QGraphicsRectItem(-self.thr,  -self.thr,  self.thr*2,  self.thr*2)  # x, y, width, height
        self.rect.setPen(pg.mkPen(color='#a4c238', width=6))  # Set the pen for the rectangle
        self.graph_widget.addItem(self.rect)
        
        # add item to plot window
        self.graph_widget.addItem(self.scatter)
        
        self.graph_widget.getPlotItem().layout.setContentsMargins(10, 3, 3, 3)
        self.graph_widget.getPlotItem().setMenuEnabled(False)  # Disable right click menu in plots
        self.graph_widget.setMinimumSize(QSize(300, 150))
        
        self.timer_interval_ms = self.timer_interval * 700
    
    def update_plot(self):
        """
        Update the scatter plot with the latest data.

        This method updates the scatter plot with the latest data point from the data queue.
        It also updates the color of the rectangular figure based on the threshold values.
        """
        if len(self._data[0]) > 1:
            data = self._data[0].pop()
            x = data[0]
            y = data[1]
            if np.abs(x[0]) >=  self.thr or np.abs(y[0]) >= self.thr:
                self.rect.setPen(pg.mkPen(color='r', width=8))
            else:
                self.rect.setPen(pg.mkPen(color='#a4c238', width=6))

            self.scatter.setData(x, y)
        self.app_qt.processEvents()

    def add_data(self, data):
        """
        Add data to the data queue.

        :param data: The data to be added to the queue.
        """
        self._data[0].append(data)

class PluginClass(HSD_Plugin):
    """
    This class represents a plugin for a iis3dwb_acc game.
    The game cosists of a point moving in a 2D space within a rectangular area. If the point
    reaches the boundaries of the rectangular area, the color of the rectangular border changes to red.

    Attributes:
        components_status (dict): A dictionary containing the status of the components.

    Methods:
        __init__(self): Initializes the PluginClass object.
        start_log_cb(self): Callback method called when logging starts.
        stop_log_cb(self): Callback method called when logging stops.
        tag_cb(self, status, label): Callback method called when a tag is received.
        process(self, data): Processes the input data and returns the processed data.
        create_plot_widget(self): Creates a plot widget for the plugin.
    """

    def __init__(self):
        super().__init__()
    
    def start_log_cb(self):
        print("PLUGIN2 start_log_cb method called")

    def stop_log_cb(self):
        print("PLUGIN2 stop_log_cb method called")

    def tag_cb(self, status, label):
        print("PLUGIN2 tag_cb method called: tag label: ", label, " status: ", status)

    def process(self, data):
        """
        Process the input data and add the mean values of x and y data to the plot widget.

        Args:
            data (dict): A dictionary containing the input data.

        Returns:
            dict: The processed data dictionary.

        """
        #print("PLUGIN2 process method called")

        if data.comp_name == 'iis3dwb_acc':
            # Extract x and y data from the input data
            acc_data = data.data
            x_data = acc_data[0]
            y_data = acc_data[1]

            x_filtered_mean = np.mean(x_data)
            y_filtered_mean = np.mean(y_data)

            self.plot_widget.add_data([[x_filtered_mean], [y_filtered_mean]])

        return data

    def create_plot_widget(self):
        """
        Creates a plot widget for the plugin.

        Returns:
            PlotScatterWidget: The created plot widget.
        """
        print("PLUGIN2 create_plot_widget method called")
        self.plot_widget = PlotScatterWidget("Plugin2", "Plugin2", -1, 1, "",  p_id=0, parent=None)
        return self.plot_widget
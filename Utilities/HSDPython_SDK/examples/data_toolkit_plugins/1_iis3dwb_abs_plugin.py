from st_dtdl_gui.Utils.DataClass import SensorPlotParams
from st_dtdl_gui.Widgets.Plots.PluginPlotWidget import PluginPlotWidget
from st_hsdatalog.HSD_GUI.HSD_DataToolkit_Pipeline import HSD_Plugin
import numpy as np

class PluginClass(HSD_Plugin):
    """
    A class representing a plugin for processing data from the iis3dwb_acc component.


    Attributes:
        plot_widget (object): The plot widget used for displaying data.
        comp_name (str): The name of the component.
        dim (int): The dimension of the data.

    Methods:
        start_log_cb(): Callback method called when logging starts.
        stop_log_cb(): Callback method called when logging stops.
        tag_cb(status, label): Callback method called when a tag is received.
        __create_sliced_dict(array, num_slices, slice_size): Slices the given array into multiple arrays.
        process(data): Processes the data and returns the modified data.
        create_plot_widget(): Creates and returns the plot widget.
    """

    def __init__(self, components_status):
        super().__init__(components_status)
        self.plot_widget = None
        self.comp_name = None
        self.dim = 1

    def start_log_cb(self):
        print("PLUGIN1 start_log_cb method called")

    def stop_log_cb(self):
        print("PLUGIN1 stop_log_cb method called")

    def tag_cb(self, status, label):
        print("PLUGIN1 tag_cb method called: tag label: ", label, " status: ", status)

    def __create_sliced_dict(self, array, num_slices, slice_size):
        """
        Slices the given array into 'num_slices' arrays, each with 'slice_size' elements,
        picked every 'num_slices' elements along the first dimension.

        :param array: NumPy array to be sliced.
        :param num_slices: Number of slices to create.
        :param slice_size: Number of elements in each slice.
        :return: Dictionary of sliced arrays.
        """
        # Calculate the step size based on the number of slices
        step = num_slices
        
        # Calculate the maximum index to slice to ensure each slice has 'slice_size' elements
        max_index = slice_size * step
        
        # Create the dictionary of slices using a dictionary comprehension
        slices_dict = {
            i: array[i:max_index:step]
            for i in range(num_slices)
        }
        
        return slices_dict

    def process(self, data):
        """
        Process the input data and computes abs value of iis3dwb_acc accelerometer signal.

        Args:
            data (dict): A dictionary containing the input data.

        Returns:
            dict: A dictionary containing the processed data.
        """
        #print("PLUGIN1 process method called")
        data_comp_name = list(data.keys())[0]
        if data_comp_name == 'iis3dwb_acc':
            data_buffer = data[data_comp_name]["data"]
            acc_data = self.__create_sliced_dict(data_buffer, 3, int(len(data_buffer/self.dim)))
            acc_data_norm = np.abs(acc_data[0]) + np.abs(acc_data[1]) + np.abs(acc_data[2])
            self.plot_widget.add_data([acc_data_norm])
            data[data_comp_name]["data"] = acc_data
        return data

    def create_plot_widget(self):
        """
        Creates a plot widget for the Plugin1.

        Returns:
            The created plot widget.
        """
        print("PLUGIN1 create_plot_widget method called")
        plot_params = SensorPlotParams("Plugin1", True, self.dim, unit="", time_window=30)
        plot_widget_factory = PluginPlotWidget()
        self.plot_widget = plot_widget_factory.create_plot("Plugin1", "Plugin1", plot_params, p_id=0, parent=None)
        return self.plot_widget
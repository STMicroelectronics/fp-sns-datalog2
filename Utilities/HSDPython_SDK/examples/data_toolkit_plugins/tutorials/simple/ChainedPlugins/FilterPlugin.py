from st_hsdatalog.HSD_datatoolkit.HSD_DataToolkit_Pipeline import HSD_Plugin
import numpy as np

class PluginClass(HSD_Plugin):

    def __init__(self):
        super().__init__()
        print("FilterPlugin has been initialized!")
    
    def process(self, data):
        if data.comp_name == "iis3dwb_acc":
            # Extract the x, y, and z components of the accelerometer data
            acc_x = data.data[0::3]
            acc_y = data.data[1::3]
            acc_z = data.data[2::3]
            # Calculate the norm of the accelerometer data
            acc_data_norm = np.sqrt(acc_x**2 + acc_y**2 + acc_z**2)
            # Update the data with the calculated norm
            data.data = acc_data_norm
        return data

    def create_plot_widget(self):
        print("FilterPlugin create_plot_widget method called")
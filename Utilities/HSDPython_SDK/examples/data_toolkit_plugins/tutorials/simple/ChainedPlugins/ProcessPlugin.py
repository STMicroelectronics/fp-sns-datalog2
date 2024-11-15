from st_hsdatalog.HSD_datatoolkit.HSD_DataToolkit_Pipeline import HSD_Plugin
import numpy as np 

class PluginClass(HSD_Plugin):

    def __init__(self):
        super().__init__()
        self.control_thr = 1.7
        print("ProcessPlugin has been initialized!")
    
    def process(self, data):
        if data.comp_name == "iis3dwb_acc":
            if np.any(data.data >= self.control_thr):
                print("Warning data above threshold !!")
        return data

    def create_plot_widget(self):
        print("ProcessPlugin create_plot_widget method called")
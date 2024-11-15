from st_hsdatalog.HSD_datatoolkit.HSD_DataToolkit_Pipeline import HSD_Plugin

class PluginClass(HSD_Plugin):

    def __init__(self):
        super().__init__()
        print("HelloWorldPlugin has been initialized!")
    
    def process(self, data):
        print("HelloWorldPlugin process method called")
        print(f"---> Received data from: {data.comp_name}")
        print(f"-----> Data: {data.data}")
        print(f"-----> Timestamp: {data.timestamp}")
        return data

    def create_plot_widget(self):
        print("HelloWorldPlugin create_plot_widget method called")
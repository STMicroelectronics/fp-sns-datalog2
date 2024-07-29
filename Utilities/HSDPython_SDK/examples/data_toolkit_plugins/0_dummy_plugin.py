from st_hsdatalog.HSD_GUI.HSD_DataToolkit_Pipeline import HSD_Plugin

class PluginClass(HSD_Plugin):
    """
    This class represents a Dummy Plugin for the HSDPython SDK Data Toolkit.

    Attributes:
        components_status (dict): A dictionary containing the status of components.

    Methods:
        __init__(self, components_status): Initializes the PluginClass object.
        start_log_cb(self): Callback method called when starting logging.
        stop_log_cb(self): Callback method called when stopping logging.
        tag_cb(self, status, label): Callback method called when a tag is received.
        process(self, data): Process the received data.
        create_plot_widget(self): Creates a plot widget.
    """

    def __init__(self, components_status):
        super().__init__(components_status)
        print(self.get_components_status())

    def start_log_cb(self):
        print("PLUGIN0 start_log_cb method called")

    def stop_log_cb(self):
        print("PLUGIN0 stop_log_cb method called")

    def tag_cb(self, status, label):
        print("PLUGIN0 tag_cb method called: tag label: ", label, " status: ", status)
    
    def process(self, data):
        # print("PLUGIN0 process method called")
        # print(f"--> Received: {data}")
        return data

    def create_plot_widget(self):
        print("PLUGIN0 create_plot_widget method called")
        
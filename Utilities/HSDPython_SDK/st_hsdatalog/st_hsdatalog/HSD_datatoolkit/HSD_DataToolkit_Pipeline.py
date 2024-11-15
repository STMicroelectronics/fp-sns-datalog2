import os
import sys
import re
import importlib
from abc import ABC, abstractmethod

class HSD_Plugin(ABC):
    def __init__(self):
        self.components_status = {}

    def get_components_status(self):
        return self.components_status
    
    def get_component_status(self, comp_name):
        return self.components_status[comp_name]

    def start_log_cb(self):
        pass

    def stop_log_cb(self):
        pass

    def tag_cb(self, status, label):
        pass

    @abstractmethod
    def process(self, data):
        pass

class HSD_DataToolkit_data:
    def __init__(self, comp_name, data, timestamp):
        self.comp_name = comp_name
        self.data = data
        self.timestamp = timestamp

class HSD_DataToolkit_Pipeline:
    # def __init__(self, plugins = [], device_status = {}):
    def __init__(self, controller):

        self.plugin_modules_names = []
        self.plugins = []
        self.controller = controller
        
        self.components_status = {}     
        plugins_path = self.controller.get_dt_plugin_folder_path()
        if plugins_path is not None:
            try:
                # List all .py files in the specified data toolkit plugins directory
                files = os.listdir(plugins_path)
                # Filter out directories and non-.py files, keeping only .py files
                py_files = [f for f in files if os.path.isfile(os.path.join(plugins_path, f)) and f.endswith('.py') and f != "__init__.py"]
                # Remove the .py extension
                self.plugin_modules_names = [os.path.splitext(f)[0] for f in py_files]                
            except Exception as e:
                print(f"An error occurred: {e}")
                return None
        else:
            return None
        
        # Add the provided path to sys.path
        sys.path.insert(0, plugins_path)

        # Clear all existing plugin plot widgets
        self.controller.clear_all_plugin_plot_widgets()

        print("len(self.plugin_modules):",len(self.plugin_modules_names))

        for plugin_name in self.plugin_modules_names:
            plugin_instance = self.validate_plugin(plugin_name)
            if plugin_instance is None:
                continue

            # Call the plugin's graphics method
            widget = plugin_instance.create_plot_widget()
            
            self.plugins.append(plugin_instance)
            
            # If the plugin returns a widget, add it to the main layout
            if widget is not None:
                widget.app_qt = self.controller.qt_app
                widget.controller = self.controller
                widget.parent = self.controller.plots_layout
                self.controller.add_plugin_plot_widget(widget)

    # def add_plugin(self, plugin):
    #     self.plugins.append(plugin)

    # def remove_plugin(self, plugin):
    #     self.plugins.remove(plugin)

    @staticmethod
    def validate_plugin(plugin_name):
        # Import the module using its name
        plugin_module = importlib.import_module(plugin_name)
        try:
            PluginClass = getattr(plugin_module, "PluginClass")
            plugin_instance = PluginClass()
            return plugin_instance
        except AttributeError as e:
            match = re.search(r"module '([^']*)'", e.args[0])
            if match:
                print(f"{match.group(1)}.py module is not a valid plugin.")
            else:
                print(f"{e}")
            return None
    
    def start(self):
        for plugin in self.plugins:
            plugin.start_log_cb()
            if hasattr(plugin, 'plot_widget') and plugin.plot_widget is not None:
                if hasattr(plugin.plot_widget, 'timer'):
                    plugin.plot_widget.timer.start(200)
                else:
                    print("Plugin Plot Widget has no timer attribute")
    
    def stop(self):
        for plugin in self.plugins:
            plugin.stop_log_cb()
            if hasattr(plugin, 'plot_widget') and plugin.plot_widget is not None:
                if hasattr(plugin.plot_widget, 'timer'):
                    plugin.plot_widget.timer.stop()
                else:
                    print("Plugin Plot Widget has no timer attribute")

    def do_tag(self, status, label):
        for plugin in self.plugins:
            plugin.tag_cb(status, label)

    def update_components_status(self, components_status):
        self.components_status = components_status
        for plugin in self.plugins:
            plugin.components_status = components_status

    def process_data(self, data_obj):
        data = data_obj
        for plugin in self.plugins:
            data = plugin.process(data)
        return data
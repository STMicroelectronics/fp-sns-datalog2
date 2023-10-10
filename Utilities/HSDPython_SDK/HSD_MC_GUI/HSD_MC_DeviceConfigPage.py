# *****************************************************************************
#  * @file    DeviceConfigPage.py
#  * @author  SRA
# ******************************************************************************
# * @attention
# *
# * Copyright (c) 2022 STMicroelectronics.
# * All rights reserved.
# *
# * This software is licensed under terms that can be found in the LICENSE file
# * in the root directory of this software component.
# * If no LICENSE file comes with this software, it is provided AS-IS.
# *
# *
# ******************************************************************************
#

from PySide6.QtCore import Slot
from HSD_MC_GUI.Widgets.HSD_MC_FastTelemetriesPlotWidget import HSD_MC_FastTelemetriesPlotWidget
from HSD_MC_GUI.Widgets.HSD_MC_ControlWidget import HSD_MC_ControlWidget
from HSD_MC_GUI.Widgets.HSD_MC_SlowTelemetriesPlotWidget import HSD_MC_SlowTelemetriesPlotWidget

from st_hsdatalog.HSD_GUI.HSD_DeviceConfigPage import HSD_DeviceConfigPage

from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
from st_dtdl_gui.Widgets.Plots.PlotsArrayWidget import PlotsArrayWidget
from st_dtdl_gui.Utils.DataClass import LinesPlotParams, MCTelemetriesPlotParams, PlotLabelParams, PlotLevelParams, PlotGaugeParams, SensorMemsPlotParams
from HSD_MC_GUI.HSD_MC_LogControlWidget import HSD_MC_LogControlWidget


from st_dtdl_gui.STDTDL_Controller import ComponentType
import st_pnpl.DTDL.dtdl_utils as DTDLUtils

import st_hsdatalog.HSD_utils.logger as logger
log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

class HSD_MC_DeviceConfigPage(HSD_DeviceConfigPage):
    
    def __init__(self, page_widget, controller):
        super().__init__(page_widget, controller)
        self.slow_mc_telemetries = []
        self.fast_mc_telemetries = []
    
    @Slot(str, dict)
    def s_component_found(self, comp_name, comp_interface):
        super().s_component_found(comp_name, comp_interface)
        #Replace the Motor Controller ComponentWidget
        if comp_name == "motor_controller":
            self.controller.remove_component_config_widget(comp_name)
            self.motor_control_widget = HSD_MC_ControlWidget(self.controller, comp_contents=comp_interface.contents, parent=self.widget_header)
            self.controller.add_component_config_widget(self.motor_control_widget)
            self.add_header_widget(self.motor_control_widget)
            self.controller.fill_component_status(comp_name)
        if comp_name == "log_controller":
            self.controller.remove_component_config_widget(comp_name)
            c_status = self.controller.get_component_status(comp_name)
            if "controller_type" in c_status["log_controller"]:
                self.log_control_widget = HSD_MC_LogControlWidget(self.controller, comp_contents=comp_interface.contents, parent=self.widget_header)
                self.controller.set_rtc_time()
                self.controller.add_component_config_widget(self.log_control_widget)
                self.add_header_widget(self.log_control_widget)
                self.controller.fill_component_status(comp_name) 

    @Slot(str, dict)
    def s_actuator_component_found(self, comp_name, comp_interface):
        #create a ComponentWidget
        comp_display_name = comp_interface.display_name if isinstance(comp_interface.display_name, str) else comp_interface.display_name.en
        act_config_widget = ComponentWidget(self.controller, comp_name, comp_display_name, ComponentType.ACTUATOR, comp_interface.contents, self.comp_id, self.device_config_widget)
        self.comp_id += 1
        self.controller.add_component_config_widget(act_config_widget)
        self.device_config_widget.layout().addWidget(act_config_widget)
        self.controller.fill_component_status(comp_name)
        plots_params_dict = {}
        comp_status = self.controller.get_component_status(comp_name)#TODO 
        contents = comp_interface.contents
        description = None
        for c in contents:
            if c.description is not None:
                description = c.description if isinstance(c.description, str) else c.description.en
                display_name = c.display_name if isinstance(c.display_name, str) else c.display_name.en
                t_root_key = list(comp_status.keys())[0]
                if description == DTDLUtils.MC_SLOW_TELEMETRY_STRING:
                    if c.name in comp_status[t_root_key]:
                        tele_status = comp_status[t_root_key][c.name]
                        self.slow_mc_telemetries.append(c)
                        t_enabled = tele_status[DTDLUtils.ENABLED_STRING]
                        t_min = tele_status[DTDLUtils.MIN_STRING]
                        t_max = tele_status[DTDLUtils.MAX_STRING]
                        t_unit = tele_status[DTDLUtils.UNIT_STRING]
                        graph_type = tele_status[DTDLUtils.GRAPH_TYPE_STRING]
                        if graph_type == DTDLUtils.GraphTypeEnum.GAUGE.value:
                            plots_params_dict[display_name] = PlotGaugeParams(c.name, t_enabled, t_min, t_max, t_unit)
                        # elif graph_type == DTDLUtils.GraphTypeEnum.V_LEVEL.value:
                        #     plots_params_dict[display_name] = PlotLevelParams(c.name, t_enabled, t_min, t_max, t_unit) # level
                        elif graph_type == DTDLUtils.GraphTypeEnum.V_LEVEL.value: #TODO!!! V_LEVEL-->LABEL
                            plots_params_dict[display_name] = PlotLabelParams(c.name, t_enabled, t_min, t_max, t_unit) # label
                elif description == DTDLUtils.MC_FAST_TELEMETRY_STRING:
                    if c.name in comp_status[t_root_key]:
                        tele_status = comp_status[t_root_key][c.name]
                        self.fast_mc_telemetries.append(c)
                        t_enabled = tele_status[DTDLUtils.ENABLED_STRING]
                        t_unit = tele_status[DTDLUtils.UNIT_STRING]
                        graph_type = tele_status[DTDLUtils.GRAPH_TYPE_STRING]
                        if graph_type == DTDLUtils.GraphTypeEnum.LINE.value:
                            plots_params_dict[display_name] = LinesPlotParams(c.name, t_enabled, 1, t_unit)
        
        if description is not None:
            plot_params = MCTelemetriesPlotParams(comp_name, True, plots_params_dict)
            if description == DTDLUtils.MC_FAST_TELEMETRY_STRING:
                # sensor_plot_widget = PlotsArrayWidget(self.controller, comp_name, comp_display_name, plots_array, p_id=self.graph_id, parent=self.plots_widget, orientation="v")
                sensor_plot_widget = HSD_MC_FastTelemetriesPlotWidget(self.controller, comp_name, comp_display_name, plot_params, 30, p_id=self.graph_id, parent=self.plots_widget)
            elif description == DTDLUtils.MC_SLOW_TELEMETRY_STRING:
                sensor_plot_widget = HSD_MC_SlowTelemetriesPlotWidget(self.controller, comp_name, comp_display_name, plot_params, 30, p_id=self.graph_id, parent=self.plots_widget)
                # sensor_plot_widget = PlotsArrayWidget(self.controller, comp_name, comp_display_name, plots_params_dict, p_id=self.graph_id, parent=self.plots_widget)
        else:
            sensor_plot_widget = PlotsArrayWidget(self.controller, comp_name, comp_display_name, plots_params_dict, p_id=self.graph_id, parent=self.plots_widget)
        self.controller.add_plot_widget(sensor_plot_widget)
        self.plots_widget.layout().addWidget(sensor_plot_widget)
        sensor_plot_widget.setVisible(True)
        
        self.graph_id +=1
        # self.controller.fill_component_status(comp_name) 

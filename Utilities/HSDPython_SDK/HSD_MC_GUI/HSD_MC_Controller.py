# *****************************************************************************
#  * @file    Controller.py
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

import os
import time
from threading import Event

from enum import Enum

from PySide6.QtCore import Signal
from st_hsdatalog.HSD_GUI.HSD_Controller import HSD_Controller
import st_pnpl.DTDL.dtdl_utils as DTDLUtils

from st_pnpl.PnPLCmd import PnPLCMDManager

from st_hsdatalog.HSD_link.HSDLink_v1 import HSDLink_v1
from st_dtdl_gui.Utils.DataReader import DataReader
from st_dtdl_gui.STDTDL_Controller import ComponentType

import st_hsdatalog.HSD_utils.logger as logger
log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

class FastTelemetryStateEnum(Enum):
    MCP_FT_DISABLE = 0
    MCP_FT_ENABLE = 1
     

class HSD_MC_Controller(HSD_Controller):
    
    sig_is_motor_started = Signal(bool, int)
    
    def __init__(self, parent=None):
        super().__init__(parent)

        # @mcp_is_connected saves the connection state between 
        # mcp master and slave
        self.mcp_is_connected = False

        # @is_motor_started saves motor state
        self.is_motor_started = False

        self.mcp_fast_telemetries_state = FastTelemetryStateEnum.MCP_FT_DISABLE

        self.mc_comp_name = "motor_controller"
        self.mc_connect_cmd_name = "connect"
        self.mc_start_cmd_name = "start_motor"
        self.mc_stop_cmd_name = "stop_motor"
        self.mc_ack_fault_cmd_name = "ack_fault"
        self.mc_set_speed_cmd_name = "set_motor_speed"
        self.mc_speed_req_name = "speed"
        self.mc_enable_fast_telemetry_cmd_name = "enable_fast_telemetry"
        self.mc_disable_fast_telemetry_cmd_name = "disable_fast_telemetry"
    
    def start_log(self, interface=1):
        if type(self.hsd_link) == HSDLink_v1:
            res = self.hsd_link.start_log(self.device_id)
        else:
            res = self.hsd_link.start_log(self.device_id, interface)
        if res:
            self.sig_logging.emit(True,interface)
            self.is_logging = True

    def get_description_string(content):
        if content.description is not None:
            return content.description if isinstance(content.description, str) else content.description.en
        return None

    def stop_log(self, interface=1):
        if self.is_logging == True:
            self.hsd_link.stop_log(self.device_id)
            if type(self.hsd_link) == HSDLink_v1:
                self.hsd_link.save_json_device_file(self.device_id)
                self.hsd_link.save_json_acq_info_file(self.device_id)
            else:
                #TODO put here a "File saving..." loading window!
                self.hsd_link.save_json_acq_info_file(self.device_id)
                time.sleep(0.5)
                self.hsd_link.save_json_device_file(self.device_id)
                if self.ispu_output_format is not None:
                    self.save_json_ispu_output_format_file()
            self.sig_logging.emit(False, interface)
            self.is_logging = False

    def start_plots(self):        
        for s in self.plot_widgets:
            s_plot = self.plot_widgets[s]
            
            if type(self.hsd_link) == HSDLink_v1:
                    sensor_data_file_path = os.path.join(self.hsd_link.get_acquisition_folder(),(str(s_plot.comp_name) + ".dat"))
                    sensor_data_file = open(sensor_data_file_path, "wb+")
                    self.sensor_data_files.append(sensor_data_file)
                    stopFlag = Event()
                    self.threads_stop_flags.append(stopFlag)

                    dimensions = s_plot.n_curves
                    sample_size = s_plot.sample_size
                    spts = s_plot.spts
                    data_format = s_plot.data_format
                    
                    dr = DataReader(self, s_plot.comp_name, spts, dimensions, sample_size, data_format)
                    self.data_readers.append(dr)
                    
                    thread = self.SensorAcquisitionThread_test_v1(stopFlag, self.hsd_link, dr, self.device_id, s_plot.s_id, s_plot.ss_id, s_plot.comp_name, sensor_data_file)
                    thread.start()
                    self.sensors_threads.append(thread)
            else:
                c_status = self.hsd_link.get_component_status(self.device_id, s_plot.comp_name)
                c_status_value = c_status[s_plot.comp_name]
                
                c_enable = c_status_value.get("enable")
                
                if c_enable == True:
                    sensor_data_file_path = os.path.join(self.hsd_link.get_acquisition_folder(),(str(s_plot.comp_name) + ".dat"))
                    sensor_data_file = open(sensor_data_file_path, "wb+")
                    self.sensor_data_files.append(sensor_data_file)
                    stopFlag = Event()
                    self.threads_stop_flags.append(stopFlag)

                    if "sensitivity" in c_status_value:
                        sensitivity = c_status_value["sensitivity"]
                    else:
                        sensitivity = 1
                    
                    sample_size = self.sample_size_from_data_type(c_status_value["data_type"])
                    data_format = self.data_format_from_data_type(c_status_value["data_type"])
                    usb_dps = c_status_value["usb_dps"]

                    interleaved_data = True
                    if c_status_value["c_type"] == ComponentType.SENSOR.value:
                        spts = c_status_value.get("samples_per_ts")
                        spts = spts["val"] if spts and "val" in spts else spts
                        #TODO what if spts is None?
                        dimensions = c_status_value["dim"]
                        
                    elif c_status_value["c_type"] == ComponentType.ALGORITHM.value:
                        spts = 0
                        dimensions = c_status_value["dim"]
                        if "algorithm_type" in c_status_value:
                            algorithm_type = c_status_value["algorithm_type"]
                        if algorithm_type == 0 or algorithm_type == 1:
                            interleaved_data = False
                    
                    elif c_status_value["c_type"] == ComponentType.ACTUATOR.value:
                        spts = 1
                        dimensions = 0

                        if s_plot.comp_name == DTDLUtils.MC_SLOW_TELEMETRY_COMP_NAME:
                            slow_telemetries_contents = [stc for stc in self.components_dtdl[s_plot.comp_name].contents if HSD_MC_Controller.get_description_string(stc) == DTDLUtils.MC_SLOW_TELEMETRY_STRING]
                            for t in slow_telemetries_contents:
                                t_display_name = t.display_name if isinstance(t.display_name, str) else t.display_name.en
                                t_enabled = c_status_value[t.name].get("enabled")
                                self.plot_widgets[s_plot.comp_name].plots_params.plots_params_dict[t_display_name].enabled = t_enabled
                                if t_enabled:
                                    dimensions += 1
                        
                        elif s_plot.comp_name == DTDLUtils.MC_FAST_TELEMETRY_COMP_NAME:
                            fast_telemetries_contents = [ftc for ftc in self.components_dtdl[s_plot.comp_name].contents if HSD_MC_Controller.get_description_string(ftc) == DTDLUtils.MC_FAST_TELEMETRY_STRING]
                            for t in fast_telemetries_contents:
                                t_display_name = t.display_name if isinstance(t.display_name, str) else t.display_name.en
                                t_enabled = c_status_value[t.name].get("enabled")
                                self.plot_widgets[s_plot.comp_name].plots_params.plots_params_dict[t_display_name].enabled = t_enabled

                            dimensions = c_status_value["dim"]
                            interleaved_data = False

                    if "_ispu" in s_plot.comp_name:
                        data_format = "b"
                        dimensions = 64
                        sample_size = 1

                    dr = DataReader(self, s_plot.comp_name, spts, dimensions, sample_size, data_format, sensitivity, interleaved_data)
                    self.data_readers.append(dr)
                
                    thread = self.SensorAcquisitionThread(stopFlag, self.hsd_link, dr, self.device_id, s_plot.comp_name, sensor_data_file, usb_dps)
                    thread.start()
                    self.sensors_threads.append(thread)
    
    def disconnect_motor(self, motor_id = 0):
        pass
    
    def start_motor(self, motor_id = 0):
        #Connection between master and slave mcp is performed the first time 
        # start motor cmd is called
        if not self.mcp_is_connected:
            self.send_command(PnPLCMDManager.create_command_cmd(self.mc_comp_name, self.mc_connect_cmd_name))
            time.sleep(0.7)
            self.mcp_is_connected = True
            #TO DO CHECK CONNECTION RESULT
        
        if self.mcp_fast_telemetries_state == FastTelemetryStateEnum.MCP_FT_DISABLE:
            if self.get_enabled_fast_telemetries():
                self.send_command(PnPLCMDManager.create_command_cmd(self.mc_comp_name, self.mc_enable_fast_telemetry_cmd_name))
                self.mcp_fast_telemetries_state = FastTelemetryStateEnum.MCP_FT_ENABLE

        time.sleep(0.7) 

        # Send Start motor cmd
        self.send_command(PnPLCMDManager.create_command_cmd(self.mc_comp_name, self.mc_start_cmd_name))
        #Emit signal
        self.sig_is_motor_started.emit(True, motor_id)
    
    def stop_motor(self, motor_id = 0):
        #Send stop motor message
        self.send_command(PnPLCMDManager.create_command_cmd(self.mc_comp_name, self.mc_stop_cmd_name))

        time.sleep(0.7)

        if self.mcp_fast_telemetries_state == FastTelemetryStateEnum.MCP_FT_ENABLE:
                self.send_command(PnPLCMDManager.create_command_cmd(self.mc_comp_name, self.mc_disable_fast_telemetry_cmd_name))
                self.mcp_fast_telemetries_state = FastTelemetryStateEnum.MCP_FT_DISABLE

        self.sig_is_motor_started.emit(False, motor_id)
    
    def ack_fault(self, motor_id = 0):
        self.send_command(PnPLCMDManager.create_command_cmd(self.mc_comp_name, self.mc_ack_fault_cmd_name))
        
    def set_motor_speed(self, value, motor_id = 0):
        self.send_command(PnPLCMDManager.create_command_cmd(self.mc_comp_name, self.mc_set_speed_cmd_name, self.mc_speed_req_name, value))
    
    def get_enabled_fast_telemetries(self, motor_id = 0):
        fast_telemetry_enabled = False
            # Check for enabled fast telemetries       
        c_status = self.components_status["fast_mc_telemetries"]
        for c in c_status:
            if type(c_status[c]) is dict:
                if "enabled" in c_status[c] and c_status[c]["enabled"] == True:
                    fast_telemetry_enabled = True
                    break
        if fast_telemetry_enabled:
            return True



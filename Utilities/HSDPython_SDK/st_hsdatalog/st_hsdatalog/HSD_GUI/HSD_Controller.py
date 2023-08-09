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
import shutil
import struct
import time
import os
import json

from datetime import datetime
from threading import Thread, Event

from PySide6.QtCore import Signal, QThread, QObject
from PySide6.QtWidgets import QFileDialog

from st_pnpl.DTDL import device_template_model as DTM
import st_pnpl.DTDL.dtdl_utils as DTDLUtils

from st_hsdatalog.HSD.HSDatalog import HSDatalog
from st_hsdatalog.HSD_link.HSDLink import HSDLink
from st_hsdatalog.HSD_link.HSDLink_v1 import HSDLink_v1

from st_hsdatalog.HSD_GUI.Widgets.HSDPlotLinesWidget import HSDPlotLinesWidget
from st_dtdl_gui.Utils.DataClass import AnomalyDetectorModelPlotParams, LinesPlotParams, MCTelemetriesPlotParams, PlotCheckBoxParams, PlotGaugeParams, PlotLabelParams, PlotLevelParams, SensorAudioPlotParams, SensorMemsPlotParams, SensorPlotParams, FFTAlgPlotParams, ClassificationModelPlotParams, DataClass, SensorRangingPlotParams, UnitMap
from st_dtdl_gui.Utils.DataReader import DataReader
from st_dtdl_gui.STDTDL_Controller import ComponentType, STDTDL_Controller

from st_hsdatalog.HSD.utils.type_conversion import TypeConversion

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

log_file_name = None
for handler in log.parent.handlers:
    if hasattr(handler, "baseFilename"):
        log_file_name = os.path.basename(getattr(handler, 'baseFilename'))

class HSD_Controller(STDTDL_Controller):

    MAX_HSD_BANDWIDTH = 6000000
    # Signals
    sig_is_auto_started = Signal(bool)
    sig_tag_done = Signal(bool, str) #(on|off),tag_label
    sig_hsd_bandwidth_exceeded = Signal(bool)
    sig_streaming_error = Signal(bool, str)
    # TODO: Next version --> Hotplug events notification support
    # sig_usb_hotplug = Signal(bool)
    # TODO: Next version --> Hotplug events notification support

    class SensorAcquisitionThread(Thread):    
        def __init__(self, event, hsd_link, data_reader, d_id, comp_name, sensor_data_file, usb_dps, sig_streaming_error = None):            

            class EmptyDataTimer(QObject):
                timeout_signal = Signal()

                def __init__(self):
                    super().__init__()
                    self.interrupt_event = Event()
                    self.timeout = 3                    

                def run_wait(self):
                    self.interrupt_event = Event()
                    time.sleep(self.timeout)
                    if not self.interrupt_event.is_set():
                        self.timeout_signal.emit()
            
            Thread.__init__(self)
            self.name = comp_name
            self.stopped = event
            self.hsd_link = hsd_link
            self.data_reader = data_reader
            self.d_id = d_id
            self.comp_name = comp_name
            self.sensor_data_file = sensor_data_file
            self.sig_streaming_error = sig_streaming_error
            self.usb_dps = usb_dps
            self.over_proto = 0
            self.t0 = 0
            self.prev_cnt = 0

            self.objThread = QThread()
            self.obj = EmptyDataTimer()
            self.obj.moveToThread(self.objThread)
            self.obj.timeout_signal.connect(self.raise_empty_data_error)
            self.objThread.started.connect(self.obj.run_wait)
        
        def raise_empty_data_error(self):
            error_msg = "No data from {} Component.\nRestart the acquisition lowering component ODR to acquire data correctly.\nHave a look in {} log file for more detailed info.".format(self.comp_name, log_file_name if log_file_name is not None else "application")
            log.error(error_msg)
            self.sig_streaming_error.emit(True, error_msg)
        
        def run(self):
            while not self.stopped.wait(0.02):
            # while not self.stopped.wait(1):
                sensor_data = self.hsd_link.get_sensor_data(self.d_id, self.comp_name)
                if sensor_data is not None:
                    if self.objThread.isRunning():
                        self.obj.interrupt_event.set()
                    nof_usb_packet = len(sensor_data[1])/(self.usb_dps + 4)
                    for p in range(int(nof_usb_packet)):
                        curr_cnt = struct.unpack("=i",sensor_data[1][p*(self.usb_dps + 4): p*(self.usb_dps + 4)+4])[0]
                        diff = curr_cnt - self.prev_cnt
                        if curr_cnt != 0 and diff != self.usb_dps:
                            error_msg = "Streaming errors in {} component!\n{} USB packets ({} bytes) lost.\nHave a look in {} log file for more detailed info.".format(self.comp_name, int(diff//self.usb_dps), diff, log_file_name if log_file_name is not None else "application")
                            if self.sig_streaming_error is not None:
                                self.sig_streaming_error.emit(True, error_msg)
                            log.error(error_msg)
                        self.prev_cnt = curr_cnt

                        self.data_reader.feed_data(DataClass(self.comp_name, sensor_data[1][p*(self.usb_dps + 4)+4: (p+1)*(self.usb_dps+4)]))
                    self.sensor_data_file.write(sensor_data[1])
                else:
                    self.objThread.start()
            if self.objThread.isRunning():
                self.obj.interrupt_event.set()

    class SensorAcquisitionThread_test_v1(SensorAcquisitionThread):

        def __init__(self, event, hsd_link, data_reader, d_id, s_id, ss_id, comp_name, sensor_data_file):            
            self.s_id = s_id
            self.ss_id = ss_id
            super().__init__(self, event, hsd_link, data_reader, d_id, comp_name, sensor_data_file)

        def run(self):
            while not self.stopped.wait(0.2):
                sensor_data = self.hsd_link.get_sensor_data(self.d_id, self.s_id, self.ss_id)
                if sensor_data is not None:
                    self.data_reader.feed_data(DataClass(self.comp_name, sensor_data[1]))
                    self.sensor_data_file.write(sensor_data[1])

    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.hsd_link = None
        self.is_hsd_link_up = False
        self.is_logging = False
        self.is_detecting = False
        self.is_automode_enabled = False
        self.curr_bandwidth = 0

        # TODO: Next version --> Hotplug events notification support
        # self.plugged_flag = False
        # self.unplugged_flag = False
        # TODO: Next version --> Hotplug events notification support
        
        self.refresh()

    # TODO: Next version --> Hotplug events notification support
    # def plug_callback(self):
    #     print("PLUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUGGED!")
    #     # if self.plugged_flag == False:
    #     #     self.sig_usb_hotplug.emit(True)
    #     #     self.plugged_flag = True
    #     #     self.unplugged_flag = False
    #         # self.refresh()

    # def unplug_callback(self):
    #     print("UNPLUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUGGED!")        
    #     # if self.unplugged_flag == False:
    #     #     self.sig_usb_hotplug.emit(False)
    #     #     self.plugged_flag = False
    #     #     self.unplugged_flag = True
    #         # self.refresh()
    # TODO: Next version --> Hotplug events notification support

    def is_com_ok(self):
        return self.is_hsd_link_up
    
    #HSD
    def get_logging_status(self):
        return self.is_logging
    
    def get_device_formatted_name(self, device):
        if isinstance(device, dict) and "devices" in device:
            fw_info_tmp = [c for c in device["devices"][0]["components"] if "firmware_info" in list(c.keys())[0]]
            if len(fw_info_tmp) == 1:
                fw_info = fw_info_tmp[0]["firmware_info"]
                d_alias = fw_info["alias"]
                if "serial_number" in fw_info:
                    d_sn = fw_info["serial_number"]
                elif "part_number" in fw_info:
                    d_sn = fw_info["part_number"]
                d_fw_name = fw_info["fw_name"]
                d_fw_version = fw_info["fw_version"]
                return "{} - [{}] {} v{}".format(d_alias, d_sn, d_fw_name, d_fw_version)
            else:
                if "board_id" in device["devices"][0]:
                    b_id = device["devices"][0]["board_id"]
                    if b_id == 14:
                        d_alias = "STWIN.box"
                    if b_id == 13:
                        d_alias = "SensorTile.box PRO"
                    return d_alias
        elif isinstance(self.hsd_link, HSDLink_v1):
            return "{} - [{}] {} v{}".format(device.device_info.alias, device.device_info.part_number, device.device_info.fw_name, device.device_info.fw_version)
    
    #HSD TODO duplicated API
    def set_rtc_time(self):
        now = datetime.now()
        time = now.strftime("%Y%m%d_%H_%M_%S")
        message = {"log_controller*set_time":{"datetime":time}}
        return self.send_command(json.dumps(message))
    
    def refresh(self):
        try:
            if self.hsd_link is not None:
                self.hsd_link.close()
            hsd_link_factory = HSDLink()
            self.hsd_link = hsd_link_factory.create_hsd_link()
            
            # TODO: Next version --> Hotplug events notification support
            # if self.plugged_flag == False:
            #     self.hsd_link = hsd_link_factory.create_hsd_link(plug_callback = self.plug_callback, unplug_callback = self.unplug_callback)
            #     self.plugged_flag = True
            # else:
            #     self.hsd_link = hsd_link_factory.create_hsd_link()
            # TODO: Next version --> Hotplug events notification support

            if self.hsd_link is not None:
                self.is_hsd_link_up = True
        except Exception as err:
            log.error("Error: {}".format(err))
            if self.hsd_link is not None:
                self.hsd_link.close()
            self.is_hsd_link_up = False
            self.sig_com_init_error.emit()
        self.sensors_threads = []
        self.threads_stop_flags = []
        self.sensor_data_files = []
        self.data_readers = []
        self.ispu_output_format = None
        self.ispu_output_format_path = None
        self.ispu_ucf_file_path = None
        self.log_msg = ""
    
    def get_device_list(self):
        devices = []
        if self.hsd_link is not None:
            devices = self.hsd_link.get_devices() 
        return devices
    
    def get_device_presentation_string(self, d_id = 0):
        if type(self.hsd_link) == HSDLink_v1:
            return None
        return self.hsd_link.get_device_presentation_string(d_id)

    def get_device_info(self, d_id = 0):
        return self.hsd_link.get_device_info(d_id)
    
    def load_device_template(self, board_id, fw_id):
        self.sig_dtm_loading_started.emit()
        dev_template_json = self.query_dtdl_model(board_id, fw_id)
        if dev_template_json == "":
            log.error("Connected device not supported (Unrecognized board_id, fw_id)")
        if isinstance(dev_template_json,dict):
            fw_name = self.hsd_link.get_firmware_info(self.device_id).get("firmware_info").get("fw_name")
            if fw_name is not None:
                splitted_fw_name = fw_name.lower().split("-")
                reformatted_fw_name = "".join([splitted_fw_name[0]] + [f.capitalize() for f in splitted_fw_name[1:]])
            for dt in dev_template_json:
                if reformatted_fw_name.lower() in  dev_template_json[dt][0].get("@id").lower():
                    dev_template_json = dev_template_json[dt]
                    break        
        super().load_local_device_template(dev_template_json)
        self.hsd_link.set_device_template(dev_template_json)
        self.sig_dtm_loading_completed.emit()
        
    def load_local_device_template(self, input_dt_file_path):
        with open(input_dt_file_path, 'r', encoding='utf-8') as json_file:
            dev_template_json = json.load(json_file)
            json_file.close()
        super().load_local_device_template(dev_template_json)
        self.hsd_link.set_device_template(dev_template_json)

    def is_sensor_enabled(self, comp_name, d_id = 0):
        return self.hsd_link.get_sensor_enable(d_id, comp_name)
    
    def get_component_status(self, comp_name):
        return self.hsd_link.get_component_status(self.device_id, comp_name)

    def __get_mems_sensor_odr(self, comp_status, comp_interface):
        if "odr" in comp_status:
            odr_index = comp_status["odr"]
            odr_enum_dname = [c for c in comp_interface.contents if c.name == "odr"][0].schema.enum_values[odr_index].display_name
            odr_value = odr_enum_dname if isinstance(odr_enum_dname,str) else odr_enum_dname.en
            odr_value = odr_value.replace(',','.')
            return float(odr_value)
        return 1

    def __get_audio_sensor_odr(self, comp_status, comp_interface):
        return self.__get_mems_sensor_odr(comp_status, comp_interface)
    
    def __get_mems_sensor_fs(self, comp_status, comp_interface):
        pass

    def __get_sensor_unit(self, prop_w_unit_name, comp_status, comp_interface):
        if prop_w_unit_name in comp_status:
            prop_content = [c for c in comp_interface.contents if c.name == prop_w_unit_name][0]
            
            if prop_content.unit is not None:
                unit = prop_content.unit
            elif prop_content.display_unit is not None:
                unit = prop_content.display_unit if isinstance(prop_content.display_unit, str) else prop_content.display_unit.en
            
            unit_dict = UnitMap().unit_dict
            if unit in unit_dict:
                unit = unit_dict[unit]

            return unit
        return ""
    
    def __get_mems_sensor_unit(self, comp_status, comp_interface):
        return self.__get_sensor_unit("fs", comp_status, comp_interface)
    
    def __get_audio_sensor_unit(self, comp_status, comp_interface):
        return self.__get_sensor_unit("aop", comp_status, comp_interface)
    
    def __get_ranging_sensor_unit(self, comp_status, comp_interface):
        return "" #self.__get_sensor_unit("aaa", comp_status, comp_interface)
    
    def __get_mc_telemetry_unit(self, telemetry_status, comp_interface):
        print(telemetry_status)
        pass
        # return self.__get_sensor_unit("aop", telemetry_status, comp_interface)

    def get_description_string(content):
        if content.description is not None:
            return content.description if isinstance(content.description, str) else content.description.en
        return None

    def get_plot_params(self, comp_name, comp_type, comp_interface, comp_status):
        if comp_status is not None and comp_name in comp_status:
            if comp_type.name == ComponentType.SENSOR.name:
                comp_status_value = comp_status[comp_name]
                enabled = comp_status_value["enable"]
                s_category = comp_status_value.get("sensor_category")
                
                dimension = comp_status_value.get("dim", 1)

                if s_category == DTDLUtils.SensorCategoryEnum.ISENSOR_CLASS_MEMS.value:
                    odr = self.__get_mems_sensor_odr(comp_status_value, comp_interface)
                    unit = self.__get_mems_sensor_unit(comp_status_value, comp_interface)
                    return SensorMemsPlotParams(comp_name, enabled, odr, dimension, unit)
                elif s_category == DTDLUtils.SensorCategoryEnum.ISENSOR_CLASS_AUDIO.value:
                    odr = self.__get_audio_sensor_odr(comp_status_value, comp_interface)
                    unit = self.__get_audio_sensor_unit(comp_status_value, comp_interface)
                    return SensorAudioPlotParams(comp_name, enabled, odr, dimension, unit)
                elif s_category == DTDLUtils.SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
                    log.warning("ISENSOR_CLASS_RANGING category not supported yet")
                    unit = self.__get_ranging_sensor_unit(comp_status_value, comp_interface)
                    return SensorRangingPlotParams(comp_name, enabled, dimension, unit)
                elif s_category == DTDLUtils.SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                    log.warning("ISENSOR_CLASS_LIGHT category not supported yet")
                elif s_category == DTDLUtils.SensorCategoryEnum.ISENSOR_CLASS_CAMERA.value:
                    log.warning("ISENSOR_CLASS_CAMERA category not supported yet")
                else: #Maintain compatibility with OLD versions (< SensorManager v3 [NO SENSOR CATEGORIES])
                    odr = self.__get_mems_sensor_odr(comp_status_value, comp_interface)
                    unit = self.__get_mems_sensor_unit(comp_status_value, comp_interface)
                    if unit == "":
                        unit = self.__get_audio_sensor_unit(comp_status_value, comp_interface)
                    return SensorMemsPlotParams(comp_name, enabled, odr, dimension, unit)
            
            elif comp_type.name == ComponentType.ALGORITHM.name:
                comp_status_value = comp_status[comp_name]
                enabled = comp_status_value["enable"]
                if "algorithm_type" in comp_status_value:
                    alg_type =  comp_status_value["algorithm_type"]
                else:
                    alg_type = 0
                
                if alg_type == DTDLUtils.AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                    return FFTAlgPlotParams(comp_name, 
                                            enabled,
                                            fft_len=comp_status_value["fft_length"], 
                                            fft_sample_freq= comp_status_value["fft_sample_freq"], 
                                            y_label = "db")
                elif alg_type == DTDLUtils.AlgorithmTypeEnum.IALGORITHM_TYPE_ANOMALY_DETECTOR.value:
                        return AnomalyDetectorModelPlotParams( comp_name,
                                                              enabled)
                elif alg_type == DTDLUtils.AlgorithmTypeEnum.IALGORITHM_TYPE_CLASSIFIER.value:
                        return ClassificationModelPlotParams( comp_name,
                                                              enabled,
                                                              num_of_class= comp_status_value["dim"])
            
            elif comp_type.name == ComponentType.ACTUATOR.name:
                
                comp_status_value = comp_status[comp_name]
                enabled = False #comp_status_value["enable"]
                
                plot_params_dict = {}
                if comp_name == DTDLUtils.MC_SLOW_TELEMETRY_COMP_NAME:
                    telemetry_contents = [ci for ci in comp_interface.contents if HSD_Controller.get_description_string(ci) == DTDLUtils.MC_SLOW_TELEMETRY_STRING]
                    for t in telemetry_contents:
                        t_display_name = t.display_name if isinstance(t.display_name, str) else t.display_name.en
                        t_unit = comp_status_value[t.name].get("unit") #self.__get_mc_telemetry_unit(comp_status_value[t.name], comp_interface)
                        t_enabled = comp_status_value[t.name].get("enabled")
                        t_type = comp_status_value[t.name].get("graph_type")
                        if t_type == DTDLUtils.GraphTypeEnum.GAUGE.value:
                            plot_params_dict[t_display_name] = PlotGaugeParams(t.name, t_enabled, comp_status_value[t.name].get("min"), comp_status_value[t.name].get("max"), t_unit)
                        elif t_type == DTDLUtils.GraphTypeEnum.V_LEVEL.value:
                            plot_params_dict[t_display_name] = PlotLevelParams(t.name, t_enabled, comp_status_value[t.name].get("min"), comp_status_value[t.name].get("max"), t_unit)
                        elif t_type == DTDLUtils.GraphTypeEnum.LABEL.value:
                            plot_params_dict[t_display_name] = PlotLabelParams(t.name, t_enabled, comp_status_value[t.name].get("min"), comp_status_value[t.name].get("max"), t_unit)
                        elif t_type == DTDLUtils.GraphTypeEnum.CHECKBOX.value:
                            plot_params_dict[t_display_name] = PlotCheckBoxParams(t.name, t_enabled, ["aaa", "bbb", "ccc"])
                        elif t_type == DTDLUtils.GraphTypeEnum.LINE.value:
                            plot_params_dict[t_display_name] = LinesPlotParams(t.name, t_enabled, 1, t_unit)
                elif comp_name == DTDLUtils.MC_FAST_TELEMETRY_COMP_NAME:
                    telemetry_contents = [ci for ci in comp_interface.contents if HSD_Controller.get_description_string(ci) == DTDLUtils.MC_FAST_TELEMETRY_STRING]
                    for t in telemetry_contents:
                        t_display_name = t.display_name if isinstance(t.display_name, str) else t.display_name.en
                        t_unit = comp_status_value[t.name].get("unit") #self.__get_mc_telemetry_unit(comp_status_value[t.name], comp_interface)
                        t_enabled = comp_status_value[t.name].get("enabled")
                        plot_params_dict[t_display_name] = LinesPlotParams(t.name, t_enabled, 1, t_unit)
                if len([tt for tt in plot_params_dict if plot_params_dict[tt].enabled]) > 0:
                    enabled = True
                return MCTelemetriesPlotParams(comp_name, enabled, plot_params_dict)
        return None
    
    def fill_component_status(self, comp_name):
        try:
            comp_status = self.get_component_status(comp_name)
            if comp_status is not None and comp_name in comp_status:
                self.components_status[comp_name] = comp_status[comp_name]
                self.sig_component_updated.emit(comp_name, comp_status[comp_name])
            else:
                log.warning("The component [{}] defined in DeviceTemplate has not a Twin in Device Status from the FW".format(comp_name))
                self.sig_component_updated.emit(comp_name, None)
                has_cmd_or_tele_contents = False
                contents = self.components_dtdl[comp_name].contents 
                for c in contents:
                    if isinstance(c.type, list):
                        has_cmd_or_tele_contents = len([cc for cc in c.type if cc == DTM.ContentType.COMMAND or cc == DTM.ContentType.TELEMETRY]) > 0
                    else:
                        has_cmd_or_tele_contents = len([c for c in contents if c.type == DTM.ContentType.COMMAND or c.type == DTM.ContentType.TELEMETRY]) > 0
                    if has_cmd_or_tele_contents == True:
                        return
                self.remove_component_config_widget(comp_name)
        except:
            log.warning("The component [{}] defined in DeviceTemplate has not a Twin in Device Status from the FW".format(comp_name))
            self.remove_component_config_widget(comp_name)
            return

    def update_component_status(self, comp_name, comp_type = ComponentType.OTHER):
        comp_status = self.get_component_status(comp_name)
        if comp_status is not None and comp_name in comp_status:
            self.components_status[comp_name] = comp_status[comp_name]
            if isinstance(comp_type,str):
                ct = comp_type
            else:
                ct = comp_type.name
            if ct == ComponentType.SENSOR.name:
                plot_params = self.get_plot_params(comp_name, comp_type, self.components_dtdl[comp_name], comp_status)
                self.sig_sensor_component_updated.emit(comp_name, plot_params)
                self.check_hsd_bandwidth()
            elif  ct == ComponentType.ALGORITHM.name:
                # comp_status_value = comp_status[comp_name]
                # self.components_status[comp_name] = comp_status_value
                # plot_params = AlgorithmPlotParams(comp_name,comp_status_value["enable"], "")
                plot_params = self.get_plot_params(comp_name, comp_type, self.components_dtdl[comp_name], comp_status)
                self.sig_algorithm_component_updated.emit(comp_name, plot_params)
            elif ct == ComponentType.ACTUATOR.name:
                plot_params = self.get_plot_params(comp_name, comp_type, self.components_dtdl[comp_name], comp_status)
                self.sig_actuator_component_updated.emit(comp_name, plot_params)
            self.sig_component_updated.emit(comp_name, comp_status[comp_name])
        else:
            log.warning("The component [{}] defined in DeviceTemplate has not a Twin in Device Status from the FW".format(comp_name))
            self.sig_component_updated.emit(comp_name, None)
    
    def update_device_status(self):
        dev_status = self.hsd_link.get_device_status(self.device_id)
        for c in dev_status["devices"][self.device_id]["components"]:
            c_dict = list(c.values())[0]
            c_name = list(c.keys())[0]
            c_type = c_dict.get("c_type", ComponentType.NONE)
            if c_type == DTDLUtils.ComponentTypeEnum.SENSOR.value:
                c_type = ComponentType.SENSOR
            elif c_type == DTDLUtils.ComponentTypeEnum.ALGORITHM.value:
                c_type = ComponentType.ALGORITHM
            elif c_type == DTDLUtils.ComponentTypeEnum.ACTUATOR.value:
                c_type = ComponentType.ACTUATOR
            elif c_type == DTDLUtils.ComponentTypeEnum.OTHER.value:
                c_type = ComponentType.OTHER
            self.update_component_status(c_name, c_type)

    def start_log(self, interface=1):
        if type(self.hsd_link) == HSDLink_v1:
            res = self.hsd_link.start_log(self.device_id)
        else:
            res = self.hsd_link.start_log(self.device_id, interface)
        if res:
            self.sig_logging.emit(True,interface)
            self.sig_streaming_error.emit(False, "")
            self.is_logging = True
    
    def start_auto_log(self, interface=1):
        self.start_log(interface)
        self.sig_is_auto_started.emit(True)
            
    def start_detect(self):
        if type(self.hsd_link) == HSDLink_v1:
            res = self.hsd_link.start_log(self.device_id)
        else:
            res = self.hsd_link.start_log(self.device_id, 1)
        if res:
            self.sig_detecting.emit(True)
            self.is_detecting = True

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
                c_status = self.get_component_status(s_plot.comp_name)
                self.components_status[s_plot.comp_name] = c_status[s_plot.comp_name]
                c_status_value = c_status[s_plot.comp_name]
                
                c_enable = c_status_value["enable"] 
                
                if c_enable == True:
                    sensor_data_file_path = os.path.join(self.hsd_link.get_acquisition_folder(),(str(s_plot.comp_name) + ".dat"))
                    sensor_data_file = open(sensor_data_file_path, "wb+")
                    self.sensor_data_files.append(sensor_data_file)
                    stopFlag = Event()
                    self.threads_stop_flags.append(stopFlag)
                    
                    c_type = c_status_value.get("c_type")
                    usb_dps = c_status_value.get("usb_dps")
                    dimensions = c_status_value.get("dim", 1)
                    sensitivity = c_status_value.get("sensitivity", 1)
                    spts = c_status_value.get("samples_per_ts", 1)
                    sample_size = TypeConversion.check_type_length(c_status_value["data_type"])
                    data_format = TypeConversion.get_format_char(c_status_value["data_type"])
                    
                    interleaved_data = True
                    raw_flat_data = False

                    if c_type == ComponentType.SENSOR.value:
                        if not isinstance(spts, int):
                            spts = spts["val"] if spts and "val" in spts else spts
                        
                    elif c_type == ComponentType.ALGORITHM.value:
                        spts = 0 #spts override (no timestamps in algorithms @ the moment)
                        algorithm_type = c_status_value.get("algorithm_type")
                        if algorithm_type == DTDLUtils.AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value \
                            or algorithm_type == DTDLUtils.AlgorithmTypeEnum.IALGORITHM_TYPE_ANOMALY_DETECTOR.value \
                            or algorithm_type == DTDLUtils.AlgorithmTypeEnum.IALGORITHM_TYPE_CLASSIFIER.value:
                            interleaved_data = False
                            if algorithm_type == DTDLUtils.AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                                dimensions = c_status_value.get("fft_length")
                    
                    elif c_type == ComponentType.ACTUATOR.value:
                        dimensions = c_status_value["n_params"]

                    if "_ispu" in s_plot.comp_name:
                        data_format = "b"
                        dimensions = 64
                        sample_size = 1
                        raw_flat_data = True
                    
                    dr = DataReader(self, s_plot.comp_name, spts, dimensions, sample_size, data_format, sensitivity, interleaved_data, raw_flat_data)
                    self.data_readers.append(dr)
                
                    thread = self.SensorAcquisitionThread(stopFlag, self.hsd_link, dr, self.device_id, s_plot.comp_name, sensor_data_file, usb_dps, self.sig_streaming_error)
                    thread.start()
                    self.sensors_threads.append(thread)

    
    
    def stop_log(self, interface=1):
        if self.is_logging == True:
            self.hsd_link.stop_log(self.device_id)
            if type(self.hsd_link) == HSDLink_v1:
                self.hsd_link.save_json_device_file(self.device_id)
                self.hsd_link.save_json_acq_info_file(self.device_id)
            else:
                #TODO put here a "File saving..." loading window!
                time.sleep(0.5)
                self.hsd_link.save_json_acq_info_file(self.device_id)
                self.hsd_link.save_json_device_file(self.device_id)
                if self.ispu_output_format_path is not None:
                    shutil.copyfile(self.ispu_output_format_path, os.path.join(self.hsd_link.get_acquisition_folder(),"ispu_output_format.json"))
                    log.info("ispu_output_format.json File correctly saved")
                if self.ispu_ucf_file_path is not None:
                    ucf_filename = os.path.basename(self.ispu_ucf_file_path)
                    shutil.copyfile(self.ispu_ucf_file_path, os.path.join(self.hsd_link.get_acquisition_folder(),ucf_filename))
                    log.info("{} File correctly saved".format(ucf_filename))
                self.sig_logging.emit(False, interface)
                self.is_logging = False
    
    def stop_auto_log(self, interface=1):
        if self.is_logging == True:
            self.hsd_link.stop_log(self.device_id)
            if type(self.hsd_link) == HSDLink_v1:
                self.hsd_link.save_json_device_file(self.device_id)
                self.hsd_link.save_json_acq_info_file(self.device_id)
            else:
                #TODO put here a "File saving..." loading window!
                time.sleep(0.5)
                self.hsd_link.save_json_acq_info_file(self.device_id)
                self.hsd_link.save_json_device_file(self.device_id)
                if self.ispu_output_format_path is not None:
                    shutil.copyfile(self.ispu_output_format_path, os.path.join(self.hsd_link.get_acquisition_folder(),"ispu_output_format.json"))
                    log.info("ispu_output_format.json File correctly saved")
                if self.ispu_ucf_file_path is not None:
                    ucf_filename = os.path.basename(self.ispu_ucf_file_path)
                    shutil.copyfile(self.ispu_ucf_file_path, os.path.join(self.hsd_link.get_acquisition_folder(),ucf_filename))
                    log.info("{} File correctly saved".format(ucf_filename))
                self.sig_logging.emit(False, interface)
                self.sig_is_auto_started.emit(False)
                self.is_logging = False
    
    def stop_detect(self):
        if self.is_detecting == True:
            self.hsd_link.stop_log(self.device_id)
            if type(self.hsd_link) == HSDLink_v1:
                self.hsd_link.save_json_device_file(self.device_id)
                self.hsd_link.save_json_acq_info_file(self.device_id)
            else:
                self.hsd_link.save_json_device_file(self.device_id)
                self.hsd_link.save_json_acq_info_file(self.device_id)
                if self.ispu_output_format_path is not None:
                    shutil.copyfile(self.ispu_output_format_path, os.path.join(self.hsd_link.get_acquisition_folder(),"ispu_output_format.json"))
                    log.info("ispu_output_format.json File correctly saved")
                if self.ispu_ucf_file_path is not None:
                    ucf_filename = os.path.basename(self.ispu_ucf_file_path)
                    shutil.copyfile(self.ispu_ucf_file_path, os.path.join(self.hsd_link.get_acquisition_folder(),ucf_filename))
                    log.info("{} File correctly saved".format(ucf_filename))
            self.sig_detecting.emit(False)
            self.is_detecting = False
    
    def stop_plots(self):
        for sf in self.threads_stop_flags:
            sf.set()
        for f in self.sensor_data_files:
            f.close()
    
    def plot_window_changed(self, plot_window_time):
        self.sig_plot_window_time_updated.emit(plot_window_time)
    
    def get_plot_widget(self, comp_name):
        if comp_name in self.plot_widgets:
            return self.plot_widgets[comp_name]
        else:
            return None
    
    def add_plot_widget(self, plot_widget):
        self.plot_widgets[plot_widget.comp_name] = (plot_widget)

    def __calculate_hsd_bandwidth(self):
        self.curr_bandwidth = 0
        sensors_status = {s:self.components_status[s] for s in self.components_status if self.components_status[s].get("c_type") == DTDLUtils.ComponentTypeEnum.SENSOR.value and self.components_status[s].get("enable")}
        for ss in sensors_status:
            # bnd = ODR*(data_type*dim)*8
            ss_status = sensors_status[ss]
            ss_dtdl_comp = self.components_dtdl[ss]
            odr = self.__get_mems_sensor_odr(ss_status, ss_dtdl_comp)
            # odr_enum_id = ss_status.get("odr")
            # odr_enum_dname = [c for c in ss_dtdl_comp.contents if c.name == "odr"][0].schema.enum_values[odr_enum_id].display_name
            # odr_value = odr_enum_dname if isinstance(odr_enum_dname,str) else odr_enum_dname.en
            # odr_value = odr_value.replace(',','.')
            # odr = float(odr_value)
            data_byte_len = TypeConversion.check_type_length(ss_status.get("data_type"))
            dim = ss_status.get("dim")                                                             
            self.curr_bandwidth += odr * data_byte_len * dim * 8
    
    def check_hsd_bandwidth(self):
        self.__calculate_hsd_bandwidth()
        # print("self.curr_bandwidth", self.curr_bandwidth)
        self.sig_hsd_bandwidth_exceeded.emit(self.curr_bandwidth > HSD_Controller.MAX_HSD_BANDWIDTH)

    def update_plot_widget(self, comp_name, plot_params, visible):
        self.plot_widgets[comp_name].update_plot_characteristics(plot_params)
        self.plot_widgets[comp_name].setVisible(visible)

    def remove_plot_widget(self, comp_name) -> HSDPlotLinesWidget:
        if comp_name in self.plot_widgets:
            return self.plot_widgets.pop(comp_name)
        else:
            log.warning("{} is not in plot widget list yet".format(comp_name))

    def add_data_to_a_plot(self, data:DataClass):
        self.plot_widgets[data.comp_name].add_data(data.data)

    def connect_to(self, d_id:int, d_text:str = None):
        self.sig_device_connected.emit(True)
        self.device_id = d_id

    def disconnect(self):
        self.sig_device_connected.emit(False)
        for pw in self.plot_widgets:
            self.plot_widgets[pw].deleteLater()
        self.plot_widgets.clear()
        
        for cw in self.cconfig_widgets:
            self.cconfig_widgets[cw].deleteLater()
        self.cconfig_widgets.clear()
        
        self.components_dtdl.clear() #From DTDL DeviceModel 
        self.components_status.clear() #From FW

    def send_command(self, json_command):
        self.hsd_link.send_command(self.device_id, json_command)
        log.info("Command sent: {}".format(json_command))
    
    def get_device_status(self):
        return self.hsd_link.get_device_status(self.device_id)
    
    def save_config(self, on_pc:bool, on_sd:bool):
        if on_pc:
            fname = QFileDialog.getSaveFileName(None, "Save Current Device Configuration", "device_config", "JSON (*.json)")
            with open(fname[0], 'w', encoding='utf-8') as f:
                json.dump(self.get_device_status(), f, ensure_ascii=False, indent=4)
        if on_sd:
            self.hsd_link.save_config(self.device_id)
            
    def load_config(self, fpath):
        self.hsd_link.update_device(self.device_id, fpath)
        self.update_device_status()
        
    def load_ispu_ucf_file(self, fpath):
        self.ispu_ucf_file_path = fpath

    def load_ispu_output_fmt_file(self, fpath):
        try:
            with open(fpath) as f:
                file_content = f.read()
                if file_content[-1] == '\x00':
                    ispu_out_json_dict = json.loads(file_content[:-1])
                else:
                    ispu_out_json_dict = json.loads(file_content)
            ispu_out_json_str = json.dumps(ispu_out_json_dict)
            f.close()
            self.ispu_output_format = json.loads(ispu_out_json_str)
            self.ispu_output_format_path = fpath
            return True
        except:
            return False
    
    def get_out_fmt_byte_count(self, of_type):
        return TypeConversion.check_type_length(of_type)
    
    def get_out_fmt_char(self, of_type):
        return TypeConversion.get_format_char(of_type)
    
    def upload_file(self, comp_name, fpath):
        #TODO
        log.error("Component: {} Generic file Upload feature not yet implemented".format(comp_name))
    
    def upload_mlc_ucf_file(self, comp_name, ucf_fpath):
        self.hsd_link.upload_mlc_ucf_file(self.device_id, comp_name, ucf_fpath)
        
    def upload_ispu_ucf_file(self, comp_name, ucf_fpath, output_json_fpath):
        self.hsd_link.upload_ispu_ucf_file(self.device_id, comp_name, ucf_fpath, output_json_fpath)
        self.sig_ispu_ucf_loaded.emit(ucf_fpath, output_json_fpath)
        
    def doTag(self, sw_tag_name, status):
        if status is True:
            self.hsd_link.set_sw_tag_on(self.device_id, sw_tag_name)
        else:
            self.hsd_link.set_sw_tag_off(self.device_id, sw_tag_name)
        self.update_component_status("tags_info")
        tag_label = self.components_status["tags_info"][sw_tag_name]["label"]
        self.sig_tag_done.emit(status, tag_label)

    def changeSWTagClassEnabled(self, sw_tag_name, new_status):
        self.hsd_link.set_sw_tag_class_enabled(self.device_id, sw_tag_name, new_status)
        
    def changeHWTagClassEnabled(self, hw_tag_name, new_status):
        self.hsd_link.set_hw_tag_class_enabled(self.device_id, hw_tag_name, new_status)
    
    def changeSWTagClassLabel(self, sw_tag_name, new_label):
        self.hsd_link.set_sw_tag_class_label(self.device_id, sw_tag_name, new_label)
        
    def changeHWTagClassLabel(self, hw_tag_name, new_label):
        self.hsd_link.set_hw_tag_class_label(self.device_id, hw_tag_name, new_label)

    def set_anomaly_classes(self, anomaly_classes):
        self.anomaly_classes = anomaly_classes
    
    def get_anomaly_classes(self):
        return self.anomaly_classes

    def set_output_classes(self, output_classes):
        self.output_classes = output_classes
    
    def get_output_classes(self):
        return self.output_classes
    
    def set_ai_anomaly_tool(self, ai_anomaly_tool):
        self.ai_anomaly_tool = ai_anomaly_tool
    
    def get_ai_anomaly_tool(self):
        return self.ai_anomaly_tool
    
    def set_ai_classifier_tool(self, ai_classifier_tool):
        self.ai_classifier_tool = ai_classifier_tool
    
    def get_ai_classifier_tool(self):
        return self.ai_classifier_tool

    def set_rtc_time(self):
        self.hsd_link.set_rtc_time(self.device_id)
    
    def do_offline_plots(self, cb_sensor_value, tag_label, start_time, end_time, active_sensor_list, active_algorithm_list, debug_flag, sub_plots_flag, raw_data_flag):
        acquisition_folder = self.hsd_link.get_acquisition_folder()
        hsd_factory = HSDatalog()
        hsd = hsd_factory.create_hsd(acquisition_folder)
        
        hsd.enable_timestamp_recovery(debug_flag)
        if tag_label == "None" or  tag_label == '':
            tag_label = None
        if cb_sensor_value == "all":
            for s in active_sensor_list:
                s_key = list(s.keys())[0]
                hsd.get_sensor_plot(s_key, None, start_time, end_time, tag_label if tag_label != "None" else None, sub_plots_flag, raw_data_flag)
            for a in active_algorithm_list:
                a_key = list(a.keys())[0]
                hsd.get_algorithm_plot(a_key, start_time, end_time)
        else:
            s_list = hsd.get_sensor_list(only_active=True)
            a_list = hsd.get_algorithm_list(only_active=True)
            sensor_comp = [s for s in s_list if cb_sensor_value in s]
            algo_comp = [a for a in a_list if cb_sensor_value in a]
            if len(sensor_comp) > 0: # == 1
                hsd.get_sensor_plot(cb_sensor_value, None, start_time, end_time, tag_label if tag_label != "None" else None, sub_plots_flag, raw_data_flag)
            elif len(algo_comp) > 0: # == 1
                a_key = list(algo_comp[0].keys())[0]
                hsd.get_algorithm_plot(a_key, start_time, end_time)
        
        self.sig_offline_plots_completed.emit()
    
    def convert_dat2wav(self, comp_name, start_time, end_time):
        acquisition_folder = self.hsd_link.get_acquisition_folder()
        hsd_factory = HSDatalog()
        hsd = hsd_factory.create_hsd(acquisition_folder)
    
        output_folder = acquisition_folder + "_Exported"  
        if not os.path.exists(output_folder):
            os.makedirs(output_folder)

        hsd.enable_timestamp_recovery(True)
        component = HSDatalog.get_component(hsd, comp_name)
        if component is not None:
            HSDatalog.convert_dat_to_wav(hsd, component, start_time, end_time, output_folder)
        
        wav_file_name = HSDatalog.get_wav_file_name(hsd, comp_name, None, output_folder)
        self.sig_wav_conversion_completed.emit(comp_name, wav_file_name)

    def set_automode_status(self, status):
        self.is_automode_enabled = status

    def get_automode_status(self):
        return self.is_automode_enabled
    
    def get_automode_settings(self):
        automode_status = self.get_component_status("automode")["automode"]
        n = automode_status.get("nof_acquisitions")
        m = automode_status.get("start_delay_s")
        m = automode_status.get("start_delay_ms") if m is None else m
        x = automode_status.get("logging_period_s")
        x = automode_status.get("datalog_time_length") if x is None else x
        y = automode_status.get("idle_period_s")
        y = automode_status.get("idle_time_length") if y is None else y
        return (n, m, x, y)

        
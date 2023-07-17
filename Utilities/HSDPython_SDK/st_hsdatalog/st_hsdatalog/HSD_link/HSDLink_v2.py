# *****************************************************************************
#  * @file    HSDLink_v2.py
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
import json
from datetime import datetime

from st_hsdatalog.HSD_utils.exceptions import InvalidCommandSetError, NoDeviceConnectedError
import st_hsdatalog.HSD_utils.logger as logger
from st_pnpl.PnPLCmd import PnPLCMDManager
from .communication.PnPL_HSD.PnPLHSD_com_manager import PnPLHSD_CommandManager, PnPLHSD_Creator
from st_pnpl.DTDL.device_template_manager import DeviceTemplateManager

from st_hsdatalog.HSD_utils.exceptions import *

log = logger.get_logger(__name__)

class HSDLink_v2:

    __com_manager = None
    __base_acquisition_folder = None
    __acquisition_folder = None
    
    def __init__(self, dev_com_type: str = 'pnpl', acquisition_folder = None, plug_callback = None, unplug_callback = None):
        self.__create_com_manager(dev_com_type, plug_callback, unplug_callback)
        
        self.sensor_data_counts = {}
        self.nof_connected_devices = 0
                
        self.__dt_manager = None
        if acquisition_folder is None:
            self.__base_acquisition_folder = "."
        else:            
            self.__base_acquisition_folder = acquisition_folder
        
        #check that there is at least one connected device
        self.nof_connected_devices = self.get_nof_devices()
        if self.nof_connected_devices <= 0:
            return
    
    def update_base_acquisition_folder(self, new_acquisition_folder):
        if new_acquisition_folder is not None:
            self.__base_acquisition_folder = new_acquisition_folder
        else:
            self.__base_acquisition_folder = "."

    def set_device_template(self, dev_template_json: dict):
        self.__dt_manager = DeviceTemplateManager(dev_template_json)

    def open(self):
        return self.__com_manager.open()
    
    def close(self):
        return self.__com_manager.close()

    def __create_com_manager(self,dev_com_type, plug_callback = None, unplug_callback = None):
        if dev_com_type == 'pnpl':
            factory = PnPLHSD_Creator()
        else:
            log.error("Invalid Command Set selected: {}".format(dev_com_type))
            raise InvalidCommandSetError(dev_com_type)
        self.__com_manager:PnPLHSD_CommandManager = factory.create_cmd_manager(plug_callback, unplug_callback)
        #Command set presentation
        log.info(self.get_cmd_set_presentation_string())

    def get_device_presentation_string(self, d_id:int):
        return self.__com_manager.get_device_presentation_string(d_id)
    
    def get_cmd_set_presentation_string(self):
        return self.__com_manager.get_cmd_set_presentation_string()

    def get_acquisition_folder(self):
        if self.__acquisition_folder is None:
            return self.__base_acquisition_folder
        return self.__acquisition_folder

    def get_nof_devices(self):
        return self.__com_manager.get_nof_devices()

    def get_version(self):
        return self.__com_manager.get_version()
    
    def get_firmware_info(self, d_id:int):
        return self.__com_manager.get_component_status(d_id, "firmware_info")

    def get_device_info(self, d_id:int):
        return self.__com_manager.get_component_status(d_id, "DeviceInformation") 

    def get_devices(self):
        return self.__com_manager.get_devices()
    
    def get_device_status(self, d_id:int):
        return self.__com_manager.get_device_status(d_id)
    
    def get_device(self, d_id:int):
        return self.__com_manager.get_device_status(d_id)
    
    def get_component_status(self, d_id:int, component_name:str):
        try:
            return self.__com_manager.get_component_status(d_id, component_name)
        except:
            return None

    def get_device_alias(self, d_id:int):
        return self.__com_manager.get_device_alias(d_id)
    
    def get_sensors_count(self, d_id:int, only_active:bool = None):
        return self.__com_manager.get_sensor_components_count(d_id, only_active)
    
    def get_sensors_names(self, d_id:int, type_filter:str="", only_active:bool = False):
        return self.__com_manager.get_sensor_components_names(d_id, type_filter, only_active)
    
    def get_sensors(self, d_id:int, type_filter:str="", only_active:bool = False):
        return self.__com_manager.get_sensor_components_status(d_id, type_filter, only_active)

    def get_algorithms_count(self, d_id:int, only_active:bool = None):
        return self.__com_manager.get_algorithm_components_count(d_id, only_active)

    def get_algorithms_names(self, d_id:int, only_active:bool = False):
        return self.__com_manager.get_algorithm_components_names(d_id, only_active)

    def get_sensor_enable(self, d_id:int, sensor_name:str):
        return self.__com_manager.get_boolean_property(d_id, sensor_name, "enable")

    def get_sensor_odr(self, d_id:int, sensor_name:str):
        return self.__com_manager.get_float_property(d_id, sensor_name, "odr")
    
    def get_sensor_measured_odr(self, d_id:int, sensor_name:str):
        return self.__com_manager.get_float_property(d_id, sensor_name, "measodr")

    def get_sensor_fs(self, d_id:int, sensor_name:str):
        return self.__com_manager.get_float_property(d_id, sensor_name, "fs")
    
    def get_sensor_aop(self, d_id:int, sensor_name:str):
        return self.__com_manager.get_float_property(d_id, sensor_name, "aop")

    def get_sensor_samples_per_ts(self, d_id:int, sensor_name:str):
        try:
            return self.__com_manager.get_integer_property(d_id, sensor_name, "samples_per_ts")
        except:
            return self.__com_manager.get_integer_property(d_id, sensor_name, "samples_per_ts", "val")
    
    def get_sensor_initial_offset(self, d_id:int, sensor_name:str):
        return self.__com_manager.get_float_property(d_id, sensor_name, "ioffset")
    
    def get_sensors_status(self, d_id:int, type_filter:str="", only_active:bool = False):
        return self.__com_manager.get_sensor_components_status(d_id, type_filter, only_active)

    def get_algorithms_status(self, d_id:int):
        return self.__com_manager.get_algorithm_components_status(d_id)

    def get_acquisition_info(self, d_id:int):
        return self.__com_manager.get_component_status(d_id, "acquisition_info")

    def get_tags_info(self, d_id:int):
        return self.__com_manager.get_component_status(d_id, "tags_info")

    def get_tag_list(self, d_id:int):
        tags_info = self.get_tags_info(d_id)
        if "tags" in tags_info:
            return tags_info["tags"]
        return list()

    def get_max_tags_per_acq(self, d_id: int):
        tags_info = self.get_tags_info(d_id)
        if "max_tags_num" in tags_info["tags_info"]:
            return tags_info["tags_info"]["max_tags_num"]
        return None

    def get_sw_tag_classes(self, d_id: int):
        sw_tags = dict()
        res = self.get_tags_info(d_id)
        for t in res["tags_info"]:
            if "sw" in t:
                sw_tags[t] = res["tags_info"][t]
        return sw_tags

    def get_sw_tag_class(self, d_id: int, tag_class_id: int):
        ret = self.__com_manager.get_string_property(d_id, "tags_info", "sw_tag{}".format(tag_class_id))
        return ret
    
    def get_sw_tag_class_label_by_id(self, d_id: int, tag_class_id: int):
        ret = self.__com_manager.get_string_property(d_id, "tags_info", "sw_tag{}".format(tag_class_id), "label")
        return ret
    
    def get_sw_tag_class_enabled_by_id(self, d_id: int, tag_class_id: int):
        ret = self.__com_manager.get_boolean_property(d_id, "tags_info", "sw_tag{}".format(tag_class_id), "enabled")
        return ret
    
    def get_sw_tag_class_status_by_id(self, d_id: int, tag_class_id: int):
        ret = self.__com_manager.get_boolean_property(d_id, "tags_info", "sw_tag{}".format(tag_class_id), "status")
        return ret
    
    def get_sw_tag_class_label(self, d_id: int, comp_name: str):
        ret = self.__com_manager.get_string_property(d_id, "tags_info", comp_name, "label")
        return ret
    
    def get_sw_tag_class_enabled(self, d_id: int, comp_name: str):
        ret = self.__com_manager.get_boolean_property(d_id, "tags_info", comp_name, "enabled")
        return ret
    
    def get_sw_tag_class_status(self, d_id: int, comp_name: str):
        ret = self.__com_manager.get_boolean_property(d_id, "tags_info", comp_name, "status")
        return ret

    # def get_hw_tag_classes(self, d_id: int):
    #     hw_tags = dict()
    #     res = self.get_tags_info(d_id)
    #     for t in res["tags_info"]:
    #         if "hw" in t:
    #             hw_tags[t] = res["tags_info"][t]
    #     return hw_tags

    # def get_hw_tag_class(self, d_id: int, tag_class_id: int):
    #     ret = self.__com_manager.get_string_property(d_id, "tags_info", "hw_tag{}".format(tag_class_id))
    #     return ret
    
    # def get_hw_tag_class_label_by_id(self, d_id: int, tag_class_id: int):
    #     ret = self.__com_manager.get_string_property(d_id, "tags_info", "hw_tag{}".format(tag_class_id), "label")
    #     return ret
    
    # def get_hw_tag_class_enabled_by_id(self, d_id: int, tag_class_id: int):
    #     ret = self.__com_manager.get_boolean_property(d_id, "tags_info", "hw_tag{}".format(tag_class_id), "enabled")
    #     return ret
    
    # def get_hw_tag_class_label(self, d_id: int, comp_name: str):
    #     ret = self.__com_manager.get_string_property(d_id, "tags_info", comp_name, "label")
    #     return ret
    
    # def get_hw_tag_class_enabled(self, d_id: int, comp_name: str):
    #     ret = self.__com_manager.get_boolean_property(d_id, "tags_info", comp_name, "enabled")
    #     return ret

    def send_command(self, d_id:int, message):
        return self.__com_manager.send_command(d_id, message)
    
    def get_boolean_property(self, d_id: int, comp_name: str, prop_name: str):
        return self.__com_manager.get_boolean_property(d_id, comp_name, prop_name)
    
    def get_integer_property(self, d_id: int, comp_name: str, prop_name: str):
        return self.__com_manager.get_integer_property(d_id, comp_name, prop_name)

    def get_string_property(self, d_id: int, comp_name: str, prop_name: str):
        return self.__com_manager.get_string_property(d_id, comp_name, prop_name)

    def get_float_property(self, d_id: int, comp_name: str, prop_name: str):
        return self.__com_manager.get_float_property(d_id, comp_name, prop_name)

    def set_acquisition_name(self, d_id:int, name:str):
        return self.__com_manager.set_property(d_id, name, "acquisition_info", "name")

    def set_acquisition_description(self, d_id:int, description:str):
        return self.__com_manager.set_property(d_id, description, "acquisition_info", "description")
    
    def set_acquisition_info(self, d_id:int, name:str, description:str):
        self.set_acquisition_name(d_id, name)
        return self.set_acquisition_description(d_id, description)

    def set_acquisition_description(self, d_id:int, description:str):
        return self.__com_manager.set_property(d_id, description, "acquisition_info", "description")

    def set_sensor_enable(self, d_id: int, new_status: bool, comp_name: str):        
        return self.__com_manager.set_property(d_id, new_status, comp_name, "enable")
    
    def set_sensor_odr(self, d_id: int, new_odr: float, comp_name: str):
        return self.__com_manager.set_property(d_id, new_odr, comp_name, "odr")

    def set_sensor_fs(self, d_id: int, new_fs: float, comp_name: str):
        return self.__com_manager.set_property(d_id, new_fs, comp_name, "fs")

    def set_sensor_aop(self, d_id: int, new_aop: float, comp_name: str):
        return self.__com_manager.set_property(d_id, new_aop, comp_name, "aop")

    def set_sensor_samples_per_ts(self, d_id: int, new_spts: int, comp_name: str):
        return self.__com_manager.set_property(d_id, new_spts, comp_name, "samples_per_ts", "val")

    def set_property(self, d_id: int, new_value, comp_name: str, prop_name: str, sub_prop_name: str = None):        
        return self.__com_manager.set_property(d_id, new_value, comp_name, prop_name, sub_prop_name)

    def set_sw_tag_class_enabled(self, d_id:int, comp_name:str, new_status: bool):
        return self.__com_manager.set_property(d_id, new_status, "tags_info", comp_name, "enabled")
    
    def set_sw_tag_class_label(self, d_id:int, comp_name:str, new_label: str):
        return self.__com_manager.set_property(d_id, new_label, "tags_info", comp_name, "label")

    def set_sw_tag_on(self, d_id:int, comp_name:str):
        return self.__com_manager.set_property(d_id, True, "tags_info", comp_name, "status")
    
    def set_sw_tag_off(self, d_id:int, comp_name:str):
        return self.__com_manager.set_property(d_id, False, "tags_info", comp_name, "status")
    
    def set_sw_tag_class_enabled_by_id(self, d_id:int, tag_class_id:int, new_status: bool):
        return self.__com_manager.set_property(d_id, new_status, "tags_info", "sw_tag{}".format(tag_class_id), "enabled")
    
    def set_sw_tag_class_label_by_id(self, d_id:int, tag_class_id:int, new_label: str):
        return self.__com_manager.set_property(d_id, new_label, "tags_info", "sw_tag{}".format(tag_class_id), "label")

    def set_sw_tag_on_by_id(self, d_id:int, tag_class_id:int):
        return self.__com_manager.set_property(d_id, True, "tags_info", "sw_tag{}".format(tag_class_id), "status")
    
    def set_sw_tag_off_by_id(self, d_id:int, tag_class_id:int):
        return self.__com_manager.set_property(d_id, False, "tags_info", "sw_tag{}".format(tag_class_id), "status")    

    # def set_hw_tag_class_enabled(self, d_id:int, comp_name:str, new_status: bool):
    #     return self.__com_manager.set_property(d_id, new_status, "tags_info", comp_name, "enabled")
    
    # def set_hw_tag_class_label(self, d_id:int, comp_name:str, new_label: str):
    #     return self.__com_manager.set_property(d_id, new_label, "tags_info", comp_name, "label")

    # def set_hw_tag_class_enabled_by_id(self, d_id:int, tag_class_id:int, new_status: bool):
    #     return self.__com_manager.set_property(d_id, new_status, "tags_info", "hw_tag{}".format(tag_class_id), "enabled")
    
    # def set_hw_tag_class_label_by_id(self, d_id:int, tag_class_id:int, new_label: str):
    #     return self.__com_manager.set_property(d_id, new_label, "tags_info", "hw_tag{}".format(tag_class_id), "label")
    
    def get_sensor_data(self, d_id:int, comp_name:str, ss_id = None):
        return self.__com_manager.get_sensor_data(d_id, comp_name)
    
    def set_rtc_time(self, d_id:int, dtime=None):
        if dtime is None:
            now = datetime.now()
            time = now.strftime("%Y%m%d_%H_%M_%S")
        message = PnPLCMDManager.create_command_cmd("log_controller","set_time","datetime", time if dtime is None else dtime)
        return self.send_command(d_id, message)
    
    def start_log(self, d_id:int, interface:int = 1, acq_folder = None, sub_folder = True):
        log.info("Log Started")
        if acq_folder is not None:
            self.update_base_acquisition_folder(acq_folder)
        if sub_folder == True:
            self.__acquisition_folder = os.path.join(self.__base_acquisition_folder, "{}".format(datetime.today().strftime('%Y%m%d_%H_%M_%S')))
        else:
            self.__acquisition_folder = self.__base_acquisition_folder
                
        if not os.path.exists(self.__acquisition_folder):
            os.makedirs(self.__acquisition_folder)
        return self.__com_manager.start_log(d_id, interface)

    def stop_log(self, d_id:int):
        log.info("Log Stopped")
        return self.__com_manager.stop_log(d_id)
    
    def save_config(self, d_id):
        message = PnPLCMDManager.create_command_cmd("log_controller","save_config")
        return self.send_command(d_id, message)
    
    def save_json_device_file(self, d_id: int, out_acq_path = None):
        json_save_path = self.__acquisition_folder
        if out_acq_path is not None:
            if not os.path.exists(out_acq_path):
                os.makedirs(out_acq_path)
            json_save_path = out_acq_path
        try:
            res = self.get_device_status(d_id)
            if res is not None:
                for i, c in enumerate(res["devices"][0]["components"]):
                    if "acquisition_info" in c:
                        acq_uuid = c["acquisition_info"]["uuid"]
                        res["devices"][0]["components"].pop(i)
                        res["uuid"] = acq_uuid
                device_status_filename = os.path.join(json_save_path, "device_config.json")
                sensor_data_file = open(device_status_filename, "w+")
                sensor_data_file.write(json.dumps(res, indent = 4))
                sensor_data_file.close()
                log.info("device_config.json Configuration file correctly saved")
                return True
        except:
            raise
    
    def save_json_acq_info_file(self, d_id: int, out_acq_path = None):
        json_save_path = self.__acquisition_folder
        if out_acq_path is not None:
            if not os.path.exists(out_acq_path):
                os.makedirs(out_acq_path)
            json_save_path = out_acq_path
        try:
            res = self.get_acquisition_info(d_id)
            if res is not None:
                acq_info_filename = os.path.join(json_save_path,"acquisition_info.json")
                acq_info_file = open(acq_info_filename, "w+")

                acq_info_file.write(json.dumps(res["acquisition_info"], indent = 4))
                acq_info_file.close()
                log.info("acquisition_info.json file correctly saved")
                return True
        except:
            raise

    def update_device(self, d_id:int, device_json_file_path): #device_config.json
        if self.__dt_manager is not None:
            return self.__com_manager.update_device(d_id, device_json_file_path, self.__dt_manager.get_components())
        else:
            print("HSD_link does not have an associated Device Template")
            pres_res = self.get_device_presentation_string(d_id)
            if pres_res is not None:
                board_id = hex(pres_res["board_id"])
                fw_id = hex(pres_res["fw_id"])
                dev_template_json = DeviceTemplateManager.query_dtdl_model(board_id, fw_id)
                self.__dt_manager = DeviceTemplateManager(dev_template_json)
                self.__com_manager.update_device(d_id, device_json_file_path, self.__dt_manager.get_components())
                log.info("Device Template automatically loaded")
            else:
                log.error("No Device Template loaded")
                raise MissingDeviceModelError

    def upload_mlc_ucf_file(self, d_id:int, comp_name:str, ucf_file_path):
        with open(ucf_file_path, "r") as f:
            lines = f.readlines()
        lines = [line.replace(' ', '') for line in lines]
        lines = [line.replace('\n', '') for line in lines]
        lines = list(filter(None, lines))
        ucf_buffer = ""
        for line in lines:
            if line == '' or line.startswith('--'):
                pass
            elif "WAIT" in line:
                time = line[4:]
                time_digit = len(time)
                missing_digit = 3-time_digit
                for i in range(missing_digit):
                    time = "0" + time
                time = "W" + time
                ucf_buffer = ucf_buffer + time
            else:
                ucf_buffer = ucf_buffer + line[2:]
        ucf_size = len(ucf_buffer)
        ucf = {"size" : ucf_size, "data" : ucf_buffer}
        message = PnPLCMDManager.create_command_cmd(comp_name,"load_file","ucf_data", ucf)
        res = self.send_command(d_id, message)
        if res is not None:
            print("UCF file [\"{}\"] loaded correctly in {} Component".format(ucf_file_path, comp_name))
        else:
            log.error("Error loading UCF file [\"{}\"] in {} Component".format(ucf_file_path, comp_name))
        return res
    
    def upload_ispu_ucf_file(self, d_id:int, comp_name:str, ucf_file_path, output_json_file_path = None):
        with open(ucf_file_path, "r") as f:
            lines = f.readlines()
        lines = [line.replace(' ', '') for line in lines]
        lines = [line.replace('\n', '') for line in lines]
        lines = list(filter(None, lines))
        ucf_buffer = ""
        for line in lines:
            if line == '' or line.startswith('--'):
                pass
            elif "WAIT" in line:
                time = line[4:]
                time_digit = len(time)
                missing_digit = 3-time_digit
                for i in range(missing_digit):
                    time = "0" + time
                time = "W" + time
                ucf_buffer = ucf_buffer + time
            else:
                ucf_buffer = ucf_buffer + line[2:]
        ucf_size = len(ucf_buffer)
        #NOTE OUTPUT FORMAT JSON File # It will be available in next release
        # with open(output_json_file_path, "r") as f:
        #     ojf = f.read()
        out_json_size = 10
        out_json_data = ""
        cmd_value = {"ucf_size" : ucf_size, "ucf_data" : ucf_buffer, "output_size" : out_json_size, "output_data" : out_json_data}
        message = PnPLCMDManager.create_command_cmd(comp_name,"load_file","files", cmd_value)
        res = self.send_command(d_id, message)
        if res is not None:
            print("UCF file [\"{}\"] loaded correctly in {} Component".format(ucf_file_path, comp_name))
        else:
            log.error("Error loading UCF file [\"{}\"] in {} Component".format(ucf_file_path, comp_name))
        return res


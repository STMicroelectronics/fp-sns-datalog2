# *****************************************************************************
#  * @file    HSDatalog_v2.py
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

import math

from datetime import datetime
from dateutil import parser
import json
import os
import struct
from collections import OrderedDict
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from mpl_toolkits.axes_grid1.inset_locator import inset_axes
from matplotlib.widgets import CheckButtons, Slider
import numpy as np
import pandas as pd

from st_hsdatalog.HSD.utils.plot_utils import PlotUtils
from st_hsdatalog.HSD_utils.exceptions import *
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.utils.cli_interaction import CLIInteraction as CLI
from st_hsdatalog.HSD.utils.file_manager import FileManager
from st_hsdatalog.HSD.utils.type_conversion import TypeConversion
from st_pnpl.DTDL.dtdl_utils import UnitMap
from st_pnpl.DTDL.device_template_manager import DeviceTemplateManager
from st_pnpl.DTDL.device_template_model import ContentSchema, SchemaType
from st_pnpl.DTDL.dtdl_utils import DTDL_SENSORS_ID_COMP_KEY, MC_FAST_TELEMETRY_COMP_NAME, MC_SLOW_TELEMETRY_COMP_NAME, AlgorithmTypeEnum, ComponentTypeEnum, SensorCategoryEnum

log = logger.get_logger(__name__)

class HSDatalog_v2:
    # Class attributes that will be used to store device model, acquisition info model,
    # and ISPU output format (if any).
    device_model = None
    acq_info_model = None
    ispu_output_format = None
    # Private attribute to store the path to the acquisition folder.
    __acq_folder_path = None
    __checkTimestamps = False
    
    def __init__(self, acquisition_folder = None):
        """
        Constructor method for initializing an instance of HSDatalog_v2.

        :param acquisition_folder: [Optional] The path to the folder where acquisition data is stored.
        """
        # If an acquisition folder is provided, proceed with initialization.
        if acquisition_folder is not None:
            # Attempt to find and load the device configuration from the acquisition folder.
            device_json_file_path = FileManager.find_file("device_config.json", acquisition_folder)
            if device_json_file_path is None:
                # If the device configuration file is missing, raise an error.
                raise MissingSensorModelError
            self.__load_device_from_file(device_json_file_path)
            # Attempt to find and load the acquisition information from the acquisition folder.
            try:
                acquisition_json_file_path = FileManager.find_file("acquisition_info.json", acquisition_folder)
                if acquisition_json_file_path is None:
                    # If the acquisition information file is missing, raise an error.
                    raise MissingAcquisitionInfoError
                self.__load_acquisition_info_from_file(acquisition_json_file_path)
                
                # Attempt to find and load the ISPU output format from the acquisition folder.
                ispu_output_json_file_path = FileManager.find_file("ispu_output_format.json", acquisition_folder)
                if ispu_output_json_file_path is not None:
                    self.__load_ispu_output_format(ispu_output_json_file_path)
                    
            except MissingAcquisitionInfoError:
                # Log an error and raise it if the acquisition information file is missing.
                log.error("No acquisition_info.json file in your Acquisition folder")
                raise
            except MissingDeviceModelError:
                # Raise an error if the device model file is missing.
                raise
        else:
            # If no acquisition folder is provided, log a warning and set the device model and acquisition info to None.
            log.warning("Acquisition folder not provided.")
            self.device_model = None
            self.acq_info_model = None

        # Store the acquisition folder path in a private attribute.
        self.__acq_folder_path = acquisition_folder
        # Data integrity ptocol counter byte size
        self.data_protocol_size = 4
        # A list of colors to be used for line plotting, for example in a graph.
        # self.lines_colors = ['#e6007e', '#a4c238', '#3cb4e6', '#ef4f4f', '#46b28e', '#e8ce0e', '#60b562', '#f99e20', '#41b3ba']
    
    #========================================================================================#
    ### Data Analisys ########################################################################
    #========================================================================================#

    ### ==> Debug ###
    def enable_timestamp_recovery(self, status):
        """Enable timestamp recovery algorithm.

        Args:
            status (bool): True to enable, False elsewhere
        """
        self.__checkTimestamps = status
    ### Debug <== ###
    
    def __load_device_from_file(self, device_json_file_path, device_id = 0):
        """Function to load a device_config.json file (Device Current Status)

        Args:
            device_json_file_path (str): device_config.json path

        Raises:
            MissingDeviceModelError: Exception returned if an error occour in device_config.json loading
        """        
        try:
            with open(device_json_file_path, encoding="UTF-8") as f:
                file_content = f.read()
                if file_content[-1] == '\x00':
                    device_json_dict = json.loads(file_content[:-1])
                else:
                    device_json_dict = json.loads(file_content)
            device_json_str = json.dumps(device_json_dict)
            f.close()
            device_dict = json.loads(device_json_str)
            self.__load_device(device_dict, device_id, True)
        except MissingDeviceModelError as e:
            raise e

    def __load_acquisition_info_from_file(self, acq_info_json_file_path):
        """Function to load a acquisition_info.json file (Acquisition_Info Component Current Status)

        Args:
            acq_info_json_file_path ([str]): acquisition_info.json path

        Raises:
            MissingAcquisitionInfoError: Exception returned if an error occour in acqusition_info.json loading
        """
        try:
            with open(acq_info_json_file_path) as f:
                file_content = f.read()
                if file_content[-1] == '\x00':
                    acq_info_json_dict = json.loads(file_content[:-1])
                else:
                    acq_info_json_dict = json.loads(file_content)
            acq_info_json_str = json.dumps(acq_info_json_dict)
            f.close()
            self.acq_info_model = json.loads(acq_info_json_str)
        except:
            raise MissingAcquisitionInfoError

    def __get_sensor_unit_from_dtdl(self, prop_w_unit_name, comp_dtdl_contents):
        property = [c for c in comp_dtdl_contents if c.name == prop_w_unit_name]
        if property is not None and len(property) > 0:
            unit = property[0].unit
            if unit is not None:
                return unit
            display_unit = property[0].display_unit
            if display_unit is not None:
                return display_unit if isinstance(display_unit, str) else display_unit.en
        return None
    
    def __load_ispu_output_format(self, ispu_output_format_file_path):
        """Function to load a ispu_output_format.json file

        Args:
            ispu_output_format_file_path ([str]): ispu_output_format.json path
        """
        with open(ispu_output_format_file_path) as f:
            file_content = f.read()
            if file_content[-1] == '\x00':
                ispu_out_json_dict = json.loads(file_content[:-1])
            else:
                ispu_out_json_dict = json.loads(file_content)
        ispu_out_json_str = json.dumps(ispu_out_json_dict)
        f.close()
        self.ispu_output_format = json.loads(ispu_out_json_str)

    @staticmethod
    def __convert_prop_enum_in_value(prop_enum_index, prop_content):
        enum_dname = prop_content.schema.enum_values[prop_enum_index].display_name
        value = enum_dname if isinstance(enum_dname,str) else enum_dname.en
        if prop_content.schema.value_schema.value == "integer":
            num_value = value.replace(',','').replace('.','')
            if num_value.isnumeric():
                return float(value.replace(',','.'))
        return value

    def __convert_enums_in_values(self, comp_dtdl_contents, comp_status):
        for property in comp_dtdl_contents:
            if property.schema is not None and isinstance(property.schema, ContentSchema) and property.schema.type == SchemaType.ENUM :
                enum_index = comp_status[property.name]
                prop_value = self.__convert_prop_enum_in_value(enum_index, property)
                comp_status[property.name] = prop_value
    
    def __load_device(self, device_dict, device_id = 0, from_file = True):
        
        if from_file:
            self.device_model = device_dict['devices'][device_id]
        else:
            self.device_model = device_dict

        board_id = hex(self.device_model["board_id"])
        fw_id = hex(self.device_model["fw_id"])
        
        dev_template_json = DeviceTemplateManager.query_dtdl_model(board_id, fw_id)
        if isinstance(dev_template_json,dict):
            if dev_template_json == {}:
                raise MissingDeviceModelError(board_id,fw_id)
            components = self.device_model.get("components")
            fw_name = None
            for c in components:
                if c.get("firmware_info") is not None:
                    fir_info = c.get("firmware_info")
                    if fir_info.get("fw_name") is not None:
                        fw_name = fir_info.get("fw_name")
            # fw_name = self.device_model.get("components").get("firmware_info").get("fw_name")
            if fw_name is not None:
                splitted_fw_name = fw_name.lower().split("-")
                reformatted_fw_name = "".join([splitted_fw_name[0]] + [f.capitalize() for f in splitted_fw_name[1:]])
                for dt in dev_template_json:
                    if reformatted_fw_name.lower() in  dev_template_json[dt][0].get("@id").lower():
                        dev_template_json = dev_template_json[dt]
                        break
        dt_manager = DeviceTemplateManager(dev_template_json)
        self.components_dtdl = dt_manager.get_components()
        for comp_name in self.components_dtdl.keys():
            comp_status = [c for c in self.device_model.get("components") if list(c.keys())[0] == comp_name]
            if len(comp_status)>0:
                comp_status = comp_status[0].get(comp_name)
                comp_dtdl_contents = [c for c in self.components_dtdl[comp_name].contents]
                c_type = comp_status.get("c_type")
                if c_type == ComponentTypeEnum.SENSOR.value:
                    s_category = comp_status.get("sensor_category")
                    if s_category == SensorCategoryEnum.ISENSOR_CLASS_MEMS.value:
                        comp_status["unit"] = self.__get_sensor_unit_from_dtdl("fs", comp_dtdl_contents)
                        if from_file:
                            self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_AUDIO.value:
                        comp_status["unit"] = self.__get_sensor_unit_from_dtdl("aop", comp_dtdl_contents)
                        if from_file:
                            self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
                        if from_file:
                            self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                        if from_file:
                            self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_CAMERA.value:
                        pass
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_PRESENCE.value:
                        if from_file:
                            self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_POWERMETER.value:
                        if from_file:
                            self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    else: #Retrocompatibility
                        if ":"+DTDL_SENSORS_ID_COMP_KEY+":" in self.components_dtdl[comp_name].id: #"sensors":
                            s_info_contents = [c for c in self.components_dtdl[comp_name].contents if c.name == "odr" or c.name == "fs" or c.name == "aop"]
                            comp_status = [x for x in self.device_model["components"] if list(x.keys())[0] == comp_name]
                            if len(comp_status) > 0:
                                if len(s_info_contents) > 0:
                                    for sc in s_info_contents:
                                        if (sc.name == "fs" or sc.name == "aop") and sc.unit is not None:
                                            comp_status[0][comp_name]["unit"] = sc.unit
                                        elif (sc.name == "fs" or sc.name == "aop") and sc.display_unit is not None:
                                            display_unit = sc.display_unit if isinstance(sc.display_unit, str) else sc.display_unit.en
                                            comp_status[0][comp_name]["unit"] = display_unit
                                        if from_file and sc.schema is not None and isinstance(sc.schema, ContentSchema) and sc.schema.type == SchemaType.ENUM :
                                            enum_index = comp_status[0][comp_name][sc.name]
                                            prop_value = float(self.__convert_prop_enum_in_value(enum_index, sc))
                                            comp_status[0][comp_name][sc.name] = prop_value
                                else:
                                    if "unit" not in comp_status[0][comp_name]:
                                        comp_status[0][comp_name]["unit"] = ""
        log.debug(f"Device Model: {self.device_model}")

    def get_data_protocol_size(self):
        return self.data_protocol_size

    def get_acquisition_path(self):
        return self.__acq_folder_path

    def get_device(self):
        """
        Retrieves the current device model from the HSDatalog instance.
        This method returns the device model that has been set for the instance, which contains information about the device.

        :return: The device model object or dictionary that holds the device's information.
        """
        # Return the device_model attribute of the HSDatalog instance.
        # This attribute should hold the current device's information.
        return self.device_model

    def set_device(self, new_device, device_id = 0, from_file = True):
        """
        Sets the device model for the HSDatalog instance and loads the device configuration.

        This method updates the device model with the new device information provided and calls an internal method to load the device configuration.
        It also logs information about the firmware, including the alias, firmware name, version, and serial or part number if available.
        This function allows to change the current HSD Device Status
            e.g.: if using hsd_link you obtain the current device template directly from the board, you can set it in your HSDatalog instance.

        :param new_device: A dictionary containing the new device information, typically obtained from a device template or directly from the board.
        """
        
        if from_file:
            # Update the device_model attribute with the first device from the new_device dictionary
            self.device_model = new_device['devices'][device_id]
        else:
            self.device_model = new_device
         # Call the private method '__load_device' to load the device configuration
        self.__load_device(new_device, device_id, from_file)
        # Retrieve firmware information from the device
        fw_info = self.get_firmware_info()["firmware_info"]
        # Log the device information, including alias, firmware name, and version
        # Check if part number is available and include it in the log message
        if "part_number" in fw_info:
            log.info("Device [{}] - {} v{} sn:{} loaded correctly!".format(fw_info['alias'], fw_info['fw_name'], fw_info['fw_version'], fw_info['part_number']))
        # If part number is not available, check for serial number and include it in the log message
        elif "serial_number" in fw_info:
            log.info("Device [{}] - {} v{} sn:{} loaded correctly!".format(fw_info['alias'], fw_info['fw_name'], fw_info['fw_version'], fw_info['serial_number']))
         # If neither part number nor serial number is available, log the information without them
        else:
            log.info("Device [{}] - {} v{} loaded correctly!".format(fw_info['alias'], fw_info['fw_name'], fw_info['fw_version']))

    def get_device_info(self):
        """
        Retrieves the device information component from the HSDatalog instance.
        This method calls another method, `get_component`, with the argument "DeviceInformation" to obtain detailed information about the device.
        The "DeviceInformation" component typically includes metadata such as the device's name, type, serial number, firmware version, and other relevant details.

        :return: The "DeviceInformation" component of the device, which is a dictionary or object containing detailed device information.
        """
        # Call the 'get_component' method with the argument "DeviceInformation" to retrieve the device information.
        # The 'get_component' method is expected to be implemented elsewhere in the HSDatalog class and should return the requested component.
        return self.get_component("DeviceInformation")
    
    #HSD2 only
    def get_firmware_info(self):
        """This fuction returns the current Status of the firmware_info Component
           DTDL Component name: firmware_info
        Returns:
            dict: firmware_info Component current Status if it exists, None elsewhere
        """      
        return self.get_component("firmware_info")

    #HSD2 here new_device_info is a json(dict) --> put new_device:DeviceInfo only in HSDv1 function definition
    def set_device_info(self, new_device_info):
        """AI is creating summary for set_device_info

        Args:
            new_device_info ([type]): [description]
        """        
        self.device_info = new_device_info

    def get_component(self, component_name):
        """This fuction returns the current Status of the {comp_name} Component

        Args:
            component_name (str): DTDL Component name

        Raises:
            MissingComponentModelError: Exception raised if the {comp_name} Component does not exist
            MissingDeviceModelError: Exception raised if the current device status does not exist

        Returns:
            dict: {comp_name} Component current Status if it exists, None elsewhere
        """        
        if self.device_model is not None:
            components = self.device_model['components']
            for c in components:
                if list(c)[0] == component_name:
                    return c
            log.error("No Model loaded for {} Component".format(component_name))
            raise MissingComponentModelError
        else:
            log.error("No Device Model loaded!")
            raise MissingDeviceModelError
    
    #missing
    #def get_sub_sensor(self, sensor_name, ss_id = None, ss_type = None):
    
    #missing
    #def get_sub_sensors(self, sensor_name, only_active = False):
    
    def get_sensor_list(self, type_filter = "", only_active = False):
        active_sensors = []
        sensor_list = self.device_model['components']
        for s in sensor_list:
            for element in s[list(s)[0]]:
                if element == 'c_type' and s[list(s)[0]]['c_type'] == ComponentTypeEnum.SENSOR.value:
                    if type_filter == "":
                        if only_active:
                            if "enable" in s[list(s)[0]] and s[list(s)[0]]['enable'] == True:
                                active_sensors.append(s)
                        else:
                            active_sensors.append(s)
                    else:
                        sensor_type = str(list(s)[0]).lower().split("_")[-1]
                        if only_active:
                            if "enable" in s[list(s)[0]] and s[list(s)[0]]['enable'] == True and sensor_type == type_filter.lower():
                                active_sensors.append(s)
                        else:
                            if sensor_type == type_filter.lower():
                                active_sensors.append(s)
        return active_sensors

    def get_algorithm_list(self, type_filter = "", only_active = False):
        active_algos = []
        algo_list = self.device_model['components']
        for s in algo_list:
            for element in s[list(s)[0]]:
                if element == 'c_type' and s[list(s)[0]]['c_type'] == ComponentTypeEnum.ALGORITHM.value:
                    if type_filter == "":
                        if only_active:
                            if "enable" in s[list(s)[0]] and s[list(s)[0]]['enable'] == True:
                                active_algos.append(s)
                        else:
                            active_algos.append(s)
                    else:
                        sensor_type = str(list(s)[0]).lower().split("_")[-1]
                        if only_active:
                            if "enable" in s[list(s)[0]] and s[list(s)[0]]['enable'] == True and sensor_type == type_filter.lower():
                                active_algos.append(s)
                        else:
                            if sensor_type == type_filter.lower():
                                active_algos.append(s)
        return active_algos
    
    def get_actuator_list(self, only_active = False):
        active_actuators = []
        actuator_list = self.device_model['components']
        for ac in actuator_list:
            for element in ac[list(ac)[0]]:
                if element == 'c_type' and ac[list(ac)[0]]['c_type'] == ComponentTypeEnum.ACTUATOR.value:
                    if only_active:
                        if "enable" in ac[list(ac)[0]] and ac[list(ac)[0]]['enable'] == True:
                            active_actuators.append(ac)
                    else:
                        active_actuators.append(ac)
        return active_actuators

    def get_sw_tag_classes(self):
        if self.device_model is not None:
            tags_info_dict = self.get_component("tags_info")
            if tags_info_dict is not None:
                return {key: value for key, value in tags_info_dict['tags_info'].items() if "sw_tag" in key}
            else:
                return {}

    def get_hw_tag_classes(self):
        if self.device_model is not None:
            tags_info_dict = self.get_component("tags_info")
            if tags_info_dict is not None:
                return {key: value for key, value in tags_info_dict['tags_info'].items() if "hw_tag" in key}
            else:
                return None

    def get_acquisition_info(self):
        return self.acq_info_model

    #HSD2 here new_device_info is a json(dict) --> put new_device:DeviceInfo only in HSDv1 function definition
    def set_acquisition_info(self, new_acquisition_info):
        self.acq_info_model = new_acquisition_info

    def get_acquisition_interface(self):
        return self.acq_info_model['interface']
    
    def get_acquisition_label_classes(self):
        if self.acq_info_model is not None:
            if "tags" in self.acq_info_model:
                return sorted(set(dic['l'] for dic in self.acq_info_model['tags']))
            else:
                log.warning("No defined tag classes in Acquisition Information Component.")
                return None
        log.warning("Empty Acquisition Info model.")
        # raise MissingAcquisitionInfoError
        return None

    def get_time_tags(self, which_tags = None):
        time_labels = []
        if self.acq_info_model is not None:
            acq_start_time = self.acq_info_model['start_time']
            acq_end_time = self.acq_info_model['end_time']
            self.s_t = datetime.strptime(acq_start_time, '%Y-%m-%dT%H:%M:%S.%fZ')
            tags = self.acq_info_model['tags']

            if which_tags is not None:
                tags = [tag for tag in tags if tag['l'] in which_tags]

            for lbl in self.get_acquisition_label_classes():
                # start_time, end_time are vectors with the corresponding 't' entries in DataTag-json
                start_time = np.array([t['ta'] for t in tags if t['l'] == lbl and t['e']])
                end_time = np.array([t['ta'] for t in tags if t['l'] == lbl and not t['e']])
                # now must associate at each start tag the appropriate end tag
                # (some may be missing because of errors in the tagging process)
                for tstart in start_time:
                    tag = {}
                    jj = [i for (i, n) in enumerate(end_time) if n >= tstart]
                    if jj:
                        tend = end_time[min(jj)]
                    else:
                        tend = acq_end_time  # if no 'end tag' found the end is eof
                    tag['label'] = lbl
                    tag['time_start'] = (datetime.strptime(tstart, '%Y-%m-%dT%H:%M:%S.%fZ') - self.s_t).total_seconds()
                    tag['time_end'] = (datetime.strptime(tend, '%Y-%m-%dT%H:%M:%S.%fZ') - self.s_t).total_seconds()
                    time_labels.append(tag)
            return time_labels
        else:
            log.error("Empty Acquisition Info model.")
            raise MissingAcquisitionInfoError

    # Helper function to convert ISO8601 time strings to seconds
    def get_seconds_from_ISO8601(self, start_time_str, end_time_str):
        start_time = parser.isoparse(start_time_str)
        end_time = parser.isoparse(end_time_str)
        duration = (end_time - start_time).total_seconds()
        return duration

    # Get tags dictionary list from acquisition_info.json file
    def get_tags(self):
        tags = []
        acq_start_time = self.acq_info_model["start_time"]
        acq_end_time = self.acq_info_model["end_time"]
        tags_array = self.acq_info_model["tags"]
        acq_duration = self.get_seconds_from_ISO8601(acq_start_time, acq_end_time)

        tag_labels = []
        for i in range(len(tags_array)):
            start_tag = tags_array[i]
            if start_tag["e"]:
                tag_label = start_tag["l"]
                tag_start_time = start_tag["ta"]
                tag_start_seconds = self.get_seconds_from_ISO8601(acq_start_time, tag_start_time)
                tag_end_seconds = acq_duration
                for j in range(i, len(tags_array)):
                    end_tag = tags_array[j]
                    if end_tag["l"] == tag_label and not end_tag["e"]:
                        tag_end_time = end_tag["ta"]
                        tag_end_seconds = self.get_seconds_from_ISO8601(acq_start_time, tag_end_time)
                        break
                if tag_label in tag_labels:
                    # Already present in tags list --> ADD tag_times!
                    for tag in tags:
                        if tag["label"] == tag_label:
                            tag["times"].append((tag_start_seconds, tag_end_seconds))
                else:
                    tag_labels.append(tag_label)
                    tags.append({"label": tag_label, "times": [(tag_start_seconds, tag_end_seconds)]})

        return tags
    
    def __get_active_mc_telemetries_names(self, ss_stat, comp_name):
        if comp_name == "slow_mc_telemetries":
            desc_telemetry = ss_stat.get("st_ble_stream")
            return [st for st in desc_telemetry if isinstance(desc_telemetry[st],dict) and desc_telemetry[st].get("enable")==True]
        elif comp_name == "fast_mc_telemetries":
            return [k for k in ss_stat.keys() if isinstance(ss_stat[k],dict) and ss_stat[k]["enabled"] == True]
    
    def __process_datalog(self, sensor_name, ss_stat, raw_data, dataframe_size, timestamp_size, raw_flag = False, start_time = None, prev_timestamp = None):

        #####################################################################
        def extract_data_and_timestamps(start_time):
        
            """ gets data from a file .dat
                np array with one column for each axis of each active subSensor
                np array with sample times
            """
            sensor_name_contains_mlc_ispu = "_mlc" in sensor_name or "_ispu" in sensor_name
            
            if c_type == ComponentTypeEnum.SENSOR.value:
                s_category = ss_stat.get("sensor_category")
                if self.__checkTimestamps == True:
                    check_timestamps = not sensor_name_contains_mlc_ispu
                else:
                    check_timestamps = False
                if s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                   frame_period = 0 if sensor_name_contains_mlc_ispu else samples_per_ts / (1/(ss_stat.get("intermeasurement_time")/1000))
                elif s_category == SensorCategoryEnum.ISENSOR_CLASS_POWERMETER.value:
                   frame_period = 0 if sensor_name_contains_mlc_ispu else samples_per_ts / (1/(ss_stat.get("adc_conversion_time")/1000000))
                else:
                    measodr = ss_stat.get("measodr")
                    if measodr is None or measodr == 0:
                        measodr = ss_stat.get("odr")
                    frame_period = 0 if sensor_name_contains_mlc_ispu else samples_per_ts / measodr
            elif c_type == ComponentTypeEnum.ALGORITHM.value:
                check_timestamps = False
                if ss_stat.get("algorithm_type") == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                    fft_sample_freq = ss_stat.get("fft_sample_freq")
                    frame_period = samples_per_ts / fft_sample_freq
                else:
                    frame_period = 0
                algo_type = ss_stat.get("algorithm_type")
            elif c_type == ComponentTypeEnum.ACTUATOR.value:
                check_timestamps = False
                frame_period = 0

            # rndDataBuffer = raw_data rounded to an integer # of frames
            rnd_data_buffer = raw_data[:int(frame_size * num_frames)]

            if start_time != 0:
                timestamp_first = start_time #TODO check with spts != 0
            else:
                timestamp_first = ss_stat.get('ioffset', 0)
            timestamps = []
            data_type = TypeConversion.get_np_dtype(data_type_string)
            data = np.zeros((data1D_per_frame * num_frames, 1), dtype=data_type)

            if timestamp_size != 0:
                for ii in range(num_frames):  # For each Frame:
                    start_frame = ii * frame_size
                    # segment_data = data in the current frame
                    segment_data = rnd_data_buffer[start_frame:start_frame + dataframe_size]
                    # segment_tS = ts is at the end of each frame
                    segment_ts = rnd_data_buffer[start_frame + dataframe_size:start_frame + frame_size]

                    # timestamp of current frame
                    timestamps.append(np.frombuffer(segment_ts, dtype='double')[0])

                    # Data of current frame
                    data_range = slice(ii * data1D_per_frame, (ii + 1) * data1D_per_frame)
                    data[data_range, 0] = np.frombuffer(segment_data, dtype=data_type)

                    # Check Timestamp consistency
                    if check_timestamps and ii > 0:
                        delta_ts = abs(timestamps[ii] - timestamps[ii - 1])
                        if delta_ts < 0.1 * frame_period or delta_ts > 10 * frame_period or np.isnan(timestamps[ii]) or np.isnan(timestamps[ii - 1]):
                            data[data_range, 0] = 0
                            timestamps[ii] = timestamps[ii - 1] + frame_period
                            log.warning("Sensor {}: corrupted data at {}".format(sensor_name, "{} sec".format(timestamps[ii])))
            else:
                data = np.frombuffer(rnd_data_buffer, dtype=data_type)
                is_first_chunk = ss_stat.get("is_first_chunk", False)
                if is_first_chunk:
                    start_time = timestamp_first
                    stop_time = timestamp_first + (num_frames * frame_period)
                else:
                    start_time = timestamp_first + frame_period
                    stop_time = timestamp_first + frame_period + (num_frames * frame_period)
                
                timestamps = np.arange(
                    start=start_time,
                    stop=stop_time,
                    step=frame_period,
                    dtype=np.float64
                )

                timestamps = timestamps[:num_frames]

            if c_type == ComponentTypeEnum.SENSOR.value:
                s_dim = ss_stat.get('dim',1)
                if raw_flag:
                    s_data = np.reshape(data, (-1, 64 if "_ispu" in sensor_name else s_dim))
                else:
                    s_data = np.reshape(data, (-1, 64 if "_ispu" in sensor_name else s_dim)).astype(dtype=np.byte if "_ispu" in sensor_name else np.float32)
            elif c_type == ComponentTypeEnum.ALGORITHM.value:
                if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                    s_data = np.reshape(data, (-1, ss_stat['fft_length'])).astype(dtype=np.float32)
            elif c_type == ComponentTypeEnum.ACTUATOR.value:
                if sensor_name == MC_SLOW_TELEMETRY_COMP_NAME or sensor_name == MC_FAST_TELEMETRY_COMP_NAME:
                    nof_telemetries = len(self.__get_active_mc_telemetries_names(ss_stat, sensor_name))
                    s_data = np.reshape(data, (-1, nof_telemetries)).astype(dtype=np.float32)
            
            if not raw_flag:
                sensitivity = float(ss_stat.get('sensitivity', 1))
                np.multiply(s_data, sensitivity, out = s_data, casting='unsafe')
            
            if len(data) == 0:
                return [],[]
            
            # if c_type == ComponentTypeEnum.SENSOR.value or c_type == ComponentTypeEnum.ALGORITHM.value:
            # samples_time: numpy array of 1 clock value per each data sample
            if samples_per_ts > 1:
                # initial_offset is relevant
                frames = num_frames
                ioffset = ss_stat.get('ioffset', 0)
                is_first_chunk = ss_stat.get("is_first_chunk", False)

                if start_time != 0 and is_first_chunk:
                    if prev_timestamp is not None:
                        timestamps = np.insert(timestamps, 0, prev_timestamp)
                    else:            
                        timestamps = np.insert(timestamps, 0, ioffset)
                else:
                    timestamps = np.append(ioffset, timestamps)

                ss_stat["ioffset"] = timestamps[-1] #NOTE! Update the ioffset with the last extracted timestamp to allow eventual batch processing (this will be the start timestamp to continue the linear interpolation for the next chunk)
                samples_time = np.zeros((frames * samples_per_ts, 1))
                # sample times between timestamps are linearly interpolated
                for ii in range(frames): # For each Frame:
                    samples_time[ii * samples_per_ts:(ii + 1) * samples_per_ts, 0] = np.linspace(timestamps[ii], timestamps[ii + 1], samples_per_ts, endpoint=False)
            else:
                # if samples_per_ts is 1, the timestamps coincides with the sample timestamp
                # initial offset and interpolation is not relevant anymore
                samples_time = np.array(timestamps).reshape(-1, 1)
                if len(timestamps) > 0:
                    ss_stat["ioffset"] = timestamps[-1] #NOTE! Update the ioffset with the last extracted timestamp to allow eventual batch processing (this will be the start timestamp to continue the linear interpolation for the next chunk)

            return s_data, samples_time
        #####################################################################
        
        c_type = ss_stat.get("c_type")

        # size of the frame. A frame is data + ts
        frame_size = dataframe_size + timestamp_size

        # number of frames = round down (//) len datalog // frame_size
        num_frames = len(raw_data) // frame_size
        
        # force int8 data_type for ISPU
        data_type_string = "int8" if "_ispu" in sensor_name else ss_stat['data_type']
        data_type_byte_num = TypeConversion.check_type_length(data_type_string)

        # data1D_per_frame = number of data samples in 1 frame
        # must be the same as samplePerTs * number of axes
        data1D_per_frame = int(dataframe_size / data_type_byte_num)

        #samples per timestamp
        spts = ss_stat.get('samples_per_ts', {})
        if isinstance(spts, int):
            samples_per_ts = spts
        else:
            samples_per_ts = spts.get('val', 0)
        
        if c_type == ComponentTypeEnum.SENSOR.value:
            samples_per_ts = samples_per_ts or int(data1D_per_frame / ss_stat.get('dim', 1))
        if c_type == ComponentTypeEnum.ALGORITHM.value:
            algo_type = ss_stat.get("algorithm_type")
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                samples_per_ts = 1#samples_per_ts or int(data1D_per_frame / ss_stat.get('dim', 1))
        elif c_type == ComponentTypeEnum.ACTUATOR.value:
            if sensor_name == "slow_mc_telemetries":
                desc_telemetry = ss_stat.get("st_ble_stream")
                nof_telemetries = len([st for st in desc_telemetry if isinstance(desc_telemetry[st],dict) and desc_telemetry[st].get("enable")==True])
            elif sensor_name == "fast_mc_telemetries":
                nof_telemetries = len([k for k in ss_stat.keys() if isinstance(ss_stat[k],dict) and ss_stat[k]["enabled"] == True])
            samples_per_ts = dataframe_size // data_type_byte_num // nof_telemetries

        return extract_data_and_timestamps(start_time)
    
    def get_sensor(self, sensor_name):
        ss_stat = self.get_component(sensor_name)
        return ss_stat
    
    def __get_sensor_status(self, sensor_name):
        ss_stat = self.get_component(sensor_name)
        return ss_stat[sensor_name]
    
    def __get_sensor_file_path(self, sensor_name):
        file_path = os.path.join(self.__acq_folder_path, FileManager.encode_file_name(sensor_name))
        if not os.path.exists(file_path):
            log.error("No such file or directory: {} found for {} sensor".format(file_path, sensor_name))
            raise MissingFileForSensorError(file_path, sensor_name)
        return file_path
    
    def __get_checked_sensor_file_path(self, sensor_name):
        file_path = os.path.join(self.__acq_folder_path, FileManager.encode_file_name(sensor_name + "_checked"))
        if not os.path.exists(file_path):
            log.error("No such file or directory: {} found for {} sensor".format(file_path, sensor_name + "_checked"))
            raise MissingFileForSensorError(file_path, sensor_name)
        return file_path

    def remove_4bytes_every_n_optimized(self, arr, N):
        # Create a boolean mask for the elements to keep
        mask = np.ones(len(arr), dtype=bool)
        for start in range(0, len(arr), N):
            mask[start:start+self.data_protocol_size] = False

        # Apply the mask to get the new array
        new_arr = arr[mask]
        
        # Concatenate the remaining slices and return the result
        return new_arr

    def get_data_and_timestamps_batch(self, comp_name, comp_status, start_time = 0, end_time = -1, raw_flag = False):
        
        log.debug("Data & Timestamp extraction algorithm STARTED...")

        # get acquisition interface
        interface = self.acq_info_model['interface']
        
        c_type = comp_status.get("c_type")

        data_protocol_size = self.get_data_protocol_size()
        data_packet_size = 0
        # data packet size (0:sd card, 1:usb, 2:ble, 3:serial)
        if interface == 0:
            data_packet_size = comp_status["sd_dps"] - data_protocol_size
        elif interface == 1:
            data_packet_size = comp_status["usb_dps"]
        elif interface == 2:
            data_packet_size = comp_status["ble_dps"]
        elif interface == 3:
            data_packet_size = comp_status["serial_dps"]
        else:
            log.error(f"Unknown interface: {interface}. check your device_config.json file")
            raise
        
        # get dat file path and size (obtained from "sensor_name + sub_sensor_type")
        file_path = self.__get_sensor_file_path(comp_name)
        file_size = os.path.getsize(file_path)

        cmplt_pkt_size = data_packet_size + data_protocol_size
        nof_data_packet = file_size // cmplt_pkt_size # "//" math.floor equivalent #CEIL

        raw_data_array = np.array([], dtype='uint8')
        
        if c_type == ComponentTypeEnum.ALGORITHM.value:
            algo_type = comp_status.get("algorithm_type")
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                # get FFT algo "dimensions" --> FFT Length
                s_dim = comp_status.get("fft_length")
            else:
                s_dim = comp_status.get('dim')
        else:
            # get sensor dimensions
            s_dim = comp_status.get('dim', 1)
        
        # get Data type byte length
        s_data_type_len = TypeConversion.check_type_length(comp_status['data_type'])
        
        # get samples per ts
        spts = comp_status.get('samples_per_ts', {})
        if isinstance(spts, int):
            s_samples_per_ts = spts
        else:
            if c_type == ComponentTypeEnum.ACTUATOR.value:
                s_samples_per_ts = spts.get('val', 1)
            else:
                s_samples_per_ts = spts.get('val', 0)
        
        if c_type == ComponentTypeEnum.SENSOR.value:

            if s_samples_per_ts != 0:
                dataframe_byte_size = s_samples_per_ts * s_dim * s_data_type_len
                timestamp_byte_size = 8
            else:
                dataframe_byte_size = s_dim * s_data_type_len
                timestamp_byte_size = 0

            # 1 sec --> ODR samples --> ODR * dim * data_type Bytes
            odr = comp_status.get("measodr", comp_status.get("odr", 1))

            tot_counters_bytes = nof_data_packet * data_protocol_size
            tot_file_data_and_times_bytes = file_size - tot_counters_bytes
            tot_timestamps_bytes = (tot_file_data_and_times_bytes // (dataframe_byte_size+timestamp_byte_size))*timestamp_byte_size
            tot_data_bytes = tot_file_data_and_times_bytes - tot_timestamps_bytes
            tot_data_samples = int(tot_data_bytes/(s_data_type_len*s_dim))

            
            start_sample_idx = math.floor(odr*start_time)
            start_data_bytes_idx = start_sample_idx * s_data_type_len * s_dim
            if s_samples_per_ts != 0:                
                nof_timestamps_in_start = math.floor(start_sample_idx/s_samples_per_ts)
            else:
                nof_timestamps_in_start = 0
                sample_end = int(odr * end_time) if end_time != -1 else -1
                if sample_end > tot_data_samples or sample_end == -1:
                    sample_end = tot_data_samples
                sample_start = int(odr * start_time)
                if sample_start > tot_data_samples:
                    return ([],[])
                read_start_bytes = sample_start * dataframe_byte_size
                read_end_bytes = sample_end * dataframe_byte_size

            start_data_and_times_bytes_idx = start_data_bytes_idx + nof_timestamps_in_start * timestamp_byte_size
            nof_counter_in_start = math.floor(start_data_and_times_bytes_idx/data_packet_size)
            start_idx = start_data_and_times_bytes_idx + nof_counter_in_start * data_protocol_size

            if start_time == 8:
                pass

            last_timestamp = tot_data_samples/odr
            if end_time == -1 or end_time > last_timestamp:
                end_time = last_timestamp

            if start_time > last_timestamp:
                return [],[]
            
            byte_chest_index = 0
            raw_data_array_index = 0
            last_index = 0
            prev_timestamp = None
            nof_prev_timestamps = 0
            
            def __extract_data(start_time, end_time, nof_prev_timestamps):
                # Preallocate the byte_chest and raw_data_array with estimated sizes to avoid repeated reallocation
                estimated_size = (nof_data_packet +1) * (cmplt_pkt_size - data_protocol_size)
                byte_chest = np.empty(estimated_size, dtype='uint8')
                raw_data_array = np.empty(estimated_size, dtype='uint8')
                
                last_index = comp_status.get("last_index", 0)
                missing_bytes = comp_status.get("missing_bytes", 0)
                saved_bytes = comp_status.get("saved_bytes", 0)
                
                if last_index == 0 and start_idx != 0:
                    data_and_ts = nof_prev_timestamps * (dataframe_byte_size + timestamp_byte_size)
                    nof_counters = math.ceil(data_and_ts/data_packet_size)                    
                    packet_bytes = (nof_counters * data_protocol_size) + (nof_prev_timestamps * (dataframe_byte_size + timestamp_byte_size))
                    missing_bytes = math.ceil(packet_bytes/cmplt_pkt_size)*cmplt_pkt_size - packet_bytes
                    log.debug(f"User customized time boudaries: {start_time}, {end_time}")
                    log.debug(f"- packet_bytes: {packet_bytes}")
                    log.debug(f"- missing_bytes: {missing_bytes}")
                    last_index = packet_bytes

                byte_chest_index = 0
                raw_data_array_index = 0
                data_byte_counter = 0
                extracted_data_length = 0
                end_time_flag = False
                skip_counter_check = False
                prev_timestamp = None
                
                if start_idx >= file_size:
                    return [],None

                with open(file_path, 'rb') as f:
                    for n in range(nof_data_packet):
                        file_index = last_index + (n * cmplt_pkt_size)
                        log.debug(f"missing_bytes: {missing_bytes}")
                        log.debug(f"file_index: {file_index}")
                        if (file_index >= file_size):
                            return [],None
                        f.seek(file_index)
                        if last_index != 0:
                            n_counter_values = math.ceil((file_index + missing_bytes)/cmplt_pkt_size)
                            data_byte_counter = (file_index + missing_bytes) - n_counter_values * data_protocol_size
                            if saved_bytes != 0 and saved_bytes <= missing_bytes:
                                raw_data = f.read(missing_bytes)
                                log.debug(f"Bytes read from file: {missing_bytes}")
                                comp_status["is_same_dps"] = True
                                if len(raw_data) < missing_bytes:
                                    return [],None
                                data_bytes = raw_data[:missing_bytes]
                                counter_bytes = []
                                skip_counter_check = True
                            else:
                                raw_data = f.read(missing_bytes + cmplt_pkt_size)
                                comp_status["is_same_dps"] = False
                                log.debug(f"Bytes read from file: {missing_bytes + cmplt_pkt_size}")
                                if len(raw_data) < missing_bytes + cmplt_pkt_size:
                                    return [],None
                                data_bytes = raw_data[:missing_bytes] + raw_data[missing_bytes + data_protocol_size:]
                                counter_bytes = raw_data[missing_bytes:missing_bytes+data_protocol_size]
                            last_index += missing_bytes
                            comp_status["missing_bytes"] = missing_bytes = 0
                        else:
                            raw_data = f.read(cmplt_pkt_size)
                            log.debug(f"Bytes read from file: {cmplt_pkt_size}")
                            if len(raw_data) < cmplt_pkt_size:
                                return [],None
                            data_bytes = raw_data[data_protocol_size:]
                            counter_bytes = raw_data[:data_protocol_size]
                        
                        if not skip_counter_check:
                            counter = struct.unpack('<I', counter_bytes)[0]
                            log.debug(f"Extracted counter: {counter}")
                            log.debug(f"data_byte_counter: {data_byte_counter}")
                            if counter != data_byte_counter + data_packet_size:
                                raise DataCorruptedException(file_path)
                            data_byte_counter = counter

                        # Directly copy data into preallocated array
                        data_bytes_length = len(data_bytes)
                        byte_chest[byte_chest_index:byte_chest_index+data_bytes_length] = np.frombuffer(data_bytes, dtype='uint8')
                        byte_chest_index += data_bytes_length

                        if timestamp_byte_size == 0:
                            if byte_chest_index >= read_end_bytes - read_start_bytes:
                                extracted_data_length = read_end_bytes - read_start_bytes
                                raw_data_array[:extracted_data_length] = byte_chest[:extracted_data_length]
                                raw_data_array_index += extracted_data_length
                                end_time_flag = True
                                
                                is_same_data_packet = comp_status.get("is_same_dps")
                                if is_same_data_packet:
                                    bytes_processed = last_index
                                    comp_status["is_same_dps"] = False
                                else:
                                    bytes_processed = (last_index + (n+1) * cmplt_pkt_size)

                                comp_status["missing_bytes"] = byte_chest_index - extracted_data_length
                                comp_status["saved_bytes"] = raw_data_array_index
                                comp_status["last_index"] = bytes_processed - comp_status["missing_bytes"]
                                byte_chest_index -= extracted_data_length
                                break
                        else:
                            extracted_timestamp = None
                            while byte_chest_index >= dataframe_byte_size + timestamp_byte_size:
                                extracted_timestamp_bytes = byte_chest[dataframe_byte_size:dataframe_byte_size+timestamp_byte_size]
                                extracted_timestamp = struct.unpack('d', extracted_timestamp_bytes)[0]
                                log.debug(f"start_time: {start_time}")
                                log.debug(f"extracted_timestamp: {extracted_timestamp}")
                                log.debug(f"end_time: {end_time}")
                                if extracted_timestamp > start_time:
                                    
                                    extracted_data_length = dataframe_byte_size + timestamp_byte_size
                                    raw_data_array[raw_data_array_index:raw_data_array_index+extracted_data_length] = byte_chest[:extracted_data_length]
                                    raw_data_array_index += extracted_data_length

                                    if end_time != -1 and extracted_timestamp >= end_time:
                                        
                                        if prev_timestamp is None and comp_status.get("is_first_chunk",False):
                                            prev_timestamp = extracted_timestamp
                                            log.debug(f"prev_timestamp: {prev_timestamp}")
                                            if prev_timestamp > start_time:
                                                if last_index == 0:
                                                    prev_timestamp = comp_status.get("ioffset",0)
                                                else:
                                                    end_time_flag = True
                                                    break

                                        end_time = extracted_timestamp
                                        end_time_flag = True
                                        
                                        is_same_data_packet = comp_status.get("is_same_dps")
                                        if is_same_data_packet:
                                            bytes_processed = last_index
                                            comp_status["is_same_dps"] = False
                                        else:
                                            bytes_processed = (last_index + (n+1) * cmplt_pkt_size)
                                        
                                        comp_status["missing_bytes"] = byte_chest_index - extracted_data_length
                                        comp_status["saved_bytes"] = raw_data_array_index
                                        comp_status["last_index"] = bytes_processed - comp_status["missing_bytes"]
                                        break
                                    else:
                                        if prev_timestamp is None and comp_status.get("is_first_chunk",False):
                                            prev_timestamp = extracted_timestamp
                                            log.debug(f"prev_timestamp: {prev_timestamp}")
                                            if prev_timestamp > start_time:
                                                if last_index == 0:
                                                    prev_timestamp = comp_status.get("ioffset",0)
                                                else:
                                                    end_time_flag = True
                                                    break
                                    
                                    byte_chest = byte_chest[extracted_data_length:]
                                    byte_chest_index -= extracted_data_length

                                else:
                                    is_first_chunk = comp_status.get("is_first_chunk", False)
                                    if is_first_chunk:
                                        prev_timestamp = extracted_timestamp
                                        log.debug(f"prev_timestamp: {prev_timestamp}")

                                    byte_chest = byte_chest[dataframe_byte_size + timestamp_byte_size:]
                                    byte_chest_index -= dataframe_byte_size + timestamp_byte_size
                                    break

                            if "last_index" not in comp_status and extracted_timestamp is not None and (last_timestamp - extracted_timestamp) < (s_samples_per_ts/odr):
                                end_time_flag = True
                                bytes_processed = (last_index + (n+1) * cmplt_pkt_size)                                    
                                comp_status["missing_bytes"] = byte_chest_index
                                comp_status["saved_bytes"] = raw_data_array_index
                                comp_status["last_index"] = bytes_processed - comp_status["missing_bytes"]
                                break

                            if last_index != 0 and extracted_timestamp is not None and extracted_timestamp < end_time and byte_chest_index != 0 and comp_status["missing_bytes"] != 0:
                                end_time_flag = True
                                comp_status["missing_bytes"] = byte_chest_index
                                comp_status["saved_bytes"] = raw_data_array_index
                                comp_status["last_index"] = last_index - comp_status["missing_bytes"]
                                break

                        if end_time_flag:
                            break

                # Trim the preallocated arrays to the actual size of the data
                byte_chest = byte_chest[:byte_chest_index]
                raw_data_array = raw_data_array[:raw_data_array_index]
                return raw_data_array, prev_timestamp
            
            nof_prev_timestamps = max(0, nof_timestamps_in_start - 2)
            raw_data_array, prev_timestamp = __extract_data(start_time, end_time, nof_prev_timestamps)

            if nof_prev_timestamps != 0:
                while prev_timestamp is not None and prev_timestamp > start_time:
                    nof_prev_timestamps -= 1
                    raw_data_array, prev_timestamp = __extract_data(start_time, end_time, nof_prev_timestamps)

            log.debug("Data & Timestamp extraction algorithm COMPLETED!")
            data, timestamp = self.__process_datalog(comp_name, comp_status, raw_data_array,
                                                     dataframe_byte_size, timestamp_byte_size,
                                                     raw_flag, start_time, prev_timestamp)
            
            if "last_index" not in comp_status:
                bytes_processed = (last_index + (nof_data_packet+1) * cmplt_pkt_size)
                comp_status["missing_bytes"] = byte_chest_index
                comp_status["saved_bytes"] = raw_data_array_index
                comp_status["last_index"] = bytes_processed - comp_status["missing_bytes"]

            #DEBUG
            log.debug(f"data Len: {len(data)}")
            log.debug(f"Time Len: {len(timestamp)}")
            return data, timestamp

        elif c_type == ComponentTypeEnum.ALGORITHM.value:
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:#"fft":
                log.debug("FFT Algorithm! No batch")
                
                dataframe_byte_size = int(s_dim * s_data_type_len)
                timestamp_byte_size = 0

                with open(file_path, "rb") as f:
                    f_data = f.read()
                    if not f_data:
                        log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(0, file_path, os.stat(f.name).st_size))
                        raise NoDataAtIndexError(0, file_path, os.stat(f.name).st_size)
                    raw_data = np.fromstring(f_data, dtype='uint8')
                    new_array = self.remove_4bytes_every_n_optimized(raw_data, cmplt_pkt_size)
                
                data, timestamp = self.__process_datalog(comp_name, comp_status, new_array, dataframe_byte_size, timestamp_byte_size, raw_flag = raw_flag, start_time=start_time)

                #DEBUG
                log.debug(f"data Len: {len(data)}")
                log.debug(f"Time Len: {len(timestamp)}")
                return data, timestamp
            else:
                log.error("Algorithm type not supported")
                return None, None

        elif c_type == ComponentTypeEnum.ACTUATOR.value:

            if comp_name == MC_SLOW_TELEMETRY_COMP_NAME or comp_name == MC_FAST_TELEMETRY_COMP_NAME:
                if data_packet_size is not None:
                    with open(file_path, "rb") as f:
                        f_data = f.read()
                        if not f_data:
                            log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(0, file_path, os.stat(f.name).st_size))
                            raise NoDataAtIndexError(0, file_path, os.stat(f.name).st_size)
                        raw_data = np.fromstring(f_data, dtype='uint8')
                        new_array = self.remove_4bytes_every_n_optimized(raw_data, cmplt_pkt_size)
                    
                    #NOTE: The following value should be obtained from:
                    # -SLOW MC TELEMETRIES: "n_of_enabled_slow_telemetries * data_type (bytes_size)"
                    # -FAST MC TELEMETRIES: "dim * data_type (bytes_size)"
                    data_packet_size = comp_status.get("usb_dps")
                    if data_packet_size is not None:
                        timestamp_byte_size = 8
                        dataframe_byte_size = data_packet_size - timestamp_byte_size

                    data, timestamp = self.__process_datalog(comp_name, comp_status, new_array, dataframe_byte_size, timestamp_byte_size, raw_flag = raw_flag )

                    #DEBUG
                    log.debug(f"data Len: {len(data)}")
                    log.debug(f"Time Len: {len(timestamp)}")
                    return data, timestamp
                else:
                    log.error("Actuator type not supported")
                    return None, None

    #TODO! DEPRECATE OR REMOVE THIS FUNCTION
    def get_data_and_timestamps(self, sensor_name, sub_sensor_type, start_time = 0, end_time = -1, raw_flag = False):
        # get sensor component status
        s_stat = self.__get_sensor_status(sensor_name)
        
        # get acquisition interface
        interface = self.acq_info_model['interface']
        
        # data protocol size:
        data_protocol_size = 4

        data_packet_size = 0
        # data packet size (0:sd card, 1:usb, 2:ble, 3:serial)
        if interface == 0:
            data_packet_size = s_stat["sd_dps"] - data_protocol_size
        elif interface == 1:
            data_packet_size = s_stat["usb_dps"]
        elif interface == 2:
            data_packet_size = s_stat["ble_dps"]
        elif interface == 3:
            data_packet_size = s_stat["serial_dps"]
        else:
            log.error(f"Unknown interface: {interface}. check your device_config.json file")
            raise
        
        # get dat file path and size (obtained from "sensor_name + sub_sensor_type")
        file_path = self.__get_sensor_file_path(sensor_name)
        file_size = os.path.getsize(file_path)
        
        cmplt_pkt_size = data_packet_size + data_protocol_size
        nof_data_packet = file_size // cmplt_pkt_size # "//" math.floor equivalent
        checked_file_path = os.path.splitext(os.path.abspath(file_path))[0] + "_checked.dat"

        #TODO: Check data integrity looking at the first 4 bytes counter
        with open(checked_file_path, 'wb') as f, open(file_path, "rb") as rf:
            # cmplt_pkt_size = data_packet_size + data_protocol_size
            for n in range(nof_data_packet):
                index = n * cmplt_pkt_size
                rf.seek(index)
                rf_data = rf.read(cmplt_pkt_size)[4:]
                if not rf_data:
                    log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(index, file_path, os.stat(f.name).st_size))
                    raise NoDataAtIndexError(index, file_path, os.stat(f.name).st_size)
                f.write(np.frombuffer(rf_data, dtype='uint8'))

        # get checked dat file path and size (obtained from "sensor_name + sub_sensor_type") reusing the same variables
        file_path = self.__get_checked_sensor_file_path(sensor_name)
        file_size = os.path.getsize(file_path)
        
        c_type = s_stat.get("c_type")

        if c_type == ComponentTypeEnum.ALGORITHM.value:
            algo_type = s_stat.get("algorithm_type")
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                # get FFT algo "dimensions" --> FFT Length
                s_dim = s_stat.get("fft_length")
            else:
                s_dim = s_stat.get('dim')
        else:
            # get sensor dimensions
            s_dim = s_stat.get('dim', 1)
        
        # get Data type byte length
        s_data_type_len = TypeConversion.check_type_length(s_stat['data_type'])
        
        # get samples per ts
        spts = s_stat.get('samples_per_ts', {})
        if isinstance(spts, int):
            s_samples_per_ts = spts
        else:
            s_samples_per_ts = spts.get('val', 0)
        
        if c_type == ComponentTypeEnum.SENSOR.value:
            #TODO sample_end = N/s = 26667 in 1 sec, 266670 in 10 sec, --> 26667*10 in 1*10 sec --> ODR*end_time(in sec) = sample_end
            #TODO sample_start = N/s = 104 in 1 sec, 1040 in 10 sec, --> 104*10 in 1*10 sec --> ODR*start_time(in sec) = sample_start
            s_category = s_stat.get("sensor_category")
            if s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                odr = 1/(s_stat.get("intermeasurement_time")/1000)
            elif s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_POWERMETER.value:
                odr = 1/(s_stat.get("adc_conversion_time")/1000000)
            else:
                odr = s_stat.get("measodr")
                if odr is None or odr == 0:
                    odr = s_stat.get("odr",1)
            sample_end = int(odr * end_time) if end_time != -1 else -1
            total_samples = file_size//(s_data_type_len * s_dim)
            if sample_end > total_samples:
                sample_end = total_samples
            sample_start = int(odr * start_time)

            #SAMPLES_PER_TS check
            if s_samples_per_ts > total_samples:
                s_samples_per_ts = 0
            try:
                # Sample per Ts == 0 #######################################################################           
                if s_samples_per_ts == 0:
                    if sample_end == -1:
                        sample_end = total_samples
                    
                    read_start_bytes = sample_start * (s_dim * s_data_type_len)
                    read_end_bytes = sample_end * (s_dim * s_data_type_len)#dataframe_byte_size

                    dataframe_byte_size = read_end_bytes - read_start_bytes
                    timestamp_byte_size = 0
                    blocks_before_ss = 0

                # Sample per Ts != 0 #######################################################################
                else:
                    dataframe_byte_size = s_samples_per_ts * s_dim * s_data_type_len
                    timestamp_byte_size = 8

                    if sample_end == -1:
                        n_of_blocks_in_file = file_size // (timestamp_byte_size + dataframe_byte_size)
                        sample_end = n_of_blocks_in_file * s_samples_per_ts
                    
                    blocks_before_ss = sample_start // s_samples_per_ts
                    blocks_before_se = sample_end // s_samples_per_ts

                    read_start_bytes = (blocks_before_ss * dataframe_byte_size) + ((blocks_before_ss - 1) * timestamp_byte_size) if blocks_before_ss > 0 else 0
                    read_end_bytes = ((blocks_before_se + 1) * dataframe_byte_size) + ((blocks_before_se + 1) * timestamp_byte_size)
                
                with open(file_path, "rb") as f:
                    f.seek(read_start_bytes)
                    raw_data = f.read(read_end_bytes - read_start_bytes)
                    if len(raw_data) == 0:
                        log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(read_start_bytes, file_path, os.stat(f.name).st_size))
                        raise NoDataAtIndexError(read_start_bytes, file_path, os.stat(f.name).st_size)
                
                raw_data = np.fromstring(raw_data, dtype='uint8')

                # if the start_sample isn't in the first block (pre_t_bytes_id != 0)
                if read_start_bytes != 0 :
                    first_timestamp = raw_data[:timestamp_byte_size] if s_samples_per_ts != 0 else 0
                    s_stat['ioffset'] = np.frombuffer(first_timestamp, dtype='double') if s_samples_per_ts != 0 else 0
                    #remove the first timestamp
                    raw_data = raw_data[timestamp_byte_size:]

                data, timestamp = self.__process_datalog(sensor_name, s_stat, raw_data,
                                                         dataframe_byte_size, timestamp_byte_size,
                                                         raw_flag, start_time)

                #DEBUG
                log.debug(f"data Len: {len(data)}")
                log.debug(f"Time Len: {len(timestamp)}")

                os.remove(file_path)

                return data, timestamp

            except MemoryError:
                log.error("Memory Error occoured! You should batch process your {} file".format(file_path))
                os.remove(file_path)
                raise
            except OverflowError:
                log.error("Memory Error occoured! You should batch process your {} file".format(file_path))
                os.remove(file_path)
                raise

        elif c_type == ComponentTypeEnum.ALGORITHM.value:
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:#"fft":
                log.info("FFT Algorithm!")
                
                dataframe_byte_size = int(s_dim * s_data_type_len)
                timestamp_byte_size = 0

                with open(file_path, "rb") as f:
                    f_data = f.read()
                    if not f_data:
                        log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(0, file_path, os.stat(f.name).st_size))
                        raise NoDataAtIndexError(0, file_path, os.stat(f.name).st_size)
                    raw_data = np.fromstring(f_data, dtype='uint8')
                
                data, timestamp = self.__process_datalog(sensor_name, s_stat, raw_data, dataframe_byte_size, timestamp_byte_size, raw_flag = raw_flag )

                #DEBUG
                log.debug(f"data Len: {len(data)}")
                log.debug(f"Time Len: {len(timestamp)}")
                os.remove(file_path)
                return data, timestamp
            else:
                log.error("Algorithm type not supported")
                os.remove(file_path)
                return None, None

        elif c_type == ComponentTypeEnum.ACTUATOR.value:
            # s_samples_per_ts = 1
            
            if sensor_name == MC_SLOW_TELEMETRY_COMP_NAME or sensor_name == MC_FAST_TELEMETRY_COMP_NAME:#"slow_mc_telemetries":
                if data_packet_size is not None:
                    timestamp_byte_size = 8
                    dataframe_byte_size = data_packet_size - timestamp_byte_size

                    # n_of_samples = sample_end - sample_start
                    # blocks_before_ss = 0

                    # if sample_end == -1:
                    n_of_samples = int(file_size/dataframe_byte_size)
                    # sample_end = n_of_samples
                    
                    # read_start_bytes = sample_start * (s_data_type_len* s_dim)
                    # read_end_bytes = sample_end * (s_data_type_len* s_dim)
                    with open(file_path, "rb") as f:
                        # f.seek(read_start_bytes)
                        # f_data = f.read(read_end_bytes - read_start_bytes)
                        f_data = f.read()
                        if not f_data:
                            log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(0, file_path, os.stat(f.name).st_size))
                            raise NoDataAtIndexError(0, file_path, os.stat(f.name).st_size)
                        raw_data = np.fromstring(f_data, dtype='uint8')

                    # print(len(raw_data))
                    # print(dataframe_byte_size)
                    if n_of_samples >= 1:
                        first_timestamp = raw_data[dataframe_byte_size:dataframe_byte_size + timestamp_byte_size]
                        # print(struct.unpack("=d",first_timestamp))
                    
                    data, timestamp = self.__process_datalog(sensor_name, s_stat, raw_data, dataframe_byte_size, timestamp_byte_size, raw_flag = raw_flag )

                    #DEBUG
                    log.debug("data Len: {}".format(len(data)))
                    log.debug("Time Len: {}".format(len(timestamp)))
                    os.remove(file_path)
                    return data, timestamp
                else:
                    log.error("Actuator type not supported")
                    os.remove(file_path)
                    return None, None
                    # raise

    def get_ispu_output_column_names(self):
        if self.ispu_output_format is not None:
            return [o["name"] for o in self.ispu_output_format["output"]]
        
    def get_ispu_output_types(self):
        if self.ispu_output_format is not None:
            return [TypeConversion.check_type(o["type"]) for o in self.ispu_output_format["output"]]
        else:
            return None

    def __get_mems_columns_names(self, ss_stat, sensor_name, s_type, numAxes):
        if not (s_type == "mlc" or s_type == "stredl" or s_type == "ispu"):
            cc = ['x', 'y', 'z'] if numAxes == 3 else ['x', 'y'] if numAxes == 2 else []
            col_prefix = s_type[0].upper() + '_' if cc else ""
            col_postfix = ''
            if "unit" in ss_stat:
                unit = ss_stat["unit"]
                col_postfix = ' [' + UnitMap().unit_dict.get(unit, unit) + ']'
            c = [col_prefix + s + col_postfix for s in cc] if cc else [s_type.upper() + col_postfix]
        else:
            if s_type == "ispu":
                c = self.get_ispu_output_column_names()
            else: 
                if numAxes > 0:
                    cc = range(ss_stat['dim'])
                    col_prefix = s_type[0].upper() + '_'
                    c = [col_prefix + str(s) for s in cc]
                else:
                    log.error("Wrong number of sensor axes ({})".format(numAxes))
                    raise NSensorAxesError(sensor_name)
        return c

    # Function to find the nearest index for a given time in the times array
    def find_nearest_index(self, array, value):
        array = np.squeeze(array)  # Remove single-dimensional entries
        idx = (np.abs(array - value)).argmin()
        return idx

    def get_component_columns_names(self, ss_stat, sensor_name):
        s_type = ""
        # d_type = ss_stat.get("data_type")
        c_type = ss_stat.get("c_type")
        if c_type == ComponentTypeEnum.SENSOR.value:
            numAxes = int(ss_stat.get('dim',1))
            s_name, s_type = FileManager.decode_file_name(sensor_name)
            s_category = ss_stat.get("sensor_category")
            if s_category is not None:
                if s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                    c = ["Red","Visible","Blue","Green","IR","Clear"]
                elif s_category == SensorCategoryEnum.ISENSOR_CLASS_PRESENCE.value:
                    c = ["Tambient (raw)","Tobject (raw)","Tobject (emb_comp)","Tpresence",
                            "Presence flag","Tmotion","Motion flag","Tobject (sw_comp)",
                            "Tobject_change (sw_comp)","Motion flag (sw_comp)","Presence flag (sw_comp)"]
                elif s_category == SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
                    if ss_stat.get('output_format'):
                        resolution = ss_stat.get("resolution")
                        if resolution is not None:
                            res = int(resolution.split("x")[0])
                        c = []
                        for i in range(res):
                            for j in range(res):
                                c += [f"Target Status T1_Z({i},{j})",f"Distance T1_Z({i},{j})"]
                    else: #NOTE: Code for old firmware versions
                        res = 4 if ss_stat['dim'] == 128 else 8
                        c = []
                        for i in range(res):
                            for j in range(res):
                                c += [f"N Target Z({i},{j})",f"Ambient per SPAD Z({i},{j})",f"Signal per SPAD T1_Z({i},{j})",
                                    f"Target Status T1_Z({i},{j})",f"Distance T1_Z({i},{j})",f"Signal per SPAD T2_Z({i},{j})",
                                    f"Target Status T2_Z({i},{j})",f"Distance T2_Z({i},{j})"]
                        
                elif s_category == SensorCategoryEnum.ISENSOR_CLASS_CAMERA:
                    raise UnsupportedSensorCategoryError(sensor_name)#TODO
                elif s_category == SensorCategoryEnum.ISENSOR_CLASS_POWERMETER.value:
                    c = ["Voltage [mV]","Voltage(VShunt) [mV]","Current [A]","Power [mW]"]     
                else:
                    c = self.__get_mems_columns_names(ss_stat, sensor_name, s_type, numAxes)
            else:
                c = self.__get_mems_columns_names(ss_stat, sensor_name, s_type, numAxes)

        elif c_type == ComponentTypeEnum.ALGORITHM.value:
            algo_type = ss_stat.get("algorithm_type")
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                cc = range(ss_stat['fft_length'])
            else:
                cc = range(ss_stat['dim'])
            col_prefix = sensor_name.upper() + '_'
            c = [col_prefix + str(s) for s in cc]
        
        elif c_type == ComponentTypeEnum.ACTUATOR.value:
            telemetries_keys = self.__get_active_mc_telemetries_names(ss_stat, sensor_name)
            cc = range(len(telemetries_keys))
            c = [tk.upper() for tk in telemetries_keys]
        
        return c

    def __to_dataframe(self, data, time, ss_stat, sensor_name, labeled = False, which_tags:list = [], raw_flag = False):
        if data is not None and time is not None:
            cols = []
            s_type = ""
            c_type = ss_stat.get("c_type")
            if c_type == ComponentTypeEnum.SENSOR.value:
                s_name, s_type = FileManager.decode_file_name(sensor_name)
            
            if s_type != "ispu":
                try:
                    if len(time) > len(data):
                        time = time[:len(data)]
                    val = np.concatenate((time, data), axis=1)
                except:
                    pass
            else:
                ispu_out_types = self.get_ispu_output_types()
                if ispu_out_types is not None:
                    output_bytes_count = sum(TypeConversion.check_type_length(ot) for ot in ispu_out_types)
                    unpack_str = "=" + ''.join(TypeConversion.get_format_char(ot) for ot in ispu_out_types)
                    output_data = [struct.unpack(unpack_str, np.array(d[0:output_bytes_count])) for d in data]
                    np_output_data = np.array(output_data)
                    final_out_data = [np_output_data[:,i] for i in range(len(ispu_out_types))]
                    final_out_data = np.transpose(final_out_data)
                    val = np.array(time)
                    val = np.append(val, final_out_data, axis=1)
                else:
                    raise MissingISPUOutputDescriptorException(sensor_name)
            
            cols =np.concatenate((["Time"], self.get_component_columns_names(ss_stat, sensor_name)), axis=0)

            try:
                ss_data_frame = pd.DataFrame(data=val, columns=cols)
            except Exception as e:
                pass

            if labeled:
                tags = self.get_tags()
                if len(tags) == 0:
                    raise MissingTagsException() 
                if len(which_tags) > 0:
                    filtered_tags = [t for t in tags if t["label"] in which_tags]
                    tags = filtered_tags

                for tag in tags:
                    tag_label = tag.get("label")
                    tag_times = tag.get("times")
                    for t in tag_times:
                        enter_time = t[0]
                        exit_time = t[1]
                        # Find the nearest indices for the enter and exit times
                        enter_index = self.find_nearest_index(time, enter_time)
                        exit_index = self.find_nearest_index(time, exit_time)
                        
                        # Create an array of booleans with the same length as times_array
                        bool_array = np.zeros_like(time, dtype=bool)
                        if enter_time <= time[-1]:
                            if not(exit_time <= time[-1] and (enter_index == exit_index)):
                                # Set True for indices between enter_index and exit_index (inclusive)
                                bool_array[enter_index:exit_index+1] = True

                        # Flatten the boolean array to match the shape of the input times array
                        bool_array = bool_array.flatten()
                        
                        if tag_label not in ss_data_frame:
                            ss_data_frame[tag_label] = bool_array
                        else:
                            ss_data_frame[tag_label] = ss_data_frame[tag_label] | bool_array

            sensitivity = ss_stat.get("sensitivity", 1)
            data_type = ss_stat.get("data_type")
            ss_data_frame["Time"] = ss_data_frame["Time"].round(decimals=6)
            for i, c in enumerate(cols):
                if c != "Time":
                    if raw_flag or sensitivity == 1 and data_type:
                        if i != 0:
                            ss_data_frame[c] = ss_data_frame[c].astype(TypeConversion.get_np_dtype(data_type))
                            if data_type in ["float","float32","double"]:
                                ss_data_frame[c] = ss_data_frame[c].round(decimals=6)
                    else:
                        ss_data_frame[c] = ss_data_frame[c].round(decimals=6)
            
            return ss_data_frame
        log.error("Error extracting data and timestamp from sensor {}".format(sensor_name))
        raise DataExtractionError(sensor_name)

    #TODO deprecate this function
    def get_dataframe(self, sensor_name, sensor_type = None, start_time = 0, end_time = -1, labeled = False, raw_flag = False):       
        # get sensor component status
        s_stat = self.__get_sensor_status(sensor_name)
        
        res = self.get_data_and_timestamps(sensor_name, sensor_type, start_time, end_time, raw_flag)
        if res is not None:
            data, time = res
            # data, time, ss_stat, sensor_name, labeled = False, which_tags:list = [], raw_flag = False
            return self.__to_dataframe(data, time, s_stat, sensor_name, labeled, [], raw_flag)
        log.error("Error extracting data and timestamps from {} sensor .dat file".format(sensor_name))
        raise DataExtractionError(sensor_name)
    
    def get_dataframe_batch(self, comp_name, comp_status, start_time = 0, end_time = -1, labeled = False, raw_flag = False, which_tags:list = []):
        res = self.get_data_and_timestamps_batch(comp_name, comp_status, start_time, end_time, raw_flag)
        if res[0] is not None and res[1] is not None:
            data, time = res
            if len(data) > 0:
                return self.__to_dataframe(data, time, comp_status, comp_name, labeled, which_tags, raw_flag)
        return None

    def get_dat_file_list(self):
        """
        Retrieves a list of .dat files from the acquisition folder associated with this HSDatalog instance.
        The method uses the FileManager class to search for .dat files within the acquisition folder path.

        :return: A list of .dat file paths.
        """
        # Use the FileManager class to get a list of .dat files from the acquisition folder.
        # The '__acq_folder_path' is an instance attribute that stores the path to the acquisition folder.
        return FileManager.get_dat_files_from_folder(self.__acq_folder_path)


    # #======================================================================================#
    ### OFFLINE Plots  #######################################################################
    #========================================================================================#

    # Plots Helper Functions ################################################################################################################
        
    def __plot_ranging_sensor(self, sensor_name, ss_data_frame, res, output_format):
        
        # Function to extract the target identifier from the key
        def __extract_target_identifier(key):
            if key != "nof_outputs":
                return "".join(key.split("_")[:-1])
            return None                
        
        # if output_format is not None:

        # Group the targets
        targets = {}
        new_shape = (res, res)

        nof_outputs = output_format.get("nof_outputs")
        for key, value in output_format.items():
            identifier = __extract_target_identifier(key)
            if identifier is not None:
                # Initialize the target group if it doesn't exist
                if identifier not in targets:
                    targets[identifier] = {'status': None, 'distance': None}
                # Assign the status or distance to the target group
                if 'status' in key:
                    targets[identifier]['status'] = value
                elif 'distance' in key:
                    targets[identifier]['distance'] = value
        
        for t in targets:
            dist_id = targets[t]['distance']["start_id"] + 1
            dist_df = ss_data_frame.iloc[:, range(dist_id,len(ss_data_frame.columns), nof_outputs)]
            status_id = targets[t]['status']["start_id"] + 1
            status_df = ss_data_frame.iloc[:, range(status_id,len(ss_data_frame.columns), nof_outputs)]
            status_df.columns = dist_df.columns
            mask_df = status_df == 5
            times = ss_data_frame["Time"]
            # use the boolean matrix as a mask for the second dataframe
            masked_df = dist_df.where(mask_df)
            nof_rows = len(masked_df)

            dist_matrices = np.empty((nof_rows, ), dtype=object)
            for r_id in range(nof_rows):
                row_t1 = masked_df.iloc[r_id]
                t1_mat = np.array(row_t1.values).reshape(new_shape).astype('float')
                t1_mat = np.swapaxes(t1_mat, 0, 1)
                dist_matrices[r_id] = t1_mat
        
            # figure axis setup 
            fig, ax = plt.subplots()
            fig.subplots_adjust(bottom=0.15)
            # add a fake legend item
            plt.legend(handles = [mpatches.Patch(color="black", label='Invalid Zone')], loc='upper left', ncol=1)

            cmap = plt.cm.get_cmap('viridis')
            cmap.set_bad(color='black')
            # display initial image
            im_h = ax.imshow(dist_matrices[0], cmap=cmap, vmin=0, vmax=4000, origin='lower')
            axins = inset_axes(ax,
                        width="5%",  
                        height="100%",
                        loc='center right',
                        borderpad= -5
                    )
            fig.colorbar(im_h, cax=axins, orientation="vertical")

            # setup a slider axis and the Slider
            ax_slider = plt.axes([0.23, 0.02, 0.56, 0.04])
            slider = Slider(ax_slider, 'Time Step', 0, nof_rows-1, valinit=times[0])

            # update the figure with a change on the slider 
            def update_depth(val):
                idx = int(round(slider.val))
                im_h.set_data(dist_matrices[idx])
                slider.valtext.set_text(times[idx])

            slider.on_changed(update_depth)
            
            fig.suptitle(f"{sensor_name.upper()}")
            plt.draw()

        self.__plot_pixels_over_time(sensor_name, ss_data_frame, res*res, masked_df)

    def __plot_pixels_over_time(self, sensor_name, ss_data_frame, resolution, t1_dist_df):
        fig, ax = plt.subplots()

        n_lines = resolution
        # extract the first column (Times column) of ss_data_frame
        times_col = ss_data_frame.iloc[:, 0]
        # extract Target 1 distance values columns from ss_data_frame
        times_df = pd.DataFrame({"Time": times_col})
        ss_t1_dist_df = pd.concat([times_df, t1_dist_df.fillna(0)], axis=1)
                                   
        matrix_l = 8 if n_lines == 64 else 4
        
        cnt = 0
        lines = []
        for i in range(matrix_l):
            for j in range(matrix_l):
                line = PlotUtils.draw_line(ax, ss_t1_dist_df, cnt, np.random.rand(3, ), f"({i},{j})", True)
                lines.append(line)
                if i+j == 0:
                    line.set_visible(True)
                else:
                    line.set_visible(False)
                cnt += 1
        
        ax.set(xlabel = 'Time (s)', ylabel = 'Distance [mm]')

        # Create the checkbox widgets
        rax = []
        check = []
        for i in range(matrix_l):
            rax.append(plt.axes([0.1+i*0.1, 0.05, 0.1, 0.23]))
            labels = [f"({i},{j%matrix_l})" for j in range(i*matrix_l, (i+1)*matrix_l)]
            actives = [False]*matrix_l
            if i == 0:
                actives[0] = True
            check.append(CheckButtons(rax[i], labels, actives))

        # Define the function to update the plot based on the checkbox state
        def update(val):
            for i, line in enumerate(lines):
                line.set_visible(check[i//matrix_l].get_status()[i%matrix_l])
            plt.draw()

        # Connect the checkbox widgets to the update function
        for i in range(matrix_l):
            check[i].on_clicked(update)

        fig.suptitle(f"{sensor_name.upper()} - Distance over time per Zone")
        fig.subplots_adjust(top=0.93, bottom=0.37)
        plt.draw()
    
    def __plot_light_sensor(self, sensor_name, ss_data_frame, cols, dim, label):
        fig = plt.figure()
        als_lines_colors = ["#FF0000", "#999999", "#0000FF", "#00FF00", "#FF00FF", "#000000"]
        for idx, k in enumerate(range(dim)):
            PlotUtils.draw_line(plt, ss_data_frame, k, als_lines_colors[idx], cols[idx])

        ax = fig.axes[0]
        # ax.set_xlim(left=-0.5)
        
        if label is not None:
            ax.patch.set_facecolor('0.8')
            ax.set_alpha(float('0.5'))
            PlotUtils.draw_tag_lines(plt, ss_data_frame, label)
            PlotUtils.set_legend(plt.gca())
        else:
            plt.legend(loc='upper left', ncol=1)

        plt.title(sensor_name.upper())
        plt.tight_layout(pad=2.0)
        plt.ylabel("ADC count")
        plt.xlabel('Time (s)')

        plt.draw()

    def __plot_presence_sensor(self, sensor_name, ss_data_frame, cols, label, software_compensation, embedded_compensation):
        fig = plt.figure()
        
        for idx in range(2):
            PlotUtils.draw_line(plt, ss_data_frame, idx, PlotUtils.lines_colors[idx], cols[idx])
        
        ax = fig.axes[0]
        # ax.set_xlim(left=-0.5)

        if label is not None:
            ax.patch.set_facecolor('0.6')
            ax.set_alpha(float('0.5'))
            PlotUtils.draw_tag_lines(plt, ss_data_frame, label)
            PlotUtils.set_legend(plt.gca())
        else:
            plt.legend(loc='upper left', ncol=1)
        
        plt.title(f"{sensor_name.upper()} - Ambient & Object (raw)")
        plt.xlabel('Time (s)')
        plt.tight_layout()
        plt.draw()

        line_color = "#335c67"
        line_color_sw = "#bb3e03"
        line_color_emb = "#ff9900"
        if software_compensation or embedded_compensation:
            fig, axs = plt.subplots(2)
            fig.suptitle(f"{sensor_name.upper()} - Presence")
        else:
            fig = plt.figure()
            axs = None
            plt.title(f"{sensor_name.upper()} - Presence")
        
        if axs is not None:
            for idx, p in enumerate(axs):
                if idx == 0:#Presence + Flag
                    color = "#97D3C2"
                    edgecolor = PlotUtils.darken_color(color, 10)
                    PlotUtils.draw_regions(p, ss_data_frame, "Presence flag", color, edgecolor, 1, '')
                    PlotUtils.draw_line(p, ss_data_frame, 3, line_color, cols[3])
                    flag_patch = mpatches.Patch(color=color, label='Presence flag')
                
                elif idx == 1:#Presence + SW Flag
                    color = "#FDD891"
                    edgecolor = PlotUtils.darken_color(color, 10)
                    if embedded_compensation:
                        PlotUtils.draw_line(p, ss_data_frame, 2, line_color_emb, cols[2])# tObj emb comp
                    if software_compensation:
                        PlotUtils.draw_regions(p, ss_data_frame, "Presence flag (sw_comp)", color, edgecolor, 1, '')
                        PlotUtils.draw_line(p, ss_data_frame, 7, line_color_sw, cols[7])# tObj sw comp
                        flag_patch = mpatches.Patch(color=color, label='Presence flag (sw_comp)')
                
                if label is not None:
                    p.patch.set_facecolor('0.6')
                    p.patch.set_alpha(float('0.5'))
                    pre_tag_handles, pre_tag_labels = p.get_legend_handles_labels()
                    if idx == 0 or (idx == 1 and software_compensation):
                        pre_tag_handles.append(flag_patch)
                        pre_tag_labels.append(flag_patch.get_label())
                    PlotUtils.draw_tag_lines(p, ss_data_frame, label, 0.4)
                    old_handles, old_labels = p.get_legend_handles_labels()
                    for h in old_handles[1:]:
                        pre_tag_handles.append(h)
                    for l in old_labels[1:]:
                        pre_tag_labels.append(l)
                else:
                    pre_tag_handles, pre_tag_labels = p.get_legend_handles_labels()
                    if idx == 0 or (idx == 1 and software_compensation):
                        pre_tag_handles.append(flag_patch)
                        pre_tag_labels.append(flag_patch.get_label())
                p.legend(handles=pre_tag_handles, labels=list(OrderedDict.fromkeys(pre_tag_labels)), loc='upper left', ncol=1)
        else:
            color = "#97D3C2"
            edgecolor = PlotUtils.darken_color(color, 10)
            PlotUtils.draw_regions(plt, ss_data_frame, "Presence flag", color, edgecolor, 1, '')
            PlotUtils.draw_line(plt, ss_data_frame, 3, line_color, cols[3])
            flag_patch = mpatches.Patch(color=color, label='Presence flag')
            
            ax = fig.axes[0]

            if label is not None:
                ax.patch.set_facecolor('0.6')
                ax.patch.set_alpha(float('0.5'))
                pre_tag_handles, pre_tag_labels = ax.get_legend_handles_labels()
                pre_tag_handles.append(flag_patch)
                pre_tag_labels.append(flag_patch.get_label())
                PlotUtils.draw_tag_lines(plt, ss_data_frame, label, 0.4)
                old_handles, old_labels = ax.get_legend_handles_labels()
                for h in old_handles[1:]:
                    pre_tag_handles.append(h)
                for l in old_labels[1:]:
                    pre_tag_labels.append(l)
            else:
                pre_tag_handles, pre_tag_labels = ax.get_legend_handles_labels()
                pre_tag_handles.append(flag_patch)
                pre_tag_labels.append(flag_patch.get_label())
                plt.legend(handles=pre_tag_handles, labels=list(OrderedDict.fromkeys(pre_tag_labels)), loc='upper left', ncol=1)

        plt.xlabel('Time (s)')
        plt.tight_layout()
        plt.draw()

        if software_compensation:
            fig, axs = plt.subplots(2)
            fig.suptitle(f"{sensor_name.upper()} - Motion")
        else:
            fig = plt.figure()
            axs = None
            plt.title(f"{sensor_name.upper()} - Motion")

        if axs is not None:
            for idx, p in enumerate(axs):
                if idx == 0:#Motion + Flag
                    color = "#97D3C2"
                    edgecolor = PlotUtils.darken_color(color, 10)
                    PlotUtils.draw_regions(p, ss_data_frame, "Motion flag", color, edgecolor, 1, '')
                    PlotUtils.draw_line(p, ss_data_frame, 5, line_color, cols[5])
                    flag_patch = mpatches.Patch(color=color, label='Motion flag')
                elif idx == 1:#Motion + SW Flag
                    color = "#FDD891"
                    edgecolor = PlotUtils.darken_color(color, 10)
                    PlotUtils.draw_regions(p, ss_data_frame, "Motion flag (sw_comp)", color, edgecolor, 1, '')
                    PlotUtils.draw_line(p, ss_data_frame, 8, line_color_sw, cols[8]) #Tobj change
                    flag_patch = mpatches.Patch(color=color, label='Motion flag (sw_comp)')
                
                if label is not None:
                    p.patch.set_facecolor('0.6')
                    p.patch.set_alpha(float('0.5'))
                    pre_tag_handles, pre_tag_labels = p.get_legend_handles_labels()
                    pre_tag_handles.append(flag_patch)
                    pre_tag_labels.append(flag_patch.get_label())
                    PlotUtils.draw_tag_lines(p, ss_data_frame, label, 0.4)
                    old_handles, old_labels = p.get_legend_handles_labels()
                    for h in old_handles[1:]:
                        pre_tag_handles.append(h)
                    for l in old_labels[1:]:
                        pre_tag_labels.append(l)
                else:
                    pre_tag_handles, pre_tag_labels = p.get_legend_handles_labels()
                    pre_tag_handles.append(flag_patch)
                    pre_tag_labels.append(flag_patch.get_label())
                p.legend(handles=pre_tag_handles, labels=list(OrderedDict.fromkeys(pre_tag_labels)), loc='upper left', ncol=1)
            
        else:
            color = "#97D3C2"
            edgecolor = PlotUtils.darken_color(color, 10)
            PlotUtils.draw_regions(plt, ss_data_frame, "Motion flag", color, edgecolor, 1, '')
            PlotUtils.draw_line(plt, ss_data_frame, 5, line_color, cols[5])
            flag_patch = mpatches.Patch(color=color, label='Motion flag')
            
            ax = fig.axes[0]

            if label is not None:               
                ax.patch.set_facecolor('0.6')
                ax.patch.set_alpha(float('0.5'))
                pre_tag_handles, pre_tag_labels = ax.get_legend_handles_labels()
                pre_tag_handles.append(flag_patch)
                pre_tag_labels.append(flag_patch.get_label())
                PlotUtils.draw_tag_lines(plt, ss_data_frame, label, 0.4)
                old_handles, old_labels = ax.get_legend_handles_labels()
                for h in old_handles[1:]:
                    pre_tag_handles.append(h)
                for l in old_labels[1:]:
                    pre_tag_labels.append(l)
            else:
                pre_tag_handles, pre_tag_labels = ax.get_legend_handles_labels()
                pre_tag_handles.append(flag_patch)
                pre_tag_labels.append(flag_patch.get_label())
            plt.legend(handles=pre_tag_handles, labels=list(OrderedDict.fromkeys(pre_tag_labels)), loc='upper left', ncol=1)

        plt.xlabel('Time (s)')
        plt.tight_layout()
        plt.draw()

    def __plot_mems_audio_sensor(self, sensor_name, ss_data_frame, cols, dim, subplots, label, raw_flag, unit, fft_params):
        if subplots:
            fig, axs = plt.subplots(dim)
            if dim == 1: axs = [axs]
            fig.suptitle(sensor_name)
            if not raw_flag and unit is not None:
                fig.text(0.04, 0.5, UnitMap().unit_dict.get(unit, unit), va='center', rotation='vertical')
            
            for idx, p in enumerate(axs):
                PlotUtils.draw_line(p, ss_data_frame, idx, PlotUtils.lines_colors[idx], cols[idx])
                # p.axes.set_xlim(left=-0.5)
                p.set(title=cols[idx])
                if label is not None:
                    p.patch.set_facecolor('0.6')
                    p.patch.set_alpha(float('0.5'))
                    PlotUtils.draw_tag_lines(p, ss_data_frame, label)
            
            PlotUtils.set_plot_time_label(axs, fig, dim)
            if label is not None:
                for ax in axs:
                    old_handles, old_labels = ax.get_legend_handles_labels()
                    ax.legend(handles=old_handles, labels=list(OrderedDict.fromkeys(old_labels)), loc='upper left', ncol=1)
        else:
            fig = plt.figure()

            if not raw_flag and unit is not None:
                plt.ylabel(UnitMap().unit_dict.get(unit, unit))

            if "_ispu" in sensor_name:
                n_lines = len(self.get_ispu_output_column_names())
            else:
                n_lines = dim
            for k in range(n_lines):
                PlotUtils.draw_line(plt, ss_data_frame, k, PlotUtils.lines_colors[k], cols[k])

            ax = fig.axes[0]
            # ax.set_xlim(left=-0.5)
            
            plt.title(sensor_name)
            plt.xlabel('Time (s)')
            plt.legend(loc='upper left')

            if label is not None:
                ax.patch.set_facecolor('0.8')
                ax.set_alpha(float('0.5'))
                PlotUtils.draw_tag_lines(plt, ss_data_frame, label)
                PlotUtils.set_legend(plt.gca())
        
        plt.draw()

        if fft_params is not None and ("_acc" in sensor_name or "_mic" in sensor_name):
            cc = ['X', 'Y', 'Z'] if len(cols) == 3 else ['X', 'Y'] if len(cols) == 2 else []
            for i, c in enumerate(cols):
                fig = plt.figure()
                
                if len(cols) == 1:
                    plt.title(sensor_name)
                else:
                    plt.title(sensor_name + " [" + cc[i] + "]")
                # extract the signal values as a NumPy array
                signal = ss_data_frame[c].values
                # create a spectrogram using matplotlib
                plt.specgram(signal, Fs=fft_params[1])

                plt.draw()
    # Plots Helper Functions ################################################################################################################

    # Plots Functions #######################################################################################################################
    
    
    def get_sensor_plot(self, sensor_name, sensor_status, start_time = 0, end_time = -1, label=None, which_tags = [], subplots=False, raw_flag = False, fft_plots = False):

        try:
            ss_data_frame = None
            if sensor_status['enable']:
                ss_data_frame = self.get_dataframe_batch(sensor_name, sensor_status, start_time, end_time, label is not None, raw_flag, which_tags)

            if ss_data_frame is not None:

                c_type = sensor_status["c_type"]
                s_category = sensor_status.get("sensor_category")

                if label is not None and not label in ss_data_frame.columns:
                    log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                    label = None

                #Check for dim (nof plots)
                if c_type == ComponentTypeEnum.SENSOR.value:
                    dim = sensor_status.get("dim")
                elif c_type == ComponentTypeEnum.ALGORITHM.value:
                    pass
                elif c_type == ComponentTypeEnum.ACTUATOR.value:                   
                    telemetry_keys = self.__get_active_mc_telemetries_names(sensor_status, sensor_name)
                    dim = len(telemetry_keys)
                    keys_filter = ["Time"]
                    keys_filter.extend([key.upper() for key in telemetry_keys if isinstance(key, str)])
                    if label is not None:
                        keys_filter.append(label)
                    subplots = True
                    ss_data_frame = ss_data_frame.filter(items=keys_filter)
                cols = ss_data_frame.columns[1:]

                if ss_data_frame is not None:
                    if s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
                        resolution = sensor_status.get("resolution")
                        if resolution is not None:
                            res = int(resolution.split("x")[0])
                            output_format = sensor_status.get("output_format")
                            self.__plot_ranging_sensor(sensor_name, ss_data_frame, res, output_format)
                    elif s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                        self.__plot_light_sensor(sensor_name, ss_data_frame, cols, dim, label)
                    elif s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_PRESENCE.value:
                        software_compensation = sensor_status.get("software_compensation")
                        embedded_compensation = sensor_status.get("embedded_compensation")
                        self.__plot_presence_sensor(sensor_name, ss_data_frame, cols, label, software_compensation, embedded_compensation)
                    else: #ISENSOR_CLASS_MEMS and ISENSOR_CLASS_AUDIO
                        fft_params = None
                        if fft_plots:
                            odr = sensor_status.get('odr', 1)
                            fft_params = (fft_plots, odr)
                        self.__plot_mems_audio_sensor(sensor_name, ss_data_frame, cols, dim, subplots, label, raw_flag, sensor_status.get('unit'), fft_params)
            else:
                log.error("Error extracting subsensor DataFrame {}".format(sensor_name))
                raise DataExtractionError(sensor_name)
            return True
        except MemoryError:
            log.error("Memory Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(sensor_name)))
            raise
        except  ValueError as e:
            print(e)
            log.error("Value Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(sensor_name)))
            raise
    
    def get_algorithm_plot(self, component_name, component_status, start_time = 0, end_time = -1, label=None, which_tags = [], subplots=False, raw_flag = False):
        try:
            ss_data_frame = None
            if component_status['enable']:
                ss_data_frame = self.get_dataframe_batch(component_name, component_status, start_time, end_time, label is not None, raw_flag, which_tags)
                
            if ss_data_frame is not None:
                algo_type = component_status.get("algorithm_type")
                if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                    s_dim = component_status.get("fft_length")
                else:
                    s_dim = component_status.get("dim")
                #Tag columns check (if any)
                if label is not None and len(ss_data_frame.columns) < s_dim + 1:
                    log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                    label = None

                #Tag columns check (selected label exists?)
                if label is not None and not label in ss_data_frame.columns:
                    log.warning("No {} label found in selected acquisition".format(label))
                    label = None
                
                if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                    fft_lenght = s_dim
                    
                    if "fft_sample_freq" in component_status:
                        fft_sample_freq = component_status["fft_sample_freq"]
                    else:
                        log.error("FFT Sample Freq. unknown")
                        raise MissingPropertyError("fft_sample_freq")
                    
                    fig = plt.figure()
                    
                    df_array = ss_data_frame.iloc[: , 1:].T.to_numpy(dtype="float")                    
                    y_value = np.square(df_array)
                    y_value =  y_value / (fft_lenght * fft_sample_freq)
                    y_value =  10 * np.log10(y_value)
                    
                    y_value = np.flipud(y_value)
                    
                    plt.imshow(y_value, cmap="viridis", aspect='auto', extent=[0,len(y_value[0]),0,fft_sample_freq/2])
                    plt.ylabel('Hz')
                    plt.tight_layout()
                    plt.colorbar()
                    plt.title(component_name)
                    # plt.xlabel('Time (s)') # TODO
                    # plt.legend(loc='upper left') # TODO
                    plt.draw()
                else:
                    log.error("Algorithm type selected is not supported.")
            
            else:
                log.error("Empty DataFrame extracted.")
        except MissingPropertyError as exc:
            log.error("Missing {} Property Error!".format(exc))
            raise
        except MemoryError:
            log.error("Memory Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(component_name)))
            raise
        except  ValueError:
            log.error("Value Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(component_name)))
            raise
    # Plots Functions #######################################################################################################################

    # #======================================================================================#
    ### OFFLINE CLI Interaction ##############################################################
    #========================================================================================#
    def prompt_device_id_select_CLI(self, device_list):
        selected_device = CLI.select_item("Device",device_list)
        selected_device_id = device_list.index(selected_device)
        return selected_device_id

    def prompt_sensor_select_CLI(self, sensor_list = None):
        if sensor_list is None:
            sensor_list = self.get_sensor_list()
        return CLI.select_item("PnPL_Component", sensor_list)
    
    def prompt_algorithm_select_CLI(self, algo_list = None):
        if algo_list is None:
            algo_list = self.get_algorithm_list()
        return CLI.select_item("PnPL_Component", algo_list)
    
    def prompt_actuator_select_CLI(self, actuator_list = None):
        if actuator_list is None:
            actuator_list = self.get_actuator_list()
        return CLI.select_item("PnPL_Component", actuator_list)
    
    def prompt_actuator_telemetries_select_CLI(self, actuator_stat, telemetry_keys):
        telemetry_list = []
        for tk in telemetry_keys:
            telemetry_list.append({tk:actuator_stat[tk]})
        return CLI.select_items("PnPL_Component", telemetry_list)
    
    def prompt_component_select_CLI(self, component_list = None):
        if component_list is None:
            sensor_list = self.get_sensor_list()
            algo_list = self.get_algorithm_list()
            component_list = sensor_list + algo_list
        return CLI.select_item("PnPL_Component", component_list)

    def prompt_file_select_CLI(self, dat_file_list = None):
        if dat_file_list is None:
            dat_file_list = FileManager.get_file_names_from_model()
        return CLI.select_item("Data File", dat_file_list)

    def prompt_label_select_CLI(self, label_list = None):
        if label_list is None:
            label_list = self.get_acquisition_label_classes()
        return CLI.select_item("Labels", label_list)

    def present_device_info(self, device_info = None):
        """
        Presents the device information to the user interface, typically through the command line interface (CLI).
        If no device information is provided, it retrieves the default device information using the get_device_info method.
        :param device_info: The device information to be presented. If None, the method retrieves the device information using the get_device_info method.
        """
        # Check if device information has been provided
        if device_info is None:
            # If not, retrieve the default device information using the 'get_device_info' method
            device_info = self.get_device_info()
        # Use the CLI (Command Line Interface) module to present the device information.
        # This could involve printing the information to the console, displaying it in a GUI, etc.
        CLI.present_item(device_info)

    def present_sensor_list(self, sensor_list = None):
        """
        Presents a list of sensors to the user interface, typically through the command line interface (CLI).
        If no sensor list is provided, it retrieves the default sensor list using the get_sensor_list method.
        :param sensor_list: [Optional] A list of sensors to be presented. If None, the method retrieves the sensor list using the get_sensor_list method.
        """
        # Check if a sensor list has been provided
        if sensor_list is None:
            # If not, retrieve the default sensor list using the 'get_sensor_list' method
            sensor_list = self.get_sensor_list()
        
        # Use the CLI (Command Line Interface) module to present the items in the sensor list.
        # This could involve printing the list to the console, displaying it in a GUI, etc.
        CLI.present_items(sensor_list)

    def present_sw_tag_classes(self, tag_class_list = None):
        """
        Presents a list of software tag classes to the user interface, typically through the command line interface (CLI).
        If no list is provided, it retrieves the software tag classes using the get_sw_tag_classes method.

        :param tag_class_list: [Optional] A list of software tag classes to be presented. If None, the method retrieves the list using the get_sw_tag_classes method.
        """
        # Check if a list of software tag classes has been provided
        if tag_class_list is None:
            # If not, retrieve the list using the 'get_sw_tag_classes' method
            tag_class_list = self.get_sw_tag_classes()
        
        # Use the CLI (Command Line Interface) module to present the list of software tag classes.
        # This could involve printing the list to the console, displaying it in a GUI, etc.
        CLI.present_items(tag_class_list)

    def present_hw_tag_classes(self, tag_class_list = None):
        """
        Presents a list of hardware tag classes to the user interface, typically through the command line interface (CLI).
        If no list is provided, it retrieves the hardware tag classes using the get_hw_tag_classes method.

        :param tag_class_list: [Optional] A list of hardware tag classes to be presented. If None, the method retrieves the list using the get_hw_tag_classes method.
        """
        # Check if a list of hardware tag classes has been provided
        if tag_class_list is None:
            # If not, retrieve the list using the 'get_hw_tag_classes' method
            tag_class_list = self.get_hw_tag_classes()
        # Use the CLI (Command Line Interface) module to present the list of hardware tag classes.
        # This could involve printing the list to the console, displaying it in a GUI, etc.
        CLI.present_items(tag_class_list)

    def present_sensor(self, sensor):
        """
        Presents information about a specific sensor to the user interface, typically through the command line interface (CLI).
        If the sensor information is provided, it is presented using the CLI module. If not, a warning is issued.

        :param sensor: The sensor information to be presented. If None, a warning message is displayed.
        """
        # Check if sensor information has been provided
        if sensor is not None:
            # If sensor information is available, use the CLI (Command Line Interface) module to present it.
            # This could involve printing the information to the console, displaying it in a GUI, etc.
            CLI.present_item(sensor)
        else:
            # If no sensor information is provided, display a warning message to the user.
            log.warning("No sensor selected")

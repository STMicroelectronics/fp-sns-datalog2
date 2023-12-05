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

from datetime import datetime
import json
import os
import struct
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.widgets import Slider
import numpy as np
import pandas as pd

from st_hsdatalog.HSD_utils.exceptions import *
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.utils.cli_interaction import CLIInteraction as CLI
from st_hsdatalog.HSD.utils.file_manager import FileManager
from st_hsdatalog.HSD.utils.type_conversion import TypeConversion
from st_pnpl.DTDL.device_template_manager import DeviceTemplateManager
from st_pnpl.DTDL.device_template_model import ContentSchema, SchemaType
from st_pnpl.DTDL.dtdl_utils import DTDL_SENSORS_ID_COMP_KEY, MC_FAST_TELEMETRY_COMP_NAME, MC_SLOW_TELEMETRY_COMP_NAME, AlgorithmTypeEnum, ComponentTypeEnum, SensorCategoryEnum

log = logger.get_logger(__name__)

class HSDatalog_v2:
    device_model = None
    acq_info_model = None
    ispu_output_format = None
    __acq_folder_path = None
    __checkTimestamps = False

    def __find_file(self, name:str, path:str):
        """ private class utility function usable to find sensor files

        Args:
            name (str): File name to search for
            path (str): Path where to search the file name specified

        Returns:
            str: The complete file path if the file exist, None elewhere
        """
        for root, dirs, files in os.walk(path):
            if name in files:
                return os.path.join(root, name)
        return None
    
    def __init__(self, acquisition_folder = None):
        """HSDatalog_v2 initialization function

        Args:
            acquisition_folder (_type_, optional): Acqusition Folder. Defaults to None.

        Raises:
            MissingSensorModelError: If device_config.json file is not present into the specified acquisition_folder
            MissingAcquisitionInfoError: If acquisition_info.json file is not present into the specified acquisition_folder
        """
        if acquisition_folder is not None:
            device_json_file_path = self.__find_file("device_config.json", acquisition_folder)
            if device_json_file_path is None:
                raise MissingSensorModelError
            self.__load_device_from_file(device_json_file_path)
            try:
                acquisition_json_file_path = self.__find_file("acquisition_info.json", acquisition_folder)
                if acquisition_json_file_path is None:
                    raise MissingAcquisitionInfoError
                self.__load_acquisition_info(acquisition_json_file_path)
                
                ispu_output_json_file_path = self.__find_file("ispu_output_format.json", acquisition_folder)
                if ispu_output_json_file_path is not None:
                    self.__load_ispu_output_format(ispu_output_json_file_path)
                    
            except MissingAcquisitionInfoError:
                log.error("No acquisition_info.json file in your Acquisition folder")
                raise
            except MissingDeviceModelError:
                raise
        else:
            log.warning("Acquisition folder not provided.")
            self.device_model = None
            self.acq_info_model = None

        self.__acq_folder_path = acquisition_folder
    
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
    
    ### ==> Data integrity Protocol ###
    def evaluate_data_integrity(self, file_content):
        """TO BE IMPLEMENTED

        Args:
            file_content (_type_): _description_
        """
        pass
        
    def remove_protocol_data(self, file_content):
        """TO BE IMPLEMENTED

        Args:
            file_content (_type_): _description_
        """
        pass

    ### Data integrity Protocol <== ###
    
    def __load_device_from_file(self, device_json_file_path):
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
            self.__load_device(device_dict)
        except MissingDeviceModelError as e:
            raise e

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
    
    def __convert_enums_in_values(self, comp_dtdl_contents, comp_status):
        for property in comp_dtdl_contents:
            if property.schema is not None and isinstance(property.schema, ContentSchema) and property.schema.type == SchemaType.ENUM :
                enum_index = comp_status[property.name]
                prop_value = self.__convert_prop_enum_in_value(enum_index, property)
                comp_status[property.name] = prop_value
    
    def __load_device(self, device_dict):
        self.device_model = device_dict['devices'][0]
        
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
                        # print("pre status: ", comp_status)
                        self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                        # print("post status: ", comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_AUDIO.value:
                        comp_status["unit"] = self.__get_sensor_unit_from_dtdl("aop", comp_dtdl_contents)
                        self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
                        self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                        self.__convert_enums_in_values(comp_dtdl_contents, comp_status)
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_CAMERA.value:
                        pass
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_PRESENCE.value:
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
                                        if sc.schema is not None and isinstance(sc.schema, ContentSchema) and sc.schema.type == SchemaType.ENUM :
                                            enum_index = comp_status[0][comp_name][sc.name]
                                            prop_value = float(self.__convert_prop_enum_in_value(enum_index, sc))
                                            comp_status[0][comp_name][sc.name] = prop_value
                                else:
                                    if "unit" not in comp_status[0][comp_name]:
                                        comp_status[0][comp_name]["unit"] = ""
        log.debug("Device Model: {}".format(self.device_model))

    @staticmethod
    def __convert_prop_enum_in_value(prop_enum_index, prop_content):
        enum_dname = prop_content.schema.enum_values[prop_enum_index].display_name
        value = enum_dname if isinstance(enum_dname,str) else enum_dname.en
        if prop_content.schema.value_schema.value == "integer":
            return float(value.replace(',','.'))
        return value
        
    
    def __load_acquisition_info(self, acq_info_json_file_path):
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

    def get_device(self):
        """This function returns the current Device Status

        Returns:
            device_model (dict): The current Device Status
        """        
        return self.device_model

    def set_device(self, new_device):
        """This function allows to change the current Device Status
           e.g.: if using hsd_link you obtain the current device template directly from the board,
                   you can set it in your HSDatalog instance.

        Args:
            new_device (dict): new Device Status to set
        """        
        self.device_model = new_device['devices'][0]
        self.__load_device(new_device)
        fw_info = self.get_firmware_info()["firmware_info"]
        if "part_number" in fw_info:
            log.info("Device [{}] - {} v{} sn:{} loaded correctly!".format(fw_info['alias'], fw_info['fw_name'], fw_info['fw_version'], fw_info['part_number']))
        elif "serial_number" in fw_info:
            log.info("Device [{}] - {} v{} sn:{} loaded correctly!".format(fw_info['alias'], fw_info['fw_name'], fw_info['fw_version'], fw_info['serial_number']))
        else:
            log.info("Device [{}] - {} v{} loaded correctly!".format(fw_info['alias'], fw_info['fw_name'], fw_info['fw_version']))

    def get_device_info(self):
        """This fuction returns the current Status of the DeviceInformation Component
           DTDL Component name: DeviceInformation
        Returns:
            dict: DeviceInformation Component current Status if it exists, None elsewhere
        """        
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

    #OK
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
    
    #OK
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

    #OK
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
    
    #OK
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

    #OK
    def get_sw_tag_classes(self):
        if self.device_model is not None:
            tags_info_dict = self.get_component("tags_info")
            return {key: value for key, value in tags_info_dict.items() if "sw_tag" in key}

    #OK
    def get_hw_tag_classes(self):
        if self.device_model is not None:
            tags_info_dict = self.get_component("tags_info")
            return {key: value for key, value in tags_info_dict.items() if "hw_tag" in key}

    #OK
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
        # for each label and for each time segment:
        # time_labels: array of tag
        #   = {'label': lbl, 'time_start': t_start, 'time_end': xxx, }
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
                    tag['Label'] = lbl
                    tag['time_start'] = (datetime.strptime(tstart, '%Y-%m-%dT%H:%M:%S.%fZ') - self.s_t).total_seconds()
                    tag['time_end'] = (datetime.strptime(tend, '%Y-%m-%dT%H:%M:%S.%fZ') - self.s_t).total_seconds()
                    time_labels.append(tag)
            return time_labels
        else:
            log.error("Empty Acquisition Info model.")
            raise MissingAcquisitionInfoError

    def get_data_stream_tags(self, sensor_name, sensor_type = None, start_time = 0, end_time = -1, which_tags = None):
        """
        returns an array of dict:
        {'Label': <Label>, 'time_start': <time start: float>, 'time_end': <time end: float>,'sample_start': <sample index start: int>, 'sample_end': <sample index end: int>}
        """
        res = self.get_data_and_timestamps(sensor_name, None, start_time, end_time)
        if res is not None:
            stream_time = res[1]
            st_tim = np.reshape(stream_time, -1)
            ind_sel = np.array(range(len(st_tim)))
            sensor_labels = []
            for tag in self.get_time_tags(which_tags):
                sampleTag = {}
                tend = float(tag['time_end'])
                if tend > st_tim[-1]: tend = st_tim[-1]
                tend = tend if tend >= 0 else st_tim[-1]
                tstart = float(tag['time_start'])
                if tstart < st_tim[0]: tstart = st_tim[0]
                if tstart > st_tim[-1]: 
                    break
                ind_inf = st_tim <= tend
                ind_sup = st_tim >= tstart
                ind_both = np.logical_and(ind_inf, ind_sup)
                jj = ind_sel[ind_both]
                if len(jj) > 0:
                    s_start = min(jj)
                    s_end = max(jj)
                    sampleTag = {'Label': tag["Label"], 'time_start': tag['time_start'], 'time_end': tag['time_end'],'sample_start': s_start, 'sample_end': s_end}
                    sensor_labels.append(sampleTag)
            return sensor_labels
        log.error("Data extraction error for sensor: {}_{}".format(sensor_name))
        raise DataExtractionError(sensor_name)

    def __get_active_mc_telemetries_names(self, ss_stat, comp_name):
        if comp_name == "slow_mc_telemetries":
            desc_telemetry = ss_stat.get("st_ble_stream")
            return [st for st in desc_telemetry if isinstance(desc_telemetry[st],dict) and desc_telemetry[st].get("enable")==True]
        elif comp_name == "fast_mc_telemetries":
            return [k for k in ss_stat.keys() if isinstance(ss_stat[k],dict) and ss_stat[k]["enabled"] == True]

    def __process_datalog(self, sensor_name, ss_stat, raw_data, dataframe_size, timestamp_size, raw_flag = False, start_time = None):

        #####################################################################
        def extract_data_and_timestamps():
        
            """ gets data from a file .dat
                np array with one column for each axis of each active subSensor
                np array with sample times
            """
            sensor_name_contains_mlc_ispu = "_mlc" in sensor_name or "_ispu" in sensor_name
            
            if c_type == ComponentTypeEnum.SENSOR.value:
                s_category = ss_stat.get("sensor_category")
                check_timestamps = not sensor_name_contains_mlc_ispu
                if not s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                    measodr = ss_stat.get("measodr")
                    if measodr is None:
                        measodr = ss_stat.get("odr")
                    frame_period = 0 if sensor_name_contains_mlc_ispu else samples_per_ts / measodr
                else:
                    frame_period = 0 if sensor_name_contains_mlc_ispu else samples_per_ts / (1//(ss_stat.get("intermeasurement_time")//1000))
            elif c_type == ComponentTypeEnum.ALGORITHM.value:
                check_timestamps = False
                frame_period = 0
                algo_type = ss_stat.get("algorithm_type")
            elif c_type == ComponentTypeEnum.ACTUATOR.value:
                check_timestamps = False
                frame_period = 0
            
            # rndDataBuffer = raw_data rounded to an integer # of frames
            rnd_data_buffer = raw_data[:int(frame_size * num_frames)]

            timestamp_first = ss_stat.get('ioffset', 0)
            timestamps = []
            
            data_type = TypeConversion.check_type(data_type_string)
            data = np.zeros((data1D_per_frame * num_frames, 1), dtype=data_type)

            for ii in range(num_frames):  # For each Frame:
                start_frame = ii * frame_size
                # segment_data = data in the current frame
                segment_data = rnd_data_buffer[start_frame:start_frame + dataframe_size]
                # segment_tS = ts is at the end of each frame
                segment_ts = rnd_data_buffer[start_frame + dataframe_size:start_frame + frame_size]

                # timestamp of current frame
                if segment_ts.size != 0:
                    timestamps.append(np.frombuffer(segment_ts, dtype='double')[0])
                else:
                    timestamp_first += frame_period if start_time is None else frame_period + start_time
                    timestamps.append(timestamp_first)

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
            
            # when you have only 1 frame, framePeriod = last timestamp
            # if num_frames == 1:
            #     timestamps.append(frame_period)

            if c_type == ComponentTypeEnum.SENSOR.value:
                s_dim = ss_stat.get('dim',1)
                s_data = np.reshape(data, (-1, 64 if "_ispu" in sensor_name else s_dim)).astype(dtype=np.byte if "_ispu" in sensor_name else float)
            elif c_type == ComponentTypeEnum.ALGORITHM.value:
                if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                    s_data = np.reshape(data, (-1, ss_stat['fft_length'])).astype(dtype=float)
            elif c_type == ComponentTypeEnum.ACTUATOR.value:
                if sensor_name == MC_SLOW_TELEMETRY_COMP_NAME or sensor_name == MC_FAST_TELEMETRY_COMP_NAME:
                    nof_telemetries = len(self.__get_active_mc_telemetries_names(ss_stat, sensor_name))
                    s_data = np.reshape(data, (-1, nof_telemetries)).astype(dtype=float)
            
            if not raw_flag:
                sensitivity = float(ss_stat.get('sensitivity', 1))
                np.multiply(s_data, sensitivity, out = s_data, casting='unsafe')
            
            # if c_type == ComponentTypeEnum.SENSOR.value or c_type == ComponentTypeEnum.ALGORITHM.value:
            # samples_time: numpy array of 1 clock value per each data sample
            if samples_per_ts > 1:
                # initial_offset is relevant
                ioffset = ss_stat.get('ioffset', 0)
                timestamps = np.append(ioffset, timestamps)
                samples_time = np.zeros((num_frames * samples_per_ts, 1))
                # sample times between timestamps are linearly interpolated
                for ii in range(num_frames): # For each Frame:
                    samples_time[ii * samples_per_ts:(ii + 1) * samples_per_ts, 0] = np.linspace(timestamps[ii], timestamps[ii + 1], samples_per_ts, endpoint=False)
            else:
                # if samples_per_ts is 1, the timestamps coincides with the sample timestamp
                # initial offset and interpolation is not relevant anymore
                samples_time = np.array(timestamps).reshape(-1, 1)
            
            # elif c_type == ComponentTypeEnum.ACTUATOR.value:
            #     # initial_offset is relevant
            #     timestamps = np.append(ss_stat.get('ioffset', 0), timestamps)
            #     samples_time = np.zeros((num_frames * samples_per_ts, 1))
            #     # sample times between timestamps are linearly interpolated
            #     for ii in range(num_frames): # For each Frame:
            #         samples_time[ii * samples_per_ts:(ii + 1) * samples_per_ts, 0] = np.linspace(timestamps[ii], timestamps[ii + 1], samples_per_ts, endpoint=False)

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
            algo_type = ss_stat.get("algorith_type")
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                samples_per_ts = 1#samples_per_ts or int(data1D_per_frame / ss_stat.get('dim', 1))
        elif c_type == ComponentTypeEnum.ACTUATOR.value:
            if sensor_name == "slow_mc_telemetries":
                desc_telemetry = ss_stat.get("st_ble_stream")
                nof_telemetries = len([st for st in desc_telemetry if isinstance(desc_telemetry[st],dict) and desc_telemetry[st].get("enable")==True])
            elif sensor_name == "fast_mc_telemetries":
                nof_telemetries = len([k for k in ss_stat.keys() if isinstance(ss_stat[k],dict) and ss_stat[k]["enabled"] == True])
            samples_per_ts = dataframe_size // data_type_byte_num // nof_telemetries

        return extract_data_and_timestamps()
    
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
    
    def get_data_and_timestamps(self, sensor_name, sub_sensor_type, start_time = 0, end_time = -1, raw_flag = False):
        # get sensor component status
        s_stat = self.__get_sensor_status(sensor_name)
        
        # get acquisition interface
        interface = self.acq_info_model['interface']
        
        # data protocol size:
        data_protocol_size = 4

        # data packet size (0:sd card, 1:usb)
        if interface == 0:
            # s_category = s_stat.get("sensor_category")
            # if s_category is not None:
            #     if s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value or s_category == SensorCategoryEnum.ISENSOR_CLASS_PRESENCE.value: #TODO remove this
            #         data_packet_size = s_stat["sd_dps"]
            #     else:
            #         data_packet_size = s_stat["sd_dps"] - data_protocol_size    
            # else:
            data_packet_size = s_stat["sd_dps"] - data_protocol_size
        else:
            data_packet_size = s_stat["usb_dps"]
        
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
        # s_samples_per_ts = s_stat.get('samples_per_ts', {}).get('val', 0)
        
        if c_type == ComponentTypeEnum.SENSOR.value:
            #TODO sample_end = N/s = 26667 in 1 sec, 266670 in 10 sec, --> 26667*10 in 1*10 sec --> ODR*end_time(in sec) = sample_end
            #TODO sample_start = N/s = 104 in 1 sec, 1040 in 10 sec, --> 104*10 in 1*10 sec --> ODR*start_time(in sec) = sample_start
            s_category = s_stat.get("sensor_category")
            if s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                odr = 1//(s_stat.get("intermeasurement_time")//1000)
            else:
                odr = s_stat.get("measodr")
                if odr is None:
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

                    # n_of_samples = sample_end - sample_start
                    if sample_end == -1:
                        sample_end = total_samples
                        # sample_end = n_of_samples
                    
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

                    # n_of_samples = sample_end - sample_start
                    
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
                
                #NOTE Removed!
                # real_start = sample_start + blocks_before_ss
                # timestamp_pre_id = max(0, blocks_before_ss * s_samples_per_ts + blocks_before_ss - 1)
                # data_offset_pre = real_start - timestamp_pre_id - (blocks_before_ss > 0)

                # if the start_sample isn't in the first block (pre_t_bytes_id != 0)
                if read_start_bytes != 0 :
                    first_timestamp = raw_data[:timestamp_byte_size] if s_samples_per_ts != 0 else 0
                    s_stat['ioffset'] = np.frombuffer(first_timestamp, dtype='double') if s_samples_per_ts != 0 else 0
                    #remove the first timestamp
                    raw_data = raw_data[timestamp_byte_size:]

                data, timestamp = self.__process_datalog(sensor_name, s_stat, raw_data,
                                                         dataframe_byte_size, timestamp_byte_size,
                                                         raw_flag, start_time)

                #NOTE Removed!
                #trim results to obtain only the requested [data,timestamp]
                # data = data[data_offset_pre:n_of_samples]
                # timestamp = timestamp[data_offset_pre:n_of_samples]

                #DEBUG
                log.debug("data Len: {}".format(len(data)))
                log.debug("Time Len: {}".format(len(timestamp)))

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
            # if "dim" not in s_stat:
            #     # algo_packet_size = s_stat["dim"]
            # # else:
            #     log.error("Missing dim property, Algorithm packet size unknown.")
            #     return None, None
            
            # algo_contents = self.components_dtdl[sensor_name].contents 
            # for ac in algo_contents:
            #     if ac.name == "algorithm_type":
            #         algo_type =  ac.schema.enum_values[s_stat["algorithm_type"]].name
            
            if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:#"fft":
                log.info("FFT Algorithm!")
                
                # dataframe_byte_size = int(s_dim * algo_packet_size * s_data_type_len)
                dataframe_byte_size = int(s_dim * s_data_type_len)
                timestamp_byte_size = 0

                # n_of_samples = sample_end - sample_start

                # blocks_before_ss = 0

                # if sample_end == -1:
                # n_of_samples = int(file_size/dataframe_byte_size)
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
                
                data, timestamp = self.__process_datalog(sensor_name, s_stat, raw_data, dataframe_byte_size, timestamp_byte_size, raw_flag = raw_flag )
                
                #trim results to obtain only the requested [data,timestamp]
                # data = data[data_offset_pre:]
                # data = data[:n_of_samples]
                # timestamp = timestamp[data_offset_pre:]
                # timestamp = timestamp[:n_of_samples]

                #DEBUG
                log.debug("data Len: {}".format(len(data)))
                log.debug("Time Len: {}".format(len(timestamp)))
                os.remove(file_path)
                return data, timestamp
            else:
                log.error("Algorithm type not supported")
                os.remove(file_path)
                return None, None
                # raise

        elif c_type == ComponentTypeEnum.ACTUATOR.value:
            # s_samples_per_ts = 1
            
            if sensor_name == MC_SLOW_TELEMETRY_COMP_NAME or sensor_name == MC_FAST_TELEMETRY_COMP_NAME:#"slow_mc_telemetries":
                usb_dps = s_stat.get("usb_dps")
                if usb_dps is not None:
                    timestamp_byte_size = 8
                    dataframe_byte_size = usb_dps - timestamp_byte_size

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

                    print(len(raw_data))
                    print(dataframe_byte_size)
                    if n_of_samples >= 1:
                        first_timestamp = raw_data[dataframe_byte_size:dataframe_byte_size + timestamp_byte_size]
                        print(struct.unpack("=d",first_timestamp))
                    
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
          
    def __v1_unit_map(self, unit):
        if unit == "gForce":
            return "g"
        elif unit == "degreeCelsius":
            return "Celsius"
        elif unit == "percent":
            return "%"
        elif unit == "decibel":
            return "Waveform"
        elif unit == "gForce":
            return "g"
        elif unit == "gForce":
            return "g"
        elif unit == "gForce":
            return "g"
        return unit

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
                col_postfix = ' [' + self.__v1_unit_map(ss_stat["unit"]) + ']'
            c = [col_prefix + s + col_postfix for s in cc] if cc else [s_type.upper()]
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

    def __to_dataframe(self, data, time, ss_stat, sensor_name, labeled = False, start_time = 0, end_time = -1, raw_flag = False):
        if data is not None and time is not None:
            cols = []
            s_type = ""
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
                        cc = range(ss_stat['dim'])
                        col_prefix = s_type[0].upper() + '_'
                        c = [col_prefix + str(s) for s in cc]
                    elif s_category == SensorCategoryEnum.ISENSOR_CLASS_CAMERA:
                        raise UnsupportedSensorCategoryError(sensor_name)#TODO
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

            if s_type != "ispu":
                val = np.concatenate((time, data), axis=1)
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
            
            cols = np.concatenate((["Time"], c), axis=0)

            ss_data_frame = pd.DataFrame(data=val, columns=cols)

            if labeled:
                tags = self.get_data_stream_tags(sensor_name, None, start_time, end_time)
                if any(bool(tag) for tag in tags):
                    for lbl in self.get_acquisition_label_classes():
                        lbl_col = np.zeros(time.shape, dtype=bool)
                        lbl_tags = [x for x in tags if x != {} and x['Label'] == lbl]
                        # for lt in lbl_tags:
                        #     lbl_col[lt['sample_start']:lt['sample_end']] = True
                        start_positions, end_positions = zip(*[(lt['sample_start'], lt['sample_end']) for lt in lbl_tags])
                        for start, end in zip(start_positions, end_positions):
                            lbl_col[start:end] = True
                        ss_data_frame[lbl] = lbl_col

            sensitivity = ss_stat.get("sensitivity", 1)
            
            mapper = {'Time': '{0:.6f}'}
            for i, c in enumerate(cols):
                if raw_flag or sensitivity == 1 and "data_type" in ss_stat:
                    if i != 0:
                        mapper[c] = '{}'.format(TypeConversion.get_str_format(ss_stat["data_type"]))
                else:
                    mapper[c] = '{0:.6f}'
                
            for key, value in mapper.items():
                ss_data_frame[key] = ss_data_frame[key].apply(value.format)
            
            return ss_data_frame
            # else:
            #     log.error("Wrong number of sensor axes ({})".format(numAxes))
            #     raise NSensorAxesError(sensor_name)
        log.error("Error extracting data and timestamp from sensor {}".format(sensor_name))
        raise DataExtractionError(sensor_name)

    def get_dataframe(self, sensor_name, sensor_type = None, start_time = 0, end_time = -1, labeled = False, raw_flag = False):       
        # get sensor component status
        s_stat = self.__get_sensor_status(sensor_name)
        
        res = self.get_data_and_timestamps(sensor_name, sensor_type, start_time, end_time, raw_flag)
        if res is not None:
            data, time = res
            return self.__to_dataframe(data, time, s_stat, sensor_name, labeled, start_time, end_time, raw_flag)
        log.error("Error extracting data and timestamps from {} sensor .dat file".format(sensor_name))
        raise DataExtractionError(sensor_name)


    # Plots Helper Functions ################################################################################################################
    def __draw_line(self, plt, ss_data_frame, idx, color, label):
        plt.plot(ss_data_frame['Time'], ss_data_frame.iloc[:, idx + 1], color=color, label=label)

    def __draw_tag_lines(self, plt, ss_data_frame, label, alpha=0.9):
        true_tag_idxs = ss_data_frame[label].loc[lambda x: x== True].index
        tag_groups = np.split(true_tag_idxs, np.where(np.diff(true_tag_idxs) != 1)[0]+1)
        for i in range(len(tag_groups)):
            start_tag_time = ss_data_frame.at[tag_groups[i][0],'Time']
            end_tag_time = ss_data_frame.at[tag_groups[i][-1],'Time']
            plt.axvspan(start_tag_time, end_tag_time, facecolor='1', alpha=alpha)
            plt.axvline(x=start_tag_time, color='g', label= "Start " + label)
            plt.axvline(x=end_tag_time, color='r', label= "End " + label)
    
    def __set_plot_time_label(self, axs, fig, dim):
        if dim > 1:
            for ax in axs.flat:
                ax.set(xlabel = 'Time (s)')
            for ax in fig.get_axes():
                ax.label_outer()
        else:
            axs[0].set(xlabel = 'Time (s)')
    
    def __set_legend(self, ax, dim):
        handles, labels = ax.get_legend_handles_labels()
        by_label = dict(zip(labels[dim:], handles[dim:]))
        ax.legend(by_label.values(), by_label.keys(), loc='upper left')

    def __draw_regions(self, plt, ss_data_frame, label, color, edgecolor, alpha, hatch):
        true_flag_idxs = ss_data_frame[label].loc[lambda x: x== 1.0].index
        flag_groups = np.split(true_flag_idxs, np.where(np.diff(true_flag_idxs) != 1)[0]+1)
        for i in range(len(flag_groups)):
            start_flag_time = ss_data_frame.at[flag_groups[i][0],'Time']
            end_flag_time = ss_data_frame.at[flag_groups[i][-1],'Time']
            plt.axvspan(start_flag_time, end_flag_time, facecolor=color, edgecolor=edgecolor, alpha=alpha, hatch=hatch)
        
    def __plot_ranging_sensor(self, sensor_name, ss_data_frame, resolution):
        new_shape = (4,4) if resolution == 16 else (8,8)
        start_t1_dist_id = 5
        start_t2_dist_id = 8
        ss_t1_dist_df = ss_data_frame.iloc[:, range(start_t1_dist_id,len(ss_data_frame.columns),8)]
        ss_t2_dist_df = ss_data_frame.iloc[:, range(start_t2_dist_id,len(ss_data_frame.columns),8)]
        dist_matrices = {"t1":[], "t2": []}
        nof_rows = len(ss_t1_dist_df)
        for r_id in range(nof_rows):
            row_t1 = ss_t1_dist_df.iloc[r_id]
            row_t2 = ss_t2_dist_df.iloc[r_id]
            t1_mat = np.array(row_t1.values).reshape(new_shape).astype('float')
            t2_mat = np.array(row_t2.values).reshape(new_shape).astype('float')
            dist_matrices["t1"].append(np.flip(t1_mat,axis=1))
            dist_matrices["t2"].append(np.flip(t2_mat,axis=1))
        fig, (ax1, ax2) = plt.subplots(1, 2)
        plt.subplots_adjust(bottom=0.25) #adjust the bottom space for the slider
        im_t1 = ax1.imshow(dist_matrices["t1"][0], cmap='viridis', vmin=0, vmax=4000)
        plt.colorbar(im_t1)
        im_t2 = ax2.imshow(dist_matrices["t2"][0], cmap='viridis', vmin=0, vmax=4000)
        plt.colorbar(im_t2)

        def update(val):
            time_step = int(slider.val)
            new_matrix_t1 = dist_matrices["t1"][time_step]
            im_t1.set_data(new_matrix_t1)
            new_matrix_t2 = dist_matrices["t2"][time_step]
            im_t2.set_data(new_matrix_t2)
            fig.canvas.draw_idle()

        ax_slider = plt.axes([0.25, 0.02, 0.65, 0.03], facecolor='lightgoldenrodyellow')
        slider = Slider(ax_slider, 'Time Step', 0, nof_rows -1, valinit=0, valstep=1)
        slider.on_changed(update)

        plt.show()
    
    def __plot_light_sensor(self, sensor_name, ss_data_frame, cols, dim, label):
        fig = plt.figure()
        als_lines_colors = ["#FF0000", "#999999", "#0000FF", "#00FF00", "#FF00FF", "#000000"]
        for idx, k in enumerate(range(dim)):
            self.__draw_line(plt, ss_data_frame, k, als_lines_colors[idx], cols[idx])

        ax = fig.axes[0]
        ax.set_xlim(left=-0.5)
        
        if label is not None:
            ax.patch.set_facecolor('0.8')
            ax.set_alpha(float('0.5'))
            self.__draw_tag_lines(plt, ss_data_frame, label)
            self.__set_legend(plt.gca(), dim)

        plt.title(sensor_name)
        plt.tight_layout()
        plt.xlabel('Time (s)')
        plt.legend(loc='upper left')

        plt.draw()
    
    def __plot_presence_sensor(self, sensor_name, ss_data_frame, cols, label):
        fig, axs = plt.subplots(2)
        fig.suptitle("{} - Ambient & Object".format(sensor_name))
        for idx, p in enumerate(axs):
            if idx == 0:#Ambient
                self.__draw_line(p, ss_data_frame, 0, np.random.rand(3, ), cols[0])
            elif idx == 1:#Object
                lines_ids = [1,2,7,8]
                for l in lines_ids:
                    self.__draw_line(p, ss_data_frame, l, np.random.rand(3, ), cols[l])
            if label is not None:
                p.patch.set_facecolor('0.6')
                p.patch.set_alpha(float('0.5'))
                self.__draw_tag_lines(p, ss_data_frame, label)
            p.legend(bbox_to_anchor=(0, 1), loc='upper left', ncol=1)
        plt.xlabel('Time (s)')
        plt.tight_layout()
        plt.draw()

        line_color = "#335c67"
        line_color_sw = "#bb3e03"
        fig, axs = plt.subplots(2)
        fig.suptitle("{} - Presence".format(sensor_name))
        for idx, p in enumerate(axs):
            if idx == 0:#Presence + Flag
                color = "#97D3C2"
                edgecolor = self.__darken_color(color, 10)
                self.__draw_regions(p, ss_data_frame, "Presence flag", color, edgecolor, 1, '')
                self.__draw_line(p, ss_data_frame, 3, line_color, cols[3])
                flag_patch = mpatches.Patch(color=color, label='Presence flag')
            elif idx == 1:#Presence + SW Flag
                color = "#FDD891"
                edgecolor = self.__darken_color(color, 10)
                self.__draw_regions(p, ss_data_frame, "Presence flag (sw_comp)", color, edgecolor, 1, '')
                self.__draw_line(p, ss_data_frame, 3, line_color_sw, cols[3])
                flag_patch = mpatches.Patch(color=color, label='Presence flag (sw_comp)')
            
            if label is not None:
                p.patch.set_facecolor('0.6')
                p.patch.set_alpha(float('0.5'))
                self.__draw_tag_lines(p, ss_data_frame, label, 0.4)

            # where some data has already been plotted to ax
            handles, labels = p.get_legend_handles_labels()
            # handles is a list, so append manual patch
            handles.append(flag_patch)
            # plot the legend
            p.legend(handles=handles, bbox_to_anchor=(0, 1), loc='upper left', ncol=1)
        plt.xlabel('Time (s)')
        plt.tight_layout()
        plt.draw()

        fig, axs = plt.subplots(2)
        fig.suptitle("{} - Motion".format(sensor_name))
        for idx, p in enumerate(axs):
            if idx == 0:#Motion + Flag
                color = "#97D3C2"
                edgecolor = self.__darken_color(color, 10)
                self.__draw_regions(p, ss_data_frame, "Motion flag", color, edgecolor, 1, '')
                self.__draw_line(p, ss_data_frame, 5, line_color, cols[5])
                flag_patch = mpatches.Patch(color=color, label='Motion flag')
            elif idx == 1:#Motion + SW Flag
                color = "#FDD891"
                edgecolor = self.__darken_color(color, 10)
                self.__draw_regions(p, ss_data_frame, "Motion flag (sw_comp)", color, edgecolor, 1, '')
                self.__draw_line(p, ss_data_frame, 5, line_color_sw, cols[5])
                flag_patch = mpatches.Patch(color=color, label='Motion flag (sw_comp)')
            
            if label is not None:
                p.patch.set_facecolor('0.6')
                p.patch.set_alpha(float('0.2'))
                self.__draw_tag_lines(p, ss_data_frame, label)

            # where some data has already been plotted to ax
            handles, labels = p.get_legend_handles_labels()
            # handles is a list, so append manual patch
            handles.append(flag_patch) 
            # plot the legend
            p.legend(handles=handles, bbox_to_anchor=(0, 1), loc='upper left', ncol=1)
        plt.xlabel('Time (s)')
        plt.tight_layout()
        plt.draw()

    def __plot_mems_audio_sensor(self, sensor_name, ss_data_frame, cols, dim, subplots, label, raw_flag, unit):
        if subplots:
            fig, axs = plt.subplots(dim)
            if dim == 1: axs = [axs]
            fig.suptitle(sensor_name)
            if not raw_flag and unit is not None:
                fig.text(0.04, 0.5, self.__v1_unit_map(unit), va='center', rotation='vertical')
            
            for idx, p in enumerate(axs):
                self.__draw_line(p, ss_data_frame, idx, np.random.rand(3, ), cols[idx])
                p.axes.set_xlim(left=-0.5)
                p.set(title=cols[idx])
                if label is not None:
                    p.patch.set_facecolor('0.6')
                    p.patch.set_alpha(float('0.5'))
                    self.__draw_tag_lines(p, ss_data_frame, label)
            
            self.__set_plot_time_label(axs, fig, dim)
            if label is not None:
                self.__set_legend(axs[0],1)
        else:
            fig = plt.figure()
            
            if not raw_flag and unit is not None:
                plt.ylabel(self.__v1_unit_map(unit))

            if "_ispu" in sensor_name:
                n_lines = len(self.get_ispu_output_column_names())
            else:
                n_lines = dim
            for k in range(n_lines):
                self.__draw_line(plt, ss_data_frame, k, np.random.rand(3, ), cols[k])

            ax = fig.axes[0]
            ax.set_xlim(left=-0.5)
            
            plt.title(sensor_name)
            plt.xlabel('Time (s)')
            plt.legend(loc='upper left')

            if label is not None:
                ax.patch.set_facecolor('0.8')
                ax.set_alpha(float('0.5'))
                self.__draw_tag_lines(plt, ss_data_frame, label)
                self.__set_legend(plt.gca(), dim)
        
        plt.draw()
    # Plots Helper Functions ################################################################################################################

    def get_sensor_plot(self, sensor_name, sensor_type = None, start_time = 0, end_time = -1, label=None, subplots=False, raw_flag = False):

        try:
            sensor = self.get_component(sensor_name)
            ss_data_frame = None
            sensor = sensor[sensor_name]
            if sensor['enable']:
                ss_data_frame = self.get_dataframe(sensor_name, sensor_type, start_time, end_time, label is not None, raw_flag)
                ss_stat = sensor

            if ss_data_frame is not None:

                # cast pandas dataframe columns from objects to float
                for k in range(len(ss_data_frame.columns)):
                    ss_data_frame.iloc[:,k] = ss_data_frame.iloc[:,k].astype("float")
                
                #Tag columns check (if any)
                # if label is not None and len(ss_data_frame.columns) < ss_stat['dim'] + 1:
                #     log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                #     label = None

                # #Tag columns check (selected label exists?)
                # if label is not None and not label in ss_data_frame.columns:
                #     log.warning("No {} label found in selected acquisition".format(label))
                #     label = None

                c_type = ss_stat["c_type"]
                s_category = ss_stat.get("sensor_category")

                if c_type == ComponentTypeEnum.ACTUATOR.value:
                    if not label in ss_data_frame.columns:
                        log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                        label = None
                else:
                    #Tag columns check (if any)
                    if label is not None and len(ss_data_frame.columns) < ss_stat['dim'] + 1:
                        log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                        label = None

                    #Tag columns check (selected label exists?)
                    if label is not None and not label in ss_data_frame.columns:
                        log.warning("No {} label found in selected acquisition".format(label))
                        label = None

                #Check for dim (nof plots)
                if c_type == ComponentTypeEnum.SENSOR.value:
                    dim = ss_stat.get("dim")
                elif c_type == ComponentTypeEnum.ALGORITHM.value:
                    pass
                elif c_type == ComponentTypeEnum.ACTUATOR.value:                   
                    telemetry_keys = self.__get_active_mc_telemetries_names(ss_stat, sensor_name)
                    dim = len(telemetry_keys)
                    keys_filter = ["Time"]
                    # selected_keys = self.prompt_actuator_telemetries_select_CLI(ss_stat, telemetry_keys)
                    keys_filter.extend([key.upper() for key in telemetry_keys if isinstance(key, str)])
                    if label is not None:
                        keys_filter.append(label)
                    subplots = True
                    ss_data_frame = ss_data_frame.filter(items=keys_filter)
                cols = ss_data_frame.columns[1:]

                if ss_data_frame is not None:
                    if s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_RANGING.value:
                        self.__plot_ranging_sensor(sensor_name, ss_data_frame, 16 if ss_stat.get("resolution") == "4x4" else 64)
                    elif s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_LIGHT.value:
                        self.__plot_light_sensor(sensor_name, ss_data_frame, cols, dim, label)
                    elif s_category is not None and s_category == SensorCategoryEnum.ISENSOR_CLASS_PRESENCE.value:
                        self.__plot_presence_sensor(sensor_name, ss_data_frame, cols, label)
                    else: #ISENSOR_CLASS_MEMS and ISENSOR_CLASS_AUDIO
                        self.__plot_mems_audio_sensor(sensor_name, ss_data_frame, cols, dim, subplots, label, raw_flag, ss_stat.get('unit'))
            else:
                log.error("Error extracting subsensor DataFrame {}".format(sensor_name))
                raise DataExtractionError(sensor_name)
            return True
        except MemoryError:
            log.error("Memory Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(sensor_name)))
            raise
        except  ValueError:
            log.error("Value Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(sensor_name)))
            raise

    def get_algorithm_plot(self, component_name, start_time = 0, end_time = -1, label=None, subplots=False, raw_flag = False):
        try:
            component = self.get_component(component_name)
            ss_data_frame = None
            component = component[component_name]
            if component['enable']:
                ss_data_frame = self.get_dataframe(component_name, None, start_time, end_time, label is not None, raw_flag)
                ss_stat = component
                
            if ss_data_frame is not None:    
                algo_type = ss_stat.get("algorithm_type")
                if algo_type == AlgorithmTypeEnum.IALGORITHM_TYPE_FFT.value:
                    s_dim = ss_stat.get("fft_length")
                else:
                    s_dim = ss_stat.get("dim")
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
                    
                    if "fft_sample_freq" in ss_stat:
                        fft_sample_freq = ss_stat["fft_sample_freq"]
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
                log.error("Empty DataFrame extrcated.")
        except MissingPropertyError as exc:
            log.error("Missing {} Property Error!".format(exc))
            raise
        except MemoryError:
            log.error("Memory Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(component_name)))
            raise
        except  ValueError:
            log.error("Value Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(component_name)))
            raise
        
    def get_dat_file_list(self):
        return FileManager.get_dat_files_from_folder(self.__acq_folder_path)
    
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

    def present_device_info(self, device_info= None):
        if device_info is None:
            device_info = self.get_device_info()
        CLI.present_item(device_info)

    def present_sensor_list(self, sensor_list = None, type_filter = "", only_active = True, ):
        if sensor_list is None:
            sensor_list = self.get_sensor_list(type_filter, only_active)
        CLI.present_items(sensor_list)

    def present_sensor(self, sensor):
        if sensor is not None:
            CLI.present_item(sensor)
        else:
            self.warning("No sensor selected")

    def present_sw_tag_classes(self, tag_class_list = None):
        if tag_class_list is None:
            tag_class_list = self.get_sw_tag_classes()
        CLI.present_items(tag_class_list)

    def present_hw_tag_classes(self, tag_class_list = None):
        if tag_class_list is None:
            tag_class_list = self.get_hw_tag_classes()
        CLI.present_items(tag_class_list)

    def __darken_color(self, color_hex, percent):
        r = int(color_hex[1:3], 16)
        g = int(color_hex[3:5], 16)
        b = int(color_hex[5:7], 16)

        r_dark = int(r * (100 - percent) / 100)
        g_dark = int(g * (100 - percent) / 100)
        b_dark = int(b * (100 - percent) / 100)

        color_dark = "#{:02x}{:02x}{:02x}".format(r_dark, g_dark, b_dark)

        return color_dark
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
import math
import struct
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from st_hsdatalog.HSD_utils.exceptions import *
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.utils.cli_interaction import CLIInteraction as CLI
from st_hsdatalog.HSD.utils.file_manager import FileManager
from st_hsdatalog.HSD.utils.type_conversion import TypeConversion
from st_pnpl.DTDL.device_template_manager import DeviceTemplateManager
from st_pnpl.DTDL.device_template_model import ContentType, ContentSchema, SchemaType

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
        except:
            raise MissingDeviceModelError

    def __load_device(self, device_dict):
        self.device_model = device_dict['devices'][0]
        
        board_id = hex(self.device_model["board_id"])
        fw_id = hex(self.device_model["fw_id"])
        
        dev_template_json = DeviceTemplateManager.query_dtdl_model(board_id, fw_id)
        dt_manager = DeviceTemplateManager(dev_template_json)
        self.components_dtdl = dt_manager.get_components()
        for comp_name in self.components_dtdl.keys():
            if self.components_dtdl[comp_name].id.split(":")[-2] == "sensors":
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
        return value.replace(',','.')
        
    
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
        log.info("Device [{}] - {} v{} sn:{} loaded correctly!".format(fw_info['alias'], fw_info['fw_name'], fw_info['fw_version'], fw_info['serial_number']))

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
                if element == 'c_type' and s[list(s)[0]]['c_type'] == 0:
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
                if element == 'c_type' and s[list(s)[0]]['c_type'] == 1:
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

    def get_time_tags(self):
        # for each label and for each time segment:
        # time_labels: array of tag
        #   = {'label': lbl, 'time_start': t_start, 'time_end': xxx, }
        time_labels = []
        if self.acq_info_model is not None:
            acq_start_time = self.acq_info_model['start_time']
            acq_end_time = self.acq_info_model['end_time']
            self.s_t = datetime.strptime(acq_start_time, '%Y-%m-%dT%H:%M:%S.%fZ')
            tags = self.acq_info_model['tags']
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

    def get_data_stream_tags(self, sensor_name, sensor_type = None, start_time = 0, end_time = -1):
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
            for tag in self.get_time_tags():
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

    def __process_datalog(self, sensor_name, ss_stat, raw_data, dataframe_size, timestamp_size, raw_flag = False):

        #####################################################################
        def extract_data_and_timestamps(samples_per_ts):
        
            """ gets data from a file .dat
                np array with one column for each axis of each active subSensor
                np array with sample times
            """                
            
            if "c_type" in ss_stat and ss_stat["c_type"] == 0:
                if "_mlc" in sensor_name or "_ispu" in sensor_name:
                    checkTimeStamps = False
                    frame_period = 0
                    samples_per_ts = 1
                else:
                    checkTimeStamps = self.__checkTimestamps
                    # samples_per_ts = ss_stat["samples_per_ts"]["val"]
                    frame_period = samples_per_ts / ss_stat['odr']
            elif "c_type" in ss_stat and ss_stat["c_type"] == 1:
                checkTimeStamps = False
                frame_period = 0 #samples_per_ts / ss_stat['odr']
                    
            # rndDataBuffer = raw_data rounded to an integer # of frames
            rndDataBuffer = raw_data[0:int(frame_size * num_frames)]
            
            timestamp_first = 0
            if "ioffset" in ss_stat:
                timestamp_first = ss_stat['ioffset']
            timestamp = []
            
            data = np.zeros((data1D_per_frame * num_frames, 1), dtype = TypeConversion.check_type(data_type_string))

            for ii in range(num_frames):  # For each Frame:
                startFrame = ii * frame_size
                # segmentData = data in the current frame
                segmentData = rndDataBuffer[startFrame: startFrame + dataframe_size]
                # segmentTS = ts is at the end of each frame
                segmentTS = rndDataBuffer[startFrame + dataframe_size: startFrame + frame_size]

                # timestamp of current frame #TODO extraccted timestamps are wrong!!!
                if segmentTS.size != 0:
                    timestamp.append(np.frombuffer(segmentTS, dtype = 'double')[0])
                else:
                    timestamp_first += frame_period
                    timestamp.append(timestamp_first)

                # Data of current frame
                data[ii * data1D_per_frame:(ii + 1) * data1D_per_frame, 0] = np.frombuffer(segmentData, dtype = TypeConversion.check_type(data_type_string))

                # Check Timestamp consistency
                if checkTimeStamps and (ii > 0):
                    deltaTS = timestamp[ii] - timestamp[ii - 1]
                    if abs(deltaTS) < 0.1 * frame_period or abs(deltaTS) > 10 * frame_period or np.isnan(
                            timestamp[ii]) or np.isnan(timestamp[ii - 1]):
                        data[ii * data1D_per_frame:(ii + 1) * data1D_per_frame, 0] = 0
                        timestamp[ii] = timestamp[ii - 1] + frame_period
                        log.warning("Sensor {}: corrupted data at {}".format(sensor_name, "{} sec".format(timestamp[ii])))

            # when you have only 1 frame, framePeriod = last timestamp
            if num_frames == 1:
                timestamp = np.append(timestamp, frame_period)

            if "c_type" in ss_stat and ss_stat["c_type"] == 0:
                if "_ispu" in sensor_name:
                    sData = np.reshape(data, (-1, 64)).astype(dtype=np.byte)
                else:
                    sData = np.reshape(data, (-1, ss_stat['dim'])).astype(dtype=float)
            elif "c_type" in ss_stat and ss_stat["c_type"] == 1:
                samples_per_ts = 1
                algo_contents = self.components_dtdl[sensor_name].contents 
                for ac in algo_contents:
                    if ac.name == "algorithm_type":
                        algo_type =  ac.schema.enum_values[ss_stat["algorithm_type"]].name ##TODO make Algo Type as a list of supported algos!
                if algo_type == "fft":
                    sData = np.reshape(data, (-1, ss_stat['dim'])).astype(dtype=float)
            
            if not raw_flag:
                if "sensitivity" in ss_stat:
                    sensitivity = float(ss_stat['sensitivity'])
                else:
                    sensitivity = 1
                for kk in range(ss_stat['dim']):
                    sData[:, int(kk)] = sData[:, int(kk)] * sensitivity

            # samples_time: numpy array of 1 clock value per each data sample
            samples_time = np.zeros((num_frames * samples_per_ts, 1))
            if (samples_per_ts >1):
                if "ioffset" in ss_stat:
                    # initial_offset is relevant
                    timestamp = np.append(ss_stat['ioffset'], timestamp)
                # sample times between timestamps are linearly interpolated
                for ii in range(num_frames):  # For each Frame:
                    temp = np.linspace(timestamp[ii], timestamp[ii+1], samples_per_ts, endpoint= False)
                    samples_time[(ii) * samples_per_ts:(ii+1) * samples_per_ts,0] = temp
            else:
                #if samples_per_ts is 1, the timestamps coincides with the sample timestamp
                #initial offset and interpolation is not relevant anymore
                samples_time = np.array(timestamp)
                samples_time = samples_time[:, None]
            return sData, samples_time
        #####################################################################
        
        # size of the frame. A frame is data + ts
        frame_size = dataframe_size + timestamp_size

        # number of frames = round down (//) len datalog // frame_size
        num_frames = len(raw_data) // frame_size
        
        if "_ispu" in sensor_name:
            data_type_string = "int8"
        else:    
            data_type_string = ss_stat['data_type']

        # data1D_per_frame = number of data samples in 1 frame
        # must be the same as samplePerTs * number of axes
        data1D_per_frame = int(dataframe_size / TypeConversion.check_type_length(data_type_string))

        #samples per timestamp
        if "samples_per_ts" in ss_stat:
            samples_per_ts = ss_stat['samples_per_ts']['val']
        else:
            samples_per_ts = 0
        
        if samples_per_ts == 0:
            samples_per_ts = int(data1D_per_frame / ss_stat['dim'])

        return extract_data_and_timestamps(samples_per_ts)
    
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
        
        #TODO 4 the moment (REMOVE PROTOCOL from file)
        
        # get acquisition interface
        interface = self.acq_info_model['interface']
        
        # data packet size (0:sd card, 1:usb)
        if interface == 0:
            data_packet_size = s_stat["sd_dps"] - 4
        else:
            data_packet_size = s_stat["usb_dps"]
        
        # data protocol size:
        data_protocol_size = 4
        
        file_path = self.__get_sensor_file_path(sensor_name)
        file_size = os.path.getsize(file_path)
        
        # n of data packet (4: data)
        nof_data_packet = math.floor(file_size/(data_packet_size + data_protocol_size))
        checked_file_path = os.path.splitext(os.path.abspath(file_path))[0] + "_checked.dat"

        with open(checked_file_path, 'wb') as f:
            with open(file_path, "rb") as rf:
                for n in range(nof_data_packet):
                    rf.seek(n*(data_packet_size+data_protocol_size))
                    rf_data = rf.read(data_packet_size+data_protocol_size)[4:]
                    if not rf_data:
                        log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(n*(data_packet_size+data_protocol_size), file_path, os.stat(f.name).st_size))
                        raise NoDataAtIndexError(n*(data_packet_size+data_protocol_size), file_path, os.stat(f.name).st_size)
                    f.write(np.fromstring(rf_data, dtype='uint8'))
        

        # get dat file path and size (obtained from "sensor_name + sub_sensor_type")
        file_path = self.__get_checked_sensor_file_path(sensor_name)
        file_size = os.path.getsize(file_path)
        
        # get sensor dimensions
        s_dim = s_stat['dim']
        
        # get Data type byte length
        s_data_type_len = TypeConversion.check_type_length(s_stat['data_type'])
        
        # get samples per ts
        if "samples_per_ts" in s_stat:
            s_samples_per_ts = s_stat['samples_per_ts']['val']
        else:
            s_samples_per_ts = 0
        
        if "c_type" in s_stat and s_stat["c_type"] == 0:
            #TODO sample_end = N/s = 26667 in 1 sec, 266670 in 10 sec, --> 26667*10 in 1*10 sec --> ODR*end_time(in sec) = sample_end
            #TODO sample_start = N/s = 104 in 1 sec, 1040 in 10 sec, --> 104*10 in 1*10 sec --> ODR*start_time(in sec) = sample_start
            if "odr" in s_stat:
                odr = s_stat['odr']
            else:
                odr = 1
            sample_end = int(odr*end_time if end_time != -1 else -1)
            sample_start = int(odr*start_time)
            
            try:
                # Sample per Ts == 0 #######################################################################           
                if s_samples_per_ts == 0:
                    dataframe_byte_size = int(s_dim * s_data_type_len)
                    timestamp_byte_size = 0

                    n_of_samples = sample_end - sample_start

                    blocks_before_ss = 0

                    if sample_end == -1:
                        n_of_samples = int((file_size - file_size % (s_data_type_len* s_dim)) / s_data_type_len)
                        sample_end = n_of_samples
                    
                    read_start_bytes = sample_start * (s_data_type_len* s_dim)
                    read_end_bytes = sample_end * (s_data_type_len* s_dim)

                # Sample per Ts != 0 #######################################################################
                else:
                    dataframe_byte_size = int(s_samples_per_ts * s_dim * s_data_type_len)
                    timestamp_byte_size = TypeConversion.check_type_length('double')

                    if sample_end == -1:
                        n_of_blocks_in_file = math.floor(file_size/(timestamp_byte_size + dataframe_byte_size))
                        sample_end = n_of_blocks_in_file * s_samples_per_ts

                    n_of_samples = sample_end - sample_start
                    
                    blocks_before_ss = math.floor(sample_start/s_samples_per_ts)
                    blocks_before_se = math.floor(sample_end/s_samples_per_ts)

                    if blocks_before_ss == 0:
                        read_start_bytes = 0
                    else:
                        read_start_bytes = (blocks_before_ss * dataframe_byte_size) + ((blocks_before_ss - 1) * timestamp_byte_size)
                    read_end_bytes = ((blocks_before_se + 1) * dataframe_byte_size) + ((blocks_before_se + 1) * timestamp_byte_size)

                with open(file_path, "rb") as f:
                    f.seek(read_start_bytes)
                    f_data = f.read(read_end_bytes - read_start_bytes)
                    if not f_data:
                        log.error("No data @ index: {} for file \"{}\" size: {}[bytes]".format(read_start_bytes, file_path, os.stat(f.name).st_size))
                        raise NoDataAtIndexError(read_start_bytes, file_path, os.stat(f.name).st_size)
                    raw_data = np.fromstring(f_data, dtype='uint8')
                    
                    #TODO:Evaluate data integrity, then
                    #TODO:Process raw data to eliminate protocol bytes

                    real_start = sample_start + blocks_before_ss
                    timestamp_pre_id = max(0, blocks_before_ss * s_samples_per_ts + blocks_before_ss - 1)

                    if blocks_before_ss == 0:
                        data_offset_pre = real_start - timestamp_pre_id
                    else:
                        data_offset_pre = real_start - timestamp_pre_id - 1

                    # if the start_sample isn't in the first block (pre_t_bytes_id != 0)
                    if read_start_bytes != 0 :
                        if s_samples_per_ts == 0:
                            first_timestamp = 0
                            s_stat['ioffset'] = 0
                        else:
                            first_timestamp = raw_data[:timestamp_byte_size]
                            s_stat['ioffset'] = np.frombuffer(first_timestamp, dtype = 'double')
                        #remove the first timestamp
                        raw_data = raw_data[timestamp_byte_size:] 

                    data, timestamp = self.__process_datalog(sensor_name, s_stat, raw_data, \
                                                                dataframe_byte_size, timestamp_byte_size,
                                                                raw_flag = raw_flag )

                    #trim results to obtain only the requested [data,timestamp]
                    data = data[data_offset_pre:]
                    data = data[:n_of_samples]
                    timestamp = timestamp[data_offset_pre:]
                    timestamp = timestamp[:n_of_samples]

                    #DEBUG
                    log.debug("data Len: {}".format(len(data)))
                    log.debug("Time Len: {}".format(len(timestamp)))
                return data, timestamp

            except MemoryError:
                log.error("Memory Error occoured! You should batch process your {} file".format(file_path))
                raise
            except OverflowError:
                log.error("Memory Error occoured! You should batch process your {} file".format(file_path))
                raise

        elif "c_type" in s_stat and s_stat["c_type"] == 1:
            if "dim" not in s_stat:
                # algo_packet_size = s_stat["dim"]
            # else:
                log.error("Missing dim property, Algorithm packet size unknown.")
                return None, None
            
            algo_contents = self.components_dtdl[sensor_name].contents 
            for ac in algo_contents:
                if ac.name == "algorithm_type":
                    algo_type =  ac.schema.enum_values[s_stat["algorithm_type"]].name ##TODO make Algo Type as a list of supported algos!
            
            if algo_type == "fft":
                log.info("FFT Algorithm!")
                
                # dataframe_byte_size = int(s_dim * algo_packet_size * s_data_type_len)
                dataframe_byte_size = int(s_dim * s_data_type_len)
                timestamp_byte_size = 0

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
                
                data, timestamp = self.__process_datalog(sensor_name, s_stat, raw_data, dataframe_byte_size, timestamp_byte_size, raw_flag = raw_flag )
                
                #trim results to obtain only the requested [data,timestamp]
                # data = data[data_offset_pre:]
                # data = data[:n_of_samples]
                # timestamp = timestamp[data_offset_pre:]
                # timestamp = timestamp[:n_of_samples]

                #DEBUG
                log.debug("data Len: {}".format(len(data)))
                log.debug("Time Len: {}".format(len(timestamp)))
                
                return data, timestamp
            else:
                log.error("Algorithm type not supported")
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
    
    def __to_dataframe(self, data, time, ss_stat, sensor_name, labeled = False, start_time = 0, end_time = -1):
        if data is not None and time is not None:
            cols = []
            s_type = ""
            if "c_type" in ss_stat and ss_stat["c_type"] == 0:
                numAxes = int(ss_stat['dim'])
                s_name, s_type = FileManager.decode_file_name(sensor_name)
                # if len(ss_stat['dim']) == ss_stat['dim']:#TODO
                if not (s_type == "mlc" or s_type == "stredl" or s_type == "ispu"):
                    if numAxes == 3:
                        cc = ['x','y','z'] #TODO cols = list(range(1,ss_stat['dim']+1)) - 
                        col_prefix = s_type[0].upper() + '_'
                        col_postfix = ''
                        if "unit" in ss_stat:
                            col_postfix = ' [' + self.__v1_unit_map(ss_stat["unit"]) + ']'
                        c = [col_prefix + s + col_postfix for s in cc]
                    elif numAxes == 2:
                        cc = ['x','y']
                        col_prefix = s_type[0].upper() + '_'
                        if "unit" in ss_stat:
                            col_postfix = ' [' + self.__v1_unit_map(ss_stat["unit"]) + ']'
                        c = [col_prefix + s + col_postfix for s in cc]
                    elif numAxes == 1:
                        c = [s_type.upper()]
                    else:
                        cc = ["out_{}".format(c) for c in range(0,numAxes)]
                        col_prefix = s_type[0].upper() + '_'
                        col_postfix = ''
                        if "unit" in ss_stat:
                            col_postfix = ' [' + self.__v1_unit_map(ss_stat["unit"]) + ']'
                        c = [col_prefix + s + col_postfix for s in cc]
                else:
                    if s_type == "ispu":
                        c = self.get_ispu_output_column_names()
                        
                        ispu_out_types = self.get_ispu_output_types()
                        output_bytes_count = 0

                        unpack_str = "="
                        for ot in ispu_out_types:
                            output_bytes_count += TypeConversion.check_type_length(ot)
                            unpack_str += TypeConversion.get_format_char(ot)
                        
                        output_data = []
                        for d in data:
                            d_trimmed = np.array(d[0:output_bytes_count])
                            output_data += struct.unpack(unpack_str, d_trimmed)
                        
                        final_out_data = []
                        for i in range(len(ispu_out_types)):
                            final_out_data.append(np.reshape(np.array(output_data[i::len(ispu_out_types)]), (-1, 1)))
                            
                        val = np.array(time)
                        for fod in final_out_data:
                            val = np.append(val, fod, axis=1)
                    else: 
                        if numAxes > 0:
                            cc = range(ss_stat['dim'])
                            col_prefix = s_type[0].upper() + '_'
                            c = [col_prefix + str(s) for s in cc]
                        else:
                            log.error("Wrong number of sensor axes ({})".format(numAxes))
                            raise NSensorAxesError(sensor_name)

            elif "c_type" in ss_stat and ss_stat["c_type"] == 1:
                cc = range(ss_stat['dim'])
                col_prefix = sensor_name.upper() + '_'
                c = [col_prefix + str(s) for s in cc]
            
            if s_type != "ispu":
                val = np.append(time, data, axis=1)
            
            cols = np.append(cols, c, axis=0)
            cols = np.append(["Time"], cols, axis=0)

            ss_data_frame = pd.DataFrame(data=val, columns=cols)

            if labeled:
                tags = self.get_data_stream_tags(sensor_name, None, start_time, end_time)
                if any(bool(tag) for tag in tags):
                    for lbl in self.get_acquisition_label_classes():
                        lbl_col = np.zeros(time.shape, dtype=bool)
                        lbl_tags = [x for x in tags if x != {} and x['Label'] == lbl]
                        for lt in lbl_tags:
                            lbl_col[lt['sample_start']:lt['sample_end']] = True
                        ss_data_frame[lbl] = lbl_col
            return ss_data_frame
            # else:
            #     log.error("Wrong number of sensor axes ({})".format(numAxes))
            #     raise NSensorAxesError(sensor_name)
        log.error("Error extracting data and timestamp from sensor {}".format(sensor_name))
        raise DataExtractionError(sensor_name)

    def get_dataframe(self, sensor_name, sensor_type = None, start_time = 0, end_time = -1, labeled = False, raw_flag = False):       
        # get sensor component status
        s_stat = self.__get_sensor_status(sensor_name)
        
        # s_stat = self.__get_sensor_status(sensor_name)
        res = self.get_data_and_timestamps(sensor_name, sensor_type, start_time, end_time, raw_flag)
        if res is not None:
            data, time = res
            return self.__to_dataframe(data, time, s_stat, sensor_name, labeled, start_time, end_time)
        log.error("Error extracting data and timestamps from {} sensor .dat file".format(sensor_name))
        raise DataExtractionError(sensor_name)
    
    def get_sensor_plot(self, sensor_name, sensor_type = None, start_time = 0, end_time = -1, label=None, subplots=False, raw_flag = False):
        try:
            sensor = self.get_component(sensor_name)
            ss_data_frame = None
            sensor = sensor[sensor_name]
            if sensor['enable']:
                ss_data_frame = self.get_dataframe(sensor_name, sensor_type, start_time, end_time, label is not None, raw_flag)
                ss_stat = sensor

            if ss_data_frame is not None:

                #Tag columns check (if any)
                if label is not None and len(ss_data_frame.columns) < ss_stat['dim'] + 1:
                    log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                    label = None

                #Tag columns check (selected label exists?)
                if label is not None and not label in ss_data_frame.columns:
                    log.warning("No {} label found in selected acquisition".format(label))
                    label = None

                #Check for dim
                dim = ss_stat["dim"]
                cols = ss_data_frame.columns[1:]
                
                if ss_data_frame is not None:
                    ### labeled and subplots
                    if label is not None and subplots:
                        fig, axs = plt.subplots(dim)
                        if dim == 1:
                            axs = (axs,)
                        tit = sensor_name
                        fig.suptitle(tit)

                        if not raw_flag:
                            fig.text(0.04, 0.5, self.__v1_unit_map(ss_stat['unit']), va='center', rotation='vertical')
                        
                        for ax in axs:
                            ax.patch.set_facecolor('0.6')
                            ax.patch.set_alpha(float('0.5'))

                        for idx, p in enumerate(axs):
                            true_tag_idxs = ss_data_frame[label].loc[lambda x: x== True].index
                            tag_groups = np.split(true_tag_idxs, np.where(np.diff(true_tag_idxs) != 1)[0]+1)
                            
                            p.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, idx + 1], color=np.random.rand(3, ), label=cols[idx])
                            p.axes.set_xlim(left=-0.5)
                                
                            for i in range(len(tag_groups)):
                                start_tag_time = ss_data_frame.at[tag_groups[i][0],'Time']
                                end_tag_time = ss_data_frame.at[tag_groups[i][-1],'Time']
                                p.axvspan(start_tag_time, end_tag_time, facecolor='1', alpha=0.9)
                                p.axvline(x=start_tag_time, color='g', label= "Start " + label)
                                p.axvline(x=end_tag_time, color='r', label= "End " + label)
                            
                            # if len(cols) > 1:
                            p.set(title=cols[idx])

                        if dim > 1:
                            for ax in axs.flat:
                                ax.set(xlabel='Time')
                            for ax in fig.get_axes():
                                ax.label_outer()
                        else:
                            axs[0].set(xlabel='Time')

                        handles, labels = axs[0].get_legend_handles_labels()
                        by_label = dict(zip(labels[1:], handles[1:]))
                        axs[0].legend(by_label.values(), by_label.keys(), loc='upper left')

                        plt.draw()

                    ### not labeled and not subplots
                    elif label is None and not subplots:
                        fig = plt.figure()
                        
                        for k in range(len(cols)):
                            plt.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, k + 1], color=np.random.rand(3, ), label=cols[k])
                        
                        ###### TEST matplotlib offline FFT ######################################################
                        # flatten_df = ss_data_frame.iloc[: , 1:].to_numpy().flatten() #removes empty Time Column
                        # plt.specgram(flatten_df, cmap="rainbow")
                        ###### TEST matplotlib offline FFT ######################################################
                        
                        if not raw_flag:
                            plt.ylabel(self.__v1_unit_map(ss_stat['unit']))

                        fig.axes[0].set_xlim(left=-0.5)

                        plt.title(sensor_name)
                        plt.xlabel('Time (s)')
                        plt.legend(loc='upper left')
                        plt.draw()

                    ### labeled and not subplots
                    elif label is not None and not subplots:
                        fig = plt.figure()
                        for k in range(dim):
                            plt.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, k + 1], color=np.random.rand(3, ), label=cols[k])
                        
                        # [c for c in self.device_model["components"] if list(c.keys())[0] == sensor_name][0][sensor_name]
                        if not raw_flag:
                            plt.ylabel(self.__v1_unit_map(ss_stat['unit']))

                        fig.axes[0].set_xlim(left=-0.5)
                        fig.axes[0].patch.set_facecolor('0.8')
                        fig.axes[0].set_alpha(float('0.5'))

                        true_tag_idxs = ss_data_frame[label].loc[lambda x: x== True].index
                        tag_groups = np.split(true_tag_idxs, np.where(np.diff(true_tag_idxs) != 1)[0]+1)

                        for i in range(len(tag_groups)):
                            # if(len(tag_groups) == 1):
                            #     start_tag_time = ss_data_frame['Time'].iloc[0]
                            #     end_tag_time = ss_data_frame['Time'].iloc[-1]
                            # else:
                            start_tag_time = ss_data_frame.at[tag_groups[i][0],'Time']
                            end_tag_time = ss_data_frame.at[tag_groups[i][-1],'Time']
                            plt.axvspan(start_tag_time, end_tag_time, facecolor='1', alpha=0.9)
                            plt.axvline(x=start_tag_time, color='g', label= "Start " + label)
                            plt.axvline(x=end_tag_time, color='r', label= "End " + label)

                        plt.title(sensor_name)
                        plt.xlabel('Time (s)')
                        
                        handles, labels = plt.gca().get_legend_handles_labels()
                        by_label = dict(zip(labels[dim:], handles[dim:]))
                        plt.legend(by_label.values(), by_label.keys(), loc='upper left')
                        
                        plt.draw()

                    ### not labeled and subplots
                    elif label is None and subplots:
                        fig, axs = plt.subplots(dim)
                        if dim == 1:
                            axs = (axs,)
                        tit = sensor_name
                        fig.suptitle(tit)
                        for idx, p in enumerate(axs):
                            p.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, idx + 1], color=np.random.rand(3, ), label=cols[idx])
                            p.set(title=cols[idx])
                            p.axes.set_xlim(left=-0.5)
                        
                        if not raw_flag:
                            fig.text(0.04, 0.5, self.__v1_unit_map(ss_stat['unit']), va='center', rotation='vertical')
                        
                        if dim > 1:
                            for ax in axs.flat:
                                ax.set(xlabel = 'Time (s)')
                                # if not raw_flag:
                                #     ax.set(ylabel = ss_stat['unit'])
                            for ax in fig.get_axes():
                                ax.label_outer()
                        else:
                            axs[0].set(xlabel = 'Time (s)')
                            # if not raw_flag:
                            #     axs[0].set(ylabel = ss_stat['unit'])
                        plt.draw()
            
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
                               
                #Tag columns check (if any)
                if label is not None and len(ss_data_frame.columns) < ss_stat['dim'] + 1:
                    log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                    label = None

                #Tag columns check (selected label exists?)
                if label is not None and not label in ss_data_frame.columns:
                    log.warning("No {} label found in selected acquisition".format(label))
                    label = None
                
                algo_contents = self.components_dtdl[component_name].contents 
                for ac in algo_contents:
                    if ac.name == "algorithm_type":
                        algo_type =  ac.schema.enum_values[ss_stat["algorithm_type"]].name ##TODO make Algo Type as a list of supported algos!
                
                if algo_type == "fft":
                    if "dim" in ss_stat:
                        fft_lenght = ss_stat["dim"]
                    else:
                        log.error("FFT length unknown")
                        raise MissingPropertyError("dim")
                    
                    if "fft_sample_freq" in ss_stat:
                        fft_sample_freq = ss_stat["fft_sample_freq"]
                    else:
                        log.error("FFT Sample Freq. unknown")
                        raise MissingPropertyError("fft_sample_freq")
                    
                    fig = plt.figure()
                    
                    df_array = ss_data_frame.iloc[: , 1:].T.to_numpy()                    
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
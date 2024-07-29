# *****************************************************************************
#  * @file    HSDatalog_v1.py
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

import json
import os
import math
import struct
from collections import OrderedDict
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from st_hsdatalog.HSD.utils.plot_utils import PlotUtils
from st_hsdatalog.HSD_utils.exceptions import *
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.model.DeviceConfig import Device, DeviceInfo, Sensor, SensorDescriptor
from st_hsdatalog.HSD.model.AcquisitionInfo import AcquisitionInfo
from st_hsdatalog.HSD.utils.cli_interaction import CLIInteraction as CLI
from st_hsdatalog.HSD.utils.file_manager import FileManager
from st_hsdatalog.HSD.utils.type_conversion import TypeConversion
from st_pnpl.DTDL.dtdl_utils import UnitMap

log = logger.get_logger(__name__)

class HSDatalog_v1:
    device_model: Device
    acq_info_model: AcquisitionInfo
    hsd2_acq_info_model: dict
    __acq_folder_path = None
    __checkTimestamps = False
    
    def __init__(self, acquisition_folder = None):
        if acquisition_folder is not None:
            device_json_file_path = FileManager.find_file("DeviceConfig.json", acquisition_folder)
            if device_json_file_path is None:
                raise AcquisitionFormatError
            self.__load_device_from_file(device_json_file_path)
            try:
                acquisition_json_file_path = FileManager.find_file("AcquisitionInfo.json", acquisition_folder)
                if acquisition_json_file_path is None:
                    raise MissingAcquisitionInfoError
                self.__load_acquisition_info_from_file(acquisition_json_file_path)
            except MissingAcquisitionInfoError:
                log.error("No AcquisitionInfo.json file in your Acquisition folder")
                raise
        else:
            log.warning("Acquisition folder not provided.")
            self.device_model = None
            self.acq_info_model = None

        self.__acq_folder_path = acquisition_folder
        self.data_protocol_size = 0
    
    #========================================================================================#
    ### Data Analisys ########################################################################
    #========================================================================================#

    def __update_hsdv1_acquisition_info(self):
        ai_dict = {}
        tags = []
        for t in self.acq_info_model.tags:
            tags.append({"l": t.label, "e": t.enable, "ta": t.t})
        ai_dict["tags"] = tags
        ai_dict["name"] = self.acq_info_model.name
        ai_dict["description"] = self.acq_info_model.description
        ai_dict["uuid"] = self.acq_info_model.uuid_acquisition
        ai_dict["start_time"] = float(self.acq_info_model.start_time)
        ai_dict["end_time"] = float(self.acq_info_model.end_time)
        ai_dict["data_ext"] = ".dat"
        ai_dict["data_fmt"] = "HSD_1.0.0"
        ai_dict["interface"] = 1
        ai_dict["schema_version"] = "2.0.0"
        ai_dict["c_type"] = 2
        self.hsd2_acq_info_model = ai_dict

    ### ==> Debug ###
    def enable_timestamp_recovery(self, status):
        self.__checkTimestamps = status
    ### Debug <== ###

    def __load_device_from_file(self, device_json_file_path):
        try:
            with open(device_json_file_path) as f:
                device_json_dict = json.load(f)
            device_json_str = json.dumps(device_json_dict)
            f.close()
            device_dict = json.loads(device_json_str)
            self.device_model = Device.from_dict(device_dict['device'])
        except:
            raise

    def __load_acquisition_info_from_file(self, acq_info_json_file_path):
        try:
            with open(acq_info_json_file_path) as f:
                acq_info_json_dict = json.load(f)
            acq_info_json_str = json.dumps(acq_info_json_dict)
            f.close()
            acq_info_dict = json.loads(acq_info_json_str)
            self.acq_info_model = AcquisitionInfo.from_dict(acq_info_dict)
            self.__update_hsdv1_acquisition_info()
        except:
            raise MissingAcquisitionInfoError

    def get_data_protocol_size(self):
        return self.data_protocol_size
    
    def get_acquisition_path(self):
        return self.__acq_folder_path

    def get_device(self):
        return self.device_model

    def set_device(self, new_device, device_id = 0, from_file = True):
        self.device_model = new_device
        log.info("Device [{}] - {} loaded correctly!".format(self.device_model.device_info.alias,self.device_model.device_info.part_number))

    def get_device_info(self):
        return self.device_model.device_info
    
    def get_firmware_info(self):
        log.warning("API not supported from HSDatalog_v1")
        return None

    def set_device_info(self, new_device_info:DeviceInfo):
        self.device_model.device_info = new_device_info

    def get_sensor(self, sensor_name):
        sensor_name = sensor_name.split('_')[0].upper()
        if self.device_model is not None:
            for s in self.device_model.sensor:
                if s.name == sensor_name:
                    c_name = ""
                    c_dict = {}
                    sensor_name = s.name.lower()
                    sub_sensor_status = s.sensor_status.sub_sensor_status
                    sub_sensor_descriptor = s.sensor_descriptor.sub_sensor_descriptor
                    for i, sd in enumerate(sub_sensor_descriptor):
                        sensor_type =  sd.sensor_type.lower()
                        c_name = sensor_name + "_" + sensor_type
                        c_dict[c_name] = {}
                        c_dict[c_name]["dim"] = sd.dimensions
                        c_dict[c_name]["data_type"] = sd.data_type
                        c_dict[c_name]["unit"] = sd.unit
                        ss = sub_sensor_status[i]
                        c_dict[c_name]["odr"] = ss.odr
                        c_dict[c_name]["fs"] = ss.fs
                        c_dict[c_name]["enable"] = ss.is_active
                        c_dict[c_name]["samples_per_ts"] = ss.samples_per_ts
                        c_dict[c_name]["ioffset"] = ss.initial_offset
                        c_dict[c_name]["measodr"] = ss.odr_measured
                        c_dict[c_name]["usb_dps"] = ss.usb_data_packet_size
                        c_dict[c_name]["sd_dps"] = ss.sd_write_buffer_size
                        c_dict[c_name]["sensitivity"] = ss.sensitivity
                        c_dict[c_name]["sensor_annotation"] = ""
                        if sensor_type == "mic":
                            c_dict[c_name]["sensor_category"] = 1
                        else:#mems
                            c_dict[c_name]["sensor_category"] = 0
                        c_dict[c_name]["c_type"] = 0
                        c_dict[c_name]["stream_id"] = ss.com_channel_number
                        c_dict[c_name]["ep_id"] = ss.com_channel_number
                    return c_dict
            log.error("No Model loaded for {} sensor".format(sensor_name))
            raise MissingSensorModelError
        else:
            log.error("No Device Model loaded!")
            raise MissingDeviceModelError

    def get_sub_sensor(self, sensor_name, ss_id = None, ss_type = None):
        sensor = self.get_sensor(sensor_name)
        if sensor is not None:
            ss_desc_list = sensor.sensor_descriptor.sub_sensor_descriptor
            ss_stat_list = sensor.sensor_status.sub_sensor_status
            if ss_id is not None:
                if ss_id < len(ss_desc_list):
                    return [ss_desc_list[ss_id], ss_stat_list[ss_id]]
                else:
                    log.error("Selected ss_id: {} doesn't exist for {} sensor".format(ss_id, sensor_name))
                    raise SubSensorIDError(sensor_name)
            elif ss_type is not None:
                for i, ssdesc in enumerate(ss_desc_list):
                    if ssdesc.sensor_type == ss_type:
                        return [ssdesc,ss_stat_list[i]]
                log.error("Selected ss_type: {} doesn't exist for {} sensor".format(ss_type, sensor_name))
                raise SubSensorTypeError(sensor_name)
            else:
                log.error("ss_id and ss_type for {} sensor are None".format(sensor_name))
                raise SensorParamsError(sensor_name)
        log.error("{} sensor not found".format(sensor_name))
        raise MissingSensorModelError

    def __get_sensor_file_path(self, sensor_name):
        file_path = os.path.join(self.__acq_folder_path, FileManager.encode_file_name(sensor_name))
        if not os.path.exists(file_path):
            log.error("No such file or directory: {} found for {} sensor".format(file_path, sensor_name))
            raise MissingFileForSensorError(file_path, sensor_name)
        return file_path

    def get_sub_sensors(self, sensor_name, only_active = False):
        sensor = self.get_sensor(sensor_name)
        if sensor is not None:
            ss_desc_list = sensor.sensor_descriptor.sub_sensor_descriptor
            ss_stat_list = sensor.sensor_status.sub_sensor_status
            new_ss_desc_list = []
            new_ss_stat_list = []
            if only_active:
                for i, sss in enumerate(ss_stat_list):
                    if sss.is_active:
                        new_ss_desc_list.append(ss_desc_list[i])
                        new_ss_stat_list.append(sss)
            return [new_ss_desc_list, new_ss_stat_list]
        log.error("No {} sensor found in Device Model".format(sensor_name))
        raise MissingSensorModelError
    
    def get_sensor_list(self, type_filter = "", only_active = False):
        active_sensors = []
        sensor_list = self.device_model.sensor.copy()
        for s in sensor_list:
            active_sensor = Sensor()
            active_sensor.id = s.id
            active_sensor.name = s.name
            active_sensor.sensor_descriptor = SensorDescriptor()
            active_sensor.sensor_status = SensorDescriptor()
            active_ss_stat_list = []
            active_ss_desc_list = []
            ss_stat_list = s.sensor_status.sub_sensor_status
            ss_desc_list = s.sensor_descriptor.sub_sensor_descriptor
            for i, sss in enumerate(ss_stat_list):
                if type_filter == "":
                    if only_active:
                        if sss.is_active:
                            active_ss_stat_list.append(sss)
                            active_ss_desc_list.append(ss_desc_list[i])
                    else:
                        active_ss_stat_list.append(sss)
                        active_ss_desc_list.append(ss_desc_list[i])
                else:
                    if only_active:
                        if sss.is_active and ss_desc_list[i].sensor_type == type_filter.upper():
                            active_ss_stat_list.append(sss)
                            active_ss_desc_list.append(ss_desc_list[i])
                    else:
                        if ss_desc_list[i].sensor_type == type_filter.upper():
                            active_ss_stat_list.append(sss)
                            active_ss_desc_list.append(ss_desc_list[i])
            active_sensor.sensor_descriptor.sub_sensor_descriptor = active_ss_desc_list
            active_sensor.sensor_status.sub_sensor_status = active_ss_stat_list
            if(len(active_ss_desc_list)>0):
                active_sensors.append(active_sensor)

        hsdv2_style_components = []
        for c in active_sensors:
            c_name = ""
            c_dict = {}
            sensor_name = c.name.lower()
            sub_sensor_status = c.sensor_status.sub_sensor_status
            sub_sensor_descriptor = c.sensor_descriptor.sub_sensor_descriptor
            for i, sd in enumerate(sub_sensor_descriptor):
                sensor_type =  sd.sensor_type.lower()
                c_name = sensor_name + "_" + sensor_type
                c_dict[c_name] = {}
                c_dict[c_name]["dim"] = sd.dimensions
                c_dict[c_name]["data_type"] = sd.data_type
                c_dict[c_name]["unit"] = sd.unit
                ss = sub_sensor_status[i]
                c_dict[c_name]["odr"] = ss.odr
                c_dict[c_name]["fs"] = ss.fs
                c_dict[c_name]["enable"] = ss.is_active
                c_dict[c_name]["samples_per_ts"] = ss.samples_per_ts
                c_dict[c_name]["ioffset"] = ss.initial_offset
                c_dict[c_name]["measodr"] = ss.odr_measured
                c_dict[c_name]["usb_dps"] = ss.usb_data_packet_size
                c_dict[c_name]["sd_dps"] = ss.sd_write_buffer_size
                c_dict[c_name]["sensitivity"] = ss.sensitivity
                c_dict[c_name]["sensor_annotation"] = ""
                if sensor_type == "mic":
                    c_dict[c_name]["sensor_category"] = 1
                else:#mems
                    c_dict[c_name]["sensor_category"] = 0
                c_dict[c_name]["c_type"] = 0
                c_dict[c_name]["stream_id"] = ss.com_channel_number
                c_dict[c_name]["ep_id"] = ss.com_channel_number
                
            hsdv2_style_components.extend([{key: value} for key, value in c_dict.items()])
        active_sensors = hsdv2_style_components

        return active_sensors

    def get_sw_tag_classes(self):
        if self.device_model is not None:
            sw_tags = self.device_model.tag_config.sw_tags
            t_dict = {}
            for swt in sw_tags:
                t_dict[f"sw_tag{swt.id}"] = {'label': swt.label, 'enabled': True, 'status': False}
            return t_dict

    def get_hw_tag_classes(self):
        if self.device_model is not None:
            hw_tags = self.device_model.tag_config.hw_tags
            t_dict = {}
            for hwt in hw_tags:
                t_dict[f"hw_tag{hwt.id}"] = {'label': hwt.label, 'enabled': hwt.enabled, 'status': False}
            return t_dict

    def get_old_acquisition_info(self):
        return self.acq_info_model

    def get_acquisition_info(self):
        return self.hsd2_acq_info_model

    def set_acquisition_info(self, new_acquisition_info):
        self.acq_info_model = new_acquisition_info
        self.__update_hsdv1_acquisition_info()

    def get_acquisition_label_classes(self):
        if self.acq_info_model is not None:
            return sorted(set(dic.label for dic in self.acq_info_model.tags))
        log.warning("Empty Acquisition Info model.")
        # raise MissingAcquisitionInfoError
        return None

    def get_time_tags(self, which_tags = None):
        # for each label and for each time segment:
        # time_labels: array of tag
        #   = {'label': lbl, 'time_start': t_start, 'time_end': xxx, }
        time_labels = []
        if self.acq_info_model is not None:
            tags = self.acq_info_model.tags

            if which_tags is not None:
                tags = [tag for tag in tags if tag.label in which_tags]
                
            for lbl in self.get_acquisition_label_classes():
                # start_time, end_time are vectors with the corresponding 't' entries in DataTag-json
                start_time = np.array([t.t for t in tags if t.label == lbl and t.enable])
                end_time = np.array([t.t for t in tags if t.label == lbl and not t.enable])
                # now must associate at each start tag the appropriate end tag
                # (some may be missing because of errors in the tagging process)
                for tstart in start_time:
                    tag = {}
                    jj = [i for (i, n) in enumerate(end_time) if n >= tstart]
                    if jj:
                        tend = end_time[min(jj)]
                    else:
                        tend = float(-1)  # if no 'end tag' found the end is eof
                    tag['label'] = lbl
                    tag['time_start'] = tstart
                    tag['time_end'] = tend
                    time_labels.append(tag)
            return time_labels
        else:
            log.error("Empty Acquisition Info model.")
            raise MissingAcquisitionInfoError
    
    def __process_datalog(self, sensor_name, ss_stat, raw_data, dataframe_size, timestamp_size, raw_flag = False, start_time = None, prev_timestamp = None):

        #####################################################################
        def extract_data_and_timestamps(start_time):
        
            """ gets data from a file .dat
                np array with one column for each axis of each active subSensor
                np array with sample times
            """
            sensor_name_contains_mlc_ispu = "_mlc" in sensor_name or "_ispu" in sensor_name
            
            if self.__checkTimestamps == True:
                check_timestamps = not sensor_name_contains_mlc_ispu
            else:
                check_timestamps = False

            measodr = ss_stat.get("measodr")
            if measodr is None or measodr == 0:
                measodr = ss_stat.get("odr")
            frame_period = 0 if sensor_name_contains_mlc_ispu else samples_per_ts / measodr
            
            # rndDataBuffer = raw_data rounded to an integer # of frames
            rnd_data_buffer = raw_data[:int(frame_size * num_frames)]

            if start_time != 0:
                timestamp_first = start_time #TODO check with spts != 0
            else:
                timestamp_first = ss_stat.get('ioffset', 0)
            timestamps = []
            
            data_type = TypeConversion.check_type(data_type_string)
            data = np.zeros((data1D_per_frame * num_frames, 1), dtype=data_type)

            if timestamp_size != 0:
                for ii in range(num_frames):  # For each Frame:
                    start_frame = ii * frame_size
                    # segment_data = data in the current frame
                    segment_data = rnd_data_buffer[start_frame:start_frame + dataframe_size]
                    # segment_tS = ts is at the end of each frame
                    segment_ts = rnd_data_buffer[start_frame + dataframe_size:start_frame + frame_size]

                    # # timestamp of current frame
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

            s_dim = ss_stat.get('dim',1)
            if raw_flag:
                s_data = np.reshape(data, (-1, 64 if "_ispu" in sensor_name else s_dim))
            else:
                s_data = np.reshape(data, (-1, 64 if "_ispu" in sensor_name else s_dim)).astype(dtype=np.byte if "_ispu" in sensor_name else np.float32)
            
            if not raw_flag:
                sensitivity = float(ss_stat.get('sensitivity', 1))
                np.multiply(s_data, sensitivity, out = s_data, casting='unsafe')

            if len(data) == 0:
                return [],[]
            
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
        
        samples_per_ts = samples_per_ts or int(data1D_per_frame / ss_stat.get('dim', 1))

        return extract_data_and_timestamps(start_time)

    #TODO Deprecate this function
    def get_data_and_timestamps(self, sensor_name, sub_sensor_type, start_time = 0, end_time = -1, raw_flag = False):

        # get sub sensor descriptor and status
        ss_desc, ss_stat = self.get_sub_sensor(sensor_name, ss_type=sub_sensor_type)

        # get dat file path (obtained from "sensor_name + sub_sensor_type")
        file_path = os.path.join(self.__acq_folder_path, FileManager.encode_file_name(sensor_name,ss_desc.sensor_type))
        if not os.path.exists(file_path):
            log.error("No such file or directory: {} found for {} sensor".format(file_path, sensor_name))
            raise MissingFileForSensorError(file_path, sensor_name)
        
        odr = ss_stat.odr
        sample_end = int(odr*end_time if end_time != -1 else -1)
        sample_start = int(odr*start_time)

        try:
            # Sample per Ts == 0 #######################################################################           
            if ss_stat.samples_per_ts == 0:
                dataframe_byte_size = int(ss_desc.dimensions * TypeConversion.check_type_length(ss_desc.data_type))
                timestamp_byte_size = 0

                n_of_samples = sample_end - sample_start

                blocks_before_ss = 0

                if sample_end == -1:
                    n_of_samples = int((os.path.getsize(file_path) - os.path.getsize(file_path) % (TypeConversion.check_type_length(ss_desc.data_type) * ss_desc.dimensions)) / TypeConversion.check_type_length(ss_desc.data_type))
                    sample_end = n_of_samples
                
                read_start_bytes = sample_start * (TypeConversion.check_type_length(ss_desc.data_type) * ss_desc.dimensions)
                read_end_bytes = sample_end * (TypeConversion.check_type_length(ss_desc.data_type) * ss_desc.dimensions)

            # Sample per Ts != 0 #######################################################################
            else:
                dataframe_byte_size = int(ss_stat.samples_per_ts * ss_desc.dimensions * TypeConversion.check_type_length(ss_desc.data_type))
                timestamp_byte_size = TypeConversion.check_type_length('double')

                if sample_end == -1:
                    n_of_blocks_in_file = math.floor(os.path.getsize(file_path)/(timestamp_byte_size + dataframe_byte_size))
                    sample_end = n_of_blocks_in_file * ss_stat.samples_per_ts

                n_of_samples = sample_end - sample_start
                
                blocks_before_ss = math.floor(sample_start/(ss_stat.samples_per_ts))
                blocks_before_se = math.floor(sample_end/(ss_stat.samples_per_ts))

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

                real_start = sample_start + blocks_before_ss
                timestamp_pre_id = max(0, blocks_before_ss * ss_stat.samples_per_ts + blocks_before_ss - 1)

                if blocks_before_ss == 0:
                    data_offset_pre = real_start - timestamp_pre_id
                else:
                    data_offset_pre = real_start - timestamp_pre_id - 1

                # if the start_sample isn't in the first block (pre_t_bytes_id != 0)
                if read_start_bytes != 0 :
                    first_timestamp = raw_data[:timestamp_byte_size]
                    ss_stat.initial_offset = np.frombuffer(first_timestamp, dtype = 'double')
                    #remove the first timestamp
                    raw_data = raw_data[timestamp_byte_size:] 

                data, timestamp = self.__process_datalog(sensor_name, ss_desc, ss_stat, raw_data, \
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
    
    def get_data_and_timestamps_batch(self, comp_name, comp_status, start_time = 0, end_time = -1, raw_flag = False):
        
        log.debug("Data & Timestamp extraction algorithm STARTED...")

        # get acquisition interface
        interface = self.hsd2_acq_info_model['interface']

        data_packet_size = 0
        # data packet size (0:sd card, 1:usb, 2:ble, 3:serial)
        if interface == 0:
            data_packet_size = comp_status["sd_dps"]
        elif interface == 1:
            data_packet_size = comp_status["usb_dps"]
        else:
            log.error(f"Unknown interface: {interface}. check your device_config.json file")
            raise
        
        # get dat file path and size (obtained from "sensor_name + sub_sensor_type")
        file_path = self.__get_sensor_file_path(comp_name)
        file_size = os.path.getsize(file_path)

        nof_data_packet = file_size // data_packet_size # "//" math.floor equivalent #CEIL

        raw_data_array = np.array([], dtype='uint8')

        # get sensor dimensions
        s_dim = comp_status.get('dim', 1)
        
        # get Data type byte length
        s_data_type_len = TypeConversion.check_type_length(comp_status['data_type'])
        
        # get samples per ts
        spts = comp_status.get('samples_per_ts', {})
        if isinstance(spts, int):
            s_samples_per_ts = spts
        else:
            s_samples_per_ts = spts.get('val', 0)

        if s_samples_per_ts != 0:
            dataframe_byte_size = s_samples_per_ts * s_dim * s_data_type_len
            timestamp_byte_size = 8
        else:
            dataframe_byte_size = s_dim * s_data_type_len
            timestamp_byte_size = 0

        # 1 sec --> ODR samples --> ODR * dim * data_type Bytes
        odr = comp_status.get("measodr", comp_status.get("odr", 1))

        tot_counters_bytes = nof_data_packet
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

        start_idx = start_data_bytes_idx + nof_timestamps_in_start * timestamp_byte_size

        last_timestamp = tot_data_samples/odr
        if end_time == -1 or (end_time > last_timestamp):
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
            estimated_size = (nof_data_packet +1) * data_packet_size
            byte_chest = np.empty(2*estimated_size, dtype='uint8')
            raw_data_array = np.empty(2*estimated_size, dtype='uint8')
            
            last_index = comp_status.get("last_index", 0)
            missing_bytes = comp_status.get("missing_bytes", 0)
            saved_bytes = comp_status.get("saved_bytes", 0)
            
            if last_index == 0 and start_idx != 0:
                packet_bytes = nof_prev_timestamps * (dataframe_byte_size + timestamp_byte_size)
                missing_bytes = math.ceil(packet_bytes/data_packet_size) * data_packet_size - packet_bytes
                log.debug(f"User customized time boudaries: {start_time}, {end_time}")
                log.debug(f"- packet_bytes: {packet_bytes}")
                log.debug(f"- missing_bytes: {missing_bytes}")
                last_index = packet_bytes

            byte_chest_index = 0
            raw_data_array_index = 0
            extracted_data_length = 0
            end_time_flag = False
            prev_timestamp = None
            
            if start_idx >= file_size:
                return [],None

            with open(file_path, 'rb') as f:
                for n in range(nof_data_packet):
                    file_index = last_index + (n * data_packet_size)
                    log.debug(f"missing_bytes: {missing_bytes}")
                    log.debug(f"file_index: {file_index}")
                    if (file_index >= file_size):
                        return [],None
                    f.seek(file_index)
                    if last_index != 0:
                        if saved_bytes != 0 and saved_bytes <= missing_bytes:
                            raw_data = f.read(missing_bytes)
                            log.debug(f"Bytes read from file: {missing_bytes}")
                            comp_status["is_same_dps"] = True
                            if len(raw_data) < missing_bytes:
                                return [],None
                            data_bytes = raw_data[:missing_bytes]
                        else:
                            raw_data = f.read(missing_bytes + data_packet_size)
                            comp_status["is_same_dps"] = False
                            log.debug(f"Bytes read from file: {missing_bytes + data_packet_size}")
                            if len(raw_data) < missing_bytes + data_packet_size:
                                return [],None
                            data_bytes = raw_data
                        last_index += missing_bytes
                        comp_status["missing_bytes"] = missing_bytes = 0
                    else:
                        raw_data = f.read(data_packet_size)
                        log.debug(f"Bytes read from file: {data_packet_size}")
                        if len(raw_data) < data_packet_size:
                            return [],None
                        data_bytes = raw_data
                    
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
                                bytes_processed = (last_index + (n+1) * data_packet_size)

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
                                        bytes_processed = (last_index + (n+1) * data_packet_size)
                                    
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
                            bytes_processed = (last_index + (n+1) * data_packet_size)                                    
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
            bytes_processed = (last_index + (nof_data_packet+1) * data_packet_size)                                    
            comp_status["missing_bytes"] = byte_chest_index
            comp_status["saved_bytes"] = raw_data_array_index
            comp_status["last_index"] = bytes_processed - comp_status["missing_bytes"]

        #DEBUG
        log.debug(f"data Len: {len(data)}")
        log.debug(f"Time Len: {len(timestamp)}")
        return data, timestamp
    
    def get_component_columns_names(self, ss_stat, sensor_name):
        s_type = ""
        numAxes = int(ss_stat.get('dim',1))
        s_name, s_type = FileManager.decode_file_name(sensor_name)
        c = self.__get_mems_columns_names(ss_stat, sensor_name, s_type, numAxes)
        return c
    
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
    
    def get_ispu_output_types(self):
        if self.ispu_output_format is not None:
            return [TypeConversion.check_type(o["type"]) for o in self.ispu_output_format["output"]]
        else:
            return None

    # Get tags dictionary list from acquisition_info.json file
    def get_tags(self):
        tags = []
        acq_start_time = self.hsd2_acq_info_model["start_time"]
        if acq_start_time == "0":
            acq_start_time = 0 #TODO CHECK THIS
        acq_end_time = self.hsd2_acq_info_model["end_time"]
        tags_array = self.hsd2_acq_info_model["tags"]
        if acq_end_time == "0" and len(tags_array) > 0:
            acq_end_time = tags_array[-1]["ta"]
        acq_duration = acq_end_time - acq_start_time

        tag_labels = []
        for i in range(len(tags_array)):
            start_tag = tags_array[i]
            if start_tag["e"]:
                tag_label = start_tag["l"]
                tag_start_seconds = start_tag["ta"]
                tag_end_seconds = acq_duration
                for j in range(i, len(tags_array)):
                    end_tag = tags_array[j]
                    if end_tag["l"] == tag_label and not end_tag["e"]:
                        tag_end_seconds = end_tag["ta"]
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

    def __to_dataframe(self, data, time, ss_stat, sensor_name, labeled = False, which_tags:list = [], raw_flag = False):
        if data is not None and time is not None:
            cols = []
            s_type = ""

            numAxes = int(ss_stat.get('dim',1))
            _, s_type = FileManager.decode_file_name(sensor_name)

            c = self.__get_mems_columns_names(ss_stat, sensor_name, s_type, numAxes)

            if s_type != "ispu":
                try:
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
            
            cols = np.concatenate((["Time"], c), axis=0)

            ss_data_frame = pd.DataFrame(data=val, columns=cols)

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
    
    def get_dataframe(self, sensor_name, sub_sensor_type, start_time = 0, end_time = -1, labeled = False, raw_flag = False):
        ss_desc, ss_stat = self.get_sub_sensor(sensor_name, ss_type=sub_sensor_type)
        res = self.get_data_and_timestamps(sensor_name, sub_sensor_type, start_time, end_time, raw_flag)
        if res is not None:
            data, time = res
            return self.__to_dataframe(data, time, ss_desc, sensor_name, sub_sensor_type, labeled, start_time, end_time)
        log.error("Error extracting data and timestamps from {}_{} sensor .dat file".format(sensor_name, sub_sensor_type))
        raise DataExtractionError(sensor_name, sub_sensor_type)

    def get_dataframe_batch(self, comp_name, comp_status, start_time = 0, end_time = -1, labeled = False, raw_flag = False, which_tags:list = []):        
        res = self.get_data_and_timestamps_batch(comp_name, comp_status, start_time, end_time, raw_flag)
        if res is not None:
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
    def __plot_sensor(self, sensor_name, ss_data_frame, cols, dim, subplots, label, raw_flag, unit, fft_params):
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
                # comp_name, comp_status, start_time = 0, end_time = -1, labeled = False, raw_flag = False, which_tags:list = []
                ss_data_frame = self.get_dataframe_batch(sensor_name, sensor_status, start_time, end_time, label is not None, raw_flag)

            if ss_data_frame is not None:
                
                #Tag columns check (if any)
                if label is not None and len(ss_data_frame.columns) < sensor_status['dim'] + 1:
                    log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                    label = None

                #Tag columns check (selected label exists?)
                if label is not None and not label in ss_data_frame.columns:
                    log.warning("No {} label found in selected acquisition".format(label))
                    label = None

                #Check for dim (nof plots)
                dim = sensor_status.get("dim")

                cols = ss_data_frame.columns[1:]

                if ss_data_frame is not None:
                    fft_params = None
                    if fft_plots:
                        odr = sensor_status.get('odr', 1)
                        fft_params = (fft_plots, odr)
                    self.__plot_sensor(sensor_name, ss_data_frame, cols, dim, subplots, label, raw_flag, sensor_status.get('unit'), fft_params)
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
    # Plots Functions #######################################################################################################################

    # #========================================================================================#
    ### OFFLINE CLI Interaction ##############################################################
    #========================================================================================#
    def prompt_device_id_select_CLI(self, device_list):
        selected_device = CLI.select_item("Device",device_list)
        selected_device_id = device_list.index(selected_device)
        return selected_device_id

    def prompt_sensor_select_CLI(self, sensor_list = None):
        if sensor_list is None:
            sensor_list = self.get_sensor_list()
        return CLI.select_item("Sensor",sensor_list)

    def prompt_file_select_CLI(self, dat_file_list = None):
        if dat_file_list is None:
            dat_file_list = FileManager.get_file_names_from_model()
        return CLI.select_item("Data File", dat_file_list)

    def prompt_label_select_CLI(self, label_list = None):
        if label_list is None:
            label_list = self.get_acquisition_label_classes()
        return CLI.select_item("Labels", label_list)
    
    def prompt_component_select_CLI(self, component_list = None):
        if component_list is None:
            sensor_list = self.get_sensor_list()
            component_list = sensor_list
        return CLI.select_item("PnPL_Component", component_list)

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
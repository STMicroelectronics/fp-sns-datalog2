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
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from st_hsdatalog.HSD_utils.exceptions import *
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.model.DeviceConfig import Device, DeviceInfo, Sensor, SensorDescriptor
from st_hsdatalog.HSD.model.AcquisitionInfo import AcquisitionInfo
from st_hsdatalog.HSD.utils.cli_interaction import CLIInteraction as CLI
from st_hsdatalog.HSD.utils.file_manager import FileManager
from st_hsdatalog.HSD.utils.type_conversion import TypeConversion

log = logger.get_logger(__name__)

class HSDatalog_v1:
    device_model: Device
    acq_info_model: AcquisitionInfo
    __acq_folder_path = None
    __checkTimestamps = False

    def __find_file(self, name, path):
        for root, dirs, files in os.walk(path):
            if name in files:
                return os.path.join(root, name)
        return None
    
    def __init__(self, acquisition_folder = None):
        if acquisition_folder is not None:
            device_json_file_path = self.__find_file("DeviceConfig.json", acquisition_folder)
            if device_json_file_path is None:
                raise AcquisitionFormatError
            self.__load_device(device_json_file_path)
            try:
                acquisition_json_file_path = self.__find_file("AcquisitionInfo.json", acquisition_folder)
                if acquisition_json_file_path is None:
                    raise MissingAcquisitionInfoError
                self.__load_acquisition_info(acquisition_json_file_path)
            except MissingAcquisitionInfoError:
                log.error("No AcquisitionInfo.json file in your Acquisition folder")
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
        self.__checkTimestamps = status
    ### Debug <== ###

    def __load_device(self, device_json_file_path):
        try:
            with open(device_json_file_path) as f:
                device_json_dict = json.load(f)
            device_json_str = json.dumps(device_json_dict)
            f.close()
            device_dict = json.loads(device_json_str)
            self.device_model = Device.from_dict(device_dict['device'])
        except:
            raise

    def __load_acquisition_info(self, acq_info_json_file_path):
        try:
            with open(acq_info_json_file_path) as f:
                acq_info_json_dict = json.load(f)
            acq_info_json_str = json.dumps(acq_info_json_dict)
            f.close()
            acq_info_dict = json.loads(acq_info_json_str)
            self.acq_info_model = AcquisitionInfo.from_dict(acq_info_dict)
        except:
            raise MissingAcquisitionInfoError

    def get_device(self):
        return self.device_model

    def set_device(self, new_device:Device):
        self.device_model = new_device
        log.info("Device [{}] - {} loaded correctly!".format(self.device_model.device_info.alias,self.device_model.device_info.part_number))

    def get_device_info(self):
        return self.device_model.device_info

    def set_device_info(self, new_device_info:DeviceInfo):
        self.device_model.device_info = new_device_info

    def get_sensor(self, sensor_name):
        if self.device_model is not None:
            for s in self.device_model.sensor:
                if s.name == sensor_name:
                    return s
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
        return active_sensors

    def get_sw_tag_classes(self):
        if self.device_model is not None:
            return self.device_model.tag_config.sw_tags

    def get_hw_tag_classes(self):
        if self.device_model is not None:
            return self.device_model.tag_config.hw_tags

    def get_acquisition_info(self):
        return self.acq_info_model

    def set_acquisition_info(self, new_acquisition_info:AcquisitionInfo):
        self.acq_info_model = new_acquisition_info

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
                tags = [tag for tag in tags if tag['l'] in which_tags]
                
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
                    tag['Label'] = lbl
                    tag['time_start'] = tstart
                    tag['time_end'] = tend
                    time_labels.append(tag)
            return time_labels
        else:
            log.error("Empty Acquisition Info model.")
            raise MissingAcquisitionInfoError

    def get_data_stream_tags(self, sensor_name, sub_sensor_type, start_time = 0, end_time = -1, which_tags = None):
        '''
        returns an array of dict:
        {'Label': <Label>, 'time_start': <time start: float>, 'time_end': <time end: float>,'sample_start': <sample index start: int>, 'sample_end': <sample index end: int>}
        '''
        res = self.get_data_and_timestamps(sensor_name, sub_sensor_type, start_time, end_time)
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
                else:
                    if sub_sensor_type != "MLC":
                        log.warning("({}_{}) No data samples corresponding to time tag [{}] were found in selected sample interval".format(sensor_name, sub_sensor_type, tag['Label']))
            return sensor_labels
        log.error("Data extraction error for sensor: {}_{}".format(sensor_name,sub_sensor_type))
        raise DataExtractionError(sensor_name,sub_sensor_type)

    def __process_datalog(self, sensor_name, ss_desc, ss_stat, raw_data, dataframe_size, timestamp_size, raw_flag = False):

        #####################################################################
        def extract_data_and_timestamps():
        
            """ gets data from a file .dat
                np array with one column for each axis of each active subSensor
                np array with sample times
            """

            if ss_desc.sensor_type != 'MLC' and ss_desc.sensor_type != 'STREDL':
                checkTimeStamps = self.__checkTimestamps
                frame_period = ss_stat.samples_per_ts / ss_stat.odr
            else:
                checkTimeStamps = False
                frame_period = 0

            # rndDataBuffer = raw_data rounded to an integer # of frames
            rndDataBuffer = raw_data[0:int(frame_size * num_frames)]

            data = np.zeros((data1D_per_frame * num_frames, 1), dtype = TypeConversion.check_type(ss_desc.data_type))
            
            timestamp_first = ss_stat.initial_offset
            timestamp = []

            for ii in range(num_frames):  # For each Frame:
                startFrame = ii * frame_size
                # segmentData = data in the current frame
                segmentData = rndDataBuffer[startFrame: startFrame + dataframe_size]
                # segmentTS = ts is at the end of each frame
                segmentTS = rndDataBuffer[startFrame + dataframe_size: startFrame + frame_size]

                # timestamp of current frame
                if segmentTS.size != 0:
                    timestamp.append(np.frombuffer(segmentTS, dtype = 'double')[0])
                else:
                    timestamp_first += frame_period
                    timestamp.append(timestamp_first)

                # Data of current frame
                data[ii * data1D_per_frame:(ii + 1) * data1D_per_frame, 0] = \
                    np.frombuffer(segmentData, dtype = TypeConversion.check_type(ss_desc.data_type))

                # Check Timestamp consistency
                if checkTimeStamps and (ii > 0):
                    deltaTS = timestamp[ii] - timestamp[ii - 1]
                    if abs(deltaTS) < 0.66 * frame_period or abs(deltaTS) > 1.33 * frame_period or np.isnan(
                            timestamp[ii]) or np.isnan(timestamp[ii - 1]):
                        data[ii * data1D_per_frame:(ii + 1) * data1D_per_frame, 0] = 0
                        timestamp[ii] = timestamp[ii - 1] + frame_period
                        log.warning("Sensor {} - {}: corrupted data at {}".format(sensor_name, ss_desc.sensor_type, "{} sec".format(timestamp[ii])))

            # when you have only 1 frame, framePeriod = last timestamp
            if num_frames == 1:
                timestamp = np.append(timestamp, frame_period)

            sData = np.reshape(data, (-1, ss_desc.dimensions)).astype(dtype=float)

            if not raw_flag:
                sensitivity = float(ss_stat.sensitivity)
                for kk in range(ss_desc.dimensions):
                    sData[:, int(kk)] = sData[:, int(kk)] * sensitivity

            # samples_time: numpy array of 1 clock value per each data sample
            samples_time = np.zeros((num_frames * ss_stat.samples_per_ts, 1))
            if (ss_stat.samples_per_ts >1):
                # initial_offset is relevant
                timestamp = np.append(ss_stat.initial_offset, timestamp)

                # sample times between timestamps are linearly interpolated
                for ii in range(num_frames):  # For each Frame:
                    temp = np.linspace(timestamp[ii], timestamp[ii+1], ss_stat.samples_per_ts, endpoint= False)
                    samples_time[(ii) * ss_stat.samples_per_ts:(ii+1) * ss_stat.samples_per_ts,0] = temp
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

        # data1D_per_frame = number of data samples in 1 frame
        # must be the same as samplePerTs * number of axes
        data1D_per_frame = int(dataframe_size / TypeConversion.check_type_length(ss_desc.data_type))

        if ss_stat.samples_per_ts == 0:
            ss_stat.samples_per_ts = int(data1D_per_frame / ss_desc.dimensions)

        return extract_data_and_timestamps()
    
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

    def __to_dataframe(self, data, time, ss_desc, sensor_name, sub_sensor_type, labeled = False, start_time = 0, end_time = -1):
        if data is not None and time is not None:
            cols = []
            numAxes = int(ss_desc.dimensions)
            if len(ss_desc.dimensions_label) == ss_desc.dimensions:
                if not (ss_desc.sensor_type == "MLC" or ss_desc.sensor_type == "STREDL"):
                    if numAxes == 3:
                        cc = ss_desc.dimensions_label
                        col_prefix = ss_desc.sensor_type[0] + '_'
                        col_postfix = ' [' + ss_desc.unit + ']'
                        c = [col_prefix + s + col_postfix for s in cc]
                    elif numAxes == 1:
                        c = [ss_desc.sensor_type]
                    else:
                        log.error("Wrong number of sensor axes ({})".format(numAxes))
                        raise NSensorAxesError(sensor_name)
                else:
                    if numAxes > 0:
                        cc = ss_desc.dimensions_label
                        col_prefix = ss_desc.sensor_type[0] + '_'
                        c = [col_prefix + s for s in cc]
                    else:
                        log.error("Wrong number of sensor axes ({})".format(numAxes))
                        raise NSensorAxesError(sensor_name)
                
                cols = np.append(cols, c, axis=0)
                cols = np.append(["Time"], cols, axis=0)
                
                val = np.append(time, data, axis=1)

                ss_data_frame = pd.DataFrame(data=val, columns=cols)

                if labeled:
                    tags = self.get_data_stream_tags(sensor_name, sub_sensor_type, start_time, end_time)
                    if any(bool(tag) for tag in tags):
                        for lbl in self.get_acquisition_label_classes():
                            lbl_col = np.zeros(time.shape, dtype=bool)
                            lbl_tags = [x for x in tags if x != {} and x['Label'] == lbl]
                            for lt in lbl_tags:
                                lbl_col[lt['sample_start']:lt['sample_end']] = True
                            ss_data_frame[lbl] = lbl_col
                return ss_data_frame
            else:
                log.error("Wrong number of sensor axes ({})".format(numAxes))
                raise NSensorAxesError(sensor_name)
        log.error("Error extracting data and timestamp from sensor {}_{}".format(sensor_name, sub_sensor_type))
        raise DataExtractionError(sensor_name, sub_sensor_type)

    def get_dataframe(self, sensor_name, sub_sensor_type, start_time = 0, end_time = -1, labeled = False, raw_flag = False):
        ss_desc, ss_stat = self.get_sub_sensor(sensor_name, ss_type=sub_sensor_type)
        res = self.get_data_and_timestamps(sensor_name, sub_sensor_type, start_time, end_time, raw_flag)
        if res is not None:
            data, time = res
            return self.__to_dataframe(data, time, ss_desc, sensor_name, sub_sensor_type, labeled, start_time, end_time)
        log.error("Error extracting data and timestamps from {}_{} sensor .dat file".format(sensor_name, sub_sensor_type))
        raise DataExtractionError(sensor_name, sub_sensor_type)
    
    def get_sensor_plot(self, sensor_name, sub_sensor_type, start_time = 0, end_time = -1, label=None, subplots=False, raw_flag = False):
        try:
            sub_sensor = self.get_sub_sensor(sensor_name, None, sub_sensor_type)
            ss_data_frame = None
            if sub_sensor[1].is_active:
                ss_data_frame = self.get_dataframe(sensor_name, sub_sensor_type, start_time, end_time, label is not None, raw_flag)
                ss_desc = sub_sensor[0]

            if ss_data_frame is not None:

                #Tag columns check (if any)
                if label is not None and len(ss_data_frame.columns) < ss_desc.dimensions + 1:
                    log.warning("No [{}] annotation has been found in the selected acquisition".format(label))
                    label = None

                #Tag columns check (selected label exists?)
                if label is not None and not label in ss_data_frame.columns:
                    log.warning("No {} label found in selected acquisition".format(label))
                    label = None

                if ss_data_frame is not None:

                    ### labeled and subplots
                    if label is not None and subplots:
                        cols = ss_desc.dimensions_label
                        fig, axs = plt.subplots(ss_desc.dimensions)
                        if ss_desc.dimensions == 1:
                            axs = (axs,)
                        tit = sensor_name + '-' + sub_sensor_type
                        fig.suptitle(tit)

                        if not raw_flag:
                            fig.text(0.04, 0.5, ss_desc.unit, va='center', rotation='vertical')
                        
                        for ax in axs:
                            ax.patch.set_facecolor('0.6')
                            ax.patch.set_alpha(float('0.5'))

                        for idx, p in enumerate(axs):
                            true_tag_idxs = ss_data_frame[label].loc[lambda x: x== True].index
                            tag_groups = np.split(true_tag_idxs, np.where(np.diff(true_tag_idxs) != 1)[0]+1)
                            
                            p.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, idx + 1], color=np.random.rand(3, ))
                            p.axes.set_xlim(left=-0.5)

                            for i in range(len(tag_groups)):
                                # if(len(tag_groups) == 1):
                                #     start_tag_time = ss_data_frame['Time'].iloc[0]
                                #     end_tag_time = ss_data_frame['Time'].iloc[-1]
                                # else:
                                start_tag_time = ss_data_frame.at[tag_groups[i][0],'Time']
                                end_tag_time = ss_data_frame.at[tag_groups[i][-1],'Time']
                                p.axvspan(start_tag_time, end_tag_time, facecolor='1', alpha=0.9)
                                p.axvline(x=start_tag_time, color='g', label= "Start " + label)
                                p.axvline(x=end_tag_time, color='r', label= "End " + label)
                            
                            if len(cols) > 1:
                                p.set(title=cols[idx])

                        if ss_desc.dimensions > 1:
                            for ax in axs.flat:
                                ax.set(xlabel='Time')
                            for ax in fig.get_axes():
                                ax.label_outer()
                        else:
                            axs[0].set(xlabel='Time')

                        #axs[0].legend(loc='upper left')
                        handles, labels = axs[0].get_legend_handles_labels()
                        by_label = dict(zip(labels, handles))
                        axs[0].legend(by_label.values(), by_label.keys(), loc='upper left')

                        plt.draw()

                    ### not labeled and not subplots
                    elif label is None and not subplots:
                        cols = ss_desc.dimensions_label
                        fig = plt.figure()
                        for k in range(ss_desc.dimensions):
                            plt.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, k + 1], color=np.random.rand(3, ), label=cols[k])
                        
                        if not raw_flag:
                            plt.ylabel(ss_desc.unit)

                        fig.axes[0].set_xlim(left=-0.5)

                        plt.title(sensor_name + '-' + ss_desc.sensor_type)
                        plt.xlabel('Time (s)')
                        if ss_desc.sensor_type in ('ACC', 'GYRO', 'MAG', 'MLC', 'STREDL'):
                            plt.legend(loc='upper left')
                        plt.draw()

                    ### labeled and not subplots
                    elif label is not None and not subplots:
                        cols = ss_desc.dimensions_label
                        fig = plt.figure()
                        for k in range(ss_desc.dimensions):
                            plt.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, k + 1], color=np.random.rand(3, ))
                        
                        if not raw_flag:
                            plt.ylabel(ss_desc.unit)

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

                        plt.title(sensor_name + '-' + ss_desc.sensor_type)
                        plt.xlabel('Time (s)')
                        
                        handles, labels = plt.gca().get_legend_handles_labels()
                        by_label = dict(zip(labels, handles))
                        plt.legend(by_label.values(), by_label.keys(), loc='upper left')
                        
                        plt.draw()

                    ### not labeled and subplots
                    elif label is None and subplots:
                        cols = ss_desc.dimensions_label 
                        fig, axs = plt.subplots(ss_desc.dimensions)
                        if ss_desc.dimensions == 1:
                            axs = (axs,)
                        tit = sensor_name + '-' + ss_desc.sensor_type
                        fig.suptitle(tit)
                        for idx, p in enumerate(axs):
                            p.plot(ss_data_frame[['Time']], ss_data_frame.iloc[:, idx + 1], color=np.random.rand(3, ), label=cols[idx])
                            p.set(title=cols[idx])
                            p.axes.set_xlim(left=-0.5)

                        if not raw_flag:
                            fig.text(0.04, 0.5, ss_desc.unit, va='center', rotation='vertical')

                        if ss_desc.dimensions > 1:
                            for ax in axs.flat:
                                ax.set(xlabel = 'Time (s)')
                                # if not raw_flag:
                                #     ax.set(ylabel = ss_desc.unit)
                            for ax in fig.get_axes():
                                ax.label_outer()
                        else:
                            axs[0].set(xlabel = 'Time (s)')
                            # if not raw_flag:
                            #     axs[0].set(ylabel = ss_desc.unit)
                        plt.draw()
            else:
                log.error("Error extracting subsensor DataFrame {}_{}".format(sensor_name, sub_sensor_type))
                raise DataExtractionError(sensor_name, sub_sensor_type)
            return True
        except MemoryError:
            log.error("Memory Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(sensor_name,sub_sensor_type)))
            raise
        except  ValueError:
            log.error("Value Error occoured! You should batch process your {} file".format(FileManager.encode_file_name(sensor_name,sub_sensor_type)))
            raise

    def get_dat_file_list(self):
        return FileManager.get_dat_files_from_folder(self.__acq_folder_path)
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

    def present_device_info(self, device_info= None):
        if device_info is None:
            device_info = self.get_device_info()
        CLI.present_item(device_info)

    def present_sensor_list(self, sensor_list = None, type_filter = "", only_active = True, ):
        if sensor_list is None:
            sensor_list = self.get_sensor_list(type_filter, only_active)
        CLI.present_items(sensor_list)

    def present_sensor(self, sensor:Sensor):
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
# *****************************************************************************
#  * @file    HSDatalog.py
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
import numpy as np
from st_hsdatalog.HSD.HSDatalog_v1 import HSDatalog_v1
from st_hsdatalog.HSD.HSDatalog_v2 import HSDatalog_v2
from st_hsdatalog.HSD.model.DeviceConfig import Device
from st_hsdatalog.HSD_utils.converters import HSDatalogConverter
from st_hsdatalog.HSD_utils.exceptions import *
import st_hsdatalog.HSD_utils.logger as logger
from st_pnpl.DTDL.dtdl_utils import ComponentTypeEnum

log = logger.get_logger(__name__)

class HSDatalog:    
    def create_hsd(self, acquisition_folder = None, device_config = None):
        self.acquisition_folder = acquisition_folder
        self.is_datalog2 = False
        if device_config is not None:
            if isinstance(device_config, Device):
                hsd = HSDatalog_v1()
            elif "schema_version" in device_config and device_config["schema_version"].split('.')[0] == "2":
                hsd = HSDatalog_v2()
            return hsd
        try:
            hsd = HSDatalog_v1(self.acquisition_folder)
        except AcquisitionFormatError:
            self.is_datalog2 = True

        if self.is_datalog2:
            hsd = HSDatalog_v2(self.acquisition_folder)

        return hsd
    
    @staticmethod
    def present_sensor_list(hsd, sensor_list):
        hsd.present_sensor_list(sensor_list)
        
    @staticmethod
    def present_device_info(hsd, device_info):
        hsd.present_device_info(device_info)
    
    @staticmethod
    def present_sw_tag_classes(hsd, sw_tag_classes):
        hsd.present_sw_tag_classes(sw_tag_classes)
        
    @staticmethod
    def present_hw_tag_classes(hsd, hw_tag_classes):
        hsd.present_hw_tag_classes(hw_tag_classes)
        
    @staticmethod
    def get_dat_file_list(hsd):
        return hsd.get_dat_file_list()

    @staticmethod
    def present_sensor(hsd, sensor):
        hsd.present_sensor(sensor)
    
    @staticmethod
    def filter_sensor_list_by_name(hsd, sensor_list, sensor_name):
        ssensor = []
        if isinstance(hsd, HSDatalog_v2):
            for s in sensor_list:
                if sensor_name.lower() in list(s.keys())[0]:
                    ssensor.append(s)
        else:
            ssensor = sensor_list[0]
        return ssensor
    
    @staticmethod
    def set_device(hsd, device):
        hsd.set_device(device)
        
    @staticmethod
    def get_device_info(hsd):
        return hsd.get_device_info()
    
    @staticmethod
    def get_ssensor_list(hsd, sensor):
        if isinstance(hsd, HSDatalog_v2):
            ssensor_names_list = []
            if type(sensor) != list:
                ssensor_names_list = list(sensor)
            else:
                for s in sensor:
                    ssensor_names_list.append(list(s.keys())[0])
            return ssensor_names_list
        else:
            ssensor_desc_list = sensor.sensor_descriptor.sub_sensor_descriptor
            return ssensor_desc_list
    
    @staticmethod
    def ask_for_component(hsd, only_active):
        s_list = hsd.get_sensor_list(only_active = only_active)
        if isinstance(hsd, HSDatalog_v2):
            a_list = hsd.get_algorithm_list(only_active = only_active)
            ac_list = hsd.get_actuator_list(only_active = only_active)
            component = hsd.prompt_component_select_CLI(s_list + a_list + ac_list)
        else:
            component = hsd.prompt_sensor_select_CLI(s_list)
        return component

    @staticmethod
    def get_sensor_list(hsd, type_filter = "", only_active = False):
        return hsd.get_sensor_list(type_filter = type_filter, only_active = only_active)
    
    @staticmethod
    def get_sensor(hsd, sensor_name):
        if isinstance(hsd, HSDatalog_v2):
            sensor = hsd.get_sensor(sensor_name)
        else:
            sensor = hsd.get_sensor(sensor_name)
        return sensor
    
    @staticmethod
    def get_sensor_sensitivity(hsd, sensor):
        if isinstance(hsd, HSDatalog_v2):
            s_key = list(sensor.keys())[0]
            sensor_sensitivity = sensor[s_key]["sensitivity"]
        else:
            s_descriptor_list = sensor.sensor_descriptor.sub_sensor_descriptor
            s_status_list = sensor.sensor_status.sub_sensor_status
            sensor_sensitivity = []
            for i, s in enumerate(s_descriptor_list):
                sensor_sensitivity.append(s_status_list[i].sensitivity)
            if len(sensor_sensitivity) == 1:
                return sensor_sensitivity[0]
            elif len(sensor_sensitivity) == 0:
                return None
            else:
                return sensor_sensitivity
    
    @staticmethod
    def get_sensor_name(hsd, sensor):
        if isinstance(hsd, HSDatalog_v2):
            return list(sensor.keys())[0]
        else:
            return sensor.name
            
    @staticmethod
    def get_sensor_odr(hsd, sensor):
        if isinstance(hsd, HSDatalog_v2):
            s_name = HSDatalog.get_sensor_name(hsd, sensor)
            if "odr" in sensor[s_name]:
                return sensor[s_name]["odr"]
            else:
                return None
        else:
            s_descriptor_list = sensor.sensor_descriptor.sub_sensor_descriptor
            s_status_list = sensor.sensor_status.sub_sensor_status
            odr = []
            for i, s in enumerate(s_descriptor_list):
                odr.append(s_status_list[i].odr)
            if len(odr) == 1:
                return odr[0]
            elif len(odr) == 0:
                return None
            else:
                return odr
                
    @staticmethod
    def get_sensor_fs(hsd, sensor):
        if isinstance(hsd, HSDatalog_v2):
            s_name = HSDatalog.get_sensor_name(hsd, sensor)
            if "fs" in sensor[s_name]:
                return sensor[s_name]["fs"]
            else:
                return None
        else:
            s_descriptor_list = sensor.sensor_descriptor.sub_sensor_descriptor
            s_status_list = sensor.sensor_status.sub_sensor_status
            fs = []
            for i, s in enumerate(s_descriptor_list):
                fs.append(s_status_list[i].fs)
            if len(fs) == 1:
                return fs[0]
            elif len(fs) == 0:
                return None
            else:
                return fs
                
    @staticmethod
    def get_sensor_spts(hsd, sensor):
        if isinstance(hsd, HSDatalog_v2):
            s_name = HSDatalog.get_sensor_name(hsd, sensor)
            spts = sensor[s_name].get("samples_per_ts")
            if isinstance(spts, int):
                return spts
            else:
                return spts.get("val")
        else:
            s_descriptor_list = sensor.sensor_descriptor.sub_sensor_descriptor
            s_status_list = sensor.sensor_status.sub_sensor_status
            spts = []
            for i, s in enumerate(s_descriptor_list):
                spts.append(s_status_list[i].samples_per_ts)
            if len(spts) == 1:
                return spts[0]
            elif len(spts) == 0:
                return None
            else:
                return spts
    
    @staticmethod
    def get_all_components(hsd, only_active):
        s_list = hsd.get_sensor_list(only_active=only_active)
        a_list = []
        ac_list = []
        if isinstance(hsd, HSDatalog_v2):
            a_list = hsd.get_algorithm_list(only_active=True)
            ac_list = hsd.get_actuator_list(only_active=True)
            return s_list + a_list + ac_list
        else:
            return s_list
    
    @staticmethod
    def get_component(hsd, component_name):
        if isinstance(hsd, HSDatalog_v2):
            return hsd.get_component(component_name)
        else:
            return hsd.get_sensor(component_name)
    
    @staticmethod
    def get_acquisition_label_classes(hsd):
        return hsd.get_acquisition_label_classes()
    
    @staticmethod
    def __check_data(hsd, comp_name, comp_type, odr, start_time, end_time):
        chunk_size = 10000000 #feel fre to change it
        chunk_time_size = chunk_size/odr # seconds
        isFirstChunk = True
        isLastChunk = False
        chunkWithErrors = 0
        lastData = 0
        time_offset = 0 if start_time is None else start_time
        while (isLastChunk == 0):

            if end_time != -1 and time_offset + chunk_time_size > end_time:
                chunk_time_size = end_time - time_offset
            
            data = hsd.get_data_and_timestamps(comp_name, comp_type, start_time = time_offset, end_time = time_offset + chunk_time_size, raw_flag = True)[0]
            if data is not None and len(data) != 0:
                if len(data) <= chunk_size:
                    isLastChunk = 1
                else:
                    time_offset = time_offset + chunk_time_size

                data = data.astype(np.int16).reshape(-1)

                # check that first data of the current chunk is last data of previous chunk + 1
                if (data[0] != lastData+1) and (isFirstChunk == False):
                    chunkWithErrors = chunkWithErrors + 1

                lastData = data[len(data)-1]
                isFirstChunk = False

                x = data[0] + np.array([i for i in range(len(data))]).astype(np.int16)

                if not (data == x).all():
                    chunkWithErrors = chunkWithErrors + 1
            else:
                isLastChunk = 1
                chunkWithErrors = chunkWithErrors + 1
                    
        log.info("--> Data check completed for Component {}".format(comp_name))
        if (chunkWithErrors == 0):
            log.info('{:>20} {:<6} : OK'.format(comp_name, "" if comp_type is None else comp_type))
        else:
            log.error('{:>20} {:<6} : ### ERRORS FOUND ###'.format(comp_name, "" if comp_type is None else comp_type))
    
    @staticmethod
    def check_dummy_data(hsd, component, start_time, end_time):
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]
            odr = None
            if "odr" in component[c_name]:
                odr = component[c_name]["odr"]
            else:
                if odr is None:
                    odr = 1
            HSDatalog.__check_data(hsd, c_name, None, odr, start_time, end_time)
        else:
            s_name = component.name
            for ss_id, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                ss_type = ss_desc.sensor_type
                odr = component.sensor_status.sub_sensor_status[ss_id].odr
                HSDatalog.__check_data(hsd, s_name, ss_type, odr, start_time, end_time)
    
    @staticmethod
    def __convert_to_xsv(hsd, comp_name, comp_type, odr, start_time, end_time, labeled, raw_data, output_folder, file_format):
        chunk_size = 10000000 #feel fre to change it (samples)
        chunk_time_size = chunk_size/odr # seconds
        isLastChunk = False
        # time_offset = 0 if start_time is None else start_time
        time_offset = start_time or 0
        
        log.info("--> Conversion started...")
        # while isLastChunk == 0:
        while not isLastChunk:
            if end_time != -1 and time_offset + chunk_time_size > end_time:
                #read exactly the missing samples up to end_time
                chunk_time_size = end_time - time_offset

            sensor_file_name = HSDatalog.get_ssensor_file_name(hsd, comp_name, comp_type, output_folder)
            
            df = hsd.get_dataframe(comp_name, comp_type, time_offset, time_offset+chunk_time_size, labeled, raw_data)
            if df is not None:
                if file_format == 'CSV':
                    HSDatalogConverter.to_csv(df, sensor_file_name, mode = 'w' if time_offset == 0 else 'a')
                else:
                    HSDatalogConverter.to_tsv(df, sensor_file_name, mode = 'w' if time_offset == 0 else 'a')

                if len(df) == 0 or len(df) < chunk_size:
                    isLastChunk = True
                    log.info("--> Conversion completed")
                else:
                    log.info("--> Chunk Conversion completed")
                    time_offset += chunk_time_size
            else:
                isLastChunk = True
    
    @staticmethod
    def get_dataframe(hsd, component, start_time = 0, end_time = -1, labeled = False, raw_data = False) -> list: 
        df = []
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]           
            df.append(hsd.get_dataframe(c_name, None, start_time, end_time, labeled, raw_data))
        else:
            s_name = component.name
            s_status = component.sensor_status
            for i, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                is_active = s_status.sub_sensor_status[i].is_active
                if is_active == True:
                    ss_type = ss_desc.sensor_type
                    df.append(hsd.get_dataframe(s_name, ss_type, start_time, end_time, labeled, raw_data))
        return df

    @staticmethod
    def get_data_stream_tags(hsd, component):
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]           
            dst = hsd.get_data_stream_tags(c_name, None)
        else:
            s_name = component.name
            s_status = component.sensor_status
            for i, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                is_active = s_status.sub_sensor_status[i].is_active
                if is_active == True:
                    ss_type = ss_desc.sensor_type
                    dst = hsd.get_data_stream_tags(s_name, ss_type)
        return dst
    
    @staticmethod
    def get_data_and_timestamps(hsd, component, start_time = 0, end_time = -1, labeled = False, raw_data = False):
        d_and_t = []
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]
            d_and_t.append(hsd.get_data_and_timestamps(c_name, None, start_time, end_time, raw_data))
        else:
            s_name = component.name
            s_status = component.sensor_status
            for i, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                is_active = s_status.sub_sensor_status[i].is_active
                if is_active == True:
                    ss_type = ss_desc.sensor_type
                    d_and_t.append(hsd.get_data_and_timestamps(s_name, ss_type, start_time, end_time, raw_data))
        return d_and_t
    
    @staticmethod
    def convert_dat_to_xsv(hsd, component, start_time, end_time, labeled, raw_data, output_folder, file_format):
        """_summary_

        Args:
            hsd (_type_): _description_
            component (_type_): _description_
            start_time (_type_): _description_
            end_time (_type_): -1 TODO
            labeled (_type_): _description_
            raw_data (_type_): _description_
            output_folder (_type_): _description_
            file_format (_type_): _description_
        """
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]
            odr = None
            if "odr" in component[c_name]:
                odr = component[c_name]["odr"]
            else:
                if odr is None:
                    odr = 1
            HSDatalog.__convert_to_xsv(hsd, c_name, None, odr, start_time, end_time, labeled, raw_data, output_folder, file_format)
        else:
            s_name = component.name
            for ss_id, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                ss_type = ss_desc.sensor_type
                odr = component.sensor_status.sub_sensor_status[ss_id].odr
                HSDatalog.__convert_to_xsv(hsd, s_name, ss_type, odr, start_time, end_time, labeled, raw_data, output_folder, file_format)
                
    @staticmethod
    def __convert_to_txt_by_tags(hsd, comp_name, comp_type, is_active, start_time, end_time, ignore_datalog_tags, acq_folder, output_folder, out_format, hsd_dfs):
        data_tags = None
        if comp_type != 'MLC' and comp_type != 'STREDL' and is_active:
            df = hsd.get_dataframe(comp_name, comp_type, start_time, end_time, labeled = not ignore_datalog_tags)
            if not ignore_datalog_tags:
                data_tags = hsd.get_data_stream_tags(comp_name, comp_type, start_time, end_time)
                if ignore_datalog_tags == False and len(data_tags) == 0:
                    log.error("--> Error in format conversion. Data corrupted for {}".format(comp_name))
                    return
                # ignore_datalog_tags = False
            if not (df is None or df.empty):
                hsd_dfs.append(df)
        HSDatalogConverter.to_txt_by_tags(output_folder, comp_name + "_" + os.path.basename(acq_folder), hsd_dfs, data_tags, out_format, mode = 'w')
        log.info("--> {} ST format conversion completed successfully".format(comp_name))

    @staticmethod
    def convert_dat_to_txt_by_tags(hsd, component, start_time, end_time, ignore_datalog_tags, acq_folder, output_folder, out_format):
        hsd_dfs = []
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]
            enable = None
            if "enable" in component[c_name]:
                enable = component[c_name]["enable"] #TODO check this
                HSDatalog.__convert_to_txt_by_tags(hsd, c_name, None, enable, start_time, end_time, ignore_datalog_tags, acq_folder, output_folder, out_format, hsd_dfs)
            else:
                if enable is None:
                    log.exception("Missing \"enable\" Properties in your device status")
                raise    
        else:
            s_name = component.name
            for ss_id, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                ss_type = ss_desc.sensor_type
                is_active = component.sensor_status.sub_sensor_status[ss_id].is_active
                HSDatalog.__convert_to_txt_by_tags(hsd, s_name, ss_type, is_active, start_time, end_time, ignore_datalog_tags, acq_folder, output_folder, out_format, hsd_dfs)
    
    @staticmethod
    def __convert_to_nanoedge_format(hsd, comp_name, comp_type, odr, signal_length, signal_increment, start_time, end_time, raw_data, output_folder):
        chunk_size = 10000000 #feel fre to change it
        chunk_time_size = chunk_size/odr # seconds
        
        if chunk_size < signal_length:
            chunk_size = signal_length
        isLastChunk = False
        time_offset = 0 if start_time is None else start_time
        
        while isLastChunk == 0:
            if end_time != -1 and time_offset + chunk_time_size > end_time:
                chunk_time_size = end_time - time_offset

            if chunk_size == 0:
                isLastChunk = 1
                log.info("--> {} Nanoedge conversion completed successfully".format(comp_name))
                break
            
            df = hsd.get_dataframe(comp_name, comp_type, time_offset, time_offset+chunk_time_size, False, raw_data)
            if df is not None:
                try:
                    if comp_type is None:
                        HSDatalogConverter.to_nanoedge_format(output_folder, comp_name, df, signal_length, signal_increment)
                    else:
                        sensor_cplt_name = "{}_{}".format(comp_name, comp_type)
                        HSDatalogConverter.to_nanoedge_format(output_folder, sensor_cplt_name, df, signal_length, signal_increment)
                        comp_name = sensor_cplt_name
                except NanoEdgeConversionError as err:
                    log.exception(err)
                    quit()
                if len(df) == 0 or len(df) < chunk_size:
                    isLastChunk = 1
                    log.info("--> {} Nanoedge conversion completed successfully".format(comp_name))
                else:
                    time_offset = time_offset + chunk_time_size
                    log.info("--> {} chunk conversion completed successfully".format(comp_name))
            else:
                isLastChunk = 1
                log.info("--> {} Nanoedge conversion completed successfully".format(comp_name))
    
    @staticmethod
    def convert_dat_to_nanoedge(hsd, component, signal_length, signal_increment, start_time, end_time, raw_data, output_folder):
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]
            if "odr" in component[c_name]:
                odr = component[c_name]["odr"]
            else:
                if odr is None:
                    odr = 1
            HSDatalog.__convert_to_nanoedge_format(hsd, c_name, None, odr, signal_length, signal_increment, start_time, end_time, raw_data, output_folder)
        else:
            s_name = component.name
            for ss_id, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                ss_type = ss_desc.sensor_type
                odr = component.sensor_status.sub_sensor_status[ss_id].odr
                HSDatalog.__convert_to_nanoedge_format(hsd, s_name, ss_type, signal_length, signal_increment, odr, start_time, end_time, raw_data, output_folder)
    
    @staticmethod
    def __convert_to_unico_format(hsd, sensor_name, sensor_type, is_active, start_time, end_time, use_datalog_tags, output_folder, out_format, hsd_dfs):
        data_tags = None
        if sensor_type != 'MLC' and is_active:
            df = hsd.get_dataframe(sensor_name, sensor_type, start_time, end_time, labeled = use_datalog_tags)
            if use_datalog_tags:
                data_tags = hsd.get_data_stream_tags(sensor_name, sensor_type, start_time, end_time)
                if use_datalog_tags == True and len(data_tags) == 0:
                    log.error("--> No tags detected in your acquisition. Try to relaunch the script without -t flag")
                    return
                use_datalog_tags = False
            if not (df is None or df.empty):
                hsd_dfs.append(df)
        HSDatalogConverter.to_unico(output_folder, sensor_name.split('_')[0], hsd_dfs, data_tags, out_format, mode = 'w')
        log.info("--> {} ST UNICO format conversion completed successfully".format(sensor_name))
        
    @staticmethod
    def convert_dat_to_unico(hsd, sensor, start_time, end_time, use_datalog_tags, output_folder, out_format):
        hsd_dfs = []
        if isinstance(hsd, HSDatalog_v2):
            for s in sensor:
                c_name = list(s.keys())[0]
                if not "_mlc" in c_name:
                    is_active = s[c_name]["enable"]
                    HSDatalog.__convert_to_unico_format(hsd, c_name, None, is_active, start_time, end_time, use_datalog_tags, output_folder, out_format, hsd_dfs)
        else:
            s_name = sensor.name
            for ss_id, ss_desc in enumerate(sensor.sensor_descriptor.sub_sensor_descriptor):
                ss_type = ss_desc.sensor_type
                if ss_type != "MLC":
                    is_active = sensor.sensor_status.sub_sensor_status[ss_id].is_active
                    HSDatalog.__convert_to_unico_format(hsd, s_name, ss_type, is_active, start_time, end_time, use_datalog_tags, output_folder, out_format, hsd_dfs)
    
    @staticmethod
    def __convert_to_wav(hsd, comp_name, comp_type, odr, n_channels, start_time, end_time, output_folder):
        try:
            log.info("Sensor: {}_{} (ODR: {})".format(comp_name, "" if comp_type is None else comp_type, odr))
            
            wav_file_name = HSDatalog.get_wav_file_name(hsd, comp_name, comp_type, output_folder)
            if odr > 0:
                wav_file = HSDatalogConverter.wav_create(wav_file_name, odr, n_channels)
            else:
                log.exception("Bad framerate selected for this sensor")
                raise
            
            chunk_size = 10000000 #feel fre to change it
            chunk_time_size = chunk_size/odr # seconds
            isLastChunk = False
            time_offset = 0 if start_time is None else start_time
            while isLastChunk == 0:
                if end_time != -1 and time_offset + chunk_time_size > end_time:
                    chunk_time_size = end_time - time_offset

                res = hsd.get_data_and_timestamps(comp_name, comp_type, start_time = time_offset, end_time = time_offset + chunk_time_size, raw_flag = True)
                if res is None:
                    HSDatalogConverter.wav_close(wav_file)   
                    log.exception("No data from selected sensor")
                    break
                data = res[0]
                # time = res[1]
                pcm_data = data.astype(np.int16)
                if len(data) == 0 or len(data) < chunk_size:
                    isLastChunk = 1
                else:
                    time_offset = time_offset + chunk_time_size
                    log.info("--> {} chunk conversion completed successfully".format(comp_name))
                HSDatalogConverter.wav_append(wav_file,pcm_data)
            HSDatalogConverter.wav_close(wav_file)
        except Exception as err:
            log.exception(err)
    
    @staticmethod
    def convert_dat_to_wav(hsd, component, start_time, end_time, output_folder):
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]
            n_channels = None
            odr = None
            if "dim" in component[c_name]:
                n_channels = component[c_name]["dim"]
            if n_channels is not None and "odr" in component[c_name]:
                odr = component[c_name]["odr"]
                HSDatalog.__convert_to_wav(hsd, c_name, None, odr, n_channels, start_time, end_time, output_folder)
            else:
                if odr is None:
                    log.exception("Missing \"odr\" Properties in your device status")
                if n_channels is None: 
                    log.exception("Missing \"dim\" Property in your device status")
                raise
        else:
            s_name = component.name
            for ss_id, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                ss_type = ss_desc.sensor_type
                odr = component.sensor_status.sub_sensor_status[ss_id].odr
                n_channels = ss_desc.dimensions
                HSDatalog.__convert_to_wav(hsd, s_name, ss_type, odr, n_channels, start_time, end_time, output_folder)
    
    @staticmethod
    def plot(hsd, component, start_time = 0, end_time = -1, label = None, subplots = False, raw_data = False):
        if isinstance(hsd, HSDatalog_v2):
            c_name = list(component.keys())[0]
            c_type = None
            if "c_type" in component[c_name]:
                c_type = component[c_name]["c_type"]
                if c_type == ComponentTypeEnum.SENSOR.value or c_type == ComponentTypeEnum.ACTUATOR.value:
                    hsd.get_sensor_plot(c_name, c_type, start_time, end_time, label = label, subplots = subplots, raw_flag = raw_data)
                elif c_type == ComponentTypeEnum.ALGORITHM.value:
                    hsd.get_algorithm_plot(c_name, start_time, end_time, label = label, subplots = subplots, raw_flag = raw_data)
            else:
                if c_type is None:
                    log.exception("Missing \"c_type\" value in your device status")
                raise
        else:
            s_name = component.name
            s_status = component.sensor_status
            for i, ss_desc in enumerate(component.sensor_descriptor.sub_sensor_descriptor):
                is_active = s_status.sub_sensor_status[i].is_active
                if is_active == True:
                    ss_type = ss_desc.sensor_type
                    hsd.get_sensor_plot(s_name, ss_type, start_time, end_time, label = label, subplots = subplots, raw_flag = raw_data)
            
    @staticmethod
    def get_ssensor_file_name(hsd, ssensor_name, ssensor_type, output_folder):
        if isinstance(hsd, HSDatalog_v2):
            return os.path.join(output_folder, "{}".format(ssensor_name))
        else:
            return os.path.join(output_folder, "{}_{}".format(ssensor_name, ssensor_type))
    
    @staticmethod
    def get_wav_file_name(hsd, ssensor_name, ssensor_type, output_folder):
        if isinstance(hsd, HSDatalog_v2):
            return os.path.join(output_folder, "{}.wav".format(ssensor_name))
        else:
            return os.path.join(output_folder, "{}_{}.wav".format(ssensor_name, ssensor_type))
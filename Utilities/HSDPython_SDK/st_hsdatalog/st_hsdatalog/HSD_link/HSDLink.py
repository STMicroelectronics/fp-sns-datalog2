# *****************************************************************************
#  * @file    HSDLink.py
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
import json
from datetime import datetime
from threading import Thread, Event

from st_hsdatalog.HSD_link.HSDLink_v2 import HSDLink_v2
from st_hsdatalog.HSD_link.HSDLink_v1 import HSDLink_v1
from st_hsdatalog.HSD_utils.exceptions import CommunicationEngineOpenError
import st_hsdatalog.HSD_utils.logger as logger
from st_pnpl.PnPLCmd import PnPLCMDManager

log = logger.get_logger(__name__)

class SensorAcquisitionThread(Thread):
    def __init__(self, event, hsd_link, sensor_data_file, d_id, s_id, ss_id = None, print_data_cnt = False):
        Thread.__init__(self)
        self.stopped = event
        self.hsd_link = hsd_link
        self.sensor_data_file = sensor_data_file
        self.d_id = d_id
        self.s_id = s_id
        self.ss_id = ss_id
        self.print_data_cnt = print_data_cnt
        if isinstance(hsd_link, HSDLink_v2):
            self.hsd_link.sensor_data_counts[self.s_id] = 0
        else:
            self.hsd_link.sensor_data_counts[(self.s_id, self.ss_id)] = 0
            
    def run(self):
        if self.print_data_cnt == True:
            self.run_print()
        else:
            self.run_no_print()

    def run_print(self):                      
        while not self.stopped.wait(0.5):
            res = self.hsd_link.get_sensor_data(self.d_id, self.s_id, self.ss_id)
            if res is not None:
                size, sensor_data = res
                ## data size increment
                if isinstance(self.hsd_link, HSDLink_v2):
                    self.hsd_link.sensor_data_counts[self.s_id] += size
                    print("Sensor [{}] - data_received: {}\n".format(self.s_id,self.hsd_link.sensor_data_counts[self.s_id]))
                else:
                    self.hsd_link.sensor_data_counts[(self.s_id, self.ss_id)] += size
                    print("Sensor [{},{}] - data_received: {}\n".format(self.s_id, self.ss_id, self.hsd_link.sensor_data_counts[(self.s_id, self.ss_id)]))
                
                # NOTE here you can queue sensor data (to process or display them)
                # NOTE: e.g. -> self.hsd_info.queue_sensor_data([sensor_data])

                ## file saving!
                res = self.sensor_data_file.write(sensor_data)

    def run_no_print(self):                      
        while not self.stopped.wait(0.5):
            res = self.hsd_link.get_sensor_data(self.d_id, self.s_id, self.ss_id)
            if res is not None:
                size, sensor_data = res
                ## data size increment
                if isinstance(self.hsd_link, HSDLink_v2):
                    self.hsd_link.sensor_data_counts[self.s_id] += size
                else:
                    self.hsd_link.sensor_data_counts[(self.s_id, self.ss_id)] += size
                # NOTE here you can queue sensor data (to process or display them)
                # NOTE: e.g. -> self.hsd_info.queue_sensor_data([sensor_data])

                ## file saving!
                res = self.sensor_data_file.write(sensor_data)

class HSDLink:
    def create_hsd_link(self, dev_com_type: str = 'stwin_hsd', acquisition_folder = None, plug_callback = None, unplug_callback = None):       
        self.dev_com_type = dev_com_type
        self.acquisition_folder = acquisition_folder
        self.is_datalog2 = False
        
        try:
            hsd_link = HSDLink_v1(self.dev_com_type, self.acquisition_folder)
            if hsd_link.nof_connected_devices == 0:
                log.warning("No HSDatalog_v1 devices connected!")
                self.is_datalog2 = True
            else:
                print("{} - HSDatalogApp.{} - INFO - Commmunication Opened correctly".format(logger.get_datetime(), __name__))
        except CommunicationEngineOpenError:
            log.error("Error opening communication using libhs_datalog_v1...")
            self.is_datalog2 = True

        time.sleep(2)

        if self.is_datalog2:
            print("{} - HSDatalogApp.{} - INFO - Trying to open communication using libhs_datalog_v2...".format(logger.get_datetime(), __name__))
            if self.dev_com_type == "stwin_hsd":
                self.dev_com_type = "pnpl"
            try:
                hsd_link = HSDLink_v2(self.dev_com_type, self.acquisition_folder, plug_callback, unplug_callback)
                if hsd_link.nof_connected_devices == 0:
                    log.warning("No HSDatalog_v2 devices connected!")
                    self.is_datalog2 = False
                    return None
                else:
                    print("{} - HSDatalogApp.{} - INFO - Commmunication Opened correctly".format(logger.get_datetime(), __name__))
            except CommunicationEngineOpenError:
                log.error("Error opening communication using libhs_datalog_v2...")
                self.is_datalog2 = False
                return None

        return hsd_link
    
    @staticmethod
    def get_versiontuple(v):
        return tuple(map(int, (v.split("."))))
    
    @staticmethod
    def is_v2(hsd_link):
        return isinstance(hsd_link, HSDLink_v2)
    
    @staticmethod
    def get_device_presentation_string(hsd_link, device_id):
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_device_presentation_string(device_id)
        else:
            return None
        
    @staticmethod
    def get_device_identity(hsd_link, device_id):
        """Alternative to get_device_presentation_string [only for HSD2 v>=1.2.0]
        """
        if HSDLink.is_v2(hsd_link):
            return hsd_link.send_command(device_id, PnPLCMDManager.create_get_identity_string_cmd())
            # fw_info = hsd_link.get_firmware_info(device_id)
            # if(HSDLink.__versiontuple(fw_info["firmware_info"]["fw_version"]) >= HSDLink.__versiontuple("1.2.0")):
            #     message = PnPLCMDManager.create_get_identity_string_cmd().encode()
            #     return hsd_link.send_command(device_id, json.dumps(message))
        return None
    
    @staticmethod
    def get_version(hsd_link):
        return hsd_link.get_version()
    
    @staticmethod
    def get_devices(hsd_link):
        if hsd_link is not None:
            return hsd_link.get_devices()
        else:
            return []
    
    @staticmethod
    def get_device(hsd_link, d_id):
        return hsd_link.get_device(d_id)
    
    @staticmethod
    def get_device_alias(hsd_link, d_id):
        return hsd_link.get_device_alias(d_id)
    
    @staticmethod
    def get_device_info(hsd_link, d_id):
        return hsd_link.get_device_info(d_id)
    
    @staticmethod
    def get_acquisition_info(hsd_link, d_id):
        return hsd_link.get_acquisition_info(d_id)
    
    @staticmethod
    def get_acquisition_folder(hsd_link):
        return hsd_link.get_acquisition_folder()

    @staticmethod
    def update_base_acquisition_folder(hsd_link, base_acquisition_path):
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.update_base_acquisition_folder(base_acquisition_path)
        else:
            return None
        
    @staticmethod
    def set_device_template(hsd_link, dev_template_json):
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_device_template(dev_template_json)
        else:
            return None
    
    @staticmethod
    def get_firmware_info(hsd_link, device_id):
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_firmware_info(device_id)
        else:
            return None
    
    @staticmethod
    def set_acquisition_info(hsd_link, device_id, acq_name, acq_desc):
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_acquisition_info(device_id, acq_name, acq_desc)
        else:
            hsd_link.set_acquisition_info(device_id, acq_name, acq_desc)

    @staticmethod            
    def get_sensor_list(hsd_link, device_id, type_filter="", only_active = False):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensors(device_id, type_filter=type_filter, only_active=only_active)        
        else:
            return hsd_link.get_sensors(device_id, type_filter=type_filter, only_active=only_active)
    
    @staticmethod
    def get_sensor_enabled(hsd_link, d_id, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_isActive(d_id, s_id, ss_id)
        else:
            return hsd_link.get_sensor_enable(d_id, sensor_name)
    
    @staticmethod
    def set_sensor_enable(hsd_link, d_id, new_status, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            if s_id is not None: 
                if ss_id is None:
                    return hsd_link.set_sensor_active(d_id, s_id, new_status)
                else:
                    return hsd_link.set_sub_sensor_active(d_id, s_id, ss_id, new_status)
        else:
            return hsd_link.set_sensor_enable(d_id, new_status, sensor_name)      

    @staticmethod
    def get_sensor_odr(hsd_link, d_id, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_odr(d_id, s_id, ss_id)
        else:
            return hsd_link.get_sensor_odr(d_id, sensor_name)

    @staticmethod 
    def set_sensor_odr(hsd_link, d_id, new_odr, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.set_sub_sensor_odr(d_id, s_id, ss_id, new_odr)
        else:
            return hsd_link.set_sensor_odr(d_id, new_odr, sensor_name)
    
    @staticmethod
    def get_sensor_fs(hsd_link, d_id, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_fs(d_id, s_id, ss_id)
        else:
            if "_mic" in sensor_name:
                return hsd_link.get_sensor_aop(d_id, sensor_name)
            else:
                return hsd_link.get_sensor_fs(d_id, sensor_name)
    
    @staticmethod
    def set_sensor_fs(hsd_link, d_id, new_fs, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.set_sub_sensor_fs(d_id, s_id, ss_id, new_fs)
        else:
            if "_mic" in sensor_name:
                return hsd_link.set_sensor_aop(d_id, new_fs, sensor_name)
            else:
                return hsd_link.set_sensor_fs(d_id, new_fs, sensor_name)
            
    @staticmethod
    def get_sensor_spts(hsd_link, d_id, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_sample_per_ts(d_id, s_id, ss_id)
        else:
            return hsd_link.get_sensor_samples_per_ts(d_id, sensor_name)
    
    @staticmethod
    def set_sensor_spts(hsd_link, d_id, new_spts, sensor_name = None, s_id = None, ss_id = None):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.set_samples_per_timestamp(d_id, s_id, ss_id, new_spts)
        else:
            return hsd_link.set_sensor_samples_per_ts(d_id, new_spts, sensor_name)
    
    def init_sensors_data_counters(self, sensor_list):
        if isinstance(self, HSDLink_v1):
            # self.sensor_data_counts = [None] * len(sensor_list)
            for i, s in enumerate(sensor_list):
                for j, ss in enumerate(s.sensor_descriptor.sub_sensor_descriptor):
                    self.sensor_data_counts[(i,j)] = 0
        else:
            self.sensor_data_counts = {s: 0 for s in sensor_list}

    @staticmethod
    def get_sw_tag_classes(hsd_link, device_id):
        return hsd_link.get_sw_tag_classes(device_id)
    
    @staticmethod
    def get_hw_tag_classes(hsd_link, device_id):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_hw_tag_classes(device_id)
        else:
            return None
    
    @staticmethod
    def get_max_tags_per_acq(hsd_link, device_id):
        return hsd_link.get_max_tags_per_acq(device_id)
    
    @staticmethod
    def get_updated_mlc_sensor_list(hsd_link, device_id, current_mlc_sensor_list):
        if isinstance(hsd_link, HSDLink_v1):
            if device_id is not None and current_mlc_sensor_list is None:
                return hsd_link.get_sub_sensors(device_id, type_filter="MLC", only_active=False)
        else:
            if device_id is not None and current_mlc_sensor_list is None:
                return hsd_link.get_sensors(device_id, type_filter="mlc", only_active=False)
            
    @staticmethod
    def get_updated_ispu_sensor_list(hsd_link, device_id, current_ispu_sensor_list):
        if isinstance(hsd_link, HSDLink_v1):
            if device_id is not None and current_ispu_sensor_list is None:
                return hsd_link.get_sub_sensors(device_id, type_filter="ISPU", only_active=False)
        else:
            if device_id is not None and current_ispu_sensor_list is None:
                return hsd_link.get_sensors(device_id, type_filter="ispu", only_active=False)
        
    @staticmethod
    def get_mlc_id(hsd_link, device_id):
        mlc_sensor_list = HSDLink.get_updated_mlc_sensor_list(hsd_link, device_id, None)
        if isinstance(hsd_link, HSDLink_v1):
            if device_id is not None and mlc_sensor_list is not None:
                for mlc_s in mlc_sensor_list:
                    if mlc_s.name == 'ISM330DHCX':
                        mlc_id = [mlc_s.id, mlc_s.sensor_descriptor.sub_sensor_descriptor[0].id]
                        return mlc_id
        else:
            if device_id is not None and mlc_sensor_list is not None:
                #NOTE WIP - @ the moment only 1 MLC per board is supported
                for mlc_s in mlc_sensor_list:
                    if 'mlc' in mlc_s:
                        mlc_sensor_list = mlc_s
                        mlc_id = mlc_sensor_list.index(mlc_s)
                        return mlc_id
    
    @staticmethod
    def get_ispu_id(hsd_link, device_id):
        ispu_sensor_list = HSDLink.get_updated_ispu_sensor_list(hsd_link, device_id, None)
        if isinstance(hsd_link, HSDLink_v1):
            if device_id is not None and ispu_sensor_list is not None:
                for ispu_s in ispu_sensor_list:
                    if ispu_s.name == 'ISM330IS':
                        ispu_id = [ispu_s.id, ispu_s.sensor_descriptor.sub_sensor_descriptor[0].id]
                        return ispu_id
        else:
            if device_id is not None and ispu_sensor_list is not None:
                #NOTE WIP - @ the moment only 1 MLC per board is supported
                for ispu_s in ispu_sensor_list:
                    if 'ispu' in ispu_s:
                        ispu_sensor_list = ispu_s
                        ispu_id = ispu_sensor_list.index(ispu_s)
                        return ispu_id
    
    @staticmethod
    def upload_mlc_ucf_file(hsd_link, device_id, ucf_file):
        # MLC configuration file [<a_mlc_configuration_file>.ufc]        
        if isinstance(hsd_link, HSDLink_v1):
            if ucf_file is not None and ucf_file != '' and os.path.exists(ucf_file):
                mlc_id = HSDLink.get_mlc_id(hsd_link, device_id)
                if mlc_id is not None:
                    res = hsd_link.upload_mlc_ucf_file(device_id, mlc_id[0], ucf_file)
                    if not res:
                        log.warning("Error in MLC configuration update!")
                    #Activate MLC sensor
                    time.sleep(0.05) #TODO remove it when this issue will be fixed in FW
                    res = hsd_link.set_sub_sensor_active(device_id, mlc_id[0], mlc_id[1], True)
                    if not res:
                        log.warning("Error in MLC enable!")
        else:
            if ucf_file is not None and ucf_file != '' and os.path.exists(ucf_file):
                mlc_sensor = HSDLink.get_updated_mlc_sensor_list(hsd_link, device_id, None)
                if mlc_sensor is not None:
                    res = hsd_link.upload_mlc_ucf_file(device_id, list(mlc_sensor.keys())[0], ucf_file)
                    if not res:
                        log.warning("Error in MLC configuration update!")
                    #Activate MLC sensor
                    time.sleep(0.05) #TODO remove it when this issue will be fixed in FW

    @staticmethod
    def upload_ispu_ucf_file(hsd_link, device_id, ucf_file):
        # ISPU configuration file [<a_ispu_configuration_file>.ufc]        
        if isinstance(hsd_link, HSDLink_v1):
            if ucf_file is not None and ucf_file != '' and os.path.exists(ucf_file):
                ispu_id = HSDLink.get_ispu_id(hsd_link, device_id)
                if ispu_id is not None:
                    res = hsd_link.upload_ispu_ucf_file(device_id, ispu_id[0], ucf_file)
                    if not res:
                        log.warning("Error in ISPU configuration update!")
                    #Activate ISPU sensor
                    time.sleep(0.05) #TODO remove it when this issue will be fixed in FW
                    res = hsd_link.set_sub_sensor_active(device_id, ispu_id[0], ispu_id[1], True)
                    if not res:
                        log.warning("Error in ISPU enable!")
        else:
            if ucf_file is not None and ucf_file != '' and os.path.exists(ucf_file):
                ispu_sensor = HSDLink.get_updated_ispu_sensor_list(hsd_link, device_id, None)
                if ispu_sensor is not None:
                    res = hsd_link.upload_ispu_ucf_file(device_id, list(ispu_sensor.keys())[0], ucf_file)
                    if not res:
                        log.warning("Error in ISPU configuration update!")
                    #Activate ISPU sensor
                    time.sleep(0.5) #TODO remove it when this issue will be fixed in FW
    
    @staticmethod
    def set_RTC(hsd_link, device_id):
        if isinstance(hsd_link, HSDLink_v2):
            now = datetime.now()
            time = now.strftime("%Y%m%d_%H_%M_%S")
            message = {"log_controller*set_time":{"datetime":time}}
            hsd_link.send_command(device_id, json.dumps(message))

    @staticmethod            
    def update_device(hsd_link, device_id, device_config_file):
        return hsd_link.update_device(device_id, device_config_file)
    
    @staticmethod
    def set_sw_tag_on_off(hsd_link, device_id, tag_id, new_tag_status):
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_sw_tag_on(device_id, tag_id) if new_tag_status else hsd_link.set_sw_tag_off(device_id, tag_id)
        else:
            hsd_link.set_sw_tag_on_by_id(device_id, tag_id) if new_tag_status else hsd_link.set_sw_tag_off_by_id(device_id, tag_id)
    
    @staticmethod
    def save_json_device_file(hsd_link, device_id, output_acquisition_path = None):
        return hsd_link.save_json_device_file(device_id, output_acquisition_path)
    
    @staticmethod
    def save_json_acq_info_file(hsd_link, device_id, output_acquisition_path = None):
        return hsd_link.save_json_acq_info_file(device_id, output_acquisition_path)
    
    def __increment_sensor_data_counter(self, size, s_id, ss_id = None):
        if ss_id is None and type(s_id) == str:
            self.hsd_link.sensor_data_counts[s_id] += size
        else:
            self.hsd_link.sensor_data_counts[(s_id,ss_id)] += size
    
    def get_sensor_data_counts(self, s_id, ss_id = None):
        if isinstance(self, HSDLink_v1):
            return self.hsd_link.sensor_data_counts[(s_id,ss_id)]
        else:
            return self.hsd_link.sensor_data_counts[s_id]
            
    @staticmethod
    def start_sensor_acquisition_thread(hsd_link, device_id, sensor, threads_stop_flags, sensor_data_files, print_data_cnt = False):
        output_acquisition_path = hsd_link.get_acquisition_folder()
        if isinstance(hsd_link, HSDLink_v1):
            for sd in sensor.sensor_descriptor.sub_sensor_descriptor:
                sensor_data_file_path = os.path.join(output_acquisition_path,(str(sensor.name) + "_"  + str(sd.sensor_type) + ".dat"))
                sensor_data_file = open(sensor_data_file_path, "wb+")
                sensor_data_files.append(sensor_data_file)
                stopFlag = Event()
                threads_stop_flags.append(stopFlag)
                thread = SensorAcquisitionThread(stopFlag, hsd_link, sensor_data_file, device_id, sensor.id, sd.id, print_data_cnt = print_data_cnt)
                thread.start()
        else:
            hsd_link.set_rtc_time(device_id)
            sensor_data_file_path = os.path.join(output_acquisition_path,(str(sensor) + ".dat"))
            sensor_data_file = open(sensor_data_file_path, "wb+")
            sensor_data_files.append(sensor_data_file)
            stopFlag = Event()
            threads_stop_flags.append(stopFlag)
            thread = SensorAcquisitionThread(stopFlag, hsd_link, sensor_data_file, device_id, sensor, print_data_cnt = print_data_cnt)
            thread.start()

    @staticmethod
    def stop_sensor_acquisition_threads(threads_stop_flags, sensor_data_files):
        for sf in threads_stop_flags:
            sf.set()
        for f in sensor_data_files:
            f.close()
    
    @staticmethod
    def stop_log(hsd_link, device_id):
        return hsd_link.stop_log(device_id)
        
    @staticmethod
    def start_log(hsd_link, device_id, sub_folder = True):
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.start_log(device_id)
        else:
            return hsd_link.start_log(device_id, sub_folder=sub_folder)
        
    @staticmethod
    def switch_bank(hsd_link, device_id):
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.switch_bank(device_id)
        else:
            return None
   
    @staticmethod
    def set_sw_tag_on(hsd_link, device_id, tag_id):
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_sw_tag_on(device_id, tag_id)
        else:
            hsd_link.set_sw_tag_on_by_id(device_id, tag_id)
    
    @staticmethod
    def set_sw_tag_off(hsd_link, device_id, tag_id):
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_sw_tag_off(device_id, tag_id)
        else:
            hsd_link.set_sw_tag_off_by_id(device_id, tag_id)

    @staticmethod
    def refresh_hsd_link(hsd_link):
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.close()
            time.sleep(3)
            hsd_link.open()
        

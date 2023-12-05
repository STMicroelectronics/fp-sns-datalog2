# *****************************************************************************
#  * @file    HSDLink_v1.py
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
import shutil
import json
from datetime import datetime

from st_hsdatalog.HSD_utils.exceptions import InvalidCommandSetError
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_link.communication.STWIN_HSD.STWINHSD_com_manager import STWINHSD_Creator
from st_hsdatalog.HSD_link.communication.STWIN_HSD.STWINHSD_CMD_com_manager import STWINHSD_CMD_Creator
from st_hsdatalog.HSD_link.communication.STWIN_HSD.STWINHSD_DLL_com_manager import STWINHSD_DLL_Creator
from st_hsdatalog.HSD_link.communication.ADEV_HSD.ADEVHSD_com_manager import ADEVHSD_Creator

log = logger.get_logger(__name__)

class HSDLink_v1:

    __com_manager = None
    __acquisition_folder = "."
    
    def __init__(self, dev_com_type: str = 'stwin_hsd', acquisition_folder = None):
        self.__create_com_manager(dev_com_type)
        
        self.sensor_data_counts = {}
        self.nof_connected_devices = 0
        
        #check that there is at least one connected device
        self.nof_connected_devices = self.get_nof_devices()
        if self.nof_connected_devices <= 0:
            return

        #acquisition folder creation
        if acquisition_folder is not None:
            if not os.path.exists(acquisition_folder):
                os.makedirs(acquisition_folder)
            self.__acquisition_folder = acquisition_folder
        else:
            self.__acquisition_folder = os.path.join(self.__acquisition_folder, "{}".format(datetime.today().strftime('%Y%m%d_%H_%M_%S')))
            os.makedirs(self.__acquisition_folder)

    def __create_com_manager(self,dev_com_type):
        if dev_com_type == 'stwin_hsd':
            factory = STWINHSD_Creator()
        elif dev_com_type == 'stwin_hsd_dll':
            factory = STWINHSD_DLL_Creator()
        elif dev_com_type == 'stwin_hsd_cmd':
            factory = STWINHSD_CMD_Creator()
        elif dev_com_type == 'adev_hsd':
            factory = ADEVHSD_Creator()
        else:
            log.error("Invalid Command Set selected: {}".format(dev_com_type))
            raise InvalidCommandSetError(dev_com_type)
        #Command set presentation
        self.__com_manager = factory.create_cmd_manager()
        log.info(self.get_cmd_set_presentation_string())

    def get_cmd_set_presentation_string(self):
        return self.__com_manager.get_cmd_set_presentation_string()

    def open(self):
        return self.__com_manager.open()
    
    def close(self):
        return self.__com_manager.close()

    def get_device_presentation_string(self):
        return self.__com_manager.get_device_presentation_string()

    def get_acquisition_folder(self):
        return self.__acquisition_folder

    def get_nof_devices(self):
        return self.__com_manager.get_nof_devices()

    def get_version(self):
        return self.__com_manager.get_version()

    def get_device_info(self, d_id:int):
        return self.__com_manager.get_device_info(d_id)

    def get_devices(self):
        return self.__com_manager.get_devices()

    def get_device(self, d_id:int):
        return self.__com_manager.get_device(d_id)

    def get_device_alias(self, d_id:int):
        return self.__com_manager.get_device_alias(d_id)

    def get_sensors_count(self, d_id:int):
        return self.__com_manager.get_sensors_count(d_id)

    def get_sub_sensors_count(self, d_id:int, s_id:int):
        return self.__com_manager.get_sub_sensors_count(d_id, s_id)

    def get_sensor_name(self, d_id:int, s_id:int):
        return self.__com_manager.get_sensor_name(d_id, s_id)

    def get_sub_sensor_type(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_type(d_id, s_id, ss_id)

    def get_sensor_descriptor(self, d_id:int, s_id:int):
        return self.__com_manager.get_sensor_descriptor(d_id, s_id)

    def get_sub_sensor_descriptor(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_descriptor(d_id, s_id, ss_id)

    def get_sub_sensor_status(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_status(d_id, s_id, ss_id)

    def get_sub_sensor_isActive(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_isActive(d_id, s_id, ss_id)

    def get_sub_sensor_odr(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_odr(d_id, s_id, ss_id)
    
    def get_sub_sensor_measured_odr(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_measured_odr(d_id, s_id, ss_id)

    def get_sub_sensor_fs(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_fs(d_id, s_id, ss_id)

    def get_sub_sensor_sample_per_ts(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_sample_per_ts(d_id, s_id, ss_id)
    
    def get_sub_sensor_initial_offset(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sub_sensor_initial_offset(d_id, s_id, ss_id)

    def get_sub_sensors(self, d_id:int, type_filter="", only_active=True):
        return self.__com_manager.get_sub_sensors(d_id, type_filter, only_active)

    def get_acquisition_info(self, d_id:int):
        return self.__com_manager.get_acquisition_info(d_id)

    def get_available_tags(self, d_id:int):
        return self.__com_manager.get_available_tags(d_id)

    def get_sw_tag_classes(self, d_id: int):
        return self.__com_manager.get_sw_tag_classes(d_id)

    def get_sw_tag_label(self, d_id: int, t_id: int):
        return self.__com_manager.get_sw_tag_label(d_id, t_id)

    def get_hw_tag_classes(self, d_id: int):
        return self.__com_manager.get_hw_tag_classes(d_id)

    def get_hw_tag_label(self, d_id: int, t_id: int):
        return self.__com_manager.get_hw_tag_label(d_id, t_id)

    def get_max_tags_per_acq(self, d_id: int):
        return self.__com_manager.get_max_tags_per_acq(d_id)

    def set_sensor_active(self, d_id: int, s_id: int, new_status: bool):
        return self.__com_manager.set_sensor_active(d_id, s_id, new_status)

    def set_sub_sensor_active(self, d_id: int, s_id: int, ss_id: int, new_status: bool):
        return self.__com_manager.set_sub_sensor_active(d_id, s_id, ss_id, new_status)

    def set_sub_sensor_odr(self, d_id: int, s_id: int, ss_id: int, odr_value: float):
        return self.__com_manager.set_sub_sensor_odr(d_id, s_id, ss_id, odr_value)

    def set_sub_sensor_fs(self, d_id: int, s_id: int, ss_id: int, fs_value: float):
        return self.__com_manager.set_sub_sensor_fs(d_id, s_id, ss_id, fs_value)

    def set_samples_per_timestamp(self, d_id: int, s_id: int, ss_id: int, spts_value: int):
        return self.__com_manager.set_samples_per_timestamp(d_id, s_id, ss_id, spts_value)

    def set_acquisition_info(self, d_id:int, name, notes):
        return self.__com_manager.set_acquisition_info(d_id, name, notes)

    def set_sw_tag_on(self, d_id:int, t_id):
        return self.__com_manager.set_sw_tag_on(d_id, t_id)

    def set_sw_tag_off(self, d_id:int, t_id):
        return self.__com_manager.set_sw_tag_off(d_id, t_id)

    def set_sw_tag_label(self, d_id:int, t_id, label):
        return self.__com_manager.set_sw_tag_label(d_id, t_id, label)

    def set_hw_tag_enabled(self, d_id:int, t_id):
        return self.__com_manager.set_hw_tag_enabled(d_id, t_id)

    def set_hw_tag_disabled(self, d_id:int, t_id):
        return self.__com_manager.set_hw_tag_disabled(d_id, t_id)

    def set_hw_tag_label(self, d_id:int, t_id, label):
        return self.__com_manager.set_hw_tag_label(d_id, t_id, label)

    def update_device(self, d_id:int, device_json_file_path):
        return self.__com_manager.update_device(d_id, device_json_file_path)

    def upload_mlc_ucf_file(self, d_id:int, s_id:int, ucf_file_path):
        res = self.__com_manager.upload_mlc_ucf_file(d_id, s_id, ucf_file_path)
        if res is not None:
            #if correctly loaded, save ucf file in acquisition folder
            shutil.copy(ucf_file_path, self.__acquisition_folder)  
        return res

    def upload_ispu_ucf_file(self, d_id:int, s_id:int, ucf_file_path):
        log.error("ISPU ucf file upload is not supported in HSDatalog1")
        return False

    def get_sensor_data(self, d_id:int, s_id:int, ss_id:int):
        return self.__com_manager.get_sensor_data(d_id, s_id, ss_id)
    
    def start_log(self, d_id:int):
        log.info("Log Started")
        return self.__com_manager.start_log(d_id)

    def stop_log(self, d_id:int):
        log.info("Log Stopped")
        return self.__com_manager.stop_log(d_id)

    def get_acquisition_header(self, d_id:int):
        return self.__com_manager.get_acquisition_header(d_id)

    def save_json_device_file(self, d_id: int, out_acq_path = None):
        json_save_path = self.__acquisition_folder
        if out_acq_path is not None:
            if not os.path.exists(out_acq_path):
                os.makedirs(out_acq_path)
            json_save_path = out_acq_path
        try:
            res = self.__com_manager.get_device(d_id)
            if res is not None:
                res_header = self.__com_manager.get_acquisition_header(d_id)
                dev_conf_dict = { res_header[0][0] : res_header[0][1], res_header[1][0] : res_header[1][1], "device" : res.to_dict()}
                device_config_filename = os.path.join(json_save_path, "DeviceConfig.json")
                sensor_data_file = open(device_config_filename, "w+")
                sensor_data_file.write(json.dumps(dev_conf_dict, indent = 4))
                sensor_data_file.close()
                log.info("DeviceConfig.json Configuration file correctly saved")
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
            res = self.__com_manager.get_acquisition_info(d_id)
            if res is not None:
                acq_info_filename = os.path.join(json_save_path,"AcquisitionInfo.json")
                acq_info_file = open(acq_info_filename, "w+")
                acq_info_file.write(json.dumps(res.to_dict(), indent = 4))
                acq_info_file.close()
                log.info("AcquisitionInfo.json file correctly saved")
                return True
        except:
            raise

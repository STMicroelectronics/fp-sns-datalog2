#
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
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_utils.exceptions import CommunicationEngineOpenError, CommunicationEngineCloseError, EmptyCommandResponse, \
    SETCommandError
import numpy as np
from st_hsdatalog.HSD_link.communication.STWIN_HSD.hsd_dll import HSD_Dll
from st_hsdatalog.HSD.model.DeviceConfig import Device, DeviceInfo, SensorDescriptor, SubSensorDescriptor, \
    SubSensorStatus, TagConfig
from st_hsdatalog.HSD.model.AcquisitionInfo import AcquisitionInfo

log = logger.get_logger(__name__)

class STWINHSD_DLL_CommandManager:

    def __init__(self):
        
        self.hsd_dll = HSD_Dll()
        if(not self.hsd_dll.hs_datalog_open()):
            log.error("Error in Communication Engine opening (libhs_datalog_v1 DLL/so)")
            raise CommunicationEngineOpenError
        else:
            log.info("Communication Engine UP (libhs_datalog_v1 DLL/so)")
    
    def __del__(self):
        if(not self.hsd_dll.hs_datalog_close()):
            log.error("Error in Communication Engine closure (libhs_datalog_v1 DLL/so)")
            raise CommunicationEngineCloseError
        else:
            log.info("Communication Engine DOWN (libhs_datalog_v1 DLL/so)")

    def __send_message(self, d_id: int, message):
        res = self.hsd_dll.hs_datalog_send_message(d_id,message,len(message))
        if res[0]:
            return res[2]
        return None

    def open(self):
        return self.hsd_dll.hs_datalog_open()
        
    def close(self):
        return self.hsd_dll.hs_datalog_close()

    def get_device_presentation_string(self):
        return "STWIN"

    def get_cmd_set_presentation_string(self):
        return "STWINHSD_DLL_CommandManager command_set: None(DLL API)"

    def get_version(self):
        res = self.hsd_dll.hs_datalog_get_version()
        return res[0]

    def get_nof_devices(self):
        res = self.hsd_dll.hs_datalog_get_device_number()
        if res[0]:
            return res[1]
        log.error("Empty response from get_nof_devices(...).")
        raise EmptyCommandResponse("get_nof_devices")

    def get_device_info(self, d_id: int):
        res = self.hsd_dll.hs_datalog_get_device_descriptor()
        if res[0]:
            dev_info_dict = json.loads(res[1])
            return DeviceInfo.from_dict(dev_info_dict['deviceInfo'])
        log.error("No DeviceInfo[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_device_info")

    def get_device(self, d_id: int):
        res = self.hsd_dll.hs_datalog_get_device(d_id)
        if res[0]:
            device_dict = json.loads(res[1])
            return Device.from_dict(device_dict['device'])
        log.error("No Device[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_device")

    def get_device_alias(self, d_id: int):
        res = self.hsd_dll.hs_datalog_get_device_name(d_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_device_alias(...).")
        raise EmptyCommandResponse("get_device_alias")

    def get_sensors_count(self, d_id: int):
        res = self.hsd_dll.hs_datalog_get_sensor_number(d_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sensors_count(...).")
        raise EmptyCommandResponse("get_sensors_count")
    
    def get_sub_sensors_count(self, d_id: int, s_id: int):
        res = self.hsd_dll.hs_datalog_get_sub_sensor_number(d_id, s_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensors_count(...).")
        raise EmptyCommandResponse("get_sub_sensors_count")
    
    def get_sensor_name(self, d_id: int, s_id: int):
        res = self.hsd_dll.hs_datalog_get_sensor_name(d_id, s_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sensor_name(...).")
        raise EmptyCommandResponse("get_sensor_name")
    
    def get_sub_sensor_type(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_sub_sensor_name(d_id, s_id, ss_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensor_type(...).")
        raise EmptyCommandResponse("get_sub_sensor_type")

    def get_sensor_descriptor(self, d_id: int, s_id: int):
        res = self.hsd_dll.hs_datalog_get_sensor_descriptor(d_id, s_id)
        if res[0]:
            s_desc_dict = json.loads(res[1])
            return SensorDescriptor.from_dict(s_desc_dict)
        log.error("Empty response from get_sensor_descriptor(...).")
        raise EmptyCommandResponse("get_sensor_descriptor")
    
    def get_sub_sensor_descriptor(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_subsensor_descriptor(d_id, s_id, ss_id)
        if res[0]:
            ss_desc_dict = json.loads(res[1])
            return SubSensorDescriptor.from_dict(ss_desc_dict)
        log.error("Empty response from get_sub_sensor_descriptor(...).")
        raise EmptyCommandResponse("get_sub_sensor_descriptor")

    def get_sub_sensor_status(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_subsensor_status(d_id, s_id, ss_id)
        if res[0]:
            ss_desc_dict = json.loads(res[1])
            return SubSensorStatus.from_dict(ss_desc_dict)
        log.error("Empty response from get_sub_sensor_status(...).")
        raise EmptyCommandResponse("get_sub_sensor_status")

    def get_sub_sensor_isActive(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_sub_sensor_active(d_id, s_id, ss_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensor_isActive(...).")
        raise EmptyCommandResponse("get_sub_sensor_isActive")

    def get_sub_sensor_odr(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_ODR(d_id, s_id, ss_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensor_odr(...).")
        raise EmptyCommandResponse("get_sub_sensor_odr")

    def get_sub_sensor_measured_odr(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_measured_ODR(d_id, s_id, ss_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensor_measured_odr(...).")
        raise EmptyCommandResponse("get_sub_sensor_measured_odr")

    def get_sub_sensor_fs(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_FS(d_id, s_id, ss_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensor_fs(...).")
        raise EmptyCommandResponse("get_sub_sensor_fs")

    def get_sub_sensor_sample_per_ts(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_samples_per_timestamp(d_id, s_id, ss_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensor_sample_per_ts(...).")
        raise EmptyCommandResponse("get_sub_sensor_sample_per_ts")

    def get_sub_sensor_initial_offset(self, d_id: int, s_id: int, ss_id: int):
        res = self.hsd_dll.hs_datalog_get_initial_offset(d_id, s_id, ss_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sub_sensor_initial_offset(...).")
        raise EmptyCommandResponse("get_sub_sensor_initial_offset")

    def get_acquisition_header(self, d_id: int):
        res = self.hsd_dll.hs_datalog_get_device(d_id)
        if res[0]:
            device_dict = json.loads(res[1])
            return ["JSONVersion", device_dict['JSONVersion']],["UUIDAcquisition",device_dict['UUIDAcquisition']]
        log.error("No Acquisition Header[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_acquisition_header")

    def get_devices(self):
        nof_devices = self.get_nof_devices()
        if nof_devices is not None:
            dev_list=[]
            for i in range(0,nof_devices):
                device = self.get_device(i)
                dev_list.append(device)
            return dev_list
        log.warning("Empty devices list")
        raise EmptyCommandResponse("get_devices")

    def get_acquisition_info(self, d_id: int):
        res = self.hsd_dll.hs_datalog_get_acquisition_info(d_id)
        if res[0]:
            acq_info_dict = json.loads(res[1])
            return AcquisitionInfo.from_dict(acq_info_dict)
        log.error("No AcquisitionInfo[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_acquisition_info")

    def get_available_tags(self, d_id: int):
        res = self.hsd_dll.hs_datalog_get_available_tags(d_id)
        if res[0]:
            tags_dict = json.loads(res[1])
            return TagConfig.from_dict(tags_dict)
        log.error("No TagConfig[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_available_tags")

    def get_sw_tag_classes(self, d_id: int):
        try:
            tag_config = self.get_available_tags(d_id)
            if tag_config is not None:
                return tag_config.sw_tags
            log.error("No TagConfig[d_id:{}] returned.".format(d_id))
            raise EmptyCommandResponse("get_sw_tag_classes")
        except:
            raise
    
    def get_sw_tag_label(self, d_id: int, t_id: int):
        res = self.hsd_dll.hs_datalog_get_sw_label(d_id, t_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_sw_tag_label(...).")
        raise EmptyCommandResponse("get_sw_tag_label")

    def get_hw_tag_classes(self, d_id: int):
        try:
            tag_config = self.get_available_tags(d_id)
            if tag_config is not None:
                return tag_config.hw_tags
            log.error("No TagConfig[d_id:{}] returned.".format(d_id))
            raise EmptyCommandResponse("get_sw_tag_classes")
        except:
            raise
    
    def get_hw_tag_label(self, d_id: int, t_id: int):
        res = self.hsd_dll.hs_datalog_get_hw_label(d_id, t_id)
        if res[0]:
            return res[1]
        log.error("Empty response from get_hw_tag_label(...).")
        raise EmptyCommandResponse("get_hw_tag_label")
    
    def get_max_tags_per_acq(self, d_id: int):
        try:
            tag_config = self.get_available_tags(d_id)
            if tag_config is not None:
                return tag_config.max_tags_per_acq
            log.error("No TagConfig[d_id:{}] returned.".format(d_id))
            raise EmptyCommandResponse("get_max_tags_per_acq")
        except:
            raise

    def set_device_alias(self, d_id: int, device_alias: str):
        message = {}
        message['command'] = 'SET'
        message['request'] = 'deviceInfo'
        message['alias'] = device_alias
        json_message = json.dumps(message)
        res = self.__send_message(d_id, json_message)
        if res is None:
            log.error("Error in Device alias[{}] parameter update".format(device_alias))
            raise SETCommandError("set_device_alias")
        return True

    def set_sensor_active(self, d_id: int, s_id: int, new_status: bool):
        res = self.hsd_dll.hs_datalog_set_sensor_active(d_id, s_id, new_status)
        if res:
            log.info("Sensor [d{},{}] sensor {} correctly.".format(d_id, s_id, "activated" if new_status else "dectivated"))
            return True
        log.error("Error in sensor [d{},{}] {}.".format(d_id, s_id, "activation" if new_status else "deactivation"))
        raise SETCommandError("set_sensor_active")

    def set_sub_sensor_active(self, d_id: int, s_id: int, ss_id: int, new_status: bool):
        res = self.hsd_dll.hs_datalog_set_sub_sensor_active(d_id, s_id, ss_id, new_status)
        if res:
            log.info("Sensor [d{},{},{}] sensor {} correctly.".format(d_id, s_id, ss_id, "activated" if new_status else "dectivated"))
            return True
        log.error("Error in sensor [d{},{},{}] {}.".format(d_id, s_id, ss_id, "activation" if new_status else "deactivation"))
        raise SETCommandError("set_sub_sensor_active")

    def set_sub_sensor_odr(self, d_id: int, s_id: int, ss_id: int, odr_value: float):
        res = self.hsd_dll.hs_datalog_set_ODR(d_id, s_id, ss_id, odr_value)
        if res:
            log.info("ODR set correctly for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
            return True
        log.error("Error setting ODR for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
        raise SETCommandError("set_sub_sensor_odr")

    def set_sub_sensor_fs(self, d_id: int, s_id: int, ss_id: int, fs_value: float):
        res = self.hsd_dll.hs_datalog_set_FS(d_id, s_id, ss_id, fs_value)
        if res:
            log.info("FS set correctly for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
            return True
        log.error("Error setting FS for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
        raise SETCommandError("set_sub_sensor_fs")

    def set_samples_per_timestamp(self, d_id: int, s_id: int, ss_id: int, spts_value: int):
        res = self.hsd_dll.hs_datalog_set_samples_per_timestamp(d_id, s_id, ss_id, spts_value)
        if res:
            log.info("Samples per TS value set correctly for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
            return True
        log.error("Error setting Samples per TS for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
        raise SETCommandError("set_samples_per_timestamp")

    def set_acquisition_info(self, d_id: int, name: str, notes: str):
        res = self.hsd_dll.hs_datalog_set_acquisition_param(d_id, name, notes)
        if res:
            log.info("Acquisition Info correctly updated.")
            return True
        log.error("Error setting Acquisition Info.")
        raise SETCommandError("set_acquisition_info")

    def set_hw_tag_enabled(self, d_id: int, t_id: int):
        res = self.hsd_dll.hs_datalog_enable_hw_tag(d_id, t_id, True)
        if res:
            log.info("TagHW [d{},{}] correctly enabled.".format(d_id, t_id))
            return True
        log.error("Error in Tag enable.")
        raise SETCommandError("set_hw_tag_enabled")

    def set_hw_tag_disabled(self, d_id: int, t_id: int):
        res = self.hsd_dll.hs_datalog_enable_hw_tag(d_id, t_id, False)
        if res:
            log.info("TagHW [d{},{}] correctly disabled.".format(d_id, t_id))
            return True
        log.error("Error in Tag disable.")
        raise SETCommandError("set_hw_tag_disabled")
    
    def set_hw_tag_label(self, d_id: int, t_id, label: str):
        res = self.hsd_dll.hs_datalog_set_hw_label(d_id, t_id, label)
        if res:
            log.info("TagSW [d{},{}] \"{}\" label correctly updated.".format(d_id, t_id, label))
            return True
        log.error("Error in Tag \"{}\" label update.".format(label))
        raise SETCommandError("set_hw_tag_label")

    def set_sw_tag_on(self, d_id: int, t_id: int):
        res = self.hsd_dll.hs_datalog_set_on_sw_tag(d_id, t_id)
        if res:
            log.info("TagSW [d{},{}] START!.".format(d_id, t_id))
            return True
        log.error("Error starting TagSW [d{},{}].".format(d_id, t_id))
        raise SETCommandError("set_sw_tag_on")
    
    def set_sw_tag_off(self, d_id: int, t_id: int):
        res = self.hsd_dll.hs_datalog_set_off_sw_tag(d_id, t_id)
        if res:
            log.info("TagSW [d{},{}] END!.".format(d_id, t_id))
            return True
        log.error("Error stopping TagSW [d{},{}].".format(d_id, t_id))
        raise SETCommandError("set_sw_tag_off")

    def set_sw_tag_label(self, d_id: int, t_id: int, label: str):
        res = self.hsd_dll.hs_datalog_set_sw_label(d_id, t_id, label)
        if res:
            log.info("TagSW [d{},{}] \"{}\" label correctly updated.".format(d_id, t_id, label))
            return True
        log.error("Error in Tag \"{}\" label update.".format(label))
        raise SETCommandError("set_sw_tag_label")

    def update_device(self, d_id: int, device_json_file_path):
        with open(device_json_file_path) as f:
            device_dict = json.load(f)
            f.close()
        device_model = Device.from_dict(device_dict['device'])
        #Update device alias
        device_alias = device_model.device_info.alias
        try:
            self.set_device_alias(d_id, device_alias)
        except:
            raise
        
        #Update sensor params
        sensor_list = device_model.sensor
        for sensor in sensor_list:
            ss_descriptor = sensor.sensor_descriptor.sub_sensor_descriptor
            for i, sss in enumerate(sensor.sensor_status.sub_sensor_status):
                # params = []
                if sss.is_active is not None:
                    res = self.hsd_dll.hs_datalog_set_sub_sensor_active(d_id, sensor.id, ss_descriptor[i].id, sss.is_active)
                    if not res:
                        log.error("Error in Sensor[{},{}] activation".format(sensor.id,ss_descriptor[i].id))
                        raise SETCommandError("hs_datalog_set_sub_sensor_active")
                if sss.odr is not None:
                    self.hsd_dll.hs_datalog_set_ODR(d_id, sensor.id, ss_descriptor[i].id, sss.odr)
                    if not res:
                        log.error("Error in Sensor[{},{}] ODR update".format(sensor.id,ss_descriptor[i].id))
                        raise SETCommandError("hs_datalog_set_ODR")
                if sss.fs is not None:
                    self.hsd_dll.hs_datalog_set_FS(d_id, sensor.id, ss_descriptor[i].id, sss.odr)
                    if not res:
                        log.error("Error in Sensor[{},{}] FS update".format(sensor.id,ss_descriptor[i].id))
                        raise SETCommandError("hs_datalog_set_FS")
                if sss.samples_per_ts is not None:
                    self.hsd_dll.hs_datalog_set_samples_per_timestamp(d_id, sensor.id, ss_descriptor[i].id, sss.samples_per_ts)
                    if not res:
                        log.error("Error in Sensor[{},{}] Samples per timestamp update".format(sensor.id,ss_descriptor[i].id))
                        raise SETCommandError("hs_datalog_set_samples_per_timestamp")

        #Update tag config
        hw_tag_class_list = device_model.tag_config.hw_tags
        for tag_hw in hw_tag_class_list:
            res = self.hsd_dll.hs_datalog_enable_hw_tag(d_id, tag_hw.id, tag_hw.enabled)
            if res is None:
                log.error("Error in HW Tag [{}] status update.".format(tag_hw.label))
                raise SETCommandError("hs_datalog_enable_hw_tag")
            res = self.hsd_dll.hs_datalog_set_hw_label(d_id, tag_hw.id, tag_hw.label)
            if res is None:
                log.error("Error in HW Tag [{}] label update.".format(tag_hw.label))
                raise SETCommandError("hs_datalog_set_hw_label")
        sw_tag_class_list = device_model.tag_config.sw_tags
        for tag_sw in sw_tag_class_list:
            res = self.hsd_dll.hs_datalog_set_sw_label(d_id, tag_sw.id, tag_sw.label)
            if res is None:
                log.error("Error in SW Tag [{}] label update.".format(tag_sw.label))
                raise SETCommandError("hs_datalog_set_sw_label")
        log.info("--> Configuration JSON successfully sent to the connected device [{}]!".format(device_alias))
        return True

    def upload_mlc_ucf_file(self, d_id: int, s_id: int, ucf_file_path):
        with open(ucf_file_path, 'rb') as f:
            ucf_buffer = np.fromfile(f, dtype='uint8')
            f.close()
            res = self.hsd_dll.hs_datalog_send_UCF_to_MLC(d_id, s_id, ucf_buffer, len(ucf_buffer))
            if res:
                log.info("--> ucf configuration file sent successfully! MLC sensor id: {}".format(s_id))
                return True
        log.error("Error uploading ucf for Sensor[{}]".format(s_id))
        raise SETCommandError("upload_mlc_ucf_file")

    def get_sensor_data(self, d_id: int, s_id: int, ss_id: int):
        size = self.hsd_dll.hs_datalog_get_available_data_size(d_id,s_id,ss_id)
        if size[1] > 0:
            data = self.hsd_dll.hs_datalog_get_data(d_id,s_id,ss_id,size[1])
            if data[0]:
                return [size[1], data[1]]
        return None

    def start_log(self,d_id: int):
        return self.hsd_dll.hs_datalog_start_log(d_id)
    
    def stop_log(self,d_id: int):
        return self.hsd_dll.hs_datalog_stop_log(d_id)

    def get_sub_sensors(self, d_id: int, type_filter="", only_active=True):
        device = self.get_device(d_id)
        active_sensors = []
        if device is not None:
            sensor_list = device.sensor
            for s in sensor_list:
                active_sensor = s
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

class STWINHSD_DLL_Creator:

    def create_cmd_manager(self):
        return STWINHSD_DLL_CommandManager()
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
from st_hsdatalog.HSD_link.communication.STWIN_HSD.STWINHSD_commands import MLCParam, STWINHSDGetDeviceInfoCmd, \
    STWINHSDGetDeviceCmd, STWINHSDGetLogStatusCmd, STWINHSDSetAcquisitionInfoCmd, STWINHSDSetMLCSensorCmd, STWINHSDSetSWTagCmd, STWINHSDGetTagConfigCmd, \
    STWINHSDGetAcquisitionInfo, STWINHSDStartLoggingCmd, STWINHSDStopLoggingCmd, IsActiveParam, ODRParam, FSParam, \
    SamplePerTSParam, STWINHSDSetSensorCmd, STWINHSDSetHWTagCmd, STWINHSDSetHWTagLabelCmd, STWINHSDSetSWTagLabelCmd, \
    STWINHSDSetDeviceAliasCmd, STWINHSDGetSubSensorStatusCmd, STWINHSDGetSensorDescriptorCmd, STWINHSDGetSubSensorDescriptorCmd
from st_hsdatalog.HSD_link.communication.STWIN_HSD.hsd_dll import HSD_Dll
from st_hsdatalog.HSD.model.DeviceConfig import Device, DeviceInfo, SensorDescriptor, SubSensorDescriptor, \
    SubSensorStatus, TagConfig
from st_hsdatalog.HSD.model.AcquisitionInfo import AcquisitionInfo

log = logger.get_logger(__name__)

class STWINHSD_CMD_Cmd():

    def get_name(self):
        return "STWINHSDCmd"

    def get_device_info_cmd(self):
        return STWINHSDGetDeviceInfoCmd()

    def get_device_cmd(self):
        return STWINHSDGetDeviceCmd()

    def get_sub_sensor_status_cmd(self, s_id, ss_id):
        return STWINHSDGetSubSensorStatusCmd(s_id, ss_id)

    def get_sensor_descriptor_cmd(self, s_id, ):
        return STWINHSDGetSensorDescriptorCmd(s_id)

    def get_sub_sensor_descriptor_cmd(self, s_id, ss_id):
        return STWINHSDGetSubSensorDescriptorCmd(s_id, ss_id)

    def get_available_tags_cmd(self):
        return STWINHSDGetTagConfigCmd()

    def get_log_status_cmd(self):
        return STWINHSDGetLogStatusCmd()
    
    def get_acquisition_info_cmd(self):
        return STWINHSDGetAcquisitionInfo()

    def set_device_alias_cmd(self, alias):
        return STWINHSDSetDeviceAliasCmd(alias)

    def set_acquisition_info_cmd(self, name, notes):
        return STWINHSDSetAcquisitionInfoCmd(name, notes)

    def set_sw_tag_on_cmd(self, t_id):
        return STWINHSDSetSWTagCmd(t_id, True)

    def set_sw_tag_off_cmd(self, t_id):
        return STWINHSDSetSWTagCmd(t_id, False)

    def set_sw_tag_label_cmd(self, t_id, label):
        return STWINHSDSetSWTagLabelCmd(t_id, label)

    def set_hw_tag_enabled_cmd(self, t_id):
        return STWINHSDSetHWTagCmd(t_id, True)

    def set_hw_tag_disabled_cmd(self, t_id):
        return STWINHSDSetHWTagCmd(t_id, False)

    def set_hw_tag_label_cmd(self, t_id, label):
        return STWINHSDSetHWTagLabelCmd(t_id, label)

    def start_log_cmd(self):
        return STWINHSDStartLoggingCmd()

    def stop_log_cmd(self):
        return STWINHSDStopLoggingCmd()

    def is_active_param_cmd(self, ss_id, is_active):
        return IsActiveParam(ss_id, is_active)

    def odr_param_cmd(self, ss_id, odr):
        return ODRParam(ss_id, odr)

    def fs_param_cmd(self, ss_id, fs):
        return FSParam(ss_id, fs)

    def sample_per_ts_param_cmd(self, ss_id, sample_per_ts):
        return SamplePerTSParam(ss_id, sample_per_ts)

    def mlc_config_param_cmd(self, ss_id, ucf_file_path):
        with open(ucf_file_path, "r") as f:
            lines = f.readlines()
            
        lines = [line.replace(' ', '') for line in lines]
        lines = [line.replace('\n', '') for line in lines]
        lines = list(filter(None, lines))
        for line in lines:
            if line == '' or line.startswith('--'):
                lines.remove(line)
        lines = [line[2:] for line in lines]
        ucf_data = ''.join(lines)
        
        return MLCParam(ss_id, len(ucf_data), ucf_data)
    
    def set_sensor_cmd(self, s_id, ss_params):
        return STWINHSDSetSensorCmd(s_id, ss_params)

    def set_mlc_sensor_cmd(seldf, s_id, mlc_params):
        return STWINHSDSetMLCSensorCmd(s_id, mlc_params)

class STWINHSD_CMD_CommandManager:

    def __init__(self, cmd_set:STWINHSD_CMD_Cmd):
        self.cmd_set = cmd_set
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
        res = self.hsd_dll.hs_datalog_send_message(d_id, message, len(message))
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
        return "STWINHSD_CommandManager command_set: {}".format(self.cmd_set.get_name())

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
        message = json.dumps(self.cmd_set.get_device_info_cmd().to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            dev_info_dict = json.loads(res)
            return DeviceInfo.from_dict(dev_info_dict['deviceInfo'])
        log.error("No DeviceInfo[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_device_info")

    def get_device(self, d_id: int):
        message = json.dumps(self.cmd_set.get_device_cmd().to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            device_dict = json.loads(res)
            device = Device.from_dict(device_dict['device'])
            self.device_model = device
            return device
        log.error("No Device[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_device")

    def get_device_alias(self, d_id: int):
        dev_info = self.get_device_info(d_id)
        if dev_info is not None:
            return dev_info.alias
        log.error("Empty response from get_device_alias(...).")
        raise EmptyCommandResponse("get_device_alias")

    def get_sensors_count(self, d_id: int):
        dev_info = self.get_device_info(d_id)
        if dev_info is not None:
            return dev_info.n_sensor
        log.error("Empty response from get_sensors_count(...).")
        raise EmptyCommandResponse("get_sensors_count")
    
    def get_sub_sensors_count(self, d_id: int, s_id: int):
        sd = self.get_sensor_descriptor(d_id, s_id)
        if sd is not None:
            return len(sd.sub_sensor_descriptor)
        log.error("Empty response from get_sub_sensors_count(...).")
        raise EmptyCommandResponse("get_sub_sensors_count")
    
    def get_sensor_name(self, d_id: int, s_id: int):
        dev = self.get_device(d_id)
        if dev is not None:
            return dev.sensor[s_id].name
        log.error("Empty response from get_sensor_name(...).")
        raise EmptyCommandResponse("get_sensor_name")
    
    def get_sub_sensor_type(self, d_id: int, s_id: int, ss_id: int):
        ssd = self.get_sub_sensor_descriptor(d_id, s_id, ss_id)
        if ssd is not None:
            return ssd.sensor_type
        log.error("Empty response from get_sub_sensor_type(...).")
        raise EmptyCommandResponse("get_sub_sensor_type")

    def get_sensor_descriptor(self, d_id: int, s_id: int):
        message = json.dumps(self.cmd_set.get_sensor_descriptor_cmd(s_id).to_dict())
        res = self.__send_message(d_id, message)
        if res is not None:
            s_desc_dict = json.loads(res)
            return SensorDescriptor.from_dict(s_desc_dict)
        log.error("Empty response from get_sensor_descriptor(...).")
        raise EmptyCommandResponse("get_sensor_descriptor")
    
    def get_sub_sensor_descriptor(self, d_id: int, s_id: int, ss_id: int):
        message = json.dumps(self.cmd_set.get_sub_sensor_descriptor_cmd(s_id, ss_id).to_dict())
        res = self.__send_message(d_id, message)
        if res is not None:
            ss_desc_dict = json.loads(res)
            return SubSensorDescriptor.from_dict(ss_desc_dict)
        log.error("Empty response from get_sub_sensor_descriptor(...).")
        raise EmptyCommandResponse("get_sub_sensor_descriptor")

    def get_sub_sensor_status(self, d_id: int, s_id: int, ss_id: int):
        message = json.dumps(self.cmd_set.get_sub_sensor_status_cmd(s_id, ss_id).to_dict())
        res = self.__send_message(d_id, message)
        if res is not None:
            ss_desc_dict = json.loads(res)
            return SubSensorStatus.from_dict(ss_desc_dict)
        log.error("Empty response from get_sub_sensor_status(...).")
        raise EmptyCommandResponse("get_sub_sensor_status")

    def get_sub_sensor_isActive(self, d_id: int, s_id: int, ss_id: int):
        sss = self.get_sub_sensor_status(d_id, s_id, ss_id)
        if sss is not None:
            return sss.is_active
        log.error("Empty response from get_sub_sensor_isActive(...).")
        raise EmptyCommandResponse("get_sub_sensor_isActive")

    def get_sub_sensor_odr(self, d_id: int, s_id: int, ss_id: int):
        sss = self.get_sub_sensor_status(d_id, s_id, ss_id)
        if sss is not None:
            return sss.odr
        log.error("Empty response from get_sub_sensor_odr(...).")
        raise EmptyCommandResponse("get_sub_sensor_odr")

    def get_sub_sensor_measured_odr(self, d_id: int, s_id: int, ss_id: int):
        sss = self.get_sub_sensor_status(d_id, s_id, ss_id)
        if sss is not None:
            return sss.odr_measured
        log.error("Empty response from get_sub_sensor_measured_odr(...).")
        raise EmptyCommandResponse("get_sub_sensor_measured_odr")

    def get_sub_sensor_fs(self, d_id: int, s_id: int, ss_id: int):
        sss = self.get_sub_sensor_status(d_id, s_id, ss_id)
        if sss is not None:
            return sss.fs
        log.error("Empty response from get_sub_sensor_fs(...).")
        raise EmptyCommandResponse("get_sub_sensor_fs")

    def get_sub_sensor_sample_per_ts(self, d_id: int, s_id: int, ss_id: int):
        sss = self.get_sub_sensor_status(d_id, s_id, ss_id)
        if sss is not None:
            return sss.samples_per_ts
        log.error("Empty response from get_sub_sensor_sample_per_ts(...).")
        raise EmptyCommandResponse("get_sub_sensor_sample_per_ts")

    def get_sub_sensor_initial_offset(self, d_id: int, s_id: int, ss_id: int):
        sss = self.get_sub_sensor_status(d_id, s_id, ss_id)
        if sss is not None:
            return sss.initial_offset
        log.error("Empty response from get_sub_sensor_initial_offset(...).")
        raise EmptyCommandResponse("get_sub_sensor_initial_offset")

    def get_acquisition_header(self, d_id: int):
        message = json.dumps(self.cmd_set.get_device_cmd().to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            device_dict = json.loads(res)
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
        message = json.dumps(self.cmd_set.get_acquisition_info_cmd().to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            acq_info_dict = json.loads(res)
            return AcquisitionInfo.from_dict(acq_info_dict)
        log.error("No AcquisitionInfo[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_acquisition_info")

    def get_available_tags(self, d_id: int):
        message = json.dumps(self.cmd_set.get_available_tags_cmd().to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            tags_dict = json.loads(res)
            return TagConfig.from_dict(tags_dict['tagConfig'])
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
        try:
            tag_config = self.get_available_tags(d_id)
            if tag_config is not None:
                for tag in tag_config.sw_tags:
                    if tag.id == t_id:
                        return tag.label
            log.error("Empty response from get_sw_tag_label(...).")
            raise EmptyCommandResponse("get_sw_tag_label")
        except:
            raise

    def get_hw_tag_classes(self, d_id: int):
        try:
            tag_config = self.get_available_tags(d_id)
            if tag_config is not None:
                return tag_config.hw_tags
            log.error("No TagConfig[d_id:{}] returned.".format(d_id))
            raise EmptyCommandResponse("get_hw_tag_classes")
        except:
            raise
    
    def get_hw_tag_label(self, d_id: int, t_id: int):
        try:
            tag_config = self.get_available_tags(d_id)
            if tag_config is not None:
                for tag in tag_config.hw_tags:
                    if tag.id == t_id:
                        return tag.label
            log.error("Empty response from get_hw_tag_label(...).")
            raise EmptyCommandResponse("get_hw_tag_label")
        except:
            raise

    def get_max_tags_per_acq(self, d_id: int):
        try:
            tag_config = self.get_available_tags(d_id)
            if tag_config is not None:
                return tag_config.max_tags_per_acq
            log.error("No TagConfig[d_id:{}] returned.".format(d_id))
            raise EmptyCommandResponse("get_max_tags_per_acq")
        except:
            raise

    def set_sensor_active(self, d_id: int, s_id: int, new_status: bool):
        try:
            ss_descriptor = self.get_sensor_descriptor(d_id, s_id).sub_sensor_descriptor
            params = []
            for ssd in ss_descriptor:
                params.append(self.cmd_set.is_active_param_cmd(ssd.id, new_status))
            message = json.dumps(self.cmd_set.set_sensor_cmd(s_id,params).to_dict())
            res = self.__send_message(d_id,message)
            if res is not None:
                log.info("Sensor [d{},{}] sensor {} correctly.".format(d_id, s_id, "activated" if new_status else "dectivated"))
                return True
            log.error("Error in sensor [d{},{}] {}.".format(d_id, s_id, "activation" if new_status else "deactivation"))
            raise SETCommandError("set_sensor_active")
        except:
            raise

    def set_sub_sensor_active(self, d_id: int, s_id: int, ss_id: int, new_status: bool):
        params = []
        params.append(self.cmd_set.is_active_param_cmd(ss_id, new_status))
        message = json.dumps(self.cmd_set.set_sensor_cmd(s_id,params).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("Sensor [d{},{},{}] sensor {} correctly.".format(d_id, s_id, ss_id, "activated" if new_status else "dectivated"))
            return True
        log.error("Error in sensor [d{},{},{}] {}.".format(d_id, s_id, ss_id, "activation" if new_status else "deactivation"))
        raise SETCommandError("set_sub_sensor_active")

    def set_sub_sensor_odr(self, d_id: int, s_id: int, ss_id: int, odr_value: float):
        params = []
        params.append(self.cmd_set.odr_param_cmd(ss_id, odr_value))
        message = json.dumps(self.cmd_set.set_sensor_cmd(s_id,params).to_dict())
        res = self.__send_message(d_id,message)
        if res:
            log.info("ODR set correctly for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
            return True
        log.error("Error setting ODR for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
        raise SETCommandError("set_sub_sensor_odr")

    def set_sub_sensor_fs(self, d_id: int, s_id: int, ss_id: int, fs_value: float):
        params = []
        params.append(self.cmd_set.fs_param_cmd(ss_id, fs_value))
        message = json.dumps(self.cmd_set.set_sensor_cmd(s_id,params).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("FS set correctly for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
            return True
        log.error("Error setting FS for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
        raise SETCommandError("set_sub_sensor_fs")

    def set_samples_per_timestamp(self, d_id: int, s_id: int, ss_id: int, spts_value: int):
        params = []
        params.append(self.cmd_set.sample_per_ts_param_cmd(ss_id, spts_value))
        message = json.dumps(self.cmd_set.set_sensor_cmd(s_id,params).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("Samples per TS value set correctly for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
            return True
        log.error("Error setting Samples per TS for sensor [d{},{},{}].".format(d_id, s_id, ss_id))
        raise SETCommandError("set_samples_per_timestamp")

    def set_acquisition_info(self, d_id: int, name: str, notes: str):
        message = json.dumps(self.cmd_set.set_acquisition_info_cmd(name, notes).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("Acquisition Info correctly updated.")
            return True
        log.error("Error setting Acquisition Info.")
        raise SETCommandError("set_acquisition_info")

    def set_hw_tag_enabled(self, d_id: int, t_id: int):
        message = json.dumps(self.cmd_set.set_hw_tag_enabled_cmd(t_id).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("TagHW [d{},{}] correctly enabled.".format(d_id, t_id))
            return True
        log.error("Error in Tag enable.")
        raise SETCommandError("set_hw_tag_enabled")

    def set_hw_tag_disabled(self, d_id: int, t_id: int):
        message = json.dumps(self.cmd_set.set_hw_tag_disabled_cmd(t_id).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("TagHW [d{},{}] correctly disabled.".format(d_id, t_id))
            return True
        log.error("Error in Tag disable.")
        raise SETCommandError("set_hw_tag_disabled")
    
    def set_hw_tag_label(self, d_id: int, t_id, label: str):
        message = json.dumps(self.cmd_set.set_hw_tag_label_cmd(t_id, label).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("TagSW [d{},{}] \"{}\" label correctly updated.".format(d_id, t_id, label))
            return True
        log.error("Error in Tag \"{}\" label update.".format(label))
        raise SETCommandError("set_hw_tag_label")

    def set_sw_tag_on(self, d_id: int, t_id: int):
        message = json.dumps(self.cmd_set.set_sw_tag_on_cmd(t_id).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("TagSW [d{},{}] START!.".format(d_id, t_id))
            return True
        log.error("Error starting TagSW [d{},{}].".format(d_id, t_id))
        raise SETCommandError("set_sw_tag_on")
    
    def set_sw_tag_off(self, d_id: int, t_id: int):
        message = json.dumps(self.cmd_set.set_sw_tag_off_cmd(t_id).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("TagSW [d{},{}] END!.".format(d_id, t_id))
            return True
        log.error("Error in SW Tag [{}] label update.".format(t_id))
        raise SETCommandError("set_sw_tag_off")

    def set_sw_tag_label(self, d_id: int, t_id: int, label: str):      
        message = json.dumps(self.cmd_set.set_sw_tag_label_cmd(t_id, label).to_dict())
        res = self.__send_message(d_id,message)
        if res is not None:
            log.info("TagSW [d{},{}] \"{}\" label correctly updated.".format(d_id, t_id, label))
            return True
        log.error("Error in SW Tag [{}] label update.".format(t_id))
        raise SETCommandError("set_sw_tag_label")

    def update_device(self, d_id: int, device_json_file_path):
        with open(device_json_file_path) as f:
            device_dict = json.load(f)
            f.close()
        device_model = Device.from_dict(device_dict['device'])
        #Update device alias
        device_alias = device_model.device_info.alias
        message = json.dumps(self.cmd_set.set_device_alias_cmd(device_alias).to_dict())
        res = self.__send_message(d_id,message)
        if res is None:
            log.error("Error in Device alias[{}] parameter update".format(device_alias))
            raise SETCommandError("set_device_alias_cmd")
        #Update sensor params
        sensor_list = device_model.sensor
        for sensor in sensor_list:
            for i, sss in enumerate(sensor.sensor_status.sub_sensor_status):
                params = []
                if sss.is_active is not None:
                    params.append(self.cmd_set.is_active_param_cmd(i, sss.is_active))
                if sss.odr is not None:
                    params.append(self.cmd_set.odr_param_cmd(i, sss.odr))
                if sss.fs is not None:
                    params.append(self.cmd_set.fs_param_cmd(i, sss.fs))
                if sss.samples_per_ts is not None:
                    params.append(self.cmd_set.sample_per_ts_param_cmd(i, sss.samples_per_ts))
                message = json.dumps(self.cmd_set.set_sensor_cmd(sensor.id,params).to_dict())
                res = self.__send_message(d_id,message)
                if res is None:
                    log.error("Error in Sensor[{}] parameters update".format(sensor.name))
                    raise SETCommandError("set_sensor_cmd")
        #Update tag config
        hw_tag_class_list = device_model.tag_config.hw_tags
        for tag_hw in hw_tag_class_list:
            message = json.dumps(self.cmd_set.set_hw_tag_enabled_cmd(tag_hw.id).to_dict()) if tag_hw.enabled else json.dumps(self.cmd_set.set_hw_tag_disabled_cmd(tag_hw.id).to_dict())
            res = self.__send_message(d_id,message)
            if res is None:
                log.error("Error in HW Tag [{}] status update.".format(tag_hw.label))
                raise SETCommandError("set_hw_tag_enabled_cmd")
            message = json.dumps(self.cmd_set.set_hw_tag_label_cmd(tag_hw.id,tag_hw.label).to_dict())
            res = self.__send_message(d_id,message)
            if res is None:
                log.error("Error in HW Tag [{}] label update.".format(tag_hw.label))
                raise SETCommandError("set_hw_tag_label_cmd")
        sw_tag_class_list = device_model.tag_config.sw_tags
        for tag_sw in sw_tag_class_list:
            message = json.dumps(self.cmd_set.set_sw_tag_label_cmd(tag_sw.id, tag_sw.label).to_dict())
            res = self.__send_message(d_id,message)
            if res is None:
                log.error("Error in SW Tag [{}] label update.".format(tag_sw.label))
                raise SETCommandError("set_sw_tag_label_cmd")
        log.info("--> Configuration JSON successfully sent to the connected device [{}]!".format(device_alias))
        return True
    
    def upload_mlc_ucf_file(self, d_id: int, s_id: int, ucf_file_path):
        ss_id = None
        for ssd in self.get_sensor_descriptor(d_id,s_id).sub_sensor_descriptor:
            if ssd.sensor_type == 'MLC':
                ss_id = ssd.id
        if ss_id is not None:
            params = []
            params.append(self.cmd_set.mlc_config_param_cmd(ss_id, ucf_file_path))
            message = json.dumps(self.cmd_set.set_mlc_sensor_cmd(s_id,params).to_dict())
            res = self.__send_message(d_id,message)
            if res is None:
                log.error("Error in MLC Sensor[{},{}] ucf upload".format(s_id,ss_id))
                raise SETCommandError("set_mlc_sensor_cmd")
            log.info("--> ucf configuration file sent successfully! MLC sensor id: [{},{}]".format(s_id,ss_id))
            return True
        log.error("Selected Sensor[{}] doesn't contain a MLC sub sensor".format(s_id))
        raise SETCommandError("upload_mlc_ucf_file")

    def get_sensor_data(self, d_id: int, s_id: int, ss_id: int):
        size = self.hsd_dll.hs_datalog_get_available_data_size(d_id,s_id,ss_id)
        if size[1] > 0:
            data = self.hsd_dll.hs_datalog_get_data(d_id,s_id,ss_id,size[1])
            if data[0]:
                return [size[1], data[1]]
        return None

    ### missing usb channels management, ok with DLL
    def start_log(self,d_id: int):
        # message = json.dumps(self.cmd_set.start_log_cmd().to_dict())
        # return self.__send_message(d_id,message)
        return self.hsd_dll.hs_datalog_start_log(d_id)
    
    ### missing usb channels management
    def stop_log(self,d_id: int):
        # message = json.dumps(self.cmd_set.stop_log_cmd().to_dict())
        # return self.__send_message(d_id,message)
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

class STWINHSD_CMD_Creator:
    def __create_cmd_set(self): return STWINHSD_CMD_Cmd()
    def create_cmd_manager(self):
        cmd_set = self.__create_cmd_set()
        return STWINHSD_CMD_CommandManager(cmd_set)

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
import serial
from threading import Semaphore
from st_hsdatalog.HSD_link.communication.PnPL_STSRL.SSTL import SSTL

from st_pnpl.DTDL import device_template_model as DTM

import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_utils.exceptions import EmptyCommandResponse, PnPLCommandError, \
    WrongDeviceConfigFile
from st_pnpl.PnPLCmd import PnPLCMDManager


log = logger.get_logger(__name__)

class PnPLSTSRL_CommandManager:

    def __init__(self, cmd_set = "pnpl"):
        self.cmd_set = cmd_set

        self.serial_port = None

        # Create a semaphore
        self.serial_semaphore = Semaphore(0)

        self.sstl_manager = None
        self.sstl_packet = None

    def send_pnpl_msg(self, PnPL_msg):
        self.sstl_manager.send_command(self.serial_port, PnPL_msg)
        self.serial_semaphore.acquire(timeout=2)
        print("SEND PnPL Msg UNLOCKED, Response:", self.sstl_packet.data)
        if self.sstl_packet.data != b'':
            return json.loads(self.sstl_packet.data.decode())
        else:
            return None

    def receive_bytes(self):
        self.sstl_packet = self.sstl_manager.receive(self.serial_port)
        if self.sstl_packet:
            if self.sstl_packet.header:
                cr = self.sstl_packet.header.cr
                if cr == 0:
                    return self.sstl_packet
                elif cr == 1:
                    # it is not possible to receive packets with this cr value (only from host to the board)
                    return "ERROR"
                elif cr == 2:
                    # response
                    self.serial_semaphore.release()
                    return self.sstl_packet
                elif cr == 3:
                    # async property change
                    return self.sstl_packet
                elif cr == 4:
                    return self.sstl_packet
        else:
            self.serial_semaphore.release()
        return self.sstl_packet
    
    def get_serial_data(self):
        return self.receive_bytes()
    
    def flush(self):
        self.serial_port.flush()
        self.serial_port.read_all()

    def open(self, com_id, speed=1843200):
        self.serial_port = serial.Serial(com_id, speed)
        self.sstl_manager = SSTL(self.serial_port)
        return self.serial_port.is_open

    def close(self):
        # Close the serial connection
        self.serial_port.close()

    #TODO filter only compatible devices
    def get_nof_devices(self):
        # Get a list of available serial ports
        ports = list(serial.tools.list_ports.comports())
        #TODO filter here
        # Return the number of connected devices
        return len(ports)
    
    def send_command(self, d_id: int, message):
        return self.send_pnpl_msg(message)

    def get_version(self):
        return "0"

    def get_device_info(self, d_id:int):
        return self.get_component_status(d_id, "firmware_info")
    
    def get_cmd_set_presentation_string(self):
        return "PnPL Commands"

    def get_device_presentation_string(self, d_id: int=None):
        if d_id is not None:
            res = self.send_pnpl_msg(PnPLCMDManager.create_get_presentation_string_cmd())
            if res is not None:
                return res
            log.error("Empty response from get_device_identity(d_id={})".format(d_id))
            raise EmptyCommandResponse("get_device_identity")
    
    def get_device_identity(self, d_id: int):
        if d_id is not None:
            res = self.send_pnpl_msg(PnPLCMDManager.create_get_presentation_string_cmd())
            if res is not None:
                return res
            log.error("Empty response from get_device_identity(d_id={})".format(d_id))
            raise EmptyCommandResponse("get_device_identity")
    
    def get_device_alias(self, d_id: int):
        return self.get_device_info(d_id)["firmware_info"]["alias"]

    def start_log(self, d_id: int, interface: int = 3):
        self.send_pnpl_msg(PnPLCMDManager.create_command_cmd("log_controller", "start_log", "interface", interface))
        return True
    
    def stop_log(self,d_id: int):
        self.send_pnpl_msg(PnPLCMDManager.create_command_cmd("log_controller", "stop_log"))
        return True

    def get_devices(self):
        nof_devices = self.get_nof_devices()
        if nof_devices is not None:
            dev_list=[]
            for i in range(0,nof_devices):
                device = self.get_device_status(i)
                dev_list.append(device)
            return dev_list
        return None

    def get_device_status(self, d_id: int):
        res = self.send_pnpl_msg(PnPLCMDManager.create_get_device_status_cmd())
        if res is not None:
            return res
        log.error("No Device Status[d_id:{}] returned.".format(d_id))
        raise EmptyCommandResponse("get_device_status")

    def get_device(self, d_id: int):
        return self.get_device_status(d_id)

    def set_device_status(self, d_id: int, device_status_json_file_path):
        pass #use update_device(self, d_id: int, device_status_json_file_path, dtdl_components)

    #TODO this could be shared with other com_manager
    def __get_components_status(self, d_id: int, c_type:int=None):
        res = dict()
        devices = self.get_device_status(d_id)
        device_status = devices["devices"][0]
        for comp in device_status["components"]:
            comp_name = list(comp.keys())[0]
            comp_status = comp[comp_name]
            if c_type is not None:
                if "c_type" in comp_status and comp_status["c_type"] == c_type:
                    res[comp_name] = comp_status
            else:
                res[comp_name] = comp_status
        return res

    #TODO this could be shared with other com_manager
    def get_sensor_components_status(self, d_id: int, type_filter, only_active):
        disabled_sensors_names = []
        sensors_c = self.__get_components_status(d_id,0)
        if type_filter == "":
            if only_active == True:
                for sc in sensors_c:
                    if "enable" in sensors_c[sc]:
                        if sensors_c[sc]["enable"] == False:
                            disabled_sensors_names.append(sc)
                    else:
                        disabled_sensors_names.append(sc)
        else:
            if only_active == True:
                for sc in sensors_c:
                    sensor_type = sc.lower().split("_")[-1]
                    if "enable" in sensors_c[sc]:
                        if sensors_c[sc]["enable"] == False:
                            disabled_sensors_names.append(sc)
                        else:
                            if sensor_type != type_filter.lower():
                                disabled_sensors_names.append(sc)
                    else:
                        disabled_sensors_names.append(sc)
            else:
                for sc in sensors_c:
                    sensor_type = sc.lower().split("_")[-1]
                    if sensor_type != type_filter.lower():
                        disabled_sensors_names.append(sc)
        if len(disabled_sensors_names) > 0:
            for dsn in disabled_sensors_names:
                del sensors_c[dsn]
        return sensors_c
    
    #TODO this could be shared with other com_manager
    def get_algorithm_components_status(self, d_id: int, only_active: bool):
        algo_c = self.__get_components_status(d_id,1)
        if only_active == True:
            disabled_algo_names = []
            for ac in algo_c:
                if "enable" in algo_c[ac]:
                    if algo_c[ac]["enable"] == False:
                        disabled_algo_names.append(ac)
                else:
                    disabled_algo_names.append(ac)
            if len(disabled_algo_names) > 0:
                for dsn in disabled_algo_names:
                    del algo_c[dsn]
        return algo_c
    
    def get_actuator_components_status(self, d_id: int, only_active: bool):
        actuator_c = self.__get_components_status(d_id,3)
        if only_active == True:
            disabled_actuator_names = []
            for ac in actuator_c:
                if "enable" in actuator_c[ac]:
                    if actuator_c[ac]["enable"] == False:
                        disabled_actuator_names.append(ac)
                else:
                    disabled_actuator_names.append(ac)
            if len(disabled_actuator_names) > 0:
                for dsn in disabled_actuator_names:
                    del actuator_c[dsn]
        return actuator_c
    
    def get_component_status(self, d_id:int, component_name:str):
        return self.send_pnpl_msg(PnPLCMDManager.create_get_component_status_cmd(component_name))
    
    def get_components_count(self, d_id: int):
        return len(self.__get_components_status(d_id))

    def get_sensor_components_count(self, d_id: int, only_active: bool):
        return len(self.get_sensor_components_status(d_id, "", only_active))

    def get_algorithm_components_count(self, d_id: int, only_active: bool):
        return len(self.get_algorithm_components_status(d_id, only_active))
    
    def get_actuator_components_count(self, d_id: int, only_active: bool):
        return len(self.get_actuator_components_status(d_id, only_active))

    def get_components_names(self, d_id: int):
        comp_names = []
        res = self.__get_components_status(d_id)
        if res is not None:
            for r in res:
                comp_names.append(r)
            return comp_names
        log.error("Empty response from get_components_names.".format(d_id))
        raise EmptyCommandResponse("get_components_names")

    def get_sensor_components_names(self, d_id: int, type_filter: str, only_active: bool):
        sensor_names = []
        res = self.get_sensor_components_status(d_id, type_filter, only_active)
        if res is not None:
            for r in res:
                if only_active == False:
                    sensor_names.append(r)
                else:
                    if res[r]["enable"]:
                        sensor_names.append(r)
            return sensor_names
        log.error("Empty response from get_sensor_components_names.".format(d_id))
        raise EmptyCommandResponse("get_sensor_components_names")

    def get_algorithm_components_names(self, d_id: int, only_active: bool):
        algo_names = []
        res = self.get_algorithm_components_status(d_id, only_active)
        if res is not None:
            for r in res:
                if only_active == False:
                    algo_names.append(r)
                else:
                    if res[r]["enable"]:
                        algo_names.append(r)
            return algo_names
        log.error("Empty response from get_algorithm_components_names.".format(d_id))
        raise EmptyCommandResponse("get_algorithm_components_names")
    
    def get_actuator_components_names(self, d_id: int, only_active: bool):
        actuator_names = []
        res = self.get_actuator_components_status(d_id, only_active)
        if res is not None:
            for r in res:
                if only_active == False:
                    actuator_names.append(r)
                else:
                    if res[r]["enable"]:
                        actuator_names.append(r)
            return actuator_names
        log.error("Empty response from get_actuator_components_names.".format(d_id))
        raise EmptyCommandResponse("get_actuator_components_names")

    def __get_property_value(self, d_id: int, comp_name: str, prop_name: str, sub_prop_name: str = None):
        c_status = self.get_component_status(d_id, comp_name)
        if sub_prop_name is None:
            return c_status.get(comp_name).get(prop_name)
        else:
            return c_status.get(comp_name).get(prop_name).get(sub_prop_name)
    
    def get_boolean_property(self, d_id: int, comp_name: str, prop_name: str, sub_prop_name: str = None):
        return self.__get_property_value(d_id, comp_name, prop_name, sub_prop_name)
    
    def get_integer_property(self, d_id: int, comp_name: str, prop_name: str, sub_prop_name: str = None):
        return self.__get_property_value(d_id, comp_name, prop_name, sub_prop_name)

    def get_float_property(self, d_id: int, comp_name: str, prop_name: str, sub_prop_name: str = None):
        return self.__get_property_value(d_id, comp_name, prop_name, sub_prop_name)

    def get_string_property(self, d_id: int, comp_name: str, prop_name: str, sub_prop_name: str = None):
        return self.__get_property_value(d_id, comp_name, prop_name, sub_prop_name)

    def set_property(self, d_id: int, value, comp_name: str, prop_name: str, sub_prop_name: str = None):
        res = None
        if value == []:
            return False
        if type(value) == bool or type(value) == int or type(value) == float or type(value) == str:
            if sub_prop_name is None:
                res = self.send_pnpl_msg(PnPLCMDManager.create_set_property_cmd(comp_name, prop_name, value))
            else:
                res = self.send_pnpl_msg(PnPLCMDManager.create_set_property_cmd(comp_name, prop_name, { sub_prop_name: value}))
        return res

    #TODO this could be shared with other com_manager
    def __is_content_a_property(self, content):
        if isinstance(content.type, list):
            return [c for c in content.type if c.value == "Property"] is not None
        else:
            return content.type.value == DTM.ContentType.PROPERTY.value
    
    #TODO this could be shared with other com_manager
    def __set_component_values(self, d_id: int, comp, dtdl_comp):
        comp_name = list(comp.keys())[0]
        for content in dtdl_comp.contents:
            prop_check = self.__is_content_a_property(content)            
            if prop_check and content.writable == True:
                if content.name in comp[comp_name].keys():
                    if isinstance(content.schema, str):
                        self.set_property(d_id, comp[comp_name][content.name], comp_name, content.name)
                    else:
                        #check property type (Enum, Object or others)
                        if content.schema.type == DTM.SchemaType.ENUM:
                            self.set_property(d_id, comp[comp_name][content.name], comp_name, content.name)
                        elif content.schema.type == DTM.SchemaType.OBJECT:
                            for field in content.schema.fields:
                                self.set_property(d_id, comp[comp_name][content.name][field.name], comp_name, content.name, field.name)
                else:
                    print("[WARNING] - wrong property name in your Device Status --> Component: {}".format(comp_name))

    #TODO this could be shared with other com_manager
    def update_device(self, d_id: int, device_status_json_file_path, dtdl_components):
        with open(device_status_json_file_path) as f:
            device_dict = json.load(f)
            f.close()
        pres_res = self.get_device_presentation_string(d_id)
        if pres_res is not None:
            board_board_id = hex(pres_res["board_id"])
            board_fw_id = hex(pres_res["fw_id"])
            file_board_id = hex(device_dict["devices"][0]["board_id"])
            file_fw_id = hex(device_dict["devices"][0]["fw_id"])
            if board_board_id == file_board_id and board_fw_id == file_fw_id:
                for component in device_dict["devices"][d_id]["components"]:
                    for key in component.keys():
                        if key in dtdl_components:
                            self.__set_component_values(d_id, component, dtdl_components[key])
            else:
                log.error(f"Wrong device_config.json selected. - ID from board: b_id{board_board_id}, fw_id{board_fw_id} - ID from file: b_id{file_board_id}, fw_id{file_fw_id}")
                raise WrongDeviceConfigFile(f"ID from board: b_id{board_board_id}, fw_id{board_fw_id} - ID from file: b_id{file_board_id}, fw_id{file_fw_id}")
    
class PnPLSTSRL_Creator:
    def __create_cmd_set(self):
        return None
    
    def create_cmd_manager(self):
        cmd_set = self.__create_cmd_set()
        return PnPLSTSRL_CommandManager(cmd_set)
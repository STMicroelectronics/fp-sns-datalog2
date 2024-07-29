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
from st_hsdatalog.HSD_link.HSDLink_v2 import HSDLink_v2_Serial
from st_hsdatalog.HSD_link.HSDLink_v1 import HSDLink_v1
from st_hsdatalog.HSD_utils.exceptions import CommunicationEngineOpenError
import st_hsdatalog.HSD_utils.logger as logger
from st_pnpl.PnPLCmd import PnPLCMDManager

log = logger.get_logger(__name__)

class SensorAcquisitionThread(Thread):
    def __init__(self, event, hsd_link, sensor_data_file, d_id, s_id, ss_id = None, print_data_cnt = False):
        """
        Initializes the SensorAcquisitionThread.

        :param event: Event to stop the thread.
        :param hsd_link: Instance of HSDLink.
        :param sensor_data_file: File to save sensor data. None if no file saving is required.
        :param d_id: Device ID.
        :param s_id: Sensor ID.
        :param ss_id: Sub-sensor ID (optional).
        :param print_data_cnt: Flag to print data count.
        """
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
        """
        Runs the thread to acquire sensor data.
        """
        if self.print_data_cnt == True:
            self.run_print()
        else:
            self.run_no_print()

    def run_print(self):
        """
        Runs the thread with data count printing.
        """                 
        while not self.stopped.wait(0.02):
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

                ## file saving
                if self.sensor_data_file is not None:
                    res = self.sensor_data_file.write(sensor_data)

    def run_no_print(self):
        """
        Runs the thread without data count printing.
        """               
        while not self.stopped.wait(0.02):
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

                ## file saving
                if self.sensor_data_file is not None:
                    res = self.sensor_data_file.write(sensor_data)

class HSDLink:
    def create_hsd_link(self, dev_com_type: str = 'st_hsd', acquisition_folder = None, plug_callback = None, unplug_callback = None):
        """
        Creates an instance of HSDLink based on the communication type.

        :param dev_com_type: Communication type (default: 'st_hsd').
        :param acquisition_folder: Folder for acquisition data.
        :param plug_callback: Callback for device plug event (not yet supported).
        :param unplug_callback: Callback for device unplug event (not yet supported).
        :return: Instance of HSDLink_v1 or HSDLink_v2.
        """
        self.dev_com_type = dev_com_type
        self.acquisition_folder = acquisition_folder
        self.is_datalog2 = False
        
        if dev_com_type == 'st_hsd':
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
                # if self.dev_com_type == "st_hsd":
                #     self.dev_com_type = "pnpl"
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
        elif dev_com_type == "st_serial_datalog":
            hsd_link = HSDLink_v2_Serial(self.dev_com_type, self.acquisition_folder)
            if hsd_link.nof_connected_devices == 0:
                log.warning("No COM devices connected!")
                self.is_datalog2 = False
                return None
            else:
                print("{} - HSDatalogApp.{} - INFO - Commmunication Opened correctly".format(logger.get_datetime(), __name__))
        return hsd_link
    
    @staticmethod
    def get_versiontuple(v):
        """
        Converts version string to a tuple of integers.

        :param v: Version string.
        :return: Tuple of integers.
        """
        return tuple(map(int, (v.split("."))))
    
    @staticmethod
    def is_v2(hsd_link):
        """
        Checks if the HSDLink instance is of version 2.

        :param hsd_link: Instance of HSDLink.
        :return: Boolean indicating if the instance is of version 2.
        """
        return isinstance(hsd_link, HSDLink_v2)
    
    @staticmethod
    def get_device_presentation_string(hsd_link, device_id):
        """
        Retrieves the device presentation string.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Device presentation string.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_device_presentation_string(device_id)
        else:
            return None
        
    @staticmethod
    def get_device_identity(hsd_link, device_id):
        """
        Retrieves the device identity string (only for HSD2 v>=1.2.0).

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Device identity string.
        """
        if HSDLink.is_v2(hsd_link):
            return hsd_link.send_command(device_id, PnPLCMDManager.create_get_identity_string_cmd())
        return None
    
    @staticmethod
    def get_version(hsd_link):
        """
        Retrieves the version of the HSDLink instance.

        :param hsd_link: Instance of HSDLink.
        :return: Version string.
        """
        return hsd_link.get_version()
    
    @staticmethod
    def get_devices(hsd_link):
        """
        Retrieves the list of connected devices.

        :param hsd_link: Instance of HSDLink.
        :return: List of connected devices.
        """
        if hsd_link is not None:
            return hsd_link.get_devices()
        else:
            return []
    
    @staticmethod
    def get_device(hsd_link, device_id):
        """
        Retrieves the device information for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Device information.
        """
        return hsd_link.get_device(device_id)
    
    @staticmethod
    def get_device_alias(hsd_link, device_id):
        """
        Retrieves the device alias for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Device alias.
        """
        return hsd_link.get_device_alias(device_id)
    
    @staticmethod
    def get_device_info(hsd_link, device_id):
        """
        Retrieves the device information for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Device information.
        """
        return hsd_link.get_device_info(device_id)
    
    @staticmethod
    def get_acquisition_info(hsd_link, device_id):
        """
        Retrieves the acquisition information for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Acquisition information.
        """
        return hsd_link.get_acquisition_info(device_id)
    
    @staticmethod
    def get_tags_info(hsd_link, device_id):
        """
        Retrieves the tags information component values for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Tags information.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_tags_info(device_id)
        else:
            return None
        
    @staticmethod
    def get_tag_list(hsd_link, device_id:int):
        """
        Retrieves the list of tags for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :return: List of tags.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_tags_info(device_id)
        else:
            return None
    
    @staticmethod
    def get_sw_tag_class(hsd_link, device_id: int, tag_class_name: str):
        """
        Retrieves the software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_name: Tag class name (no Label).
        :return: Software tag class.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class(device_id, tag_class_name)
        else:
            return None
        
    @staticmethod
    def get_sw_tag_class_label(hsd_link, device_id: int, tag_class_name: str):
        """
        Retrieves the label of a software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_name: Tag class name.
        :return: Software tag class label.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class_label(device_id, tag_class_name)
        else:
            return None
    
    @staticmethod
    def get_sw_tag_class_enabled(hsd_link, device_id: int, tag_class_name: str):
        """
        Retrieves the enabled status of a software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_name: Tag class name.
        :return: Enabled status of the software tag class.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class_enabled(device_id, tag_class_name)
        else:
            return None
    
    @staticmethod
    def get_sw_tag_class_status(hsd_link, device_id: int, tag_class_name: str):
        """
        Retrieves the status of a software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_name: Tag class name.
        :return: Status of the software tag class.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class_status(device_id, tag_class_name)
        else:
            return None

    @staticmethod    
    def set_sw_tag_class_enabled(hsd_link, device_id:int, tag_class_name:str, new_status: bool):
        """
        Sets the enabled status of a software tag class for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_name: Tag class name.
        :param new_status: New enabled status.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_class_enabled(device_id, tag_class_name, new_status)
        else:
            return None
        
    @staticmethod
    def get_sw_tag_class_by_id(hsd_link, device_id: int, tag_class_id: int):
        """
        Retrieves the software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_id: Tag class id.
        :return: Software tag class.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class_by_id(device_id, tag_class_id)
        else:
            return None
    
    @staticmethod
    def get_sw_tag_class_label_by_id(hsd_link, device_id: int, tag_class_id: int):
        """
        Retrieves the label of a software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_id: Tag class ID.
        :return: Software tag class label.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class_label_by_id(device_id, tag_class_id)
        else:
            return None  
    
    @staticmethod
    def get_sw_tag_class_enabled_by_id(hsd_link, device_id: int, tag_class_id: int):
        """
        Retrieves the enabled status of a software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_id: Tag class ID.
        :return: Enabled status of the software tag class.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class_enabled_by_id(device_id, tag_class_id)
        else:
            return None
    
    @staticmethod
    def get_sw_tag_class_status_by_id(hsd_link, device_id: int, tag_class_id: int):
        """
        Retrieves the status of a software tag class for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_class_id: Tag class ID.
        :return: Status of the software tag class.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sw_tag_class_status_by_id(device_id, tag_class_id)
        else:
            return None
    
    @staticmethod
    def set_sw_tag_class_label(hsd_link, device_id:int, tag_class_name:str, new_label: str):
        """
        Sets the label of a software tag class for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_name: Tag class name.
        :param new_label: New label.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_class_label(device_id, tag_class_name, new_label)
        else:
            return None
    
    @staticmethod
    def set_sw_tag_on(hsd_link, device_id:int, tag_class_name:str):
        """
        Sets the status of a software tag class to ON for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_name: Tag class name.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_on(device_id, tag_class_name)
        else:
            return None
    
    @staticmethod
    def set_sw_tag_off(hsd_link, device_id:int, tag_class_name:str):
        """
        Sets the status of a software tag class to OFF for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_name: Tag class name.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_off(device_id, tag_class_name)
        else:
            return None
    
    @staticmethod
    def set_sw_tag_class_enabled_by_id(hsd_link, device_id:int, tag_class_id:int, new_status: bool):
        """
        Sets the enabled status of a software tag class for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_id: Tag class ID.
        :param new_status: New enabled status.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_class_enabled_by_id(device_id, tag_class_id, new_status)
        else:
            return None
    
    @staticmethod
    def set_sw_tag_class_label_by_id(hsd_link, device_id:int, tag_class_id:int, new_label: str):
        """
        Sets the label of a software tag class for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_id: Tag class ID.
        :param new_label: New label.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_class_label_by_id(device_id, tag_class_id, new_label)
        else:
            return None

    @staticmethod
    def set_sw_tag_on_by_id(hsd_link, device_id:int, tag_class_id:int):
        """
        Sets the status of a software tag class to ON for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_id: Tag class ID.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_on_by_id(device_id, tag_class_id)
    
    @staticmethod
    def set_sw_tag_off_by_id(hsd_link, device_id:int, tag_class_id:int):
        """
        Sets the status of a software tag class to OFF for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param tag_class_id: Tag class ID.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sw_tag_off_by_id(device_id, tag_class_id)
    
    @staticmethod
    def get_acquisition_folder(hsd_link):
        """
        Retrieves the acquisition folder path.

        :param hsd_link: Instance of HSDLink.
        :return: Acquisition folder path.
        """
        return hsd_link.get_acquisition_folder()

    @staticmethod
    def update_base_acquisition_folder(hsd_link, base_acquisition_path):
        """
        Updates the base acquisition folder path.

        :param hsd_link: Instance of HSDLink.
        :param base_acquisition_path: New base acquisition folder path.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.update_base_acquisition_folder(base_acquisition_path)
        else:
            return None
        
    @staticmethod
    def set_device_template(hsd_link, dev_template_json):
        """
        Sets the device template.

        :param hsd_link: Instance of HSDLink.
        :type hsd_link: HSDLink_v2
        :param dev_template_json: Device template JSON.
        :type dev_template_json: str
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_device_template(dev_template_json)
        else:
            return None
    
    @staticmethod
    def get_firmware_info(hsd_link, device_id):
        """
        Retrieves the firmware information for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Firmware information.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_firmware_info(device_id)
        else:
            return None
    
    @staticmethod
    def set_acquisition_info(hsd_link, device_id, acq_name, acq_desc):
        """
        Sets the next acquisition information (name and description) for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param acq_name: Acquisition name.
        :param acq_desc: Acquisition description.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_acquisition_info(device_id, acq_name, acq_desc)
        else:
            hsd_link.set_acquisition_info(device_id, acq_name, acq_desc)
    
    @staticmethod
    def set_acquisition_name(hsd_link, device_id:int, acq_name:str):
        """
        Sets the acquisition name for a specific device ID. (HSD_v2 only)

        :param device_id: Device ID.
        :param acq_name: Acquisition name.
        :return: None

        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            hsd_link.set_acquisition_name(device_id, acq_name)
        else:
            return None

    @staticmethod
    def set_acquisition_description(hsd_link, device_id:int, description:str):
        """
        Sets the acquisition description for a specific device ID. (HSD_v2 only)
        
        :param device_id: Device ID.
        :param description: Acquisition description.
        :return: None
        
        Note: If the HSDLink object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            hsd_link.set_acquisition_description(device_id, description)
        else:
            return None

    @staticmethod
    def get_component_status(hsd_link, device_id, component_name:str):
        """
        Retrieves the current value of a component for a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Component status.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_component_status(device_id, component_name)
        else:
            return None

    @staticmethod            
    def get_sensor_list(hsd_link, device_id, type_filter="", only_active=False):
        """
        Retrieves a list of sensors from the HSDLink object.

        :param hsd_link: The HSDLink object to retrieve the sensor list from.
        :param device_id: The ID of the device to retrieve the sensor list for.
        :param type_filter: Optional. A filter to only include sensors of a specific type.
        :param only_active: Optional. If True, only active sensors will be included in the list. default: False
        :return: A list of sensors matching the specified criteria.
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensors(device_id, type_filter=type_filter, only_active=only_active)        
        else:
            return hsd_link.get_sensors(device_id, type_filter=type_filter, only_active=only_active)
    
    @staticmethod
    def get_sensors_names(hsd_link, device_id, type_filter="", only_active=False):
        """
        Retrieves the names of sensors components from the HSDLink object. (HSD_v2 only)

        :param hsd_link (HSDLink_v2): The HSDLink object.
        :param device_id (int): The ID of the device.
        :param type_filter (str): Optional. The type of sensor to filter by.
        :param only_active (bool): Optional. If True, only active sensors will be returned.
        :return: list: A list of sensor names.

        Note:
        - If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sensors_names(device_id, type_filter, only_active)
        else:
            return None
        
    @staticmethod
    def get_sensors_count(hsd_link, device_id, only_active=False):
        """
        Retrieves the count of sensors components from the HSDLink object. (HSD_v2 only)

        :param hsd_link (HSDLink_v2): The HSDLink object.
        :param device_id (int): The ID of the device.
        :param only_active (bool): Optional. If True, only active sensors will be returned.
        :return: the number of sensors.

        Note:
        - If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sensors_count(device_id, only_active)
        else:
            return None
    
    @staticmethod
    def get_algorithms_count(hsd_link, device_id, only_active = False):
        """
        Retrieves the count of algorithms components from the HSDLink object. (HSD_v2 only)

        :param hsd_link (HSDLink_v2): The HSDLink object.
        :param device_id (int): The ID of the device.
        :param only_active (bool): Optional. If True, only active algorithms will be returned.
        :return: the number of algorithms.

        Note:
        - If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_algorithms_count(device_id, only_active)
        else:
            return None

    @staticmethod
    def get_algorithms_names(hsd_link, device_id, only_active = False):
        """
        Retrieves the names of algorithms components from the HSDLink object. (HSD_v2 only)

        :param hsd_link (HSDLink_v2): The HSDLink object.
        :param device_id (int): The ID of the device.
        :param only_active (bool): Optional. If True, only active algorithms will be returned.
        :return: list: A list of algorithm names.

        Note:
        - If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_algorithms_names(device_id, only_active)
        else:
            return None

    @staticmethod
    def get_algorithm_list(hsd_link, device_id, only_active = False):
        """
        Retrieves the algorithms components from the HSDLink object. (HSD_v2 only)

        :param hsd_link (HSDLink_v2): The HSDLink object.
        :param device_id (int): The ID of the device.
        :param only_active (bool): Optional. If True, only active algorithms will be returned.
        :return: list: A list of algorithm.

        Note:
        - If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_algorithms_status(device_id, only_active)
        else:
            return None
    
    @staticmethod
    def get_sensor_enabled(hsd_link, device_id, sensor_name = None, s_id = None, ss_id = None):
        """
        Retrieves the enabled status of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: Enabled status of the sensor.
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_isActive(device_id, s_id, ss_id)
        else:
            return hsd_link.get_sensor_enable(device_id, sensor_name)
    
    @staticmethod
    def set_sensor_enable(hsd_link, device_id, new_status, sensor_name = None, s_id = None, ss_id = None):
        """
        Sets the enabled status of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param new_status: New enabled status.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            if s_id is not None: 
                if ss_id is None:
                    return hsd_link.set_sensor_active(device_id, s_id, new_status)
                else:
                    return hsd_link.set_sub_sensor_active(device_id, s_id, ss_id, new_status)
        else:
            return hsd_link.set_sensor_enable(device_id, new_status, sensor_name)      

    @staticmethod
    def get_sensor_odr(hsd_link, device_id, sensor_name = None, s_id = None, ss_id = None):
        """
        Retrieves the output data rate (ODR) of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: ODR of the sensor.
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_odr(device_id, s_id, ss_id)
        else:
            return hsd_link.get_sensor_odr(device_id, sensor_name)

    @staticmethod 
    def set_sensor_odr(hsd_link, device_id, new_odr, sensor_name = None, s_id = None, ss_id = None):
        """
        Sets the output data rate (ODR) of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param new_odr: New ODR.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.set_sub_sensor_odr(device_id, s_id, ss_id, new_odr)
        else:
            return hsd_link.set_sensor_odr(device_id, new_odr, sensor_name)
    
    @staticmethod
    def get_sensor_fs(hsd_link, device_id, sensor_name = None, s_id = None, ss_id = None):
        """
        Retrieves the full scale (FS) of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: FS of the sensor.
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_fs(device_id, s_id, ss_id)
        else:
            if "_mic" in sensor_name:
                return hsd_link.get_sensor_aop(device_id, sensor_name)
            else:
                return hsd_link.get_sensor_fs(device_id, sensor_name)
    
    @staticmethod
    def set_sensor_fs(hsd_link, device_id, new_fs, sensor_name = None, s_id = None, ss_id = None):
        """
        Sets the full scale (FS) of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param new_fs: New FS.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.set_sub_sensor_fs(device_id, s_id, ss_id, new_fs)
        else:
            if "_mic" in sensor_name:
                return hsd_link.set_sensor_aop(device_id, new_fs, sensor_name)
            else:
                return hsd_link.set_sensor_fs(device_id, new_fs, sensor_name)
            
    @staticmethod
    def get_sensor_aop(self, device_id:int, sensor_name:str):
        """
        Retrieves the acoustic overload point (AOP) of a microphone sensor. (HSD_v2 only)

        :param device_id: Device ID.
        :param sensor_name: Sensor name.
        :return: AOP of the sensor.
        """
        if isinstance(self, HSDLink_v2):
            return self.get_sensor_aop(device_id, sensor_name)
        else:
            return None
    
    @staticmethod
    def set_sensor_aop(hsd_link, device_id: int, new_aop: float, sensor_name: str):
        """
        Sets the acoustic overload poit (AOP) of a microphone. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param new_aop: New AOP.
        :param sensor_name: Sensor name.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.set_sensor_aop(device_id, new_aop, sensor_name)
        else:
            return None
            
    @staticmethod
    def get_sensor_spts(hsd_link, device_id, sensor_name = None, s_id = None, ss_id = None):
        """
        Retrieves the samples per timestamp (SPTS) of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: SPTS of the sensor.
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_sub_sensor_sample_per_ts(device_id, s_id, ss_id)
        else:
            return hsd_link.get_sensor_samples_per_ts(device_id, sensor_name)
    
    @staticmethod
    def set_sensor_spts(hsd_link, device_id, new_spts, sensor_name = None, s_id = None, ss_id = None):
        """
        Sets the samples per timestamp (SPTS) of a sensor.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param new_spts: New SPTS.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.set_samples_per_timestamp(device_id, s_id, ss_id, new_spts)
        else:
            return hsd_link.set_sensor_samples_per_ts(device_id, new_spts, sensor_name)

    @staticmethod    
    def get_sensor_initial_offset(hsd_link, device_id, sensor_name = None, s_id = None, ss_id = None):
        """
        Retrieves the initial offset of a sensor. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param sensor_name: Sensor name (optional).
        :param s_id: Sensor ID (optional).
        :param ss_id: Sub-sensor ID (optional).
        :return: Initial offset of the sensor.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_sensor_initial_offset(device_id, sensor_name)
        else:
            return None

    def init_sensors_data_counters(self, sensor_list):
        """
        Initializes the data counters for sensors.

        :param sensor_list: List of sensors.
        :return: None
        """
        if isinstance(self, HSDLink_v1):
            for i, s in enumerate(sensor_list):
                for j, ss in enumerate(s.sensor_descriptor.sub_sensor_descriptor):
                    self.sensor_data_counts[(i,j)] = 0
        else:
            self.sensor_data_counts = {s: 0 for s in sensor_list}

    @staticmethod
    def get_sw_tag_classes(hsd_link, device_id):
        """
        Retrieves the software tag classes for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Software tag classes.
        """
        return hsd_link.get_sw_tag_classes(device_id)
    
    @staticmethod
    def get_hw_tag_classes(hsd_link, device_id):
        """
        Retrieves the hardware tag classes for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Hardware tag classes.
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.get_hw_tag_classes(device_id)
        else:
            return None
    
    @staticmethod
    def get_max_tags_per_acq(hsd_link, device_id):
        """
        Retrieves the maximum number of tags per acquisition for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: Maximum number of tags per acquisition.
        """
        return hsd_link.get_max_tags_per_acq(device_id)
    
    @staticmethod
    def get_updated_mlc_sensor_list(hsd_link, device_id, current_mlc_sensor_list):
        """
        Retrieves the updated list of MLC sensors for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param current_mlc_sensor_list: Current list of MLC sensors.
        :return: Updated list of MLC sensors.
        """
        if isinstance(hsd_link, HSDLink_v1):
            if device_id is not None and current_mlc_sensor_list is None:
                return hsd_link.get_sub_sensors(device_id, type_filter="MLC", only_active=False)
        else:
            if device_id is not None and current_mlc_sensor_list is None:
                return hsd_link.get_sensors(device_id, type_filter="mlc", only_active=False)
            
    @staticmethod
    def get_updated_ispu_sensor_list(hsd_link, device_id, current_ispu_sensor_list):
        """
        Retrieves the updated list of ISPU sensors for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param current_ispu_sensor_list: Current list of ISPU sensors.
        :return: Updated list of ISPU sensors.
        """
        if isinstance(hsd_link, HSDLink_v1):
            if device_id is not None and current_ispu_sensor_list is None:
                return hsd_link.get_sub_sensors(device_id, type_filter="ISPU", only_active=False)
        else:
            if device_id is not None and current_ispu_sensor_list is None:
                return hsd_link.get_sensors(device_id, type_filter="ispu", only_active=False)
        
    @staticmethod
    def get_mlc_id(hsd_link, device_id):
        """
        Retrieves the MLC ID for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: MLC ID.
        """
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
        """
        Retrieves the ISPU ID for a specific device ID.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: ISPU ID.
        """
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
        """
        Uploads the MLC UCF file to the device.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param ucf_file: Path to the MLC UCF file.
        :return: None
        """
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
            else:
                log.error("UCF file not found!")
                return None

    @staticmethod
    def upload_ispu_ucf_file(hsd_link, device_id, ucf_file):
        """
        Uploads the ISPU UCF file to the device.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param ucf_file: Path to the ISPU UCF file.
        :return: None
        """
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
                else:
                    log.error("UCF file not found!")
                    return None
    
    @staticmethod
    def set_RTC(hsd_link, device_id):
        """
        Sets the RTC time on the device.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v2):
            now = datetime.now()
            time = now.strftime("%Y%m%d_%H_%M_%S")
            message = {"log_controller*set_time":{"datetime":time}}
            hsd_link.send_command(device_id, json.dumps(message))

    @staticmethod            
    def update_device(hsd_link, device_id, device_config_file):
        """
        Updates the device configuration.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param device_config_file: Path to the device configuration file.
        :return: None
        """
        return hsd_link.update_device(device_id, device_config_file)
    
    @staticmethod
    def set_sw_tag_on_off(hsd_link, device_id, tag_id, new_tag_status):
        """
        Sets the software tag on or off.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_id: Tag ID.
        :param new_tag_status: New tag status (True for on, False for off).
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_sw_tag_on(device_id, tag_id) if new_tag_status else hsd_link.set_sw_tag_off(device_id, tag_id)
        else:
            hsd_link.set_sw_tag_on_by_id(device_id, tag_id) if new_tag_status else hsd_link.set_sw_tag_off_by_id(device_id, tag_id)
    
    @staticmethod
    def save_json_device_file(hsd_link, device_id, output_acquisition_path = None):
        """
        Saves the device configuration to a JSON file.
        - "DeviceConfig.json" for HSDv1
        - "device_config.json" for HSDv2

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param output_acquisition_path: Path to save the JSON file.
        :return: None
        """
        return hsd_link.save_json_device_file(device_id, output_acquisition_path)
    
    @staticmethod
    def save_json_acq_info_file(hsd_link, device_id, output_acquisition_path = None, manual_tags = None):
        """
        Saves the acquisition information to a JSON file.
        - "AcquisitionInfo.json" for HSDv1
        - "acquisition_info.json" for HSDv2

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param output_acquisition_path: Path to save the JSON file.
        :param manual_tags: Manual tags to include in the JSON file.
        :return: None
        """
        return hsd_link.save_json_acq_info_file(device_id, output_acquisition_path, manual_tags)
    
    def __increment_sensor_data_counter(self, size, s_id, ss_id = None):
        """
        Increments the sensor data counter.

        :param size: Size of the data.
        :param s_id: Sensor ID.
        :param ss_id: Sub-sensor ID (optional).
        :return: None
        """
        if ss_id is None and type(s_id) == str:
            self.hsd_link.sensor_data_counts[s_id] += size
        else:
            self.hsd_link.sensor_data_counts[(s_id,ss_id)] += size
    
    def get_sensor_data_counts(self, s_id, ss_id = None):
        """
        Retrieves the sensor data counts.

        :param s_id: Sensor ID.
        :param ss_id: Sub-sensor ID (optional).
        :return: Sensor data counts.
        """
        if isinstance(self, HSDLink_v1):
            return self.hsd_link.sensor_data_counts[(s_id,ss_id)]
        else:
            return self.hsd_link.sensor_data_counts[s_id]
            
    @staticmethod
    def start_sensor_acquisition_thread(hsd_link, device_id, sensor, threads_stop_flags, sensor_data_files, print_data_cnt = False):
        """
        Starts the sensor acquisition thread.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param sensor: Sensor object.
        :param threads_stop_flags: List of thread stop flags.
        :param sensor_data_files: List of sensor data files.
        :param print_data_cnt: Flag to print data count.
        :return: None
        """
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
        """
        Stops the sensor acquisition threads.

        :param threads_stop_flags: List of thread stop flags.
        :param sensor_data_files: List of sensor data files.
        :return: None
        """
        for sf in threads_stop_flags:
            sf.set()
        for f in sensor_data_files:
            f.close()
    
    @staticmethod
    def stop_log(hsd_link, device_id):
        """
        Stops the logging on the device.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: None
        """
        return hsd_link.stop_log(device_id)
        
    @staticmethod
    def start_log(hsd_link, device_id, sub_folder = True):
        """
        Starts the logging on the device.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param sub_folder: Flag to create a sub-folder for the log.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            return hsd_link.start_log(device_id)
        else:
            return hsd_link.start_log(device_id, sub_folder=sub_folder)
        
    @staticmethod
    def switch_bank(hsd_link, device_id):
        """
        Switches the bank on the device.

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.switch_bank(device_id)
        else:
            return None

    @staticmethod   
    def save_config(hsd_link, device_id):
        """
        Send a command to save the current configuration on the device SD card (if mounted).

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :return: None
        """
        return hsd_link.save_config(device_id)
   
    @staticmethod
    def set_sw_tag_on(hsd_link, device_id, tag_id):
        """
        Put a Tag [On] an ongoing acquisition (Enter into a tag region identified by the tag_id).

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_id: Tag ID.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_sw_tag_on(device_id, tag_id)
        else:
            hsd_link.set_sw_tag_on_by_id(device_id, tag_id)
    
    @staticmethod
    def set_sw_tag_off(hsd_link, device_id, tag_id):
        """
        Put a Tag [Off] an ongoing acquisition (Exit from a tag region identified by the tag_id).

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param tag_id: Tag ID.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.set_sw_tag_off(device_id, tag_id)
        else:
            hsd_link.set_sw_tag_off_by_id(device_id, tag_id)

    @staticmethod
    def refresh_hsd_link(hsd_link):
        """
        Refreshes the HSDLink instance.
        Used only for HSDv1 (no effect on HSDv2).

        :param hsd_link: Instance of HSDLink.
        :return: None
        """
        if isinstance(hsd_link, HSDLink_v1):
            hsd_link.close()
            time.sleep(3)
            hsd_link.open()

    @staticmethod
    def send_command(hsd_link, device_id:int, message):
        """
        Sends a command to a specific device ID. (HSD_v2 only)

        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param message: Command message.
        :return: Command response.

        Note: If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.send_command(device_id, message)
        else:
            return None
    
    @staticmethod
    def get_boolean_property(hsd_link, device_id: int, comp_name: str, prop_name: str):
        """
        Retrieves a boolean property value for a specific device ID. (HSD_v2 only)
        
        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param comp_name: Component name.
        :param prop_name: Property name.
        :return: Boolean property value.

        Note: If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_boolean_property(device_id, comp_name, prop_name)
        else:
            return None
    
    @staticmethod
    def get_integer_property(hsd_link, device_id: int, comp_name: str, prop_name: str):
        """
        Retrieves an integer property value for a specific device ID. (HSD_v2 only)
        
        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param comp_name: Component name.
        :param prop_name: Property name.
        :return: Integer property value.

        Note: If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_integer_property(device_id, comp_name, prop_name)
        else:
            return None

    @staticmethod
    def get_string_property(hsd_link, device_id: int, comp_name: str, prop_name: str):
        """
        Retrieves a string property value for a specific device ID. (HSD_v2 only)
        
        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param comp_name: Component name.
        :param prop_name: Property name.
        :return: String property value.

        Note: If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_string_property(device_id, comp_name, prop_name)
        else:
            return None

    @staticmethod
    def get_float_property(hsd_link, device_id: int, comp_name: str, prop_name: str):
        """
        Retrieves a float property value for a specific device ID. (HSD_v2 only)
        
        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param comp_name: Component name.
        :param prop_name: Property name.
        :return: Float property value.

        Note: If the hsd_link object is not an instance of HSDLink_v2, None is returned.
        """
        if isinstance(hsd_link, HSDLink_v2):
            return hsd_link.get_float_property(device_id, comp_name, prop_name)
        else:
            return None

    @staticmethod        
    def set_property(self, device_id: int, new_value, comp_name: str, prop_name: str, sub_prop_name: str = None):
        """
        Sets a property value for a specific device ID. (HSD_v2 only)
        
        :param hsd_link: Instance of HSDLink.
        :param device_id: Device ID.
        :param new_value: New property value.
        :param comp_name: Component name.
        :param prop_name: Property name.
        :param sub_prop_name: Sub-property name (optional).
        :return: None
        """
        if isinstance(self, HSDLink_v2):
            return self.set_property(device_id, new_value, comp_name, prop_name, sub_prop_name)
        else:
            return None
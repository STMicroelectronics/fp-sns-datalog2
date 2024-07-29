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
import enum
import numpy as np
import pandas as pd
from pandas import DataFrame
from warnings import warn
from st_hsdatalog.HSD.HSDatalog_v1 import HSDatalog_v1
from st_hsdatalog.HSD.HSDatalog_v2 import HSDatalog_v2
from st_hsdatalog.HSD.model.DeviceConfig import Device
from st_hsdatalog.HSD.utils.file_manager import FileManager
from st_hsdatalog.HSD_utils.converters import NanoedgeCSVWriter, HSDatalogConverter
from st_hsdatalog.HSD_utils.exceptions import *
import st_hsdatalog.HSD_utils.logger as logger
from st_pnpl.DTDL.dtdl_utils import ComponentTypeEnum

log = logger.get_logger(__name__)

class HSDatalog:    
    
    class HSDVersion(enum.Enum):
        V1 = 1
        V2 = 2
        INVALID = -1

    # A class-level attribute that sets the default chunk size for data samples
    DEFAULT_SAMPLES_CHUNK_SIZE = 10000000

    def create_hsd(self, acquisition_folder = None, device_config = None):
        """
        Creates an instance of the appropriate HSDatalog version based on the provided device configuration or the data in the acquisition folder.

        :param acquisition_folder: [Optional] The path to the folder where acquisition data is stored.
        :param device_config: [Optional] The configuration of the device, which can be used to determine the version of HSDatalog to instantiate.
        :return: An instance of HSDatalog_v1 or HSDatalog_v2.
        """

        # Store the acquisition folder path in the instance variable
        self.acquisition_folder = acquisition_folder
        # Initialize a flag to determine if the datalog is version 2
        self.is_datalog2 = False
        
        # If a device configuration is provided, determine the version of HSDatalog to create
        if device_config is not None:
            # If the device configuration is an instance of the Device class, create HSDatalog version 1
            if isinstance(device_config, Device):
                hsd = HSDatalog_v1()
            # If the device configuration contains a schema version starting with "2", create HSDatalog version 2
            elif "schema_version" in device_config and device_config["schema_version"].split('.')[0] == "2":
                hsd = HSDatalog_v2()
            # Return the created HSDatalog instance
            return hsd
        
        # If no device configuration is provided, attempt to create HSDatalog version 1 using the acquisition folder
        try:
            hsd = HSDatalog_v1(self.acquisition_folder)
        except AcquisitionFormatError:
            # If an AcquisitionFormatError is raised, it indicates the data format is not compatible with version 1
            # Set the flag to indicate that the datalog is version 2
            self.is_datalog2 = True

        # If the datalog is determined to be version 2, create HSDatalog version 2 using the acquisition folder
        if self.is_datalog2:
            hsd = HSDatalog_v2(self.acquisition_folder)
        # Return the created HSDatalog instance
        return hsd
    
    @staticmethod
    def validate_hsd_folder(acquisition_folder) -> HSDVersion:
        """
        Validates the HSD folder by checking for specific files and returns the HSD version.

        :param acquisition_folder (str): The path to the acquisition folder.
        :return HSDVersion: The HSD version based on the files found in the acquisition folder.
        """
        # If an acquisition folder is provided, proceed with initialization.
        if acquisition_folder is not None:
            # Attempt to find and load the device configuration from the acquisition folder.
            device_json_file_path = FileManager.find_file("device_config.json", acquisition_folder)
            if device_json_file_path is None:
                device_json_file_path = FileManager.find_file("DeviceConfig.json", acquisition_folder)
                if device_json_file_path is not None:
                    acquisition_json_file_path = FileManager.find_file("AcquisitionInfo.json", acquisition_folder)
                    if acquisition_json_file_path is not None:
                        return HSDatalog.HSDVersion.V1
            else:
                acquisition_json_file_path = FileManager.find_file("acquisition_info.json", acquisition_folder)
                if acquisition_json_file_path is not None:
                    return HSDatalog.HSDVersion.V2
        return HSDatalog.HSDVersion.INVALID

    @staticmethod
    def present_sensor_list(hsd, sensor_list = None):
        """
        Presents a list of sensors to the user interface, typically through the command line interface (CLI).

        This static method delegates the task of presenting the sensor list to an instance method of the same name.

        :param hsd: An instance of the class that contains the present_sensor_list instance method.
        :param sensor_list: [Optional] A list of sensors to be presented. If None, the method retrieves the sensor
        list using the instance's get_sensor_list method.
        """

        # Call the instance method 'present_sensor_list' on the 'hsd' object, passing the 'sensor_list' parameter.
        # This will handle the actual presentation of the sensor list.
        hsd.present_sensor_list(sensor_list)
    
    @staticmethod
    def present_device_info(hsd, device_info = None):
        """
        Presents the device information to the user interface, typically through the command line interface (CLI).

        This static method delegates the task of presenting the device information to an instance method of the same name.

        :param hsd: An instance of the HSDatalog class that contains the present_device_info instance method.
        :param device_info: [Optional] The device information to be presented. If None, the instance method will retrieve the device information.
        """

        # Call the instance method 'present_device_info' on the 'hsd' object, passing the 'device_info' parameter.
        # This will handle the actual presentation of the device information.
        hsd.present_device_info(device_info)
    
    @staticmethod
    def get_sw_tag_classes(hsd):
        """
        Retrieves the software tag classes from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :return: A list or dictionary of software tag classes.
        """
        # Directly return the software tag classes using the corresponding method from the 'hsd' instance.
        return hsd.get_sw_tag_classes()

    @staticmethod
    def present_sw_tag_classes(hsd, sw_tag_classes = None):
        """
        Presents the software tag classes to the user interface, typically through the command line interface (CLI).

        This static method delegates the task of presenting the software tag classes to an instance method of the same name.

        :param hsd: An instance of the HSDatalog class that contains the present_sw_tag_classes instance method.
        :param sw_tag_classes: [Optional] The software tag classes to be presented. If None, the instance method will retrieve the tag
        list using the instance's get_sensor_list method.
        """
        # Call the instance method 'present_sw_tag_classes' on the 'hsd' object, passing the 'sw_tag_classes' parameter.
        # This will handle the actual presentation of the software tag classes.
        hsd.present_sw_tag_classes(sw_tag_classes)
    
    @staticmethod
    def get_hw_tag_classes(hsd):
        """
        Retrieves the hardware tag classes from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :return: A list or dictionary of hardware tag classes.
        """
        # Directly return the hardware tag classes using the corresponding method from the 'hsd' instance.
        return hsd.get_hw_tag_classes()

    @staticmethod
    def present_hw_tag_classes(hsd, hw_tag_classes = None):
        """
        Presents the hardware tag classes to the user interface, typically through the command line interface (CLI).

        This static method delegates the task of presenting the hardware tag classes to an instance method of the same name.

        :param hsd: An instance of the HSDatalog class that contains the present_hw_tag_classes instance method.
        :param hw_tag_classes: [Optional] The hardware tag classes to be presented. If None, the instance method will retrieve the tag
        list using the instance's get_sensor_list method.
        """
        # Call the instance method 'present_hw_tag_classes' on the 'hsd' object, passing the 'hw_tag_classes' parameter.
        # This will handle the actual presentation of the hardware tag classes.
        hsd.present_hw_tag_classes(hw_tag_classes)

    @staticmethod
    def present_sensor(hsd, sensor):
        """
        Presents information about a specific sensor to the user interface, typically through the command line interface (CLI).
        
        This static method delegates the task of presenting the sensor information to an instance method of the same name.

        :param hsd: An instance of the HSDatalog class that contains the present_sensor instance method.
        :param sensor: The sensor information to be presented.
        """
        # Call the instance method 'present_sensor' on the 'hsd' object, passing the 'sensor' parameter.
        # This will handle the actual presentation of the sensor information.
        hsd.present_sensor(sensor)

    @staticmethod
    def get_dat_file_list(hsd):
        """
        Retrieves a list of .dat files from the acquisition folder associated with the HSDatalog instance.

        This static method delegates the task of retrieving the .dat file list to an instance method of the same name.

        :param hsd: An instance of the HSDatalog class that contains the get_dat_file_list instance method.
        :return: A list of .dat file paths.
        """
        # Call the instance method 'get_dat_file_list' on the 'hsd' object.
        # This will return the list of .dat file paths from the acquisition folder.
        return hsd.get_dat_file_list()
    
    @staticmethod
    def filter_sensor_list_by_name(hsd, sensor_name, sensor_list = None):
        """
        Filters a list of sensors by name, returning only those that contain the specified sensor name.

        If no sensor list is provided, the method retrieves all components using the get_all_components method.
        The filtering is case-insensitive.

        :param hsd: An instance of the HSDatalog class
        :param sensor_name: The name of the sensor to filter by. The method will return sensors that contain this name.
        :param sensor_list: [Optional] A list of sensor dictionaries to be filtered. If None, the method retrieves all components.
        :return: A list of sensor dictionaries that contain the specified sensor name.
        """
        # Initialize an empty list to store sensors that match the filter criteria
        ssensor = []
        # If no sensor list is provided, retrieve all components
        if sensor_list is None:
            sensor_list = HSDatalog.get_all_components(hsd, only_active=False)
        # Iterate over each sensor in the provided or retrieved sensor list
        for s in sensor_list:
            # Check if the sensor name (converted to lowercase) is in the name of the current sensor (also converted to lowercase)
            # The sensor's name is assumed to be the key in the sensor dictionary
            if sensor_name.lower() in list(s.keys())[0]:
                # If the sensor name matches, add the sensor to the list of filtered sensors
                ssensor.append(s)
        # Return the list of sensors that matched the filter criteria
        return ssensor
    
    @staticmethod
    def set_device(hsd, device, device_id = 0, from_file = True):
        """
        Sets the device for the given HSDatalog instance.

        This static method delegates the task of setting the device to the instance method 'set_device' of the HSDatalog class.

        :param hsd: An instance of the HSDatalog class that contains the set_device instance method.
        :param device: The device object to be set for the HSDatalog instance.
        :param device_id: The ID of the device. Default is 0.
        :param from_file: A boolean indicating whether to load the device from a file. Default is True.
        """
        # Call the instance method 'set_device' on the 'hsd' object, passing the 'device' parameter.
        # This will set the device for the HSDatalog instance.
        hsd.set_device(device, device_id, from_file)
        
    @staticmethod
    def get_device(hsd):
        """
        Retrieves the current device from the given HSDatalog instance.

        This static method delegates the task of retrieving the device to the instance method 'get_device' of the HSDatalog class.

        :param hsd: An instance of the HSDatalog class that contains the get_device instance method.
        :return: The current device object of the HSDatalog instance.
        """
        # Call the instance method 'get_device' on the 'hsd' object.
        # This will return the current device of the HSDatalog instance.
        return hsd.get_device()
    
    def get_acquisition_path(hsd):
        return hsd.get_acquisition_path()
    
    @staticmethod
    def get_acquisition_info(hsd):
        """
        Retrieves the current acquisition_info from the given HSDatalog instance.

        This static method delegates the task of retrieving the acquisition_info to the instance method 'get_acquisition_info' of the HSDatalog class.

        :param hsd: An instance of the HSDatalog class that contains the get_acquisition_info instance method.
        :return: The current acquisition_info object of the HSDatalog instance.
        """
        # Call the instance method 'get_acquisition_info' on the 'hsd' object.
        # This will return the current acquisition_info of the HSDatalog instance.
        return hsd.get_acquisition_info()
    
    @staticmethod
    def get_firmware_info(hsd):
        return hsd.get_firmware_info()
    
    @staticmethod
    def get_device_info(hsd):
        """
        Retrieves the device information from the given HSDatalog instance.

        This static method delegates the task of retrieving the device information to the instance method 'get_device_info' of the HSDatalog class.

        :param hsd: An instance of the HSDatalog class that contains the get_device_info instance method.
        :return: The device information of the HSDatalog instance.
        """
        # Call the instance method 'get_device_info' on the 'hsd' object.
        # This will return the device information of the HSDatalog instance.
        return hsd.get_device_info()
    
    @staticmethod
    def ask_for_component(hsd, only_active):
        """
        Asks the user to select a component from a list that may include sensors, algorithms, and actuators, depending on the version of HSDatalog.

        :param hsd: An instance of the HSDatalog class or its subclass, which provides methods to retrieve lists of components and to prompt the user for selection.
        :param only_active: A boolean indicating whether to retrieve only active components (sensors, algorithms, actuators).
        :return: The selected component after the user has made a choice through the CLI.
        """
        # Retrieve the list of sensors, potentially filtering for only active ones
        s_list = hsd.get_sensor_list(only_active = only_active)
        
        # Check if the 'hsd' instance is of type HSDatalog_v2, which supports additional component types
        if isinstance(hsd, HSDatalog_v2):
            # Retrieve lists of algorithms and actuators, potentially filtering for only active ones
            a_list = hsd.get_algorithm_list(only_active = only_active)
            ac_list = hsd.get_actuator_list(only_active = only_active)
            # Combine the lists of sensors, algorithms, and actuators into a single list for selection
            combined_list = s_list + a_list + ac_list
            # Prompt the user to select a component from the combined list using the CLI
            component = hsd.prompt_component_select_CLI(combined_list)
        else:
            # If 'hsd' is not an instance of HSDatalog_v2, only sensors are available for selection
            # Prompt the user to select a sensor from the list using the CLI
            component = hsd.prompt_component_select_CLI(s_list)
        # Return the component selected by the user
        return component

    @staticmethod
    def get_sensor_list(hsd, type_filter = "", only_active = False):
        """
        Retrieves a list of sensors from the HSDatalog instance, with optional filtering by type and activity status.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param type_filter: A string to filter sensors by type.
        :param only_active: A boolean indicating whether to retrieve only active sensors.
        :return: A list of sensors that match the filter criteria.
        """
        # Call the 'get_sensor_list' method on the 'hsd' instance, passing the filter criteria.
        # This will return a list of sensors that match the type filter and activity status.
        return hsd.get_sensor_list(type_filter = type_filter, only_active = only_active)
    
    @staticmethod
    def get_sensor(hsd, sensor_name):
        """
        Retrieves a specific sensor by name from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor_name: The name of the sensor to retrieve.
        :return: The sensor object that matches the given name.
        """
        # Retrieve the sensor by name from the 'hsd' instance.
        # This method assumes that both HSDatalog_v2 and other versions have a 'get_sensor' method.
        sensor = hsd.get_sensor(sensor_name)
        # Return the retrieved sensor
        return sensor
    
    @staticmethod
    def get_sensor_name(hsd, sensor):
        """
        Retrieves the name of a sensor from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor: The sensor object or dictionary from which to extract the name.
        :return: The name of the sensor as a string.
        """
        # If it is, the sensor is expected to be a dictionary where the key is the sensor name.
        # The method returns the first key from the sensor dictionary, which is the sensor name.
        return list(sensor.keys())[0]
    
    @staticmethod
    def get_sensor_sensitivity(hsd, sensor):
        """
        Retrieves the sensitivity of a sensor from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor: The sensor object or dictionary from which to extract the sensitivity.
        :return: The sensitivity value, a list of sensitivity values, or None if not found.
        """
        # Retrieve the sensor name using the 'get_sensor_name' method.
        s_name = HSDatalog.get_sensor_name(hsd, sensor)
        # Access the sensor's sensitivity using the sensor name as the key.
        return sensor[s_name].get("sensitivity", 1)
    
    @staticmethod
    def get_sensor_sensitivity_by_name(hsd, sensor_name):
        """
        Retrieves the ODR of a sensor by its name from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor_name: The name of the sensor for which to retrieve the ODR.
        :return: The ODR value(s) for the sensor with the given name.
        """
        sensor = HSDatalog.get_sensor(hsd, sensor_name)
        return HSDatalog.get_sensor_sensitivity(hsd, sensor)
            
    @staticmethod
    def get_sensor_odr(hsd, sensor):
        """
        Retrieves the Output Data Rate (ODR) of a sensor from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor: The sensor object or dictionary from which to extract the ODR.
        :return: The ODR value, a list of ODR values, or None if not found.
        """
        # Retrieve the sensor name using the 'get_sensor_name' method.
        s_name = HSDatalog.get_sensor_name(hsd, sensor)
        # Access the sensor's ODR using the sensor name as the key.
        return sensor[s_name].get("odr")
    
    @staticmethod
    def get_sensor_odr_by_name(hsd, sensor_name):
        """
        Retrieves the ODR of a sensor by its name from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor_name: The name of the sensor for which to retrieve the ODR.
        :return: The ODR value(s) for the sensor with the given name.
        """
        # Retrieve the sensor object or dictionary using the sensor name.
        sensor = HSDatalog.get_sensor(hsd, sensor_name)
        # Retrieve and return the ODR of the sensor by calling 'get_sensor_odr'.
        return HSDatalog.get_sensor_odr(hsd, sensor)
    
    @staticmethod
    def get_sensor_measodr(hsd, sensor):
        """
        Retrieves the measured ODR (measODR) of a sensor from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor: The sensor object or dictionary from which to extract the measODR.
        :return: The measODR value, a list of measODR values, or None if not found.
        """
        # Retrieve the sensor name using the 'get_sensor_name' method.
        s_name = HSDatalog.get_sensor_name(hsd, sensor)
        # Access the sensor's measured ODR using the sensor name as the key.
        return sensor[s_name].get("measodr")
    
    @staticmethod
    def get_sensor_measodr_by_name(hsd, sensor_name):
        """
        Retrieves the measured ODR of a sensor by its name from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor_name: The name of the sensor for which to retrieve the measured ODR.
        :return: The measured ODR value(s) for the sensor with the given name.
        """
        # Retrieve the sensor object or dictionary using the sensor name.
        sensor = HSDatalog.get_sensor(hsd, sensor_name)
        # Retrieve and return the measured ODR of the sensor by calling 'get_sensor_measodr'.
        return HSDatalog.get_sensor_measodr(hsd, sensor)
              
    @staticmethod
    def get_sensor_fs(hsd, sensor):
        """
        Retrieves the Full Scale (FS) range of a sensor from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor: The sensor object or dictionary from which to extract the FS range.
        :return: The FS value, a list of FS values, or None if not found.
        """
        # Retrieve the sensor name using the 'get_sensor_name' method.
        s_name = HSDatalog.get_sensor_name(hsd, sensor)
        # Access the sensor's FS using the sensor name as the key.
        return sensor[s_name].get("fs")
        
    @staticmethod
    def get_sensor_fs_by_name(hsd, sensor_name):
        """
        Retrieves the FS range of a sensor by its name from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor_name: The name of the sensor for which to retrieve the FS range.
        :return: The FS range value(s) for the sensor with the given name.
        """
        # Retrieve the sensor object or dictionary using the sensor name.
        sensor = HSDatalog.get_sensor(hsd, sensor_name)
        # Retrieve and return the FS range of the sensor by calling 'get_sensor_fs'.
        return HSDatalog.get_sensor_fs(hsd, sensor)

    @staticmethod
    def get_sensor_spts(hsd, sensor):
        """
        Retrieves the number of samples per timestamp (SPTS) of a sensor from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor: The sensor object or dictionary from which to extract the SPTS.
        :return: The SPTS value, a list of SPTS values, or None if not found.
        """
        # Retrieve the sensor name using the 'get_sensor_name' method.
        s_name = HSDatalog.get_sensor_name(hsd, sensor)
        # Access the sensor's SPTS using the sensor name as the key.
        spts = sensor[s_name].get("samples_per_ts")
        # Check if the SPTS is an integer or a dictionary.
        if isinstance(spts, int):
            return spts
        else:
            # If multiple SPTS values are found, return the list of SPTS.
            return spts.get("val")
            
    @staticmethod
    def get_sensor_spts_by_name(hsd, sensor_name):
        """
        Retrieves the SPTS of a sensor by its name from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param sensor_name: The name of the sensor for which to retrieve the SPTS.
        :return: The SPTS value(s) for the sensor with the given name.
        """
        # Retrieve the sensor object or dictionary using the sensor name.
        sensor = HSDatalog.get_sensor(hsd, sensor_name)
        # Retrieve and return the SPTS of the sensor by calling 'get_sensor_spts'.
        return HSDatalog.get_sensor_spts(hsd, sensor)
    
    @staticmethod
    def get_all_components(hsd, only_active=True):
        """
        Retrieves a combined list of sensors, algorithms, and actuators from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param only_active: A boolean indicating whether to retrieve only active components.
        :return: A list of all components (sensors, algorithms, actuators) if HSDatalog_v2, otherwise just sensors.
        """
        # Retrieve the list of sensors, with an option to filter only active ones.
        s_list = HSDatalog.get_sensor_list(hsd, only_active = only_active)
        # Initialize lists for algorithms and actuators.
        a_list = []
        ac_list = []
        # Check if the 'hsd' instance is of the HSDatalog_v2 class.
        if isinstance(hsd, HSDatalog_v2):
            # If it is, retrieve lists of active algorithms and actuators.
            a_list = hsd.get_algorithm_list(only_active = only_active)
            ac_list = hsd.get_actuator_list(only_active = only_active)
            # Combine and return the lists of sensors, algorithms, and actuators.
            return s_list + a_list + ac_list
        else:
            # If 'hsd' is not an instance of HSDatalog_v2, return only the list of sensors.
            return s_list
    
    @staticmethod
    def get_component(hsd, component_name):
        """
        Retrieves a specific component by name from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param component_name: The name of the component to retrieve.
        :return: The component object that matches the given name.
        """
        # Check if the 'hsd' instance is of the HSDatalog_v2 class.
        if isinstance(hsd, HSDatalog_v2):
            # If it is, use the 'get_component' method to retrieve the component by name.
            return hsd.get_component(component_name)
        else:
            # If 'hsd' is not an instance of HSDatalog_v2, use the 'get_sensor' method to retrieve the sensor by name.
            # This assumes that non-v2 instances only have sensors as components.
            return hsd.get_sensor(component_name)
        
    @staticmethod
    def get_component_name(hsd, component):
        """
        Retrieves the name of a component from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param component: The component object or dictionary from which to extract the name.
        :return: The name of the component as a string.
        """
        # If it is, the component is expected to be a dictionary where the key is the component name.
        # The method returns the first key from the component dictionary, which is the component name.
        return list(component.keys())[0]
    
    @staticmethod
    def get_acquisition_label_classes(hsd):
        """
        Retrieves the acquisition label classes from the HSDatalog instance.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :return: A list or dictionary of acquisition label classes.
        """
        # Directly return the acquisition label classes using the corresponding method from the 'hsd' instance.
        return hsd.get_acquisition_label_classes()

    @staticmethod
    def reset_status_conversion_side_info(comp_status, ioffset):
        """
        Resets the conversion side information of a component status.

        :param comp_status: The status dictionary of the component to be reset.
        :param ioffset: The initial offset value to set in the component status.
        """
        # Set the initial offset value in the component status.
        comp_status["ioffset"] = ioffset
        # Remove keys related to conversion side information if they exist.
        comp_status.pop("missing_bytes", None)
        comp_status.pop("saved_bytes", None)
        comp_status.pop("last_index", None)
        comp_status.pop("is_first_chunk", None)

    @staticmethod
    def __compute_chunk_time_size(chunk_size:int, comp_status:dict) -> float: 
        """
        Calculate the time duration for a data chunk based on its size and the output data rate.

        Args:
        chunk_size (int): The size of the data chunk.
        comp_status (dict): A dictionary containing the status of the component, which includes the output data rate ('odr').

        Returns:
        float: The time duration in seconds for the given data chunk.
        """
        # Retrieve the output data rate (odr) from the component status, defaulting to 1 if not present.
        odr = comp_status.get("measodr", comp_status.get("odr",1))
        # Calculate the time duration of each chunk based on the chunk size and odr.
        return chunk_size/odr # seconds

    @staticmethod
    def __create_group_keys(df:DataFrame, column_name:str) -> DataFrame:
        """
        Add a group key to a DataFrame to identify consecutive rows with the same value in a specified column.

        Args:
        df (pandas.DataFrame): The DataFrame to process.
        column_name (str): The name of the column to analyze for consecutive values.

        Returns:
        pandas.DataFrame: The DataFrame with an additional column for group keys.
        """

        # Compare each row with the previous row to determine if the value has changed.
        # The result is a Series of True/False, where True indicates a change in value.
        value_changed = df[column_name].ne(df[column_name].shift(1)) # ne() -> "not equal" -> compare the original column value with its shifted version.
        
        # Use cumsum() on the boolean Series to create a group key.
        # cumsum() treats True as 1 and False as 0, so it increments the sum (and thus the group key)
        # each time it encounters a True (i.e., a change in value).
        df[f'{column_name}_group_key'] = value_changed.cumsum()
        return df

    @staticmethod
    def __save_tag_group_in_file(comp_name:str, group_df:DataFrame, tag_label:str, tag_info:dict, tags_columns:list, output_folder:str, out_format:str, with_times:bool, columns_labels = "default") -> None:
        """
        Save a group of tagged data to a file, creating a new file or appending data to an existing one as
        necessary based on tagging information contained in "tag_info" input dict.
        "tags_info" will be automatically updated wwithin this function

        Args:
        comp_name (str): The name of the component.
        group_df (pandas.DataFrame): The DataFrame containing the group of data to save.
        tag_label (str): The label of the tag.
        tag_info (dict): A dictionary to contain tagging information (used to maintain information about last chunk if needed).
        tags_columns (list): A list of columns related to tags (All tag classes labels in acquisition).
        output_folder (str): The folder path where the output files will be saved.
        out_format (str): The output format of the file [CSV,TSV,TXT].
        with_times (bool): A flag indicating whether to include time information in saved file.
        columns_labels: [Optional] Column labels for the output file. Default is "default".
        
        Returns:
        None
        """
        tag_batch_first_time = group_df.iloc[0, 0]
        tag_batch_last_time = group_df.iloc[-1, 0]
        tag_batch_sample_time = (tag_batch_last_time - tag_batch_first_time) / len(group_df)
        
        if tag_info[tag_label]["last_timestamp"] != 0:
            exp_next_time = round((tag_info[tag_label]["last_timestamp"] + tag_batch_sample_time), 6)

            if tag_batch_first_time > exp_next_time + 0.5:  # next tag group --> new file (with 0.5 seconds of tolerance)
                tag_info[tag_label]["tag_group_id"] += 1
                tag_info[tag_label]["first_chunk"] = True

        tag_info[tag_label]["last_timestamp"] = tag_batch_last_time
        
        # Drop tag columns and group key columns
        group_df = group_df.drop([f"{label}" for label in tags_columns], axis=1)
        group_df = group_df.drop([f"{label}_group_key" for label in tags_columns], axis=1)

        # File handling
        tag_sub_folder = os.path.join(output_folder, tag_label)
        os.makedirs(tag_sub_folder, exist_ok=True)
        label_file_name = f"{tag_label}_{comp_name}_dataLog_{tag_info[tag_label]['tag_group_id']}"
        label_file_path = os.path.join(tag_sub_folder, label_file_name)
        
        # Determine the file mode ('write' for the first chunk, 'append' for subsequent chunks).
        file_mode = 'w' if tag_info[tag_label]["first_chunk"] else 'a'
        tag_info[tag_label]["first_chunk"] = False
    
        # Save the group DataFrame
        HSDatalogConverter.to_unico(label_file_path, group_df, out_format, file_mode, with_times, columns_labels)
    
    @staticmethod
    def get_time_tags(hsd, which_tags = None):
        """
        Retrieves time tags from the HSDatalog instance, with an optional filter.

        Time tags are markers that relate to specific points in time within the data acquisition.
        This method can retrieve all time tags or a subset based on the filter provided.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param which_tags: Optional parameter to specify which time tags to retrieve. If None, all time tags are retrieved.
        :return: A list or dictionary of time tags, filtered by the 'which_tags' parameter if provided.
        """
        # Directly return the time tags using the corresponding method from the 'hsd' instance.
        # The 'which_tags' parameter allows for optional filtering of the time tags to be retrieved.
        return hsd.get_time_tags(which_tags)

    @staticmethod
    def get_data_and_timestamps(hsd, component, start_time = 0, end_time = -1, raw_data = False, chunk_size=DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Retrieves data and timestamps for a given component within a specified time range.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param component: A dictionary containing the component's name and status.
        :param start_time: The start time for data retrieval (the closest greater timestamp will be selected).
        :param end_time: The end time for data retrieval (the closest greater timestamp will be selected). If -1, data is retrieved until the end.
        :param raw_data: A boolean indicating whether to retrieve raw data.
        :param chunk_size: The size of the data chunks to retrieve.
        :return: A list of data and timestamps.
        :raises MemoryError: If a memory error occurs during the retrieval of data and timestamps.
        """
        try:
            # Initialize an empty list to store data and timestamps.
            d_and_t = []

            # Extract the component name and status from the 'component' dictionary.
            # The 'component' dictionary is expected to have only one key-value pair.
            c_name = list(component.keys())[0]
            c_status = component[c_name]

            # Retrieve data and timestamps in batches using the private method '__get_data_and_timestamps_batch'.
            d_and_t = HSDatalog.__get_data_and_timestamps_batch(hsd, c_name, c_status, start_time, end_time, raw_data, chunk_size)
            return d_and_t
        except MemoryError as e:
            # Handle the MemoryError by logging it, raising an error, or taking other appropriate action.
            print(f"MemoryError encountered while retrieving data and timestamps for component {c_name}.\
                    Try to extract data and timestamps in batches by calling this function on smaller subsequent time intervals,\
                    varying the start_time and end_time parameters.: {e}")
            raise

    @staticmethod
    def get_data_and_timestamps_by_name(hsd, comp_name, start_time=0, end_time=-1, raw_data=False, chunk_size=DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Retrieves data and timestamps for a given component within a specified time range by its name.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param comp_name: The name of the component from which to retrieve data and timestamps.
        :param start_time: The start of the time range for which to retrieve data (inclusive).
        :param end_time: The end of the time range for which to retrieve data (exclusive). If -1, retrieves until the latest data.
        :param raw_data: A boolean indicating whether to retrieve raw data or processed data.
        :param chunk_size: The size of each data chunk to retrieve at a time. Defaults to DEFAULT_SAMPLES_CHUNK_SIZE.
        :return: A list of data and timestamps.
        """
        # Retrieve the component object or dictionary using the component name.
        component = HSDatalog.get_component(hsd, comp_name)
        # Retrieve data and timestamps in batches using the HSDatalog 'get_data_and_timestamps' method.
        # The method will handle the specified time range, data type, and chunk size.
        return HSDatalog.get_data_and_timestamps(hsd, component, start_time, end_time, raw_data, chunk_size)
    
    @staticmethod
    def __get_data_and_timestamps_batch(hsd, comp_name, comp_status, start_time = 0, end_time = -1, raw_data = False, chunk_size=DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Retrieves data and timestamps for a given component in batches within a specified time range.

        This is a private helper method that is called by 'get_data_and_timestamps' to handle batch retrieval.

        :param hsd: An instance of the HSDatalog class or its subclass.
        :param comp_name: The name of the component.
        :param comp_status: The status dictionary of the component.
        :param start_time: The start time for data retrieval (the closest greater timestamp will be selected).
        :param end_time: The end time for data retrieval (the closest greater timestamp will be selected). If -1, data is retrieved until the end.
        :param raw_data: A boolean indicating whether to retrieve raw data.
        :param chunk_size: The size of the data chunks to retrieve.
        :return: A list of data and timestamps in batches.
        """
        # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
        chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)

        # Initialize the flag to determine if the current chunk is the last one
        is_last_chunk = False
        # Set the initial time offset to the start time or 0 if not provided.
        time_offset = start_time or 0
        
        log.info(f"--> {comp_name} Data and Timestamps extraction started...")
        
        # Initialize an empty list to store data and timestamps for each batch.
        d_and_t = []
        
        # Retrieve the initial offset from the component status or use 0 if not provided.
        ioffset = comp_status.get('ioffset', 0)
        # Set the start and end times for the first batch.
        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        # Mark the first batch.
        comp_status["is_first_chunk"] = True
        while not is_last_chunk:
            # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
            if end_time != -1 and next_end_time > end_time:
                #read exactly the missing samples up to end_time
                next_end_time = end_time
            
            # Retrieve the dataframe for the current chunk.
            data_time = hsd.get_data_and_timestamps_batch(comp_name, comp_status, next_start_time, next_end_time, raw_data)
            
            if comp_status["c_type"] == ComponentTypeEnum.ACTUATOR.value or comp_status["c_type"] == ComponentTypeEnum.ALGORITHM: #NO BATCHES FOR ACTUATORS AND ALGORITHMS
                is_last_chunk = True
                log.info("--> Conversion completed")
            
            if data_time is not None:
                # Check if the retrieved data is empty.
                if len(data_time[0]) == 0:
                    # If the dataframe is empty, mark the last chunk and log completion.
                    is_last_chunk = True
                    log.info(f"--> {comp_name} Data and Timestamps extraction completed")
                else:
                    # Log the completion of the current chunk and append the dataframe to the list.
                    log.info(f"--> {comp_name} Data and Timestamps Chunk extraction completed")
                    d_and_t.append(data_time)
                    # Update the start time for the next chunk to the timestamp of the last sample in the current chunk.
                    next_start_time = data_time[1][-1] + chunk_time_size
                    # Update the end time for the next chunk.
                    next_end_time = next_start_time + chunk_time_size
            else:
                # If no data is returned, mark the last chunk and log completion.
                is_last_chunk = True
                log.info(f"--> {comp_name} Data and Timestamps extraction completed")

        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)
        # Return the list of dataframes.
        return d_and_t

    @staticmethod
    def get_dataframe(hsd, component, start_time = 0, end_time = -1, labeled = False, raw_data = False, which_tags:list = [], chunk_size=DEFAULT_SAMPLES_CHUNK_SIZE) -> list: 
        """
        Retrieves data as a list of dataframes for a given component within a specified time range.

        :param hsd: An instance of HSDatalog.
        :param component: A dictionary where the key is the component name and the value is its status.
        :param start_time: The start time for the data retrieval (the closest greater timestamp will be selected).
        :param end_time: The end time for the data retrieval (the closest greater timestamp will be selected).
        :param labeled: Boolean to choose whether the output should contain information about labels (Input data must be labelled).
        :param raw_data: Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param which_tags: [Optional] List of tags to filter the data.
        :param chunk_size: [Optional] The size of the data chunk (in samples) to be processed at a time. Default value = HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples
        :return: A list containing the dataframes of the retrieved data.
        :raises MemoryError: If a memory error occurs during the retrieval of data and timestamps.
        """
        try:
            # Initialize an empty list to store the dataframes that will be retrieved.
            df = []

            # Extract the component name and status from the 'component' dictionary.
            # The 'component' dictionary is expected to have only one key-value pair.
            c_name = list(component.keys())[0]
            c_status = component[c_name]

            # Call the private method '__get_dataframe_batch' to retrieve data in batches.
            # This method is expected to return a list of dataframes containing the data for the specified component.
            df = HSDatalog.__get_dataframe_batch(hsd, c_name, c_status, start_time, end_time, labeled, raw_data, which_tags, chunk_size)
            return df
        except MemoryError as e:
            # Handle the MemoryError by logging it, raising an error, or taking other appropriate action.
            print(f"MemoryError encountered while retrieving dataframes for component {c_name}.\
                    Try to extract dataframes in batches by calling this function on smaller subsequent time intervals,\
                    varying the start_time and end_time parameters.: {e}")
            raise

    @staticmethod
    def __get_dataframe_batch(hsd, comp_name, comp_status, start_time = 0, end_time = -1, labeled = False, raw_data = False, which_tags:list = [], chunk_size=DEFAULT_SAMPLES_CHUNK_SIZE) -> list:   
        """
        Retrieves data in batches as dataframes for a given component within a specified time range.

        :param hsd: An instance of HSDatalog_v2.
        :param comp_name: The name of the component.
        :param comp_status: A dictionary containing the status of the component.
        :param start_time: The start time for the data retrieval (the closest greater timestamp will be selected).
        :param end_time: The end time for the data retrieval (the closest greater timestamp will be selected).
        :param labeled: Boolean to choose whether the output should contain information about labels (Input data must be labelled).
        :param raw_data: Boolean to get raw data output (not multiplied by sensitivity).
        :param chunk_size: [Optional] The size of the data chunk (in samples) to be processed at a time. Default value = {HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE}
        :return: A list containing the dataframes of the retrieved data.
        """
        
        # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
        chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)

        # Initialize the flag to determine if the current chunk is the last one
        is_last_chunk = False
        # Set the initial time offset to the start time or 0 if not provided.
        time_offset = start_time or 0
        
        log.info(f"--> {comp_name} Conversion started...")
        
        # Initialize an empty list to store dataframes.
        dataframes = []
        # data = None
        
        ioffset = comp_status.get('ioffset', 0)
        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        comp_status["is_first_chunk"] = True
        while not is_last_chunk:
            # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
            if end_time != -1 and next_end_time > end_time:
                #read exactly the missing samples up to end_time
                next_end_time = end_time
                is_last_chunk = True
            
            # Retrieve the dataframe for the current chunk.
            dataframe = hsd.get_dataframe_batch(comp_name, comp_status, next_start_time, next_end_time, labeled, raw_data, which_tags)
            
            if start_time == next_start_time:
                # Trim the DataFrame if specific start_time is selected
                dataframe = dataframe[(dataframe['Time'] >= next_start_time)]

            if end_time == next_end_time:
                if dataframe is not None:
                    # Trim the DataFrame if specific end_time is selected
                    dataframe = dataframe[(dataframe['Time'] <= next_end_time)]

            # After the first chunk, update the status to no longer be the first chunk.
            if comp_status["is_first_chunk"]:
                comp_status["is_first_chunk"] = False

            if comp_status["c_type"] == ComponentTypeEnum.ACTUATOR.value or comp_status["c_type"] == ComponentTypeEnum.ALGORITHM: #NO BATCHES FOR ACTUATORS AND ALGORITHMS
                is_last_chunk = True
                log.info("--> Conversion completed")
            
            if dataframe is not None:
                # Print the dataframe for debugging purposes (optional).
                # print(f"{dataframe}\n")
                if len(dataframe) == 0:
                    # If the dataframe is empty, mark the last chunk and log completion.
                    is_last_chunk = True
                    log.info(f"--> {comp_name} Conversion completed")
                    print()
                else:
                    # Log the completion of the current chunk and append the dataframe to the list.
                    log.info(f"--> {comp_name} Chunk Conversion completed")
                    dataframes.append(dataframe)
                    # Increment the time offset for the next chunk.
                    # time_offset += chunk_time_size
                    next_start_time = dataframe.iloc[-1,0]
                    next_end_time = next_start_time + chunk_time_size
            else:
                # If no dataframe is returned, mark the last chunk.
                is_last_chunk = True
                log.info(f"--> {comp_name} Conversion completed")
        
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)
        # Return the list of dataframes.
        return dataframes
    
    @staticmethod
    def __check_data_batch(hsd, comp_name, comp_status, start_time, end_time, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Checks data for continuity and integrity in batches within a specified time range.

        :param comp_name: The name of the component.
        :param comp_status: A dictionary containing the status of the component.
        :param start_time: The start time for the data check (the closest greater timestamp will be selected).
        :param end_time: The end time for the data check (the closest greater timestamp will be selected).
        :param chunk_size: [Optional] The size of the data chunk (in samples) to be processed at a time. Default value = {HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE}
        """

        # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
        chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)

        # Initialize the flag to determine if the current chunk is the first one.
        is_first_chunk = True
        # Initialize the flag to determine if the current chunk is the last one.
        is_last_chunk = False
        # Set the initial time offset to the start time or 0 if not provided.
        time_offset = start_time or 0
        chunk_with_errors = 0
        last_data = 0

        log.info(f"--> {comp_name} DUMMY Data check started...")
        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        # Mark the first chunk to handle any initial conditions.
        comp_status["is_first_chunk"] = True
        while not is_last_chunk:
            # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
            if end_time != -1 and next_end_time > end_time:
                #read exactly the missing samples up to end_time
                next_end_time = end_time
            
            # Retrieve data and timestamps for the current chunk.
            res = hsd.get_data_and_timestamps_batch(comp_name, comp_status, next_start_time, next_end_time, True)
            
            # After the first chunk, update the status to no longer be the first chunk.
            if comp_status["is_first_chunk"]:
                comp_status["is_first_chunk"] = False

            if res is not None:
                data = res[0]
                # Check if the current chunk is the last one based on the data length.
                if len(data) == 0:
                    is_last_chunk = True
                    break

                # Increment the time offset for the next chunk.
                next_start_time = res[1][-1][0]
                next_end_time = next_start_time + chunk_time_size

                # Convert data to int16 and flatten it.
                data = data.astype(np.int16).reshape(-1)
                
                # Check for continuity between chunks. Check that first data of the current chunk is last data of previous chunk + 1
                # or if the last data of the previous chunk is 32767 and the first data of the current chunk is -32768 (int16 wrap around)
                if not is_first_chunk:
                    expected_next_value = last_data + 1 if last_data != np.iinfo(np.int16).max else np.iinfo(np.int16).min
                    if data[0] != expected_next_value:
                        chunk_with_errors += 1

                # Update the last data point for the next iteration.
                # last_data = data[-1]

                # Update the last data point for the next iteration.
                last_data = data[-1]
                is_first_chunk = False

                # Create a sequence of expected data values for comparison.
                x = data[0] + np.array([i for i in range(len(data))]).astype(np.int16)

                # Check if the actual data matches the expected sequence.
                if not (data == x).all():
                    chunk_with_errors = chunk_with_errors + 1
            else:
                # If no data is returned, mark the last chunk and increment error count.
                is_last_chunk = True
                chunk_with_errors = chunk_with_errors + 1
        
        # Log the results of the data check.
        if (chunk_with_errors == 0):
            log.info(f"### OK ###")
        else:
            log.error(f"### ERRORS FOUND ###")
        log.info(f"--> {comp_name} Data check completed.")
        comp_status.pop("missing_bytes", None)
        comp_status.pop("saved_bytes", None)
        comp_status.pop("last_index", None)
        print("")
    
    @staticmethod
    def check_dummy_data(hsd, component, start_time, end_time, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Checks the dummy data for a given component within a specified time range.

        :param hsd: An instance of HSDatalog.
        :param component: A dictionary where the key is the component name and the value is its status.
        :param start_time: The start time for the data check (the closest greater timestamp will be selected).
        :param end_time: The end time for the data check (the closest greater timestamp will be selected).
        :param chunk_size: [Optional] The size of the data chunk (in samples) to be checked at a time. Default value = HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples
        """

        # Extract the component name from the dictionary keys. Assumes there is only one key-value pair.
        c_name = list(component.keys())[0]
        
        # Retrieve the status of the component using the component name.
        c_status = component[c_name]
        
        # Call the private method __check_data_batch of the HSDatalog class to perform the data check.
        # This method will use the provided parameters to check the data for the specified component and time range.
        HSDatalog.__check_data_batch(hsd, c_name, c_status, start_time, end_time, chunk_size)
    
    @staticmethod
    def __convert_to_xsv_batch(hsd, comp_name, comp_status, start_time, end_time, labeled, raw_data, output_folder, file_format, which_tags:list = [], no_timestamps = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts sensor data to a specified file format (CSV, TSV, PARQUET) in batches.

        :param hsd: An instance of HSDatalog
        :param comp_name: The name of the component.
        :param comp_status: A dictionary containing the status of the component.
        :param start_time: The start time for the data conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the data conversion (the closest greater timestamp will be selected).
        :param labeled: Boolean to choose whether the output should contain information about labels (Input data must be labelled).
        :param raw_data: Boolean to get raw data output (not multiplied by sensitivity).
        :param output_folder: The folder where the output files will be saved.
        :param file_format: The desired output file format ('CSV', 'TSV', 'PARQUET').
        :param which_tags: [Optional] List of tags labels to be included into exported file.
        :param no_timestamps: [Optional] Boolean to decide whether to exclude timestamps from the output (if true, then no Time columns in exported file).
        :param chunk_size: [Optional] The size of the data chunk (in samples) to be processed at a time. Default value = HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples
        """
        # Generate the file path for the sensor data.
        sensor_file_path = HSDatalog.get_sensor_file_path(comp_name, output_folder)
        
        # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
        chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)

        # Initialize the flag to determine if the current chunk is the last one.
        is_last_chunk = False
        # Set the initial time offset to the start time or 0 if not provided.
        time_offset = start_time or 0
        
        log.info(f"--> {comp_name} Conversion started...")
        ioffset = comp_status.get('ioffset', 0)
        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        # Mark the first chunk to handle any initial conditions.
        comp_status["is_first_chunk"] = True
        while not is_last_chunk:
            # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
            if end_time != -1 and next_end_time > end_time:
                #read exactly the missing samples up to end_time
                next_end_time = end_time
            
            log.debug("extracting df...")
            # Retrieve the data frame for the current chunk.
            df = hsd.get_dataframe_batch(comp_name, comp_status, next_start_time, next_end_time, labeled, raw_data, which_tags)
            
            # After the first chunk, update the status to no longer be the first chunk.
            if comp_status["is_first_chunk"]:
                comp_status["is_first_chunk"] = False
            
            log.debug("df extracted")
            if comp_status["c_type"] == ComponentTypeEnum.ACTUATOR.value or comp_status["c_type"] == ComponentTypeEnum.ALGORITHM: #NO BATCHES FOR ACTUATORS AND ALGORITHMS
                is_last_chunk = True
                log.info("--> Conversion completed")

            if df is not None:
                # Check if this is the last chunk based on the end time and the last timestamp in the dataframe.
                if end_time != -1 and df.iloc[-1,0] >= end_time:
                    is_last_chunk = True
                    log.info("--> Conversion completed")
                # If timestamps should not be included, drop the 'Time' column from the dataframe.
                if no_timestamps:
                    df.drop("Time", axis=1, inplace=True)
                # Determine the file mode ('write' for the first chunk, 'append' for subsequent chunks).
                file_mode = 'w' if next_start_time == (start_time or 0) else 'a'
                log.debug(f"df to {file_format} STARTED...")
                # Convert the data frame to the specified file format and save it to the file path.
                if file_format == 'CSV':
                    HSDatalogConverter.to_csv(df, sensor_file_path, mode=file_mode)
                elif file_format == 'TSV':
                    HSDatalogConverter.to_tsv(df, sensor_file_path, mode=file_mode)
                elif file_format == 'PARQUET':
                    HSDatalogConverter.to_parquet(df, sensor_file_path, mode=file_mode)
                elif file_format == 'HDF5':
                    HSDatalogConverter.to_hdf5(df, sensor_file_path, mode=file_mode)
                log.debug(f"df to {file_format} COMPLETED!")

                # If the dataframe is empty, mark the last chunk and log completion.
                if len(df) == 0:
                    is_last_chunk = True
                    log.info("--> Conversion completed")
                else:
                    log.debug("--> Chunk Conversion completed")
                    # Increment the time offset by the chunk time size for the next iteration.
                    # This sets up the start time for the next chunk.
                    next_start_time = float(df.iloc[-1,0])
                    # Calculate the end time for the next chunk.
                    next_end_time = next_start_time + chunk_time_size
            else:
                # If no data frame is returned, mark the last chunk.
                # This could happen if there is no more data to process or if an error occurred.
                is_last_chunk = True
                log.info("--> Conversion completed")
        
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)
    
    @staticmethod
    def convert_dat_to_xsv(hsd, component, start_time, end_time, labeled, raw_data, output_folder, file_format, which_tags:list = [], no_timestamps = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data from .dat format to a specified file format (CSV, TSV, Apache PARQUET) for a given component.

        :param hsd: An instance of HSDatalog.
        :param component: A dictionary where the key is the component name and the value is its status.
        :param start_time: The start time for the data conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the data conversion (the closest greater timestamp will be selected).
        :param labeled: Boolean to choose whether the output should contain information about labels (Input data must be labelled).
        :param raw_data: Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param output_folder: The directory where the converted files will be saved.
        :param file_format: The desired output file format ('CSV', 'TSV', 'PARQUET').
        :param which_tags: [Optional] List of tags labels to be included into exported file.
        :param no_timestamps: [Optional] Boolean to decide whether to exclude timestamps from the output (if true, then no Time columns in exported file).
        :param chunk_size: [Optional] The size of the data chunk (in samples) to be processed at a time. Default value = HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples
        """
        c_name = list(component.keys())[0]
        c_status = component[c_name]
        HSDatalog.__convert_to_xsv_batch(hsd, c_name, c_status, start_time, end_time, labeled, raw_data, output_folder, file_format, which_tags, no_timestamps, chunk_size)

    @staticmethod
    def __convert_to_txt_by_tags_batch(hsd, comp_name, comp_status, start_time, end_time, output_folder, out_format, which_tags:list = [], with_untagged = False, no_timestamps = False, raw_data = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts sensor data to a text-based format (TXT, CSV, TSV) filtered by tags in batches.

        :param hsd: An instance of HSDatalog, which provides methods for data conversion.
        :param comp_name: The name of the component.
        :param comp_status: A dictionary containing the status of the component.
        :param start_time: The start time for the data conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the data conversion (the closest greater timestamp will be selected).
        :param output_folder: The directory where the converted files will be saved.
        :param out_format: The desired output file format ('TXT', 'CSV', 'TSV').
        :param which_tags: [Optional] List of tags labels to be included into exported file.
        :param with_untagged: [Optional] Boolean indicating whether to include untagged data in the conversion. A dedicated untagged folder will be created
        :param no_timestamps: [Optional] Boolean indicating whether to exclude timestamps from the output.
        :param raw_data: [Optional] Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        """

        # Get the path of the current data acquisition.
        acquisition_path = hsd.get_acquisition_path()
        # Extract the folder name from the acquisition path.
        acquisition_folder_name = os.path.basename(acquisition_path)

        # Retrieve the list of available tags (label classes) for the acquisition.
        tags_columns = HSDatalog.get_acquisition_label_classes(hsd)
        # If no tags are available, raise an exception.
        if len(tags_columns) == 0:
            raise MissingTagsException()

        # If untagged data should be included, prepare the sub-folder and file path for untagged data.
        if with_untagged:
            # Flag to indicate if it's the first batch of untagged data.
            first_untagged = True
            # Create the sub-folder for untagged data if it doesn't exist.
            untagged_sub_folder = os.path.join(output_folder, "untagged")
            os.makedirs(untagged_sub_folder, exist_ok=True)
            # Determine the file path for untagged data based on the output folder.
            if output_folder != acquisition_path + "_Exported":
                untagged_file_path = os.path.join(untagged_sub_folder,f"untagged_{comp_name}_{acquisition_folder_name}_dataLog")
            else:
                untagged_file_path = os.path.join(untagged_sub_folder,f"untagged_{comp_name}_dataLog")

        # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
        chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)
        
        # Initialize the flag to determine if the current chunk is the last one.
        is_last_chunk = False
        # Set the initial time offset to the start time or 0 if not provided.
        time_offset = start_time or 0

        # Determine if timestamps should be included in the output.
        with_times = not no_timestamps
        
        # If specific tags are provided, filter the tags_columns list to include only those tags.
        if len(which_tags) > 0:
            # Filter the tag_colimns list based on the received which_tags list.
            filtered_list = [item for item in tags_columns if item in which_tags]
            # Identify and log errors for tags that are not present in the acquisition tags list.
            non_present_strings = [item for item in which_tags if item not in tags_columns]
            for item in non_present_strings:
                log.error(f"Error: '{item}' is not present in acquisition tags list.")
            # If there are valid tags after filtering, use the filtered list.
            if len(filtered_list) > 0:
                tags_columns = filtered_list
            else:
                # If no valid tags are found, raise an exception.
                raise MissingTagsException("No valid tags were found in your selected tag filtering list.")

        # Initialize a dictionary to store the last timestamp and tag group index for each tag.
        last_tags_batch_info = {} # Structure: {tag: {last_timestamp, tag_group_id, first_chunk}}
        for t in tags_columns:
            # Initialize the dictionary with default values for each tag.
            last_tags_batch_info[t] = {"last_timestamp":0,"tag_group_id":0,"first_chunk":True}
        
        log.info(f"--> {comp_name} Conversion started...")
        ioffset = comp_status.get('ioffset', 0)
        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        # Mark the first chunk to handle any initial conditions.
        comp_status["is_first_chunk"] = True
        while not is_last_chunk:
            # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
            if end_time != -1 and next_end_time > end_time:
                #read exactly the missing samples up to end_time
                next_end_time = end_time
            
            # Retrieve the data frame for the current chunk.
            df = hsd.get_dataframe_batch(comp_name, comp_status, next_start_time, next_end_time, True, raw_data)

            # After the first chunk, update the status to no longer be the first chunk.
            if comp_status["is_first_chunk"]:
                comp_status["is_first_chunk"] = False

            if comp_status["c_type"] == ComponentTypeEnum.ACTUATOR.value or comp_status["c_type"] == ComponentTypeEnum.ALGORITHM: #NO BATCHES FOR ACTUATORS AND ALGORITHMS
                is_last_chunk = True
                log.info("--> Conversion completed")

            if df is not None:
                # If the data frame is empty, mark the last chunk and log completion.
                if len(df) == 0:
                    is_last_chunk = True
                    log.info("--> Conversion completed")
                    break
                else:
                    log.debug("--> Chunk Conversion completed")
                    # Increment the time offset by the chunk time size for the next iteration.
                    next_start_time = float(df.iloc[-1,0])
                    next_end_time = next_start_time + chunk_time_size

                # If untagged data should be included, process it separately.
                if with_untagged:
                    # Select rows where none of the tags_columns have a value (untagged data).
                    untagged_df = df[~df[tags_columns].any(axis=1)]
                    # Drop tag columns from the untagged data frame.
                    untagged_df = untagged_df.drop([f"{label}" for label in tags_columns], axis=1)
                    # Determine the file mode ('write' for the first chunk, 'append' for subsequent chunks).
                    wtype = 'w' if first_untagged else 'a'
                    # Reset the flag after the first chunk of untagged data is processed.
                    first_untagged = False
                    # Convert the untagged data frame to the specified file format and save it.
                    HSDatalogConverter.to_unico(untagged_file_path, untagged_df, out_format, wtype, True)
                
                # Create group keys for each Tag Class in the data frame.
                for tag_label in tags_columns:
                    df = HSDatalog.__create_group_keys(df, tag_label)
                
                # Process groups for each Tag Class.
                for tag_label in tags_columns:
                    # Create a column name for the group key based on the tag label.
                    group_key_column = f'{tag_label}_group_key'
                    # Group the data frame by the group key.
                    grouped = df.groupby(group_key_column)
                    
                    # Iterate over each group and process tagged data.
                    for group_key, group_df in grouped:
                        # Check if the first row of the group has the tag label set to True.
                        if group_df[tag_label].iloc[0]:
                            # Determine the file path for the tagged data based on the output folder.
                            if output_folder != acquisition_path + "_Exported":
                                HSDatalog.__save_tag_group_in_file(comp_name + "_" + acquisition_folder_name, group_df, tag_label, last_tags_batch_info, tags_columns, output_folder, out_format, with_times)
                            else:
                                HSDatalog.__save_tag_group_in_file(comp_name, group_df, tag_label, last_tags_batch_info, tags_columns, output_folder, out_format, with_times)
            else:
                # If no data frame is returned, mark the last chunk.
                is_last_chunk = True
                log.info("--> Conversion completed")
        
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)

    @staticmethod
    def convert_dat_to_txt_by_tags(hsd, component, start_time, end_time, output_folder, out_format, which_tags:list = [], with_untagged = False, no_timestamps = False, raw_data = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data from .dat format to a text-based format (TXT, CSV, TSV) filtered by tags.

        :param hsd: An instance of HSDatalog, which provides methods for data conversion.
        :param component: A dictionary with component names as keys and their statuses as values.
        :param start_time: The start time for the data conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the data conversion (the closest greater timestamp will be selected).
        :param output_folder: The directory where the converted files will be saved.
        :param out_format: The desired output file format ('TXT, 'CSV', 'TSV').
        :param which_tags: [Optional] List of tags to filter the data by.
        :param with_untagged: [Optional] Boolean indicating whether to include untagged data in the conversion.
        :param no_timestamps: [Optional] Boolean indicating whether to exclude timestamps from the output.
        :param raw_data: [Optional] Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        """
        
        # Extract the component name from the dictionary; assumes there is only one key-value pair.
        c_name = list(component.keys())[0]
        # Retrieve the status of the component using the component name.
        c_status = component[c_name]

        # Call the private method __convert_to_txt_by_tags_batch of the HSDatalog class.
        # This method will perform the actual conversion of data to the specified file format in batches,
        # filtered by the specified tags and including untagged data if specified.
        HSDatalog.__convert_to_txt_by_tags_batch(hsd, c_name, c_status, start_time, end_time, output_folder, out_format, which_tags, with_untagged, no_timestamps, raw_data, chunk_size)
    
    def __convert_to_nanoedge_format_batch(hsd, comp_name, comp_status, signal_length, signal_increment, start_time, end_time, raw_data, output_folder, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data to NanoEdge CSV format in batches for a given component within a specified time range.

        :param hsd: An instance of HSDatalog.
        :param comp_name: The name of the component.
        :param comp_status: A dictionary containing the status of the component.
        :param signal_length: The length of each segment when performing segmentation.
        :param signal_increment: Parameter to control the overlap. e.g.: signal_increment = signal_length/2 is 50% overlap.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param raw_data: Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param output_folder: The folder where the output files will be saved.
        :param chunk_size: [Optional] The size of the data chunk (in samples) to be processed at a time. Default value = HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        """
        
        # Retrieve necessary parameters from the component status.
        odr = comp_status.get("measodr", comp_status.get("odr",1)) # Output data rate
        spts = comp_status.get("samples_per_ts", 0) # Samples per timestamp
        io = comp_status.get("ioffset", 0) # Initial offset
        # Extract the first timestamp (if spts == 0 --> first timestamp is the initial offset)
        first_timestamp = (spts/odr) + io

        # Ensure chunk size is sufficient for the signal length
        chunk_size = max(chunk_size, signal_length)
        # Calculate the time duration of each chunk based on the chunk size and odr.
        chunk_time_size = chunk_size/odr # seconds
        # Ensure chunk time size is at lease one timestamp
        chunk_time_size = max(chunk_time_size, first_timestamp)
        
        # Initialize the flag to determine if the current chunk is the last one.
        is_last_chunk = False
        # Mark the first chunk to handle any initial conditions.
        comp_status["is_first_chunk"] = True
        # Set the initial time offset to the start time or 0 if not provided.
        time_offset = start_time or 0
        # Initialize a flag to track if there were any errors during conversion.
        errors_in_conversion = False
        log.info(f"--> {comp_name} Conversion started...")
        # Initialize a CSV writer for NanoEdge format.
        csv_writer = NanoedgeCSVWriter(output_folder, comp_name, signal_length, signal_increment)
        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        while not is_last_chunk:
            # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
            if end_time != -1 and next_end_time > end_time:
                #read exactly the missing samples up to end_time
                next_end_time = end_time

             # If chunk_size is 0, mark the last chunk and log completion.
            if chunk_size == 0:
                is_last_chunk = 1
                log.info(f"--> {comp_name} Nanoedge conversion completed successfully")
                break
            
            # Retrieve the data frame for the current chunk.
            df = hsd.get_dataframe_batch(comp_name, comp_status, next_start_time, next_end_time, False, raw_data)

            if comp_status["c_type"] == ComponentTypeEnum.ACTUATOR.value or comp_status["c_type"] == ComponentTypeEnum.ALGORITHM: #NO BATCHES FOR ACTUATORS AND ALGORITHMS
                is_last_chunk = True
                log.info("--> Conversion completed")

            if df is not None:
                try:
                    if not ("_ispu" in comp_name or "_mlc" in comp_name):
                        # Write the data frame to NanoEdge format using the CSV writer.
                        # Use 'w' for write (first chunk) or 'a' for append (subsequent chunks).
                        if comp_status["is_first_chunk"]:
                            csv_writer.to_nanoedge_format_batch(df,"w")
                            comp_status["is_first_chunk"] = False
                        else:
                            csv_writer.to_nanoedge_format_batch(df,"a")
                except NanoEdgeConversionError:
                    # If an error occurs during conversion, set the error flag.
                    errors_in_conversion = True
                if len(df) == 0:
                    is_last_chunk = True
                    # Log an error message if there were errors, otherwise log successful completion.
                    if errors_in_conversion:
                        log.error(f"--x ERRORS in {comp_name} Nanoedge conversion process")
                    else:
                        # Flush any remaining data in the CSV writer.
                        csv_writer.flush()  
                        log.info(f"--> {comp_name} Nanoedge conversion completed successfully")
                else:
                    # Log an error message if there were errors, otherwise log successful chunk conversion.
                    if errors_in_conversion:
                        log.error(f"--x ERRORS in {comp_name} chunk conversion")
                    else:    
                        log.info(f"--> {comp_name} chunk conversion completed successfully")
                    # Increment the time offset for the next chunk.
                    next_start_time = float(df.iloc[-1,0])
                    next_end_time = next_start_time + chunk_time_size
            else:
                # If no data frame is returned, mark the last chunk.
                is_last_chunk = True
                if errors_in_conversion:
                    log.error(f"--x ERRORS in {comp_name} Nanoedge conversion process")
                else:
                    # Flush any remaining data in the CSV writer to ensure all data is written to the file.
                    csv_writer.flush()
                    log.info(f"--> {comp_name} Nanoedge conversion completed successfully")
        
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, io)
    
    @staticmethod
    def convert_dat_to_nanoedge(hsd, component, signal_length, signal_increment, start_time, end_time, raw_data, output_folder, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data from .dat format to NanoEdge format.

        :param hsd: An instance of HSDatalog, which provides methods for data conversion.
        :param component: A dictionary with component names as keys and their statuses as values.
        :param signal_length: The length of each segment when performing segmentation.
        :param signal_increment: Parameter to control the overlap. e.g.: signal_increment = signal_length/2 is 50% overlap.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param raw_data: Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param output_folder: The directory where the converted files will be saved.
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        """

        # Extract the component name from the dictionary; assumes there is only one key-value pair.
        c_name = list(component.keys())[0]
        # Retrieve the status of the component using the component name.
        c_status = component[c_name]

        # Call the private method __convert_to_nanoedge_format_batch of the HSDatalog class.
        # This method will perform the actual conversion of data to NanoEdge format in batches,
        # using the provided parameters such as signal length, signal increment, and chunk size.
        HSDatalog.__convert_to_nanoedge_format_batch(hsd, c_name, c_status, signal_length, signal_increment, start_time, end_time, raw_data, output_folder, chunk_size)
    
    @staticmethod
    def __convert_to_unico_format_batch(hsd, components, start_time, end_time, use_datalog_tags, output_folder, out_format, columns_labels = "default", with_times = False, raw_data = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data from multiple components to UNICO format in batches.

        :param hsd: An instance of HSDatalog, which provides methods for data conversion.
        :param components: A list of dictionaries, each with a single component's name as the key and its status as the value.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param use_datalog_tags: Boolean indicating if datalog tags should be used in the conversion.
        :param output_folder: The directory where the converted files will be saved.
        :param out_format: The desired output file format ('TXT', 'CSV', 'TSV').
        :param columns_labels: [Optional] Column labels for the output file. Default is "default" ("mlc_tool" is allowed).
        :param with_times: [Optional] Boolean indicating whether to include timestamps in the output. Default is False.
        :param raw_data: [Optional] Boolean indicating whether to output raw data (not multiplied by sensitivity). Default is False.
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        """

        # Create the output folder if it does not exist.
        if not os.path.exists(output_folder):
            os.makedirs(output_folder)
        
        # Initialize flags for batch processing.
        is_last_chunk = False
        is_first_chunk = True

        # Start the conversion log.
        if len(components) == 1:
            comp_name = list(components[0].keys())[0]
            log.info(f"--> {comp_name} Conversion started...")
        else:
            return HSDatalog.__convert_to_unico_agg_format_batch(hsd, components, start_time, end_time, use_datalog_tags, output_folder, out_format, "same_sensor", columns_labels, with_times, False, raw_data, chunk_size)
        
        # extract component names, status
        c = components[0]
        comp_name = list(c.keys())[0]
        comp_status = c[comp_name]
        # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
        chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)
        # Set the initial time offset to the start time or 0 if not provided.
        time_offset = start_time or 0
    
        ioffset = comp_status.get('ioffset',0)
        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        # Mark the first chunk to handle any initial conditions.
        comp_status["is_first_chunk"] = True
        while not is_last_chunk:
            # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
            if end_time != -1 and next_end_time > end_time:
                #read exactly the missing samples up to end_time
                next_end_time = end_time

            # Retrieve the dataframe for the current chunk.
            df = hsd.get_dataframe_batch(comp_name, comp_status, next_start_time, next_end_time, use_datalog_tags, raw_data)
            
            if comp_status["c_type"] == ComponentTypeEnum.ACTUATOR.value or comp_status["c_type"] == ComponentTypeEnum.ALGORITHM: #NO BATCHES FOR ACTUATORS AND ALGORITHMS
                is_last_chunk = True
                log.info("--> Conversion completed")
            
            # After the first chunk, update the status to no longer be the first chunk.
            if comp_status["is_first_chunk"]:
                comp_status["is_first_chunk"] = False
            
            if df is not None:
                if columns_labels == "mlc_tool":
                    cols_to_exclude = ["Time"]
                    # Define a prefix to add to column names based on the component name.
                    prefix = f"{comp_name}"
                    new_names = {}
                    new_names["Time"] = "Time[s]"
                    if use_datalog_tags:
                        tags = HSDatalog.get_time_tags(hsd)
                        tag_labels = set([info['label'] for info in tags])
                        for t in tag_labels:
                            cols_to_exclude.append(t)
                    for col in df.columns:
                        if col not in cols_to_exclude:
                            sensor_type_str = col.split(' ')[0]
                            # Create a dictionary of column name mappings with the new prefix.
                            if "_" in sensor_type_str:
                                new_col_name = prefix + "_" + sensor_type_str.split('_')[-1] + " " + df.columns[1].split(' ')[-1]
                            else:
                                new_col_name = prefix + " " + df.columns[1].split(' ')[-1]
                            new_names[col] = new_col_name
                    # Rename columns in the dataframe according to the new names mapping.
                    df = df.rename(columns=new_names)
                # Determine the file mode ('write' for the first chunk, 'append' for subsequent chunks) and file name.
                if is_first_chunk:
                    file_mode = 'w'
                    is_first_chunk = False
                    file_name = comp_name
                else:
                    file_mode = 'a'
                file_path = os.path.join(output_folder, file_name)
                # Convert the data frame to the specified file format and save it.
                HSDatalogConverter.to_unico(file_path, df, out_format, file_mode, with_times, columns_labels)
                # If the data frame is empty, mark the last chunk and log completion.
                if len(df) == 0:
                    is_last_chunk = True
                    log.info(f"--> {file_name} ST UNICO format conversion completed successfully")
                else:
                    log.debug("--> Chunk Conversion completed")
                    # Increment the time offset by the chunk time size for the next iteration.
                    # This sets up the start time for the next chunk.
                    next_start_time = float(df.iloc[-1,0])
                    # Calculate the end time for the next chunk.
                    next_end_time = next_start_time + chunk_time_size
            else:
                # If no data frame is returned, mark the last chunk.
                # This could happen if there is no more data to process or if an error occurred.
                is_last_chunk = True
                log.info("--> Conversion completed")
        
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)
    
    @staticmethod
    def __convert_to_unico_agg_format_batch(hsd, components, start_time, end_time, use_datalog_tags, output_folder, out_format, aggregation, columns_labels = "default", with_times = False, with_untagged = False, raw_data = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data from multiple components to an aggregated UNICO format in batches, with options for tag-based splitting.

        :param hsd: An instance of HSDatalog, which provides methods for data conversion.
        :param components: A list of dictionaries, each with a single component's name as the key and its status as the value.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param use_datalog_tags: Boolean indicating if datalog tags should be used in the conversion.
        :param output_folder: The directory where the converted files will be saved.
        :param out_format: The desired output file format ('TXT', 'CSV', 'TSV').
        :param aggregation: The method of aggregation to be used ('single_file' or 'split_per_tags').
        :param columns_labels: [Optional] Column labels for the output file. Default is "default".
        :param with_times: [Optional] Boolean indicating whether to include timestamps in the output.
        :param with_untagged: [Optional] Boolean indicating whether to include untagged data in the conversion.
        :param raw_data: [Optional] Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        """
        
        # Get the path of the current data acquisition.
        acquisition_path = hsd.get_acquisition_path()
        
        # Create the output folder if it does not exist.
        if not os.path.exists(output_folder):
            os.makedirs(output_folder)
        
        # If aggregation is "split_per_tags", force use_datalog_tags to True and prepare for untagged data.
        if aggregation == "split_per_tags":
            use_datalog_tags = True
            last_tags_batch_info = {} # Dictionary to store the last timestamp and tag group index for each tag.
            if with_untagged:
                first_untagged = True # Flag to indicate if it's the first batch of untagged data.
                untagged_sub_folder = os.path.join(output_folder, "untagged") # Sub-folder for untagged data.
                os.makedirs(untagged_sub_folder, exist_ok=True) # Create the sub-folder if it doesn't exist.
                untagged_file_path = os.path.join(untagged_sub_folder,"untagged_Aggregated_dataLog") # Path for untagged data file.
        
        # Initialize flags for batch processing.
        is_last_chunk = False
        is_first_chunck = True
        log.info(f"--> {output_folder} Unico Conversion started (with aggregated data)...")
        
        # Initialize variables for processing.
        tot_axes = 0 # Total number of axes across all components.
        comp_names = [] # List to store component names.
        comp_status = [] # List to store component statuses.
        ioffsets = []

        original_labels = ["Time"]
        
        for c in components:
            comp_name = list(c.keys())[0]
            comp_columns = hsd.get_component_columns_names(c[comp_name], comp_name)            
        
            if columns_labels == "mlc_tool":
                dim = c[comp_name].get("dim",1)
                if dim > 1:
                    # Define a prefix to add to column names
                    prefix = f"{list(c.keys())[0]}_"
                    # Create a dictionary of column name mappings
                    original_labels.extend([prefix + col.split(' ')[0].split('_')[-1] + " " + col.split(' ')[-1] for col in comp_columns])
                else:
                    # Define a prefix to add to column names
                    prefix = f"{list(c.keys())[0]}"
                    original_labels.append(prefix + " " + comp_columns[0].split(' ')[-1])
            else:
                # Define a prefix to add to column names
                suffix = f"_{list(c.keys())[0]}"
                # Create a dictionary of column name mappings
                original_labels.extend([col + suffix for col in comp_columns])

        if use_datalog_tags:
            tags = HSDatalog.get_time_tags(hsd)
            tag_labels = set([info['label'] for info in tags])
            original_labels.extend(list(tag_labels))

        # Find the maximum initial offset across all components.
        max_ioffset = list(max(components, key=lambda c : c[list(c.keys())[0]]["ioffset"]).values())[0]["ioffset"]
        # Sort components by their measured output data rate (measodr) or output data rate (odr), in descending order.
        sorted_components = sorted(components, key=lambda c : c[list(c.keys())[0]].get("measodr",c[list(c.keys())[0]].get("odr",1)), reverse=True)
        # Get the component with the fastest output data rate.
        fast_c = sorted_components[0][list(sorted_components[0].keys())[0]]
        
        # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr) of the fastest component.
        chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, fast_c)

        # Set the initial time offset to the maximum of the start time or the maximum initial offset.
        time_offset =  start_time or 0

        # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
        if end_time != -1 and next_end_time > end_time:
            #read exactly the missing samples up to end_time
            next_end_time = end_time

        # Initialize the count of tag columns and the times for the next start and end.
        tags_columns_count = -1
        next_end_time = 0
        next_start_time = 0

        # Prepare component names, status, and total axes count for batch processing.
        for i, c in enumerate(sorted_components):
            comp_names.append(list(c.keys())[0])
            comp_status.append(c[comp_names[-1]])
            tot_axes += c[comp_names[-1]].get("dim",1)
            ioffsets.append(comp_status[-1].get('ioffset',0))
            # Mark the first chunk to handle any initial conditions.
            comp_status[-1]["is_first_chunk"] = True

        next_start_time = time_offset
        next_end_time = time_offset+chunk_time_size
        # Begin processing data in chunks.
        while not is_last_chunk:
            comp_dataframes = []
            comp_dataframes_names = []
            # Loop through each sorted component to retrieve and process their data frames.
            for i, c in enumerate(sorted_components):
                # Retrieve the data frame for the current chunk.
                if i == 0:
                    df = hsd.get_dataframe_batch(comp_names[i], comp_status[i], next_start_time, next_end_time, use_datalog_tags, raw_data)

                    if comp_status[i]["c_type"] == ComponentTypeEnum.ACTUATOR.value or comp_status[i]["c_type"] == ComponentTypeEnum.ALGORITHM: #NO BATCHES FOR ACTUATORS AND ALGORITHMS
                        is_last_chunk = True
                        log.info("--> Conversion completed")

                    if df is not None:
                        # Check if this is the last chunk based on the end time and the last timestamp in the dataframe.
                        if end_time != -1 and df.iloc[-1,0] >= end_time:
                            is_last_chunk = True
                            log.info("--> Conversion completed")

                        if tags_columns_count == - 1:
                            # Determine the number of tag columns in the dataframe.
                            tags_columns = HSDatalog.get_acquisition_label_classes(hsd) # Retrieve tag columns from the HSDatalog instance.
                            if aggregation == "split_per_tags" and last_tags_batch_info == {}:
                                # Initialize last_tags_batch_info for each tag column.
                                for t in tags_columns:
                                    last_tags_batch_info[t] = {"last_timestamp":0,"tag_group_id":0,"first_chunk":True}
                else:
                    # Retrieve the data frame for subsequent components within the same chunk.
                    df = hsd.get_dataframe_batch(comp_names[i], comp_status[i], next_start_time, next_end_time, use_datalog_tags, raw_data)

                # After the first chunk, update the status to no longer be the first chunk.
                if comp_status[i]["is_first_chunk"]:
                    comp_status[i]["is_first_chunk"] = False

                if df is None:
                    # If no data frame is retrieved, clear the list and break the loop.
                    comp_dataframes = []
                    break
                
                if use_datalog_tags and i != len(sorted_components) - 1:
                    # If using datalog tags, drop the tag columns for all but the last component.
                    df.drop(columns = tags_columns, inplace=True)
                
                comp_dataframes.append(df)
                comp_dataframes_names.append(comp_names[i])
            
            if comp_dataframes != [] and all(element is not None for element in comp_dataframes):
                # If data frames are retrieved for all components, proceed to merge them.
                if len(comp_dataframes) > 1:
                    # Merge multiple data frames into one.
                    df = HSDatalogConverter.merge_dataframes(comp_dataframes, comp_dataframes_names, columns_labels, list(tags_columns))
                    df = df[original_labels]
                    if columns_labels == "mlc_tool":
                        df = df.rename(columns={'Time': 'Time[s]'})
                else:
                    # If there's only one data frame, use it as is.
                    df = comp_dataframes[0]
                
                if df is not None:
                    # If the merged data frame is not empty, proceed to save it.
                    if aggregation == "single_file" or aggregation == "same_sensor":
                        # If using single file aggregation, determine the file mode.
                        if is_first_chunck:
                            file_mode = 'w'
                            is_first_chunck = False
                        else:
                            file_mode = 'a'
                        
                        acquisition_folder_name = os.path.basename(acquisition_path)
                        # Construct the file path for the aggregated data.
                        #If aggregation is "same_sensor"
                        if aggregation == "same_sensor":
                            s_name, _ = FileManager.decode_file_name(list(components[0].keys())[0])
                            file_path = os.path.join(output_folder, s_name)    
                        elif aggregation == "single_file":
                            #If aggregation is "single_file", save the merged dataframe as a single file
                            file_path = os.path.join(output_folder, f"Aggregated_{acquisition_folder_name}")
                        # Convert the data frame to the specified file format and save it.
                        HSDatalogConverter.to_unico(file_path, df, out_format, file_mode, with_times, columns_labels)
                    elif aggregation == "split_per_tags":
                        # If using tag-based splitting, handle untagged and tagged data separately.
                        if with_untagged:
                            # Process untagged data.
                            untagged_df = df[~df[tags_columns].any(axis=1)]
                            # Drop tag columns
                            untagged_df = untagged_df.drop([f"{label}" for label in tags_columns], axis=1)
                            if first_untagged:
                                wtype = 'w'
                                first_untagged = False
                            else:
                                wtype = 'a'
                            HSDatalogConverter.to_unico(untagged_file_path, untagged_df, out_format, wtype, with_times, columns_labels)
                        
                        # Create group keys for each Tag Class
                        for tag_label in tags_columns:
                            df = HSDatalog.__create_group_keys(df, tag_label)

                        # Process groups for each Tag Class
                        for tag_label in tags_columns:
                            group_key_column = f'{tag_label}_group_key'
                            grouped = df.groupby(group_key_column)

                            for group_key, group_df in grouped:
                                if group_df[tag_label].iloc[0]:
                                    # Save each group of tagged data to a separate file.
                                    HSDatalog.__save_tag_group_in_file("Aggregated", group_df, tag_label, last_tags_batch_info, tags_columns, output_folder, out_format, with_times, columns_labels)

                    # If the data frame is empty, mark the last chunk and log completion.
                    if len(df) == 0:
                        is_last_chunk = True
                        log.info(f"--> Conversion completed")
                    else:
                        log.debug("--> Chunk Conversion completed")
                        # Increment the time offset for the next chunk.
                        next_start_time = float(df.iloc[-1,0])
                        next_end_time = next_start_time + chunk_time_size
                else:
                    # If no data frame is returned, mark the last chunk.
                    is_last_chunk = True
                    log.info("--> Conversion completed")
            else:
                # If no data frame is returned, mark the last chunk.
                is_last_chunk = True
                log.info("--> Conversion completed")
        
        # Reset the status conversion side information for the components statuses.
        for i, c in enumerate(sorted_components):
            HSDatalog.reset_status_conversion_side_info(comp_status[i], ioffsets[i])
    
    @staticmethod
    def convert_dat_to_unico(hsd, components, start_time, end_time, use_datalog_tags, output_folder, out_format, columns_labels = "default", with_times = False, raw_data = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data from .dat format to UNICO format.
        
        :param hsd: An instance of HSDatalog, which provides methods for data conversion.
        :param components: A list of dictionaries, each with a single component's name as the key and its status as the value.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param use_datalog_tags: Boolean indicating if datalog tags should be used in the conversion.
        :param output_folder: The directory where the converted files will be saved.
        :param out_format: The desired output file format ('TXT', 'CSV', 'TSV').
        :param columns_labels: [Optional] Column labels for the output file. Default is "default".
        :param with_times: [Optional] Boolean indicating whether to include timestamps in the output.
        :param raw_data: [Optional] Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        """

        # Call the private method __convert_to_unico_format_batch of the HSDatalog class that performs the actual conversion process.
        # The method called is responsible for handling the conversion in batches and applying the specified parameters.
        HSDatalog.__convert_to_unico_format_batch(hsd, components, start_time, end_time, use_datalog_tags, output_folder, out_format, columns_labels, with_times, raw_data, chunk_size)
    
    @staticmethod
    def convert_dat_to_unico_aggregated(hsd, aggregation, start_time, end_time, use_datalog_tags, output_folder, out_format, columns_labels = "default", with_times = False, with_untagged= False, raw_data = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts data from .dat format to an aggregated UNICO format.
        
        :param hsd: An instance of HSDatalog, which provides methods for data conversion.
        :param aggregation: The method of aggregation to be used ('single_file' or 'split_per_tags').
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param use_datalog_tags: Boolean indicating if datalog tags should be used in the conversion.
        :param output_folder: The directory where the converted files will be saved.
        :param out_format: The desired output file format ('TXT', 'CSV', 'TSV').
        :param columns_labels: [Optional] Column labels for the output file. Default is "default".
        :param with_times: [Optional] Boolean indicating whether to include timestamps in the output.
        :param with_untagged: [Optional] Boolean indicating whether to include untagged data in the conversion.
        :param raw_data: [Optional] Boolean indicating whether to output raw data (not multiplied by sensitivity).
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is DEFAULT_SAMPLES_CHUNK_SIZE.
        
        This method retrieves all active components and initiates the batch conversion process with aggregation options.
        """
        
        # Retrieve a list of all active components from the input acquisition folder.
        # Only components that are active (currently collecting data) are included in the conversion process.
        components = HSDatalog.get_all_components(hsd, only_active=True)

        # Call the private method __convert_to_unico_agg_format_batch of the HSDatalog class that performs the actual conversion process.
        # This method handles the conversion in batches and applies the specified parameters, including aggregation options.
        HSDatalog.__convert_to_unico_agg_format_batch(hsd, components, start_time, end_time, use_datalog_tags, output_folder, out_format, aggregation, columns_labels, with_times, with_untagged, raw_data, chunk_size)
    
    @staticmethod
    def __convert_to_wav_batch(hsd, comp_name, comp_status, start_time, end_time, output_folder, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts sensor data to WAV format in batches and saves the output to a file.

        :param hsd: An instance of HSDatalog, which provides methods for data retrieval.
        :param comp_name: The name of the component (sensor) whose data is being converted.
        :param comp_status: A dictionary containing the status of the component, including 'dim' and 'odr'.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param output_folder: The directory where the converted WAV file will be saved.
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        
        This method processes the data in chunks to manage memory usage and ensure that large datasets can be converted.
        It creates a WAV file, appends data to it in chunks, and closes the file upon completion.
        """
        try:
            # Extract the number of channels and output data rate (ODR) from the component status.
            n_channels = comp_status.get("dim")
            odr = comp_status.get("odr")
            ioffset = comp_status.get("ioffset")
            log.info(f"Sensor: {comp_name} (ODR: {odr})")
            
            # Generate the file path for the WAV file to be created.
            wav_file_path = HSDatalog.get_wav_file_path(hsd, comp_name, output_folder)
            # Validate the ODR and create the WAV file.
            if odr is None or odr<= 0:
                log.exception("Bad framerate selected for this sensor")
                raise ValueError("ODR must be a positive number")
            
            wav_file = HSDatalogConverter.wav_create(wav_file_path, odr, n_channels)
            
            # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
            chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)
            is_last_chunk = False
            time_offset = start_time or 0
            
            log.info(f"--> {comp_name} Conversion started...")
            next_start_time = time_offset
            next_end_time = time_offset+chunk_time_size

            comp_status["is_first_chunk"] = True
            while not is_last_chunk:
                # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
                if end_time != -1 and next_end_time > end_time:
                    next_end_time = end_time #read exactly the missing samples up to end_time

                # Retrieve data and timestamps for the current chunk.
                res = hsd.get_data_and_timestamps_batch(comp_name, comp_status, start_time = next_start_time, end_time = next_end_time, raw_flag = True)
                
                # After the first chunk, update the status to no longer be the first chunk.
                if comp_status["is_first_chunk"]:
                    comp_status["is_first_chunk"] = False

                if res is None:
                    HSDatalogConverter.wav_close(wav_file)
                    log.exception("No data from selected sensor")
                    break

                data = res[0]
                if len(data) == 0:
                    is_last_chunk = True
                else:
                    # Convert the data to PCM format and append to the WAV file.
                    pcm_data = data.astype(np.int16)
                    HSDatalogConverter.wav_append(wav_file,pcm_data)
                    log.info(f"--> File {wav_file_path} converted chunk appended successfully")

                    # Update the start time for the next chunk.
                    next_start_time = res[1][-1]
                    next_end_time = next_start_time + chunk_time_size
            
            # Close the WAV file after all chunks have been processed.
            HSDatalogConverter.wav_close(wav_file)
            log.info(f"--> {comp_name} Conversion completed")
        except Exception as err:
            # Handle any exceptions that occur during the conversion process.
            log.exception(err)
            # Close the WAV file in case of an exception.
            if 'wav_file' in locals():
                HSDatalogConverter.wav_close(wav_file)
        
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)
    
    @staticmethod
    def __convert_to_wav_by_tags_batch(hsd, comp_name, comp_status, start_time, end_time, output_folder, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts sensor data to WAV format in batches based on tags and saves the output to separate files.

        :param hsd: An instance of HSDatalog, which provides methods for data retrieval.
        :param comp_name: The name of the component (sensor) whose data is being converted.
        :param comp_status: A dictionary containing the status of the component, including 'dim' and 'odr'.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param output_folder: The directory where the converted WAV files will be saved.
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        
        Converts sensor data to WAV format in batches based on tags and saves the output to separate files.
        Each tag group will have its own WAV file, allowing for segmented audio based on the tagging system.
        """
        def save_tag_group_in_wav(group_df, tag_label, tag_info, tags_columns, output_folder, wav_file = None):
            """
            Saves a group of data associated with a specific tag into a WAV file.

            :param group_df: The dataframe containing the group of data to be saved.
            :param tag_label: The label of the tag associated with the data.
            :param tag_info: Dictionary containing information about the tag groups.
            :param tags_columns: List of tag column names in the dataframe.
            :param output_folder: The directory where the WAV file will be saved.
            :param wav_file: [Optional] The WAV file object to append data to. If None, a new file will be created.
            :return: The WAV file object.
            """
            # Drop tag columns and group key columns from the dataframe.
            group_df = group_df.drop([f"{label}" for label in tags_columns], axis=1)
            group_df = group_df.drop([f"{label}_group_key" for label in tags_columns], axis=1)
            group_df.drop("Time", axis=1, inplace=True)

            # Convert the remaining dataframe to numpy array for WAV data.
            pcm_data = group_df.to_numpy()

            # Create sub-folder for the tag if it doesn't exist.
            tag_sub_folder = os.path.join(output_folder, tag_label)
            os.makedirs(tag_sub_folder, exist_ok=True)

            # Construct the file path for the WAV file.
            acquisition_folder_name = os.path.basename(acquisition_path)
            label_file_name = f"{tag_label}_{comp_name}_{acquisition_folder_name}_{tag_info[tag_label]['tag_group_id']}.wav"
            label_file_path = os.path.join(tag_sub_folder, label_file_name)
            
            # Create a new WAV file or append to an existing one.
            # Determine the file mode ('write' for the first chunk, 'append' for subsequent chunks).
            if tag_info[tag_label]["first_chunk"]:
                wav_file = HSDatalogConverter.wav_create(label_file_path, odr, n_channels)
                HSDatalogConverter.wav_append(wav_file, pcm_data)
                tag_info[tag_label]["first_chunk"] = False
            else:
                HSDatalogConverter.wav_append(wav_file, pcm_data)
            return wav_file
        
        try:
            # Retrieve the acquisition path and sensor details.
            acquisition_path = hsd.get_acquisition_path()
            n_channels = comp_status.get("dim")
            odr = comp_status.get("odr")
            ioffset = comp_status.get("ioffset")
            log.info(f"Sensor: {comp_name} (ODR: {odr})")
            
            # Validate the ODR and number of channels.
            if odr is None or odr <= 0:
                log.exception("Invalid ODR value for this sensor")
                raise
            if n_channels is None:
                log.exception("Missing 'dim' property in your device status")
                raise
            
            # Retrieve the tag columns from the acquisition.
            tags_columns = HSDatalog.get_acquisition_label_classes(hsd)
            
            # Calculate the time duration of each chunk based on the chunk size and the output data rate (odr).
            chunk_time_size = HSDatalog.__compute_chunk_time_size(chunk_size, comp_status)
            is_last_chunk = False
            time_offset = start_time or 0
            
            # Initialize information for tag groups and WAV file objects.
            last_tags_batch_info = {}
            wav_files = {}
            for t in tags_columns:
                last_tags_batch_info[t] = {"last_timestamp":0,"tag_group_id":0,"first_chunk":True}
                wav_files[t] = {}
            
            log.info(f"--> {comp_name} Conversion started...")
            next_start_time = time_offset
            next_end_time = time_offset+chunk_time_size
            
            # Mark the first chunk to handle any initial conditions.
            comp_status["is_first_chunk"] = True
            # Process data in chunks until the last chunk is reached.
            while not is_last_chunk:
                # Adjust the chunk size if the end time is specified and the current chunk exceeds it.
                if end_time != -1 and next_end_time > end_time:
                    #read exactly the missing samples up to end_time
                    next_end_time = end_time

                # Retrieve the data frame for the current chunk.
                df = hsd.get_dataframe_batch(comp_name, comp_status, time_offset, time_offset+chunk_time_size, True, True)
                if df is not None:
                    # If the data frame is empty, mark the last chunk and log completion.
                    if len(df) == 0:
                        is_last_chunk = True
                        log.info("--> Conversion completed")
                        break
                    else:
                        log.debug("--> Chunk Conversion completed")
                        # Increment the time offset by the chunk time size for the next iteration.
                        next_start_time = float(df.iloc[-1,0])
                        next_end_time = next_start_time + chunk_time_size

                    # Create group keys for each Tag Class to identify unique tag groups within the data.
                    for tag_label in tags_columns:
                        df = HSDatalog.__create_group_keys(df, tag_label)
                    
                    # Process groups for each Tag Class and save them into separate WAV files.
                    for tag_label in tags_columns:
                        group_key_column = f'{tag_label}_group_key'
                        grouped = df.groupby(group_key_column)

                        for group_key, group_df in grouped:
                            if group_df[tag_label].iloc[0]:
                                # Calculate the expected next timestamp for the current tag group.
                                tag_batch_first_time = group_df.iloc[0, 0]
                                tag_batch_last_time = group_df.iloc[-1, 0]
                                tag_batch_sample_time = (tag_batch_last_time - tag_batch_first_time) / len(group_df)
                                
                                # Check if there is a significant gap since the last timestamp, indicating a new tag group.
                                if last_tags_batch_info[tag_label]["last_timestamp"] != 0:
                                    exp_next_time = round((last_tags_batch_info[tag_label]["last_timestamp"] + tag_batch_sample_time), 6)

                                    if tag_batch_first_time > exp_next_time + 0.5: # If gap is more than 0.5 seconds, start a new file.
                                        last_tags_batch_info[tag_label]["tag_group_id"] += 1
                                        last_tags_batch_info[tag_label]["first_chunk"] = True

                                # Update the last timestamp for the current tag group.
                                last_tags_batch_info[tag_label]["last_timestamp"] = tag_batch_last_time
                                group_id = last_tags_batch_info[tag_label]["tag_group_id"]
                                
                                # Save the current group of data to a WAV file, either by creating a new file or appending to an existing one.
                                if group_id in wav_files[tag_label]:
                                    wav_files[tag_label][group_id] = save_tag_group_in_wav(group_df, tag_label, last_tags_batch_info, tags_columns, output_folder, wav_files[tag_label][group_id])
                                else:
                                    wav_files[tag_label][group_id] = save_tag_group_in_wav(group_df, tag_label, last_tags_batch_info, tags_columns, output_folder)
                else:
                    # If no data frame is returned, mark the last chunk.
                    is_last_chunk = True
                    log.info("--> Conversion completed")

            # Close all the WAV files that were created during the conversion process.
            for tag_label, group_files in wav_files.items():
                for group_id, wav_file in group_files.items():
                    HSDatalogConverter.wav_close(wav_file)
            log.info(f"--> {comp_name} Conversion completed")

        except Exception as err:
            # Log any exceptions that occur during the conversion process and ensure all resources are released.
            log.exception(err)
            # Close any WAV files that may have been left open due to an exception.
            for tag_label, group_files in wav_files.items():
                for group_id, wav_file in group_files.items():
                    if wav_file is not None:
                        HSDatalogConverter.wav_close(wav_file)
        
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)
    
    @staticmethod
    def convert_dat_to_wav(hsd, component, start_time, end_time, output_folder, split_per_tags = False, chunk_size = DEFAULT_SAMPLES_CHUNK_SIZE):
        """
        Converts sensor data from DAT format to WAV format.

        :param hsd: An instance of HSDatalog, which provides methods for data retrieval.
        :param component: A dictionary with a single component's name as the key and its status as the value.
        :param start_time: The start time for the conversion (the closest greater timestamp will be selected).
        :param end_time: The end time for the conversion (the closest greater timestamp will be selected).
        :param output_folder: The directory where the converted WAV files will be saved.
        :param split_per_tags: [Optional] Boolean indicating if the data should be split into separate WAV files based on tags. Defaut is False
        :param chunk_size: [Optional] The number of samples per data chunk during conversion. Default is HSDatalog.DEFAULT_SAMPLES_CHUNK_SIZE = 10M Samples.
        
        Depending on the 'split_per_tags' flag, this method will either convert the entire component's data into a single WAV file
        or create separate WAV files for each tag group within the data.
        """
        
        # Extract the component name and status from the input dictionary.
        c_name = list(component.keys())[0]
        c_status = component[c_name]
        
        # Set the sensitivity for the conversion to 1 to force raw data processing.
        c_status["sensitivity"] = 1
        
        # If 'split_per_tags' is True, convert the data into separate WAV files for each tag group.
        if split_per_tags:
            HSDatalog.__convert_to_wav_by_tags_batch(hsd, c_name, c_status, start_time, end_time, output_folder, chunk_size)
        else: # Otherwise, convert the entire component's data into a single WAV file.
            HSDatalog.__convert_to_wav_batch(hsd, c_name, c_status, start_time, end_time, output_folder, chunk_size)
    
    #TODO Add comments
    @staticmethod
    def plot(hsd, component, start_time = 0, end_time = -1, label = None, which_tags = [], subplots = False, raw_data = False, fft_plots = False):
        comp_name = list(component.keys())[0]
        comp_status = component[comp_name]
        ioffset = comp_status.get("ioffset",0)
        c_type = None
        if "c_type" in comp_status:
            c_type = comp_status["c_type"]
            comp_status["is_first_chunk"] = True
            if c_type == ComponentTypeEnum.SENSOR.value or c_type == ComponentTypeEnum.ACTUATOR.value:
                hsd.get_sensor_plot(comp_name, comp_status, start_time, end_time, label = label, which_tags = which_tags, subplots = subplots, raw_flag = raw_data, fft_plots = fft_plots)
            elif c_type == ComponentTypeEnum.ALGORITHM.value:
                hsd.get_algorithm_plot(comp_name, comp_status, start_time, end_time, label = label, which_tags = which_tags, subplots = subplots, raw_flag = raw_data)
        else:
            if c_type is None:
                log.exception("Missing \"c_type\" value in your device status")
            raise
        # Reset the status conversion side information for the component status.
        HSDatalog.reset_status_conversion_side_info(comp_status, ioffset)

    @staticmethod
    def get_sensor_file_path(sensor_name: str, output_folder: str):
        """
        Constructs the file path for a sensor's data file within a specified output folder.

        :param sensor_name: The name of the sensor for which the file path is being constructed.
        :param output_folder: The directory where the sensor's data file will be located.
        :return: The full file path for the sensor's data file.
        
        This method simply joins the output folder path with the sensor name to create a file path.
        """
        return os.path.join(output_folder, sensor_name)
    
    @staticmethod
    def get_ssensor_file_name(hsd, ssensor_name, ssensor_type, output_folder):
        """
        Constructs the file path for a smart sensor's data file within a specified output folder.
        
        This function is deprecated and will be removed in a future version.
        Use '__get_sensor_file_path' instead.

        :param hsd: An instance of HSDatalog, which provides methods for data retrieval.
        :param ssensor_name: The name of the smart sensor for which the file path is being constructed.
        :param ssensor_type: The type of the smart sensor.
        :param output_folder: The directory where the smart sensor's data file will be located.
        :return: The full file path for the smart sensor's data file.
        """
        # Issue a deprecation warning to inform users that this function is deprecated.
        warn(
            "get_ssensor_file_name is deprecated and will be removed in a future version. "
            "Use '__get_sensor_file_path' instead.",
            DeprecationWarning
        )
        # Check if the HSDatalog instance is of version 2.
        if isinstance(hsd, HSDatalog_v2):
            # For HSDatalog_v2, return the file path using only the smart sensor name.
            return os.path.join(output_folder, "{}".format(ssensor_name))
        else:
            # For HSDatalog_v1, include both the smart sensor name and type in the file path.
            return os.path.join(output_folder, "{}_{}".format(ssensor_name, ssensor_type))
    
    @staticmethod
    def get_wav_file_path(hsd, comp_name, output_folder):
        """
        Constructs the file path for a component's WAV file within a specified output folder.

        :param hsd: An instance of HSDatalog, which provides methods for data retrieval.
        :param comp_name: The name of the component (sensor or device) for which the WAV file path is being constructed.
        :param output_folder: The directory where the WAV file will be saved.
        :return: The full file path for the component's WAV file.
        
        This method is used to generate a standardized file path for storing WAV files by appending the '.wav' extension
        to the component name and joining it with the output folder path.
        """
        return os.path.join(output_folder, f"{comp_name}.wav")

    @staticmethod
    def get_wav_file_name(hsd, ssensor_name, ssensor_type, output_folder):
        """
        Constructs the file path for a WAV file based on the smart sensor's name and type within a specified output folder.
        
        This function is deprecated and will be removed in a future version.
        Use 'get_ssensor_file_name' instead.

        :param hsd: An instance of HSDatalog, which provides methods for data retrieval.
        :param ssensor_name: The name of the smart sensor for which the WAV file path is being constructed.
        :param ssensor_type: The type of the smart sensor.
        :param output_folder: The directory where the WAV file will be located.
        :return: The full file path for the WAV file.
        """
        # Issue a deprecation warning to inform users that this function is deprecated.
        warn(
            "get_wav_file_name is deprecated and will be removed in a future version. "
            "Use 'get_ssensor_file_name' instead.",
            DeprecationWarning
        )
        # Check if the HSDatalog instance is of version 2.
        if isinstance(hsd, HSDatalog_v2):
            # For HSDatalog_v2, return the file path using only the smart sensor name with a '.wav' extension.
            return os.path.join(output_folder, "{}.wav".format(ssensor_name))
        else:
            # For HSDatalog_v1, include both the smart sensor name and type in the file path with a '.wav' extension.
            return os.path.join(output_folder, "{}_{}.wav".format(ssensor_name, ssensor_type))
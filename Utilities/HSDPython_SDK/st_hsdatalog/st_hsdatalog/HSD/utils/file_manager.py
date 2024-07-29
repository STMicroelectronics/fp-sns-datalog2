
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

class FileManager:
    
    @staticmethod
    def decode_file_name(file_name):
        sn = file_name.split(".")[0].split("_")
        [sensor_name, sensor_type] = ["_".join(sn[:-1]),sn[-1]]
        return sensor_name, sensor_type

    @staticmethod
    def encode_file_name(sensor_name, sub_sensor_type = None, ext = '.dat'):
        if sub_sensor_type is None:
            file_name = sensor_name + ext
        else:
            file_name = sensor_name + '_' + sub_sensor_type + ext
        return file_name

    @staticmethod
    def get_file_names_from_model(device):
        """
        list of files that are supposed to be in the datalog based on DeviceConfig.json
        """
        data_files_list = []
        for s in device.sensor:
            sss_list = s.sensor_status.sub_sensor_status
            for i, ssd in enumerate(s.sensor_descriptor.sub_sensor_descriptor):
                if sss_list[i].is_active:
                    file_name = FileManager.encode_file_name(s.name,ssd.sensor_type)
                    data_files_list.append(file_name)
        return data_files_list

    @staticmethod
    def get_dat_files_from_folder(acq_folder_path):
        file_names = []
        with os.scandir(acq_folder_path) as listOfEntries:
            for entry in listOfEntries:
                # print all entries that are files
                if entry.is_file():
                    if entry.name.endswith('.dat') and "_checked" not in entry.name:
                        file_names.append(entry.name)
        return file_names
    
    def find_file(name:str, path:str):
        """
        Private method to find a file by name within a given path.

        :param name: The name of the file to find.
        :param path: The directory path to search for the file.
        :return: The full path to the file if found, otherwise None.
        """
        # Walk through the directory tree starting at 'path'.
        for root, dirs, files in os.walk(path):
            # If the file is found, return the full path to the file.
            if name in files:
                return os.path.join(root, name)
        # If the file is not found, return None.
        return None
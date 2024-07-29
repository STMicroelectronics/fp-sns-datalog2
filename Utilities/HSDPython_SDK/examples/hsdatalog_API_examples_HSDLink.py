#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_API_examples_HSDLink.py
#  * @author  SRA
#  * @version 1.0.0
#  * @date    11-Jul-2024
# *****************************************************************************
#
#                   Copyright (c) 2020 STMicroelectronics.
#                             All rights reserved
#
#   This software component is licensed by ST under BSD-3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause

import os
import time
from threading import Event
from st_hsdatalog.HSD_link.HSDLink import HSDLink, SensorAcquisitionThread
from st_pnpl.PnPLCmd import PnPLCMDManager

def main():
    """
    This is the main function that demonstrates the usage of the HSDLink API.
    NOTE: It is necessary to connect a compatible device to the PC to run the script.

    It performs various operations such as retrieving device information, setting sensor parameters,
    uploading UCF files, and updating device configuration.

    Usage:
    1. Set the `acquisition_folder` variable to the path of the acquisition folder you want to analyze.
    2. Create an instance of HSDLink.
    3. Create the appropriate HSDLink instance based on the connected board.
    4. Retrieve the version of the HSDLink instance.
    5. Get the list of connected devices.
    6. Retrieve device presentation, identity, alias, and information.
    7. Retrieve acquisition information and folder path.
    8. Update the base acquisition folder.
    9. Retrieve firmware information.
    10. Set acquisition name and description.
    11. Set acquisition information.
    12. Get sensor list, counts, and names.
    13. Get component status and algorithm information.
    14. Get and set sensor enabled status, ODR, FS, AOP, SPTS, and initial offset.
    15. Retrieve available tag classes, tags, and maximum tags per acquisition.
    16. Get updated MLC and ISPU sensor lists.
    17. Upload MLC and ISPU UCF files.
    18. Set RTC time on the device.
    19. Start an acquisition process to log sensor data for a specific duration and save it to a file.
        During the acquisition, the script will set and unset software tags.

    Note: Update the file paths for UCF files and device configuration as per your setup.
          Update also the sensor names, sensor parameters and the acquisition duration as per your requirements.

    """
def main():

    
    # change the "path/to/your/acquisition_folder" with the path of the acquisition folder you want to analyze
    acquisition_folder = "path/to/your/acquisition_folder"

    # Create an instance of HSDLink
    hsd_link = HSDLink()

    # Create the appropriate HSDLink instance based on the connected board
    hsd_link_instance = hsd_link.create_hsd_link(dev_com_type='st_hsd', acquisition_folder=acquisition_folder)

    if hsd_link is None:
        print("No compatible devices connected.")
        return

    # Get the version of the HSDLink instance
    version = hsd_link.get_version(hsd_link_instance)
    print(f"HSDLink Version: {version}")
    if version != "High Speed Datalog DLL v2":
        print("this is a test script for DATALOG2. Wrong FW detected.")
        quit()

    # Get the list of connected devices
    devices = hsd_link.get_devices(hsd_link_instance)
    print(f"Connected Devices: {devices}")

    if not devices:
        print("No devices found.")
        return
    
    # Use the first connected device for demonstration
    device_id = 0

    # Get Device Presentation String
    device_presentation = hsd_link.get_device_presentation_string(hsd_link_instance, device_id)
    print(f"Device Presentation: {device_presentation}")

    # Get device identity
    device_identity = hsd_link.get_device_identity(hsd_link_instance, device_id)
    print(f"Device Identity: {device_identity}")
    
    # Get device alias
    device_alias = hsd_link.get_device_alias(hsd_link_instance, device_id)
    print(f"Device Alias: {device_alias}")

    # Get device information
    device_info = hsd_link.get_device_info(hsd_link_instance, device_id)
    print(f"Device Info: {device_info}")

    # Get acquisition information
    acquisition_info = hsd_link.get_acquisition_info(hsd_link_instance, device_id)
    print(f"Acquisition Info: {acquisition_info}")

    # Get acquisition folder
    acquisition_folder_path = hsd_link.get_acquisition_folder(hsd_link_instance)
    print(f"Acquisition Folder: {acquisition_folder_path}")

    # Update base acquisition folder
    new_base_acquisition_folder = "new_base_acquisition_folder"
    hsd_link.update_base_acquisition_folder(hsd_link_instance, new_base_acquisition_folder)
    acquisition_folder_path = hsd_link.get_acquisition_folder(hsd_link_instance)
    print(f"Updated Acquisition Folder: {acquisition_folder_path}")

    # Get firmware information
    firmware_info = hsd_link.get_firmware_info(hsd_link_instance, device_id)
    print(f"Firmware Info: {firmware_info}")

    # Set acquisition name
    hsd_link.set_acquisition_name(hsd_link_instance, device_id, "New Name")
    # Set acquisition description
    hsd_link.set_acquisition_description(hsd_link_instance, device_id, "New Description")
    acquisition_info = hsd_link.get_acquisition_info(hsd_link_instance, device_id)
    print(f"Updated Acquisition Info: {acquisition_info}")
    # Set acquisition information
    hsd_link.set_acquisition_info(hsd_link_instance, device_id, "New Acquisition", "Description of the acquisition")
    acquisition_info = hsd_link.get_acquisition_info(hsd_link_instance, device_id)
    print(f"Updated Acquisition Info: {acquisition_info}")

    # Get sensor list
    sensor_list = hsd_link.get_sensor_list(hsd_link_instance, device_id)
    print(f"Sensor List: {sensor_list}")
    # Get Sensor Components number
    sensor_counts = hsd_link.get_sensors_count(hsd_link_instance, device_id)
    print(f"Sensor Counts: {sensor_counts}")
    # Get Sensor Components names list
    sensor_names = hsd_link.get_sensors_names(hsd_link_instance, device_id)
    print(f"Sensor Names: {sensor_names}")
    print(hsd_link.get_sensors_names(hsd_link_instance, device_id))

    if not sensor_list:
        print("No sensors found.")
        return

    # Use the first sensor names for demonstration
    comp_name = sensor_names[0]
    
    # Get Component Status
    comp_status = hsd_link.get_component_status(hsd_link_instance, device_id, comp_name)
    print(f"Component Status: {comp_status}")

    # Get Algorithm Components Status. (ComponentType = c_type = 1 = ALGORITHM)
    algorithm_list = hsd_link.get_algorithm_list(hsd_link_instance, device_id)
    print(f"Algorithm List: {algorithm_list}")
    # Get Algorithm Components number
    algorithms_count = hsd_link.get_algorithms_count(hsd_link_instance, device_id)
    print(f"Algorithm Counts: {algorithms_count}")
    # Get Algorithm Components names list
    algorithms_names = hsd_link.get_algorithms_names(hsd_link_instance, device_id)
    print(f"Algorithm Names: {algorithms_names}")

    # Use the first sensor for demonstration
    sensor_name, _ = next(iter(sensor_list.items()))
    # Get sensor enabled status
    sensor_enabled = hsd_link.get_sensor_enabled(hsd_link_instance, device_id, sensor_name=sensor_name)
    print(f"Sensor Enabled: {sensor_enabled}")

    # Set sensor enabled status (True in this example)
    hsd_link.set_sensor_enable(hsd_link_instance, device_id, True, sensor_name=sensor_name)

    # Get sensor ODR Enum id. (The correspondent value can be obtained from the odr Enum property defined in the Device Template Model)
    sensor_odr = hsd_link.get_sensor_odr(hsd_link_instance, device_id, sensor_name=sensor_name)
    print(f"Sensor ODR: {sensor_odr}")

    # Set sensor ODR Enum id. (The correspondent value can be obtained from the odr Enum property defined in the Device Template Model)
    hsd_link.set_sensor_odr(hsd_link_instance, device_id, 6, sensor_name=sensor_name)

    # Get sensor FS. (The correspondent value can be obtained from the odr Enum property defined in the Device Template Model)
    sensor_fs = hsd_link.get_sensor_fs(hsd_link_instance, device_id, sensor_name=sensor_name)
    print(f"Sensor FS: {sensor_fs}")

    # Set sensor ODR (The correspondent value can be obtained from the odr Enum property defined in the Device Template Model)
    hsd_link.set_sensor_fs(hsd_link_instance, device_id, 2, sensor_name=sensor_name)

    # Get sensor AOP (Only for Microphones). (The correspondent value can be obtained from the odr Enum property defined in the Device Template Model)
    try:
        sensor_aop = hsd_link.get_sensor_aop(hsd_link_instance, device_id, sensor_name=sensor_name)
        print(f"Sensor AOP: {sensor_aop}")

        # Set sensor AOP (Only for Microphones). (The correspondent value can be obtained from the odr Enum property defined in the Device Template Model)
        hsd_link.set_sensor_aop(hsd_link_instance, device_id, 3, comp_name)
    except:
        print("Sensor AOP is not available for this sensor.")

    # Get sensor SPTS
    sensor_spts = hsd_link.get_sensor_spts(hsd_link_instance, device_id, sensor_name=sensor_name)
    print(f"Sensor SPTS: {sensor_spts}")

    # Set sensor SPTS
    hsd_link.set_sensor_spts(hsd_link_instance, device_id, 1000, sensor_name=sensor_name)

    # Get initial offset
    ioffset = hsd_link.get_sensor_initial_offset(hsd_link_instance, device_id, sensor_name=sensor_name)
    print(f"Initial Offset: {ioffset}")

    # Get available Tag classes information
    tags_info = hsd_link.get_tags_info(hsd_link_instance, device_id)
    print(f"Tags Info: {tags_info}")

    # Get available Tag list (same as tags_info component)
    tags_list = hsd_link.get_tag_list(hsd_link_instance, device_id)
    print(f"Tags List: {tags_list}")

    # Get available software tag classes
    sw_tag_classes = hsd_link.get_sw_tag_classes(hsd_link_instance, device_id)
    print(f"Software Tag Classes: {sw_tag_classes}")

    # Get available hardware tag classes
    hw_tag_classes = hsd_link.get_hw_tag_classes(hsd_link_instance, device_id)
    print(f"Hardware Tag Classes: {hw_tag_classes}")

    # Get maximum tags per acquisition
    max_tags_per_acq = hsd_link.get_max_tags_per_acq(hsd_link_instance, device_id)
    print(f"Max Tags Per Acquisition: {max_tags_per_acq}")

    # Get updated MLC sensor list
    updated_mlc_sensor_list = hsd_link.get_updated_mlc_sensor_list(hsd_link_instance, device_id, None)
    print(f"Updated MLC Sensor List: {updated_mlc_sensor_list}")

    # Get updated ISPU sensor list
    updated_ispu_sensor_list = hsd_link.get_updated_ispu_sensor_list(hsd_link_instance, device_id, None)
    print(f"Updated ISPU Sensor List: {updated_ispu_sensor_list}")

    # Upload MLC UCF file (Check in your setup there is a MLC sensor available and update the "path/to/mlc.ucf" to a valid MLC UCF file)
    # One MLC component per board is supported
    hsd_link.upload_mlc_ucf_file(hsd_link_instance, device_id, "path/to/mlc.ucf")

    # Upload ISPU UCF file (Check in your setup there is a ISPU sensor available and update the "path/to/ispu.ucf" to a valid ISPU UCF file)
    # One ISPU component per board is supported
    hsd_link.upload_ispu_ucf_file(hsd_link_instance, device_id, "path/to/ispu.ucf")

    # Set RTC time on the device
    hsd_link.set_RTC(hsd_link_instance, device_id)

    # Update device configuration
    # Update the device_config_path variable with a valid device_config.json file
    try:
        device_config_path = "path/to/device_config.json"
        hsd_link.update_device(hsd_link_instance, device_id, device_config_path)
    except:
        print(f"Error in device status update. Check your provided {device_config_path} file.")

    # Example tag classes configuration (using IDs)
    sw_tags = hsd_link.get_sw_tag_classes(hsd_link_instance, device_id)
    print(f"SW Tag Classes: {sw_tags}")
    for i in range(len(sw_tags)):
        # Get SW Tag Class by ID
        print(hsd_link.get_sw_tag_class_by_id(hsd_link_instance, device_id, i))
        # Get SW Tag Class Label by ID
        print(hsd_link.get_sw_tag_class_label_by_id(hsd_link_instance, device_id, i))
        # Get SW Tag Class Enabled by ID
        print(hsd_link.get_sw_tag_class_enabled_by_id(hsd_link_instance, device_id, i))
        if i == 0:
            # Set SW Tag Class Enabled status by ID
            print(hsd_link.set_sw_tag_class_label_by_id(hsd_link_instance, device_id, i,"Normal"))
            # Get SW Tag Class Label by ID
            print(hsd_link.get_sw_tag_class_label_by_id(hsd_link_instance, device_id, i))
            # Set SW Tag Class Enabled status by ID
            print(hsd_link.set_sw_tag_class_enabled_by_id(hsd_link_instance, device_id, i,True))
        elif i == 1:
            # Set SW Tag Class Enabled status by ID
            print(hsd_link.set_sw_tag_class_label_by_id(hsd_link_instance, device_id, i,"Anormal"))
            # Get SW Tag Class Label by ID
            print(hsd_link.get_sw_tag_class_label_by_id(hsd_link_instance, device_id, i))
            # Set SW Tag Class Enabled status by ID
            print(hsd_link.set_sw_tag_class_enabled_by_id(hsd_link_instance, device_id, i,True))
        else:
            # Set SW Tag Class Enabled status by ID
            print(hsd_link.set_sw_tag_class_enabled_by_id(hsd_link_instance, device_id, i,False))
            # Get SW Tag Class Label by ID
            print(hsd_link.get_sw_tag_class_enabled_by_id(hsd_link_instance, device_id, i))
    # Get SW Tag Classes
    sw_tags = hsd_link.get_sw_tag_classes(hsd_link_instance, device_id)
    print(sw_tags)

    # Example tag classes configuration (using properties names)
    for swt in sw_tags:
        # Get SW Tag Class by name (not Label)
        print(hsd_link.get_sw_tag_class(hsd_link_instance, device_id, swt))
        # Get SW Tag Class Label by name (not Label)
        print(hsd_link.get_sw_tag_class_label(hsd_link_instance, device_id, swt))
        # Get SW Tag Class Enabled status by name (not Label)
        print(hsd_link.get_sw_tag_class_enabled(hsd_link_instance, device_id, swt))
        if swt == "sw_tag0":
            # Set SW Tag Class Enabled status by name (not Label)
            print(hsd_link.set_sw_tag_class_label(hsd_link_instance, device_id, swt,"Normal"))
            # Get SW Tag Class Label by name (not Label)
            print(hsd_link.get_sw_tag_class_label(hsd_link_instance, device_id, swt))
            # Set SW Tag Class Enabled status by name (not Label)
            print(hsd_link.set_sw_tag_class_enabled(hsd_link_instance, device_id, swt,True))
        elif swt == "sw_tag1":
            # Set SW Tag Class Enabled status by name (not Label)
            print(hsd_link.set_sw_tag_class_label(hsd_link_instance, device_id, swt,"Anormal"))
            # Get SW Tag Class Label by name (not Label)
            print(hsd_link.get_sw_tag_class_label(hsd_link_instance, device_id, swt))
            # Set SW Tag Class Enabled status by name (not Label)
            print(hsd_link.set_sw_tag_class_enabled(hsd_link_instance, device_id, swt,True))
        else:
            # Set SW Tag Class Enabled status by name (not Label)
            print(hsd_link.set_sw_tag_class_enabled(hsd_link_instance, device_id, swt,False))
            # Get SW Tag Class Label by name (not Label)
            print(hsd_link.get_sw_tag_class_enabled(hsd_link_instance, device_id, swt))
    # Get SW Tag Classes
    sw_tags = hsd_link.get_sw_tag_classes(hsd_link_instance, device_id)
    print(sw_tags)
    
    # Half-managed API: Set properties
    print(hsd_link.set_property(hsd_link_instance, device_id, False,"stts22h_temp","enable"))
    print(hsd_link.get_component_status(hsd_link_instance, device_id, "stts22h_temp"))
    print(hsd_link.set_property(hsd_link_instance, device_id, "New_Tag_Class","tags_info","sw_tag2","label"))
    print(hsd_link.get_component_status(hsd_link_instance, device_id, "tags_info"))
    print(hsd_link.set_property(hsd_link_instance, device_id, True,"tags_info","sw_tag2","enabled"))
    print(hsd_link.get_component_status(hsd_link_instance, device_id, "tags_info"))

    # Non-managed API: Send manually created messages
    # Get Device status
    message = "{\"get_status\":\"all\"}"
    print(hsd_link.send_command(hsd_link_instance, device_id, message))
    message = PnPLCMDManager.create_get_device_status_cmd()
    print(hsd_link.send_command(hsd_link_instance, device_id, message))
    
    # Get Component status (e.g. firmware_info)
    component_name = "firmware_info"
    message = "{\"get_status\":\"firmware_info\"}"
    print(hsd_link.send_command(hsd_link_instance, device_id, message))
    message = PnPLCMDManager.create_get_component_status_cmd(component_name)
    print(hsd_link.send_command(hsd_link_instance, device_id, message))

    # Set properties
    message = "{\"stts22h_temp\":{\"enable\":false}}"
    message = PnPLCMDManager.create_set_property_cmd("stts22h_temp","enable",False)
    print(hsd_link.send_command(hsd_link_instance, device_id, message))
    message = "{\"tags_info\":{\"sw_tag2\":{\"label\":\"New_Tag_Class\"}}}"
    message = PnPLCMDManager.create_set_property_cmd("tags_info",["sw_tag2","label"],"New_Tag_Class")
    print(hsd_link.send_command(hsd_link_instance, device_id, message)) 

    # Command
    message = PnPLCMDManager.create_command_cmd("log_controller","set_time")
    print(hsd_link.send_command(hsd_link_instance, device_id, message))

    # Set RTC time on the device
    hsd_link.set_RTC(hsd_link_instance, device_id)

    # Get the first sensor name. Change the index to get another sensor
    sensor_names = hsd_link.get_sensors_names(hsd_link_instance, device_id)
    
    # Start sensor acquisition thread
    threads_stop_flags = []
    sensor_data_files = []

    # Start data log progess. The log will be stopped after 12 seconds in this example
    # The log will be saved in the acquisition folder and the device_config.json and acq_info.json
    # files will be saved in the same folder. The device_config.json and acq_info.json files can be 
    # saved in a different folder by providing the path as an argument
    print(hsd_link.start_log(hsd_link_instance, device_id))

    for sn in sensor_names:
        is_enabled = hsd_link.get_sensor_enabled(hsd_link_instance, device_id, sn)
        if(is_enabled):
            # Create sensor data file
            sensor_data_file_path = os.path.join(hsd_link.get_acquisition_folder(hsd_link_instance),(sn + ".dat"))
            # Open sensor data file
            sensor_data_file = open(sensor_data_file_path, "wb+")
            # Append sensor data file to the list
            sensor_data_files.append(sensor_data_file)
            # Create sensor acquisition thread
            stopFlag = Event()
            threads_stop_flags.append(stopFlag)
            thread = SensorAcquisitionThread(stopFlag, hsd_link_instance, sensor_data_file, device_id, sn)
            thread.start()
        else:
            print(f"[WARNING]: Sensor {sn} is not enabled.")

    # Set software tag 0 to On (Enter into the Tag Class 0 section)
    hsd_link.set_sw_tag_on(hsd_link_instance, device_id, 0)
    time.sleep(1)
    # Set software tag 0 to Off (Exit from the Tag Class 0 section)
    hsd_link.set_sw_tag_off(hsd_link_instance, device_id, 0)
    time.sleep(1)
    # Set software tag 1 to On (Enter into the Tag Class 0 section)
    hsd_link.set_sw_tag_on(hsd_link_instance, device_id, 1)
    time.sleep(1)
    # Set software tag 1 to Off (Exit from the Tag Class 0 section)
    hsd_link.set_sw_tag_off(hsd_link_instance, device_id, 1)
    
    # Wait for 12 seconds (the log will be stopped after 12 seconds in this example. Change the time as per your requirement)
    time.sleep(12)

    # Stop sensor acquisition thread
    for sf in threads_stop_flags:
        sf.set()
    # Close sensor data files
    for f in sensor_data_files:
        f.close()

    # Stop data log process
    print(hsd_link.stop_log(hsd_link_instance, device_id))

    # Save JSON device file
    hsd_link.save_json_device_file(hsd_link_instance, device_id)
    # Update the "path/to/device_config.json" with a valid path to save the device_config.json file in that path
    hsd_link.save_json_device_file(hsd_link_instance, device_id, "path/to/save/device_config.json")
    # Save JSON acquisition info file
    hsd_link.save_json_acq_info_file(hsd_link_instance, device_id)
    # Update the "path/to/acq_info.json" with a valid path to save the acq_info.json file in that path
    hsd_link.save_json_acq_info_file(hsd_link_instance, device_id, "path/to/save/acq_info.json")

    print("\n---> End of the demonstration.")

if __name__ == "__main__":
    main()

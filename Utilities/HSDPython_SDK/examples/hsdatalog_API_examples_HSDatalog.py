#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_API_examples_HSDatalog.py
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

from st_hsdatalog.HSD.HSDatalog import HSDatalog
import matplotlib.pyplot as plt
import os

def main():
    """
    Entry point of the program.
    
    This function performs various operations using the HSDatalog class.
    It validates the HSD folder (HSDv1 or HSDv2), presents sensor and device
    information, gets acquisition and firmware information, and performs
    data conversion and plotting.
    """

    # change the "path/to/your/acquisition_folder" with the path of the acquisition folder you want to analyze
    # acquisition_folder = "path/to/your/acquisition_folder"
    acquisition_folder = "20240712_15_18_51"

    # Create an instance of HSDatalog
    hsd = HSDatalog()

    # Validate the HSD folder and determine the version
    hsd_version = hsd.validate_hsd_folder(acquisition_folder)
    print(f"HSD Version: {hsd_version}\n")

    # Create the appropriate HSDatalog instance based on the folder content
    hsd_instance = hsd.create_hsd(acquisition_folder=acquisition_folder)

    # Get device information
    device_info = hsd.get_device_info(hsd_instance)
    print(f"\nDevice Info: {device_info}")
    # Present the device information
    hsd.present_device_info(hsd_instance)
    
    # Get sensor list
    sensor_list = hsd.get_sensor_list(hsd_instance)
    print(f"\nSensor List: {sensor_list}\n")
    # Get sensor list with filters
    print(hsd.get_sensor_list(hsd_instance, only_active = True),"\n") # Only active sensors
    print(hsd.get_sensor_list(hsd_instance, type_filter = "acc"),"\n") # Only accelerometers
    print(hsd.get_sensor_list(hsd_instance, type_filter = "acc", only_active = True),"\n") # Only active accelerometers
    # Present the list of sensors
    hsd.present_sensor_list(hsd_instance)

    # Get the name of the first sensor in the obtained sensor list (change the index to get another sensor)
    sensor_name = hsd.get_sensor_name(hsd_instance, sensor_list[0])
    print(f"\nSensor Name: {sensor_name}\n")
    # Get specific sensor by name
    sensor = hsd.get_sensor(hsd_instance, sensor_name)
    print(f"Sensor: {sensor}")
    # Present the sensor information
    hsd.present_sensor(hsd_instance, sensor)

    # Get and present software tag classes
    sw_tag_classes = hsd.get_sw_tag_classes(hsd_instance)
    print(f"\nSoftware Tag Classes: {sw_tag_classes}\n")
    hsd.present_sw_tag_classes(hsd_instance, sw_tag_classes)

    # Get and present hardware tag classes
    hw_tag_classes = hsd.get_hw_tag_classes(hsd_instance)
    print(f"\nHardware Tag Classes: {hw_tag_classes}\n")
    hsd.present_hw_tag_classes(hsd_instance, hw_tag_classes)

    # Get the list of .dat files
    dat_files = hsd.get_dat_file_list(hsd_instance)
    print(f".dat Files: {dat_files}\n")

    # Get and Set device configuration
    device_config = hsd.get_device(hsd_instance)
    print(f"Device Configuration: {device_config}\n")
    hsd.set_device(hsd_instance, device_config, 0, False)

    # Get acquisition information
    acquisition_info = hsd.get_acquisition_info(hsd_instance)
    print(f"Acquisition Info: {acquisition_info}\n")

    # Get firmware information
    firmware_info = hsd.get_firmware_info(hsd_instance)
    print(f"Firmware Info: {firmware_info}\n")

    # Ask for a component
    component = hsd.ask_for_component(hsd_instance, only_active=True)
    print(f"Selected Sensor: {component}\n")

    # Filter sensors by name
    sensor_name = hsd.get_sensor_name(hsd_instance, component)
    print(f"Selected Sensor name: {sensor_name}\n")
    filtered_sensors = hsd.filter_sensor_list_by_name(hsd_instance, sensor_name)
    print(f"Filtered Sensors: {filtered_sensors}\n")

    # Get sensor properties
    sensor_name = hsd.get_sensor_name(hsd_instance, sensor)
    sensor_sensitivity = hsd.get_sensor_sensitivity(hsd_instance, sensor)
    sensor_odr = hsd.get_sensor_odr(hsd_instance, sensor)
    sensor_measodr = hsd.get_sensor_measodr(hsd_instance, sensor)
    sensor_fs = hsd.get_sensor_fs(hsd_instance, sensor)
    sensor_spts = hsd.get_sensor_spts(hsd_instance, sensor)
    # Get sensor properties by name
    sensor_sensitivity_by_name = hsd.get_sensor_sensitivity_by_name(hsd_instance, sensor_name)
    sensor_odr_by_name = hsd.get_sensor_odr_by_name(hsd_instance, sensor_name)
    sensor_measodr_by_name = hsd.get_sensor_measodr_by_name(hsd_instance, sensor_name)
    sensor_fs_by_name = hsd.get_sensor_fs_by_name(hsd_instance, sensor_name)
    sensor_spts_by_name = hsd.get_sensor_spts_by_name(hsd_instance, sensor_name)

    print(f"Sensor Name: {sensor_name}")
    print(f"Sensor Sensitivity: {sensor_sensitivity}")
    print(f"Sensor Sensitivity (by name): {sensor_sensitivity_by_name}")
    print(f"Sensor ODR: {sensor_odr}")
    print(f"Sensor ODR (by name): {sensor_odr_by_name}")
    print(f"Sensor MeasODR: {sensor_measodr}")
    print(f"Sensor MeasODR (by name): {sensor_measodr_by_name}")
    print(f"Sensor FS: {sensor_fs}")
    print(f"Sensor FS (by name): {sensor_fs_by_name}")
    print(f"Sensor SPTS: {sensor_spts}")
    print(f"Sensor SPTS (by name): {sensor_spts_by_name}")

    # Get all components
    all_components = hsd.get_all_components(hsd_instance)
    print(f"\nAll Components: {all_components}\n")

    # Ask for a component
    component = hsd.ask_for_component(hsd_instance, only_active=True)
    print(f"Selected Component: {component}\n")
    component_name = hsd.get_component_name(hsd_instance, component)
    print(f"Selected Component name: {component}\n")

    # Get a specific component by name
    component = hsd.get_component(hsd_instance, component_name)
    print(f"Component: {component}\n")

    # Get acquisition label classes
    acquisition_label_classes = hsd.get_acquisition_label_classes(hsd_instance)
    print(f"Acquisition Label Classes: {acquisition_label_classes}\n")

    # Get time tags
    time_tags = hsd.get_time_tags(hsd_instance)
    print(f"Time Tags: {time_tags}\n")

    # Get time tags with class
    if len(acquisition_label_classes) > 0:
        # Get the name of the first label class in the obtained acquisition label class list (change the index to get another class)
        tag_class_name = acquisition_label_classes[0]
        time_tags_with_class = hsd.get_time_tags(hsd_instance, tag_class_name)
        print(f"Time Tags class {tag_class_name}: {time_tags_with_class}\n")

    # Get data and timestamps
    data_and_timestamps = hsd.get_data_and_timestamps(hsd_instance, component)
    print(f"Data and Timestamps: {data_and_timestamps}\n")

    # Get data and timestamps by name
    data_and_timestamps_by_name = hsd.get_data_and_timestamps_by_name(hsd_instance, component_name)
    print(f"Data and Timestamps by Name {component_name}: {data_and_timestamps_by_name}\n")

    # Example of dataframe extraction (different ways to extract dataframes)
    total = 6 #seconds
    
    # Automatized way: Get data as a dataframe with default chunk size (10M samples).
    dataframe = hsd.get_dataframe(hsd_instance, component, start_time=0, end_time=total)[0]
    print(dataframe)

    # Fixed chunk size: Get data as a dataframe with specific chunk size (Extracted dataframes will have the same length).
    # NOTE: chunck size must be >= samples_per_timestamp
    sensor_spts = hsd.get_sensor_spts(hsd_instance, component)
    print(sensor_spts)
    dfs_samples = hsd.get_dataframe(hsd_instance, component, start_time=0, end_time=total, chunk_size=sensor_spts)
    for df in dfs_samples:
        print(df)

    # Extract data using time boundaries: Get data as a dataframe specifying start and end times.
    for i in range(total):
        df = hsd.get_dataframe(hsd_instance, component, start_time=i, end_time=i+1)[0]
        print(df)

    # [UNCOMMENT This to check an acquisition folder containing Dummy data] - Check dummy data
    # hsd.check_dummy_data(hsd_instance, component, start_time=0, end_time=-1)

    output_folder = "HSD_test_output_folder"
    # Create the output folder if it doesn't exist
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # Convert data to various formats
    print(f"\nConverting data to various formats...")
    print(f"To CSV conversion started...")
    hsd.convert_dat_to_xsv(hsd_instance, component, start_time=0, end_time=-1, labeled=False, raw_data=False, output_folder=output_folder, file_format="CSV")
    print(f"To CSV conversion completed.")
    print(f"To TXT Conversion filtered by tags started...")
    hsd.convert_dat_to_txt_by_tags(hsd_instance, component, start_time=0, end_time=-1, output_folder=output_folder, with_untagged = True, out_format="TXT")
    print(f"To TXT Conversion filtered by tags completed.")
    print(f"To Nanoedge format conversion started...")
    hsd.convert_dat_to_nanoedge(hsd_instance, component, signal_length=1000, signal_increment=500, start_time=0, end_time=-1, raw_data=False, output_folder=output_folder)
    print(f"To Nanoedge format conversion completed.")
    hsd.convert_dat_to_unico(hsd_instance, [component], start_time=0, end_time=-1, use_datalog_tags=False, output_folder=output_folder, out_format="TXT")
    print(f"To Unico format conversion started...")
    hsd.convert_dat_to_unico_aggregated(hsd_instance, aggregation="single_file", start_time=0, end_time=-1, use_datalog_tags=False, output_folder=output_folder, with_untagged = True, out_format="CSV")
    print(f"To Unico format conversion completed...")
    print(f"To Wav conversion started...")
    hsd.convert_dat_to_wav(hsd_instance, component, start_time=0, end_time=-1, output_folder=output_folder)
    print(f"To Wav conversion completed.")

    # Plot data
    hsd.plot(hsd_instance, component, start_time=0, end_time=-1)
    plt.show()

if __name__ == "__main__":
    main()










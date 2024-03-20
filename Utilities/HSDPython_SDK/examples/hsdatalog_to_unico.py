#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_to_unico.py
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
import click
import pandas as pd
from st_hsdatalog.HSD.model.DeviceConfig import Sensor
from st_hsdatalog.HSD_utils.converters import HSDatalogConverter
from st_hsdatalog.HSD_utils.dtm import HSDatalogDTM
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.HSDatalog import HSDatalog

from st_hsdatalog.HSD_utils.exceptions import MissingDeviceModelError, MissingISPUOutputDescriptorException

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")
script_version = "2.0.0"

def show_help(ctx, param, value):
    if value and not ctx.resilient_parsing:
        click.secho(ctx.get_help(), color=ctx.color)
        click.secho("\n-> Script execution examples:")
        click.secho("\n-> HSDatalog1:")
        click.secho("   python hsdatalog_to_unico.py ..\\..\\STWIN_acquisition_examples\\STWIN_00003", fg='cyan')
        click.secho("   python hsdatalog_to_unico.py ..\\..\\STWIN_acquisition_examples\\STWIN_00003 -o .\\toUnico -t -f TSV", fg='cyan')
        click.secho("\n-> HSDatalog2:")
        click.secho("   python hsdatalog_to_unico.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_03_20", fg='cyan')
        click.secho("   python hsdatalog_to_unico.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_03_20 -o ..\\..\\toUnico -t -f TSV", fg='cyan')
        ctx.exit()

@click.command()
@click.argument('acq_folder', type=click.Path(exists=True))
@click.option('-o', '--output_folder', help="Output folder (this will be created if it doesn't exist)")
@click.option('-s', '--sensor_name', help="Sensor Name - use \"all\" to extract all active sensors data, otherwise select a specific sensor by name", default='all')
@click.option('-st','--start_time', help="Start Time - Data conversion will start from this time (seconds)", type=int, default=0)
@click.option('-et','--end_time', help="End Time - Data conversion will end up in this time (seconds)", type=int, default=-1)
@click.option('-t', '--use_datalog_tags', is_flag=True, help="Enable this flag to include information about annotations taken during acquisition (if any) in the exported data", default=False)
@click.option('-f', '--out_format', help="Select exported data format", type=click.Choice(['TXT', 'CSV', 'TSV'], case_sensitive=False))
@click.option('-cdm','--custom_device_model', help="Upload a custom Device Template Model (DTDL)", type=(int, int, str))
@click.option('-ag','--aggregation', help="Data aggregation strategy, exported data format remains selectable by using -f parameter (default value: CSV)",  type=click.Choice(['single_file', 'split_per_tags']))
@click.version_option(script_version, '-v', '--version', prog_name="HSDatalogToUnico", is_flag=True, help="HSDatalogToUnico Converter tool version number")
@click.option('-d', '--debug', is_flag=True, help="[DEBUG] Check for corrupted data and timestamps", default=False)
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_toUnico(acq_folder, output_folder, sensor_name, start_time, end_time, use_datalog_tags, out_format, custom_device_model, aggregation, debug):

    if custom_device_model is not None:
        HSDatalogDTM.upload_custom_dtm(custom_device_model)
    
    hsd_factory = HSDatalog()
    try:
        hsd = hsd_factory.create_hsd(acq_folder)
    except MissingDeviceModelError as error:
        log.error("Device Template Model identifyed by [{}] not supported".format(error))
        log.info("Check your input acquisition folder, then try to upload a custom Device Template Model using -cdm flag".format(error))
        return

    output_folder = acq_folder + "_Exported" if output_folder is None else output_folder
    if out_format is None:
        out_format = 'csv'

    df_flag = True
    while df_flag:
        if aggregation is not None:
            convert_aggregated_data(hsd, aggregation, start_time, end_time, use_datalog_tags, output_folder, out_format)
            df_flag = False
        else:
            if sensor_name == 'all':
                component_list = HSDatalog.get_all_components(hsd, only_active=True)
                for component in component_list:
                    sensor = __get_sensor_comp(hsd, component)
                    convert_data(hsd, sensor, start_time, end_time, use_datalog_tags, output_folder, out_format, acq_folder)
                df_flag = False
            else:
                component = HSDatalog.get_component(hsd, sensor_name)
                if component is not None:
                    sensor = __get_sensor_comp(hsd, component)
                    convert_data(hsd, sensor, start_time, end_time, use_datalog_tags, output_folder, out_format, acq_folder)
                else:
                    log.error("No \"{}\" Component found in your Device Configuration file.".format(sensor_name))
                df_flag = False

def __get_sensor_comp(hsd, component):
    if isinstance(component, Sensor):
        return component
    else:
        sensor_name = list(component.keys())[0].split('_')[:-1][0].upper()
        s_list = HSDatalog.get_all_components(hsd, only_active=True)
        return HSDatalog.filter_sensor_list_by_name(hsd, s_list, sensor_name)
    
def __aggregate_dataframes(hsd, component_list, start_time, end_time, use_datalog_tags, tag_classes):    
    dataframes = []
    # Loop through each component in the component list
    for i, component in enumerate(component_list):
        # Get a dataframe for the current component
        df = HSDatalog.get_dataframe(hsd, component, start_time, end_time, use_datalog_tags, False)
        # If the dataframe is not empty, process it
        if not (df is None or len(df) == 0):
            # If the current component is not the last one and use_datalog_tags is True, drop tag classes from the dataframe
            if i != (len(component_list)-1) and use_datalog_tags:
                df[0] = df[0].drop(columns=tag_classes)
            # Define a list of columns to exclude
            cols_to_exclude = ["Time"]
            # If the current component is the last one and use_datalog_tags is True, add tag classes to the list of columns to exclude
            if i == (len(component_list)-1) and use_datalog_tags:
                cols_to_exclude.extend(tag_classes)
            # Define a suffix to add to column names
            suffix = f"_{list(component.keys())[0]}"
            # Create a dictionary of column name mappings
            new_names = {col: col + suffix for col in df[0].columns if col not in cols_to_exclude}
            # Rename columns in the dataframe
            df[0] = df[0].rename(columns=new_names)
            # Append the processed dataframe to the list of dataframes
            dataframes.append(df[0])
    # return a merged single dataframe
    return HSDatalogConverter.merge_dataframes(dataframes)

def __save_aggregated_file(df, file_path, out_format):
    if out_format.lower() == "txt":
        HSDatalogConverter.to_xsv(df, file_path, '.txt', '\t')
    elif out_format.lower() == "csv":
        HSDatalogConverter.to_csv(df, file_path)
    else:
        HSDatalogConverter.to_tsv(df, file_path)

def convert_aggregated_data(hsd, aggregation, start_time, end_time, use_datalog_tags, output_folder, out_format):
    # Get a list of all active components the input acquisition folder
    component_list = HSDatalog.get_all_components(hsd, only_active=True)
    tag_classes = []
    
    # NOTE: If aggregation is "split_per_tags", force use_datalog_tags to True
    if aggregation == "split_per_tags":
        use_datalog_tags = True
    
    # If use_datalog_tags is True, get a list of tag classes
    if use_datalog_tags: 
        data_tags = HSDatalog.get_data_stream_tags(hsd, component_list[0])
        tag_classes = list(set([tag['Label'] for tag in data_tags]))

    # Merge all dataframes into a single dataframe
    merged_df = __aggregate_dataframes(hsd, component_list, start_time, end_time, use_datalog_tags, tag_classes)

    # If aggregation is "single_file", save the merged dataframe as a single file
    if aggregation == "single_file":
        __save_aggregated_file(merged_df, f"{output_folder}", out_format)
    
    # If aggregation is "split_per_tags", split the merged dataframe by tag class and save each split as a separate file
    elif aggregation == "split_per_tags":
        # Loop through each tag class
        for tag_class_label in tag_classes:
            # Loop through each group of rows in the merged dataframe where the tag class is present
            for y, (k, tag_df) in enumerate(merged_df[merged_df[tag_class_label] == 1].groupby((merged_df[tag_class_label] != 1).cumsum())):
                # Drop tag classes from the dataframe
                tag_df = tag_df.drop([tag['Label'] for tag in data_tags], axis=1)
                # Define a file name for the current tag class and group
                labelFileName = f"{tag_class_label}_aggregated_dataLog_{y}"
                # Define a subfolder for the current tag class
                tag_sub_folder = os.path.join(output_folder, tag_class_label)
                # If the subfolder does not exist, create it
                if not os.path.exists(tag_sub_folder):
                    os.makedirs(tag_sub_folder)
                # Define a file path for the current tag class and group
                labelFilePath = os.path.join(tag_sub_folder, labelFileName)
                # Save the dataframe as a file in the specified format
                __save_aggregated_file(tag_df, labelFilePath, out_format)

def convert_data(hsd, sensor, start_time, end_time, use_datalog_tags, output_folder, out_format, acq_folder):
    try:
        df = HSDatalog.convert_dat_to_unico(hsd, sensor, start_time, end_time, use_datalog_tags, output_folder, out_format)
        return df
    except MissingISPUOutputDescriptorException as ispu_err:
        log.error(ispu_err)
        log.warning("Copy the right ISPU output descriptor file in your \"{}\" acquisition folder renamed as \"ispu_output_format.json\"".format(acq_folder))
    except Exception as err:
        log.exception(err)
 
if __name__ == '__main__':
    hsd_toUnico()


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
from st_hsdatalog.HSD.model.DeviceConfig import Sensor
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
@click.version_option(script_version, '-v', '--version', prog_name="HSDatalogToUnico", is_flag=True, help="HSDatalogToUnico Converter tool version number")
@click.option('-d', '--debug', is_flag=True, help="[DEBUG] Check for corrupted data and timestamps", default=False)
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_toUnico(acq_folder, output_folder, sensor_name, start_time, end_time, use_datalog_tags, out_format, custom_device_model, debug):

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
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    df_flag = True
    while df_flag:        
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


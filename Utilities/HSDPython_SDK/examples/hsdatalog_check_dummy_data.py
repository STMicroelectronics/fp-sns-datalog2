#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_check_dummy_data.py
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

import click
from st_hsdatalog.HSD_utils.dtm import HSDatalogDTM
from st_hsdatalog.HSD_utils.exceptions import MissingDeviceModelError, MissingISPUOutputDescriptorException
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.HSDatalog import HSDatalog

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")
script_version = "2.0.0"

def show_help(ctx, param, value):
    if value and not ctx.resilient_parsing:
        click.secho(ctx.get_help(), color=ctx.color)
        click.secho("\n-> Script execution examples:")
        click.secho("\n-> HSDatalog1:")
        click.secho("   python hsdatalog_check_dummy_data.py ..\\..\\STWIN_acquisition_examples\\DummyData", fg='cyan')
        click.secho("   python hsdatalog_check_dummy_data.py ..\\..\\STWIN_acquisition_examples\\DummyData -s IMP34DT05 \n", fg='cyan')
        click.secho("\n-> HSDatalog2:")
        click.secho("   python hsdatalog_check_dummy_data.py ..\\..\\STWIN.box_acquisition_examples\\DummyData", fg='cyan')
        click.secho("   python hsdatalog_check_dummy_data.py ..\\..\\STWIN.box_acquisition_examples\\DummyData -s imp34dt05_mic \n", fg='cyan')
        ctx.exit()

@click.command()
@click.argument('acq_folder', type=click.Path(exists=True))
@click.option('-s', '--sensor_name', help="Sensor name - specify a sensor name to limit the analysis. ", default='all')
@click.option('-st','--start_time', help="Sample Start - Data analysis will start from this time (seconds)", type=int, default=0)
@click.option('-et','--end_time', help="Sample End - Data analysis will end up in this time (seconds)", type=int, default=-1)
@click.option('-cdm','--custom_device_model', help="Upload a custom Device Template Model (DTDL)", type=(int, int, str))
@click.version_option(script_version, '-v', '--version', prog_name="HSDatalogToUnico", is_flag=True, help="HSDatalogToUnico Converter tool version number")
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_dataframe(acq_folder, sensor_name, start_time, end_time, custom_device_model):
    
    if custom_device_model is not None:
        HSDatalogDTM.upload_custom_dtm(custom_device_model)

    hsd_factory = HSDatalog()
    try:
        hsd = hsd_factory.create_hsd(acq_folder)
    except MissingDeviceModelError as error:
        log.error("Device Template Model identifyed by [{}] not supported".format(error))
        log.info("Check your input acquisition folder, then try to upload a custom Device Template Model using -cdm flag".format(error))
        return

    df_flag = True
    while df_flag:
        if sensor_name == 'all':
            component_list = HSDatalog.get_all_components(hsd, only_active=True)
            for component in component_list:
                check_data(hsd, component, start_time, end_time, acq_folder)
            df_flag = False
        else:
            component = HSDatalog.get_component(hsd, sensor_name)
            if component is not None:
                check_data(hsd, component, start_time, end_time, acq_folder)
            else:
                log.error("No \"{}\" Component found in your Device Configuration file.".format(sensor_name))
            df_flag = False

def check_data(hsd, component, start_time, end_time, acq_folder):
    try:
        HSDatalog.check_dummy_data(hsd, component, start_time, end_time)
    except MissingISPUOutputDescriptorException as ispu_err:
        log.error(ispu_err)
        log.warning("Copy the right ISPU output descriptor file in your \"{}\" acquisition folder renamed as \"ispu_output_format.json\"".format(acq_folder))
    except Exception as err:
        log.exception(err)
 
if __name__ == '__main__':
    hsd_dataframe()


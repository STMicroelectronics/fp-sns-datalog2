#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_plot.py
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
import matplotlib.pyplot as plt
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
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN_acquisition_examples\\STWIN_00001", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN_acquisition_examples\\STWIN_00001 -s all", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN_acquisition_examples\\STWIN_00002 -s IIS3DWB -st 1 -et 10", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN_acquisition_examples\\STWIN_00002 -s all -l", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN_acquisition_examples\\STWIN_00002 -l -p -r", fg='cyan')
        click.secho("\n-> HSDatalog2:")
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_18_08", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_18_08 -s all", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_18_08 -s iis3dwb_acc -st 1 -et 10", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_18_08 -s all -l", fg='cyan')
        click.secho("   python hsdatalog_plot.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_18_08 -l -p -r", fg='cyan')
        ctx.exit()

@click.command()
@click.argument('acq_folder', type=click.Path(exists=True))
@click.option('-s', '--sensor_name', help="Component name (Sensor or Algorithm) - use \"all\" to extract all active Component data, otherwise select a specific Component by name", default='')
@click.option('-st','--start_time', help="Start Time - Data plot will start from this time (seconds)", type=int, default=0)
@click.option('-et','--end_time', help="End Time - Data plot will end up in this time (seconds)", type=int, default=-1)
@click.option('-r', '--raw_data', is_flag=True, help="Uses Raw data (not multiplied by sensitivity)", default=False)
@click.option('-l', '--labeled', is_flag=True, help="Plot data including information about annotations taken during acquisition (if any)", default=False)
@click.option('-p', '--subplots', is_flag=True, help="Multiple subplot for multi-dimensional sensors", default=False)
@click.option('-cdm','--custom_device_model', help="Upload a custom Device Template Model (DTDL)", type=(int, int, str))
@click.version_option(script_version, '-v', '--version', prog_name="HSDatalogToUnico", is_flag=True, help="HSDatalogToUnico Converter tool version number")
@click.option('-d', '--debug', is_flag=True, help="[DEBUG] Check for corrupted data and timestamps", default=False)
@click.option("-h"," --help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_plot(acq_folder, sensor_name, start_time, end_time, raw_data, labeled, subplots, custom_device_model, debug):

    if custom_device_model is not None:
        HSDatalogDTM.upload_custom_dtm(custom_device_model)

    hsd_factory = HSDatalog()
    try:
        hsd = hsd_factory.create_hsd(acq_folder)
    except MissingDeviceModelError as error:
        log.error("Device Template Model identifyed by [{}] not supported".format(error))
        log.info("Check your input acquisition folder, then try to upload a custom Device Template Model using -cdm flag".format(error))
        return

    hsd.enable_timestamp_recovery(debug)

    label = None
    if labeled:
        label_list = hsd.get_acquisition_label_classes()
        label = hsd.prompt_label_select_CLI(label_list)
        if label is not None:
            log.info(label)
        else:
            quit()

    plot_flag = True
    while plot_flag:
        if sensor_name == '':
            component = HSDatalog.ask_for_component(hsd, only_active=True)
            if component is not None:
                plot(hsd, component, start_time, end_time, label, subplots, raw_data, acq_folder)
            else:
                break
        elif sensor_name == 'all':
            component_list = HSDatalog.get_all_components(hsd, only_active=True)
            for component in component_list:
                plot(hsd, component, start_time, end_time, label, subplots, raw_data, acq_folder)
            plot_flag = False
        else:
            component = HSDatalog.get_component(hsd, sensor_name)
            if component is not None:
                plot(hsd, component, start_time, end_time, label, subplots, raw_data, acq_folder)
            else:
                log.error("No \"{}\" Component to plot found in your Device Configuration file.".format(sensor_name))
                quit()
            plot_flag = False
        
        plt.show()

def plot(hsd, component, start_time, end_time, label, subplots, raw_data, acq_folder):
    try:
        df = HSDatalog.plot(hsd, component, start_time, end_time, label, subplots, raw_data)
        return df
    except MissingISPUOutputDescriptorException as ispu_err:
        log.error(ispu_err)
        log.warning("Copy the right ISPU output descriptor file in your \"{}\" acquisition folder renamed as \"ispu_output_format.json\"".format(acq_folder))
    except Exception as err:
        log.exception(err)
 
if __name__ == '__main__':
    hsd_plot()


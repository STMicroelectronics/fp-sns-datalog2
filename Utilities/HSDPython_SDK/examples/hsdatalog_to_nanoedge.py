#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_to_nanoedge.py
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
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.HSDatalog import HSDatalog

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

def show_help(ctx, param, value):
    if value and not ctx.resilient_parsing:
        click.secho(ctx.get_help(), color=ctx.color)
        click.secho("\n-> Script execution examples:")
        click.secho("\n-> HSDatalog1:")
        click.secho("   python hsdatalog_to_nanoedge.py ..\\..\\STWIN_acquisition_examples\\STWIN_00001 -s IIS3DWB -sl 1000 -si 750", fg='cyan')
        click.secho("   python hsdatalog_to_nanoedge.py ..\\..\\STWIN_acquisition_examples\\STWIN_00001 -o .\\ToNanoedge -s all -sl 32 -si 96 -r", fg='cyan')
        click.secho("\n-> HSDatalog2:")
        click.secho("   python hsdatalog_to_nanoedge.py ..\\..\\STWIN.box_acquisition_examples\\STWINBOX_00001 -s iis3dwb_acc -sl 1000 -si 750", fg='cyan')
        click.secho("   python hsdatalog_to_nanoedge.py ..\\..\\STWIN.box_acquisition_examples\\STWINBOX_00001 -o .\\ToNanoedge -s all -sl 32 -si 96 -r", fg='cyan')
        ctx.exit()

def validate_signal_increment(ctx, param, value):
    if value < 0:
        raise click.BadParameter('\'{d}\'. It must be a positive value. Please retry'.format(d=value))
    return value

@click.command()
@click.argument('acq_folder', type=click.Path(exists=True))
@click.option('-o', '--output_folder', help="Output folder (this will be created if it doesn't exist)")
@click.option('-s', '--sensor_name', help="Sensor Name - use \"all\" to convert all active sensors data, otherwise select a specific sensor by name", default='')
@click.option('-sl','--signal_length', help="The length of each segment when performing segmentation", type=int, required=True)
@click.option('-si','--signal_increment', help="Parameter to control the overlap, signal_increment = None, is equal to no overlap, signal_increment = frameLength/2 is fifty percent overlap", type=int, callback=validate_signal_increment, default=0)
@click.option('-st','--start_time', help="Start Time - Data conversion will start from this time (seconds)", type=int, default=0)
@click.option('-et','--end_time', help="End Time - Data conversion will end up in this time (seconds)", type=int, default=-1)
@click.option('-r', '--raw_data', is_flag=True, help="Uses Raw data (not multiplied by sensitivity)", default=False)
@click.option('-d', '--debug', is_flag=True, help="[DEBUG] Check for corrupted data and timestamps", default=False)
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_dataframe(acq_folder, output_folder, sensor_name, signal_length, signal_increment, start_time, end_time, raw_data, debug):

    hsd_factory = HSDatalog()
    hsd = hsd_factory.create_hsd(acq_folder)

    output_folder = acq_folder + "_Exported" if output_folder is None else output_folder

    hsd.enable_timestamp_recovery(debug)

    df_flag = True
    while df_flag:
        if sensor_name == '':
            component = HSDatalog.ask_for_component(hsd, only_active=True)
            if component is not None:
                HSDatalog.convert_dat_to_nanoedge(hsd, component, signal_length, signal_increment, start_time, end_time, raw_data, output_folder)
            else:
                break

        elif sensor_name == 'all':
            component_list = HSDatalog.get_all_components(hsd, only_active=True)
            for component in component_list:
                HSDatalog.convert_dat_to_nanoedge(hsd, component, signal_length, signal_increment, start_time, end_time, raw_data, output_folder)
            df_flag = False
        
        else:
            component = HSDatalog.get_component(hsd, sensor_name)
            if component is not None:
                HSDatalog.convert_dat_to_nanoedge(hsd, component, signal_length, signal_increment, start_time, end_time, raw_data, output_folder)
            else:
                log.exception("No \"{}\" Component found in your Device Configuration file.".format(sensor_name))
            df_flag = False
 
if __name__ == '__main__':
    hsd_dataframe()



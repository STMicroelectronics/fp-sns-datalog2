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
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_utils.converters import HSDatalogConverter
from st_hsdatalog.HSD.HSDatalog import HSDatalog

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

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
@click.option('-s', '--sensor_name', help="Sensor name - use \"all\" to select all MLC sensors (currently only the ISM330DHCX is supported)", default="ISM330DHCX")
@click.option('-st','--start_time', help="Start Time - Data conversion will start from this time (seconds)", type=int, default=0)
@click.option('-et','--end_time', help="End Time - Data conversion will end up in this time (seconds)", type=int, default=-1)
@click.option('-t', '--use_datalog_tags', is_flag=True, help="Enable this flag to include information about annotations taken during acquisition (if any) in the exported data", default=False)
@click.option('-f', '--out_format', help="Select exported data format", type=click.Choice(['TXT', 'CSV', 'TSV'], case_sensitive=False))
@click.option('-d', '--debug', is_flag=True, help="[DEBUG] Check for corrupted data and timestamps", default=False)
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_toUnico(acq_folder, output_folder, sensor_name, start_time, end_time, use_datalog_tags, out_format, debug):

    hsd_factory = HSDatalog()
    hsd = hsd_factory.create_hsd(acq_folder)

    output_folder = acq_folder + "_Exported" if output_folder is None else output_folder
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
        
    hsd.enable_timestamp_recovery(debug)

    if sensor_name == "all":
        sensor_name = "ISM330DHCX"
            
    if sensor_name == "ISM330DHCX":
        s_list = HSDatalog.get_all_components(hsd, only_active=True)
        sensor = HSDatalog.filter_sensor_list_by_name(hsd, s_list, sensor_name)
        
    if sensor is not None:
        HSDatalog.convert_dat_to_unico(hsd, sensor, start_time, end_time, use_datalog_tags, output_folder, out_format)
    else:
        log.exception("No \"{}\" sensor found in your Device Configuration.".format(sensor.name))
        quit()
 
if __name__ == '__main__':
    hsd_toUnico()


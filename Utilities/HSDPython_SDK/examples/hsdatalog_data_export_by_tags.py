#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_data_export_by_tags.py
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
from st_hsdatalog.HSD.HSDatalog import HSDatalog

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

def show_help(ctx, param, value):
    if value and not ctx.resilient_parsing:
        click.secho(ctx.get_help(), color=ctx.color)
        click.secho("\n-> Script execution examples:")
        click.secho("\n-> HSDatalog1:")
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN_acquisition_examples\\STWIN_00003-s all", fg='cyan')
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN_acquisition_examples\\STWIN_00003 -f CSV -s all -o .\\expData", fg='cyan')
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN_acquisition_examples\\STWIN_00003 -f CSV -s ISM330DHCX -o .\\expData", fg='cyan')
        click.secho("\n-> HSDatalog2:")
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39 -s all", fg='cyan')
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39 -f CSV -s all -o .\\expData", fg='cyan')
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39 -s ism330dhcx_gyro", fg='cyan')
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39 -f CSV -s ism330dhcx_gyro", fg='cyan')
        click.secho("   python hsdatalog_data_export_by_tags.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39 -f CSV -s ism330dhcx_gyro -it -o .\\expDataNoTags", fg='cyan')
        ctx.exit()

@click.command()
@click.argument('acq_folder', type=click.Path(exists=True))
@click.option('-o', '--output_folder', help="Output folder (this will be created if it doesn't exist)")
@click.option('-s', '--sensor_name', help="Sensor Name - use \"all\" to extract all active sensors data, otherwise select a specific sensor by name", default='')
@click.option('-st','--start_time', help="Start Time - Data conversion will start from this time (seconds)", type=int, default=0)
@click.option('-et','--end_time', help="End Time - Data conversion will end up in this time (seconds)", type=int, default=-1)
@click.option('-it','--ignore_datalog_tags', is_flag=True, help="Use this flag to ignore information about annotations taken during acquisition (if any) in the exported data", default=False)
@click.option('-f', '--out_format', help="Select exported data format", type=click.Choice(['TXT', 'CSV', 'TSV'], case_sensitive=False))
@click.option('-d', '--debug', is_flag=True, help="[DEBUG] Check for corrupted data and timestamps", default=False)
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_exportByTags(acq_folder, output_folder, sensor_name, start_time, end_time, ignore_datalog_tags, out_format, debug):

    hsd_factory = HSDatalog()
    hsd = hsd_factory.create_hsd(acq_folder)

    output_folder = acq_folder + "_Exported" if output_folder is None else output_folder
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
        
    hsd.enable_timestamp_recovery(debug)

    df_flag = True
    while df_flag:
        if sensor_name == '':
            component = HSDatalog.ask_for_component(hsd, only_active=True)
            if component is not None:
                HSDatalog.convert_dat_to_txt_by_tags(hsd, component, start_time, end_time, ignore_datalog_tags, acq_folder, output_folder, out_format)
            else:
                break

        elif sensor_name == 'all':
            component_list = HSDatalog.get_all_components(hsd, only_active=True)
            for component in component_list:
                HSDatalog.convert_dat_to_txt_by_tags(hsd, component, start_time, end_time, ignore_datalog_tags, acq_folder, output_folder, out_format)
            df_flag = False
        
        else:
            component = HSDatalog.get_component(hsd, sensor_name)
            if component is not None:
                HSDatalog.convert_dat_to_txt_by_tags(hsd, component, start_time, end_time, ignore_datalog_tags, acq_folder, output_folder, out_format)
            else:
                log.error("No \"{}\" Component found in your Device Configuration file.".format(sensor_name))
            df_flag = False
 
if __name__ == '__main__':
    hsd_exportByTags()


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
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.HSDatalog import HSDatalog

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

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
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_dataframe(acq_folder, sensor_name, start_time, end_time):
    
    hsd_factory = HSDatalog()
    hsd= hsd_factory.create_hsd(acq_folder)

    df_flag = True
    while df_flag:
        if sensor_name == 'all':
            component_list = HSDatalog.get_all_components(hsd, only_active=True)
            for component in component_list:
                HSDatalog.check_dummy_data(hsd, component, start_time, end_time)
            df_flag = False
        else:
            component = HSDatalog.get_component(hsd, sensor_name)
            if component is not None:
                HSDatalog.check_dummy_data(hsd, component, start_time, end_time)
            else:
                log.error("No \"{}\" Component found in your Device Configuration file.".format(sensor_name))
            df_flag = False
 
if __name__ == '__main__':
    hsd_dataframe()


#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_dataframes.py
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
        click.secho("   python hsdatalog_dataframes.py ..\\..\\STWIN_acquisition_examples\\STWIN_00002", fg='cyan')
        click.secho("   python hsdatalog_dataframes.py ..\\..\\STWIN_acquisition_examples\\STWIN_00002 -s all", fg='cyan')
        click.secho("   python hsdatalog_dataframes.py ..\\..\\STWIN_acquisition_examples\\STWIN_00002 -s IIS3DWB -l -r", fg='cyan')
        click.secho("\n-> HSDatalog2:")
        click.secho("   python hsdatalog_dataframes.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39", fg='cyan')
        click.secho("   python hsdatalog_dataframes.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39 -s all", fg='cyan')
        click.secho("   python hsdatalog_dataframes.py ..\\..\\STWIN.box_acquisition_examples\\20221017_13_04_39 -s iis3dwb_acc -l -r", fg='cyan')
        ctx.exit()

@click.command()
@click.argument('acq_folder', type=click.Path(exists=True))
@click.option('-s', '--sensor_name', help="Sensor Name - use \"all\" to obtain all active sensors dataframes, otherwise select a specific sensor by name", default='')
@click.option('-st','--start_time', help="Start Time - Data conversion will start from this time (seconds)", type=int, default=0)
@click.option('-et','--end_time', help="End Time - Data conversion will end up in this time (seconds)", type=int, default=-1)
@click.option('-r', '--raw_data', is_flag=True, help="Uses Raw data (not multiplied by sensitivity)", default=False)
@click.option('-l', '--labeled', is_flag=True, help="Includes annotations taken during acquisition (if any) in the extracted dataframe", default=False)
@click.option('-d', '--debug', is_flag=True, help="[DEBUG] Check for corrupted data and timestamps", default=False)
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_dataframe(acq_folder, sensor_name, start_time, end_time, raw_data, labeled, debug):

    hsd_factory = HSDatalog()
    hsd = hsd_factory.create_hsd(acq_folder)
    hsd.enable_timestamp_recovery(debug)

    df_flag = True
    while df_flag:
        if sensor_name == '':
            component = HSDatalog.ask_for_component(hsd, only_active=True)
            if component is not None:
                try:
                    df = HSDatalog.get_dataframe(hsd, component, start_time, end_time, labeled, raw_data)
                    if df is not None:
                        [log.info("\nDataFrame - Start time: {}, End time: {}\n{}".format(start_time, x.values[-1][0] , x)) for x in df if not x.empty]
                except Exception as err:
                    log.exception(err)
            else:
                break

        elif sensor_name == 'all':
            component_list = HSDatalog.get_all_components(hsd, only_active=True)
            for component in component_list:
                try:
                    df = HSDatalog.get_dataframe(hsd, component, start_time, end_time, labeled, raw_data)
                    if not (df is None or len(df) == 0):
                        for d in df:
                            log.info("\nDataFrame - Start time: {}, End time: {}\n{}".format(start_time, d.values[-1][0] ,d))
                except Exception as err:
                    log.exception(err)
            df_flag = False
        else:
            component = HSDatalog.get_component(hsd, sensor_name)
            if component is not None:
                df = HSDatalog.get_dataframe(hsd, component, start_time, end_time, labeled, raw_data)
            else:
                log.error("No \"{}\" Component found in your Device Configuration file.".format(sensor_name))
            df_flag = False
 
if __name__ == '__main__':
    hsd_dataframe()


#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_cli.py
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

from asciimatics.scene import Scene
from asciimatics.screen import Screen
from asciimatics.exceptions import ResizeScreenError
import sys
import os
import click
import shutil
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_link.HSDLink import HSDLink
from Views.cli_views import HSDMainView, HSDLoggingView

from st_pnpl.DTDL.device_template_manager import DeviceTemplateManager

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

class HSDInfo():

    class CLIFlags():
        
        def __init__(self, output_folder, sub_datetime_folder, acq_name, acq_desc, file_config, ucf_file, time_sec, interactive_mode):
            self.output_folder = output_folder
            self.sub_datetime_folder = sub_datetime_folder
            self.acq_name = acq_name
            self.acq_desc = acq_desc
            self.file_config = file_config
            self.ucf_file = ucf_file
            self.time_sec = time_sec
            self.interactive_mode = interactive_mode
    
    cli_flags = None
    version = None
    device_list = None
    sensor_list = None
    sensor_count = None
    tag_list = []
    highlighted_device_id = None
    selected_device_id = None
    selected_fw_info = None
    mlc_sensor_list = None
    selected_mlc_id = None
    is_log_started = False
    is_log_manually_stopped = False
    output_acquisition_path = None
    threads_stop_flags = []
    tag_status_list = []
    start_time = None

    def __init__(self, cli_flags):
        self.cli_flags = cli_flags

        #Initialize the HSD_PythonSDK HSD_link module
        self.hsd_link_factory = HSDLink()
        self.hsd_link = self.hsd_link_factory.create_hsd_link(acquisition_folder = cli_flags.output_folder)
        
        #Get the connected device list
        self.device_list = HSDLink.get_devices(self.hsd_link)

        if len(self.device_list) == 0:
            quit()

        #Updates the output folder field
        if self.cli_flags.output_folder is None:
            self.cli_flags.output_folder = HSDLink.get_acquisition_folder(self.hsd_link)
                
        #Multiple connected devices management
        if self.device_list is None or len(self.device_list) > 1:
            self.selected_device_id = None
        else:
            self.selected_device_id = 0
            pres_res = HSDLink.get_device_presentation_string(self.hsd_link, self.selected_device_id)
            if pres_res is not None:
                board_id = hex(pres_res["board_id"])
                fw_id = hex(pres_res["fw_id"])
                self.load_device_template(board_id,fw_id)
    
    def is_hsd_link_v2(self):
        if self.hsd_link is not None:
            return HSDLink.is_v2(self.hsd_link)
        return False
    
    def load_device_template(self, board_id, fw_id):
        dev_template_json = DeviceTemplateManager.query_dtdl_model(board_id, fw_id)
        HSDLink.set_device_template(self.hsd_link, dev_template_json)
    
    def update_fw_info(self):
        self.selected_fw_info = HSDLink.get_firmware_info(self.hsd_link, self.selected_device_id)

    def check_output_folder(self):
        if self.output_acquisition_path is None:
            if self.cli_flags.output_folder is not None:
                if not os.path.exists(self.cli_flags.output_folder):
                    os.makedirs(self.cli_flags.output_folder)
                self.output_acquisition_path = self.cli_flags.output_folder
            else:
                self.output_acquisition_path = HSDLink.get_acquisition_folder(self.hsd_link)

    def save_device_in_output_folder(self):
        return HSDLink.save_json_device_file(self.hsd_link, self.selected_device_id, self.output_acquisition_path)

    def update_acq_params(self):
        # Set acquisition name and description parameters to the device
        if self.cli_flags.acq_name is None:
            self.cli_flags.acq_name = "STWIN_Acq"
        if self.cli_flags.acq_desc is None:
            self.cli_flags.acq_desc = ""

        HSDLink.set_acquisition_info(self.hsd_link, self.selected_device_id, self.cli_flags.acq_name, self.cli_flags.acq_desc)

    def upload_device_conf_file(self):
        # Device configuration file [DeviceConfig.json]
        if self.cli_flags.file_config != '' and os.path.exists(self.cli_flags.file_config):
            res = HSDLink.update_device(self.hsd_link, self.selected_device_id, self.cli_flags.file_config)
            if not res:
                log.warning("Error in Device configuration update. The default configuration will be used!")
            else:
                shutil.copy(self.cli_flags.file_config, self.output_acquisition_path)
    
    def update_sensor_list(self):
        if self.selected_device_id is not None:
            self.sensor_list = HSDLink.get_sensor_list(self.hsd_link, self.selected_device_id, only_active=True)
    
    def init_sensor_data_counters(self):
        HSDLink.set_RTC(self.hsd_link, self.selected_device_id)
        all_sensor_list = HSDLink.get_sensor_list(self.hsd_link, self.selected_device_id, only_active=False)
        HSDLink.init_sensors_data_counters(self.hsd_link, all_sensor_list)

    def update_mlc_sensor_list(self):
        self.mlc_sensor_list = HSDLink.get_updated_mlc_sensor_list(self.hsd_link, self.selected_device_id, self.mlc_sensor_list)
        if self.mlc_sensor_list is not None and len(self.mlc_sensor_list) > 0:
            self.selected_mlc_id = HSDLink.get_mlc_id(self.hsd_link, self.selected_device_id)

    def upload_mlc_ucf_file(self):
        if self.selected_mlc_id is not None:
            HSDLink.upload_mlc_ucf_file(self.hsd_link, self.selected_device_id, self.cli_flags.ucf_file)
            self.update_sensor_list()

    def update_tag_list(self):
        if self.selected_device_id is not None:
            self.tag_list = HSDLink.get_sw_tag_classes(self.hsd_link, self.selected_device_id)

    def init_tag_status_list(self):
        self.tag_status_list = [False] * len(self.tag_list)

    def do_tag(self, t_id):
        self.tag_status_list[t_id] = not self.tag_status_list[t_id]
        HSDLink.set_sw_tag_on_off(self.hsd_link, self.selected_device_id, t_id, self.tag_status_list[t_id])

    def start_log(self):
        self.is_log_started = HSDLink.start_log(self.hsd_link, self.selected_device_id, self.cli_flags.sub_datetime_folder)
        self.threads_stop_flags = []
        self.sensor_data_files = []

        for s in self.sensor_list:
            HSDLink.start_sensor_acquisition_thread(self.hsd_link, self.selected_device_id, s, self.threads_stop_flags, self.sensor_data_files)
        self.output_acquisition_path = HSDLink.get_acquisition_folder(self.hsd_link)

    def stop_log(self):
        for sf in self.threads_stop_flags:
            sf.set()
        for f in self.sensor_data_files:
            f.close()
        HSDLink.stop_log(self.hsd_link, self.selected_device_id)
        self.is_log_started = False
        HSDLink.save_json_device_file(self.hsd_link, self.selected_device_id, self.output_acquisition_path)
        HSDLink.save_json_acq_info_file(self.hsd_link, self.selected_device_id, self.output_acquisition_path)
        HSDLink.refresh_hsd_link(self.hsd_link) #Needed by HSDLink_v1

def show_help(ctx, param, value):
    if value and not ctx.resilient_parsing:
        click.secho(ctx.get_help(), color=ctx.color)
        click.secho("\n-> Script execution examples:")
        click.secho("   python hsdatalog_cli.py", fg='cyan')
        click.secho("   python hsdatalog_cli.py -o .\your_out_folder", fg='cyan')
        click.secho("   python hsdatalog_cli.py -t 10", fg='cyan')
        click.secho("   python hsdatalog_cli.py -i", fg='cyan')
        click.secho("   python hsdatalog_cli.py -t 20 -an your_acq_name -ad your_acq_descr", fg='cyan')
        click.secho("   python hsdatalog_cli.py -f ..\\STWIN_config_examples\\DeviceConfig.json -u ..\\STWIN_config_examples\\UCF_examples\\ism330dhcx_six_d_position.ucf", fg='cyan')
        ctx.exit()

def validate_duration(ctx, param, value):
    if value < -1 or value == 0:
        raise click.BadParameter('\'{d}\'. Please retry'.format(d=value))
    return value

@click.command()
@click.option('-o', '--output_folder', help="Output folder (this will be created if it doesn't exist)")
@click.option('-s', '--sub_datetime_folder', help="Put automatic datetime sub-folder in Output folder [HighSpeedDatalogv2 Only] (this will be created if it doesn't exist)", type=bool, default=True)
@click.option('-an','--acq_name', help="Acquisition name", type=str)
@click.option('-ad','--acq_desc', help="Acquisition description", type=str)
@click.option('-f', '--file_config', help="Device configuration file (JSON)", default='')
@click.option('-u', '--ucf_file', help="UCF Configuration file for MLC", default='')
@click.option('-t', '--time_sec', help="Duration of the current acquisition [seconds]", callback=validate_duration, type=int, default=-1)
@click.option('-i', '--interactive_mode', help="Interactive mode. It allows to select a connected device, get info and start the acquisition process",  is_flag=True, default=False)
@click.option("-h", "--help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def hsd_CLI(output_folder, sub_datetime_folder, acq_name, acq_desc, file_config, ucf_file, time_sec, interactive_mode):
    last_scene = None

    cli_flags = HSDInfo.CLIFlags(output_folder, sub_datetime_folder, acq_name, acq_desc, file_config, ucf_file, time_sec, interactive_mode)
    hsd_info = HSDInfo(cli_flags)

    while True:
        try:
            Screen.wrapper(demo, catch_interrupt=True, arguments=[last_scene, hsd_info])

            sys.exit(0)
        except ResizeScreenError as e:
            last_scene = e.scene

def demo(screen, scene, hsd_info):
    if hsd_info.cli_flags.interactive_mode or len(hsd_info.device_list) == 0:
        scenes = [
            Scene([HSDMainView(screen, hsd_info)], -1, name="Main"),
            Scene([HSDLoggingView(screen, hsd_info)], -1, name="Logger")
        ]
    else:
        hsd_info.selected_device_id = 0
        scenes = [      
            Scene([HSDLoggingView(screen, hsd_info)], -1, name="Logger")
        ]

    screen.play(scenes, stop_on_resize=True, start_scene=scene, allow_int=True)

if __name__ == '__main__':          
    hsd_CLI()

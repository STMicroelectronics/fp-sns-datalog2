#!/usr/bin/env python
# coding: utf-8 
# *****************************************************************************
#  * @file    hsdatalog_dataframes.py
#  * @author  SRA
#  * @version 2.0.0
#  * @date    22-Jun-2022
# *****************************************************************************
#
#                   Copyright (c) 2020 STMicroelectronics.
#                             All rights reserved
#
#   This software component is licensed by ST under BSD-3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause

import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD.HSDatalog import HSDatalog

log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

acq_folder = ".\\20220627_04_04_30"
#TODO new_device = ".\\test_device_config.json"
hsd_factory = HSDatalog()
hsd = hsd_factory.create_hsd(acq_folder)


print(hsd.get_device())
#TODO (hsd.set_device(new_device))
print(hsd.get_device_info())
print(hsd.get_firmware_info())

sensor_name = "ism330dhcx_acc"
print(hsd.get_sensor(sensor_name))
print(hsd.get_sensor_list())
print(hsd.get_sensor_list(only_active = True))
print(hsd.get_sensor_list(type_filter = "acc"))
print(hsd.get_sensor_list(type_filter = "acc", only_active = False))

print(hsd.get_sw_tag_classes())
print(hsd.get_hw_tag_classes())

print(hsd.get_acquisition_info())
acq_interface_dict = {0:"SD Card", 1:"USB"}
print(acq_interface_dict[hsd.get_acquisition_interface()])

print(hsd.get_acquisition_label_classes()) #NOTE test with a labeled acquisition
print(hsd.get_time_tags()) #NOTE test with a labeled acquisition
print(hsd.get_data_stream_tags("iis2mdc_mag"))

print(hsd.get_data_and_timestamps("iis2mdc_mag"))










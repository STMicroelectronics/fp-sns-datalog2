#!/usr/bin/env python
# coding: utf-8 

#
#                   Copyright (c) 2020 STMicroelectronics.
#                             All rights reserved
#
#   This software component is licensed by ST under BSD-3-Clause license,
#   the "License"; You may not use this file except in compliance with the
#   License. You may obtain a copy of the License at:
#                        https://opensource.org/licenses/BSD-3-Clause

import os
import click
import time
import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_link.HSDLink import HSDLink
from st_hsdatalog.HSD_utils.exceptions import EmptyCommandResponse

log = logger.get_logger(__name__)

if __name__ == '__main__':

    #WP2 TODO Device Template (.\\FP-SNS-DATALOG1.json) downloaded by Device Template Registry   
    hsd_link_factory = HSDLink()
    hsd_link = hsd_link_factory.create_hsd_link()

    #DLL Info functions
    ##GET DLL version
    dll_version = hsd_link.get_version()
    print(dll_version)
    if dll_version != "High Speed Datalog DLL v2":
        log.error("this is a test script for DATALOG2. Wrong FW+Board detected.")
        quit()
    ##GET Commands Presentation String
    print(hsd_link.get_cmd_set_presentation_string())
    ##GET Device Presentation String
    print(hsd_link.get_device_presentation_string(0))
    ##GET Acquisition folder path
    print(hsd_link.get_acquisition_folder())
    ##GET Number of connected devices
    print(hsd_link.get_nof_devices())
    ##GET Device Status of all connected devices
    print(hsd_link.get_devices())

    ##GET Device 0 Firmware Information Component Status
    print(hsd_link.get_firmware_info(0))
    ##GET Device 0 Device Information Component Status
    print(hsd_link.get_device_info(0))
    ##GET Device 0 Alias. Property of FW Info component
    print(hsd_link.get_device_alias(0))
    ##GET Device 0 Status. Array of all its Components status
    print(hsd_link.get_device_status(0))

    ##GET Device 0 Sensor Components Status. (ComponentType = c_type = 0 = SENSOR)
    print(hsd_link.get_sensors_status(0))
    ##GET Device 0 Sensor Components number
    print(hsd_link.get_sensors_count(0))
    ##GET Device 0 Sensor Components names list
    print(hsd_link.get_sensors_names(0))

    ##GET Device 0 Algorithm Components Status. (ComponentType = c_type = 1 = ALGORITHM)
    print(hsd_link.get_algorithms_status(0))
    ##GET Device 0 Sensor Components number
    print(hsd_link.get_algorithms_count(0))
    ##GET Device 0 Sensor Components names list
    print(hsd_link.get_algorithms_names(0))

    ##GET Device 0, HTS221 Temperature Sensor Component Status (component name = stts22h_temp)
    print(hsd_link.get_component_status(0,"stts22h_temp"))
    ##GET Device 0, INEXISTENT Sensor Component Status (component name = aaa) # ERROR
    try:
        print(hsd_link.get_component_status(0,"aaa"))
    except EmptyCommandResponse:
        log.error("Error catched correctly!")
        pass
    ##GET Device 0, IIS3DWB Accelerometer Sensor Component Status (component name = ism330dhcx_acc)
    print(hsd_link.get_component_status(0,"ism330dhcx_acc"))
    
    ##GET Device 0, IMP34DT05 Microphone Sensor Component enable Property value (component name = imp34dt05_mic, property_name (hidden by HSD_link) = enable)
    print(hsd_link.get_sensor_enable(0,"imp34dt05_mic"))
    ##SET Device 0, IMP34DT05 Microphone Sensor Component enable Property value to False (component name = imp34dt05_mic, property_name (hidden by HSD_link) = enable)
    print(hsd_link.set_sensor_enable(0, False, "imp34dt05_mic"))
    ###Check if the value has been changed
    print(hsd_link.get_sensor_enable(0,"imp34dt05_mic"))

    sensors = ["stts22h_temp",
            "imp34dt05_mic",
            "iis2mdc_mag"]
    for s in sensors:            
        ##GET Device 0, s Sensor Component enable Property Status
        print(hsd_link.get_sensor_enable(0,s))

    ##GET Device 0, IIS2MDC Magnetometer Sensor Component ODR Property value (component name = iis2mdc_mag, property_name (hidden by HSD_link) = odr)
    print(hsd_link.get_sensor_odr(0,"iis2mdc_mag"))
    ##SET Device 0, IIS2MDC Magnetometer Sensor Component ODR Property value to 50.0 (component name = iis2mdc_mag, property_name (hidden by HSD_link) = odr)
    print(hsd_link.set_sensor_odr(0, 50.0, "iis2mdc_mag"))
    ##SET Device 0, IIS2MDC Magnetometer Sensor Component ODR Property value to 50 (component name = iis2mdc_mag, property_name (hidden by HSD_link) = odr)
    print(hsd_link.set_sensor_odr(0, 50, "iis2mdc_mag"))
    ###Check if the value has been changed
    print(hsd_link.get_sensor_odr(0,"iis2mdc_mag"))
    ##GET Device 0, IIS2MDC Magnetometer Sensor Component Measured ODR Property value (component name = iis2mdc_mag, property_name (hidden by HSD_link) = measodr)
    print(hsd_link.get_sensor_measured_odr(0,"iis2mdc_mag"))

    ##GET Device 0, IIS3DWB Accelerometer Sensor Component FS Property value (component name = ism330dhcx_acc, property_name (hidden by HSD_link) = fs)
    print(hsd_link.get_sensor_fs(0,"ism330dhcx_acc"))
    ##SET Device 0, IIS3DWB Accelerometer Sensor Component FS Property value  to 8 (component name = ism330dhcx_acc, property_name (hidden by HSD_link) = fs)
    print(hsd_link.set_sensor_fs(0, 8, "ism330dhcx_acc"))
    ###Check if the value has been changed
    print(hsd_link.get_sensor_fs(0,"ism330dhcx_acc"))

    ##GET Device 0, STTS22H Temperature Sensor Component Samples per Timestamp Property value (component name = stts22h_temp, property_name (hidden by HSD_link) = samples_per_ts)
    print(hsd_link.get_sensor_samples_per_ts(0,"stts22h_temp"))
    ##SET Device 0, STTS22H Temperature Sensor Component Samples per Timestamp Property value (component name = stts22h_temp, property_name (hidden by HSD_link) = samples_per_ts)
    print(hsd_link.set_sensor_samples_per_ts(0, 2, "stts22h_temp"))
    ###Check if the value has been changed
    print(hsd_link.get_sensor_samples_per_ts(0,"stts22h_temp"))

    ##GET Device 0, IIS2MDC Magnetometer Sensor Component Initial Offset Property value (component name = iis2mdc_mag, property_name (hidden by HSD_link) = ioffset)
    print(hsd_link.get_sensor_initial_offset(0,"iis2mdc_mag"))
    ##GET Device 0, IIS2MDC Magnetometer Sensor Component Measured ODR Property value (component name = iis2mdc_mag, property_name (hidden by HSD_link) = measodr)
    print(hsd_link.get_sensor_measured_odr(0,"iis2mdc_mag"))

    ##GET Device 0 Acquisition Info Component Status (component_name = acquisition_info)
    print(hsd_link.get_acquisition_info(0))
    ##SET Device 0 Acquisition Info Component Name (component name = acquisition_info, property_name (hidden by HSD_link) = name)
    print(hsd_link.set_acquisition_name(0,"Test_Vespucci_Acquisition"))
    ###Check if the value has been changed
    print(hsd_link.get_acquisition_info(0))
    ##SET Device 0 Acquisition Info Component Description (component name = acquisition_info, property_name (hidden by HSD_link) = description)
    print(hsd_link.set_acquisition_description(0,"a Test Acquisition Description"))
    ###Check if the value has been changed
    print(hsd_link.get_acquisition_info(0))
    
    ##GET Device 0 Tags Info Component Status (component_name = tags_info)
    print(hsd_link.get_tags_info(0))
    ##GET Device 0 Tags Info Component Tag List Property value(component_name = tags_info, property_name (hidden by HSD_link) = tags)
    print(hsd_link.get_tag_list(0))
    ##GET Device 0 Tags Info Component Max Tags per Acquisition Property value (component_name = tags_info, property_name (hidden by HSD_link) = max_tag_per_acq)
    print(hsd_link.get_max_tags_per_acq(0))
    
    sw_tags = hsd_link.get_sw_tag_classes(0) 
    print(sw_tags)
    for i in range(len(sw_tags)):
        print(hsd_link.get_sw_tag_class(0,i))
        print(hsd_link.get_sw_tag_class_label(0,i))
        print(hsd_link.get_sw_tag_class_enabled(0,i))
        if i == 0:
            print(hsd_link.set_sw_tag_class_label(0,i,"Normal"))
            print(hsd_link.get_sw_tag_class_label(0,i))
            print(hsd_link.set_sw_tag_class_enabled(0,i,True))
        if i == 1:
            print(hsd_link.set_sw_tag_class_label(0,i,"Anormal"))
            print(hsd_link.get_sw_tag_class_label(0,i))
            print(hsd_link.set_sw_tag_class_enabled(0,i,True))
        else:
            print(hsd_link.set_sw_tag_class_enabled(0,i,False))
            print(hsd_link.get_sw_tag_class_enabled(0,i))
    sw_tags = hsd_link.get_sw_tag_classes(0) 
    print(sw_tags)

    hw_tags = hsd_link.get_hw_tag_classes(0) 
    print(hw_tags)
    for i in range(len(hw_tags)):
        print(hsd_link.get_hw_tag_class(0,i))
        print(hsd_link.get_hw_tag_class_label(0,i))
        print(hsd_link.get_hw_tag_class_enabled(0,i))
        if i == 0:
            print(hsd_link.set_hw_tag_class_label(0,i,"Normal_hw"))
            print(hsd_link.get_hw_tag_class_label(0,i))
            print(hsd_link.set_hw_tag_class_enabled(0,i,True))
        if i == 1:
            print(hsd_link.set_hw_tag_class_label(0,i,"Anormal_hw"))
            print(hsd_link.get_hw_tag_class_label(0,i))
            print(hsd_link.set_hw_tag_class_enabled(0,i,True))
        else:
            print(hsd_link.set_hw_tag_class_enabled(0,i,False))
            print(hsd_link.get_hw_tag_class_enabled(0,i))
    hw_tags = hsd_link.get_hw_tag_classes(0) 
    print(hw_tags)

    message = "{\"get_status\":\"all\"}"
    print(hsd_link.send_command(0,message))
    
    from st_pnpl.PnPLCmd import PnPLCMDManager
    
    # message = PnPLCMDManager.create_get_device_status_cmd()
    # message = PnPLCMDManager.create_command_cmd("ai_application","start")
    # message = PnPLCMDManager.create_command_cmd("ai_application","stop")
    # message = PnPLCMDManager.create_command_cmd("ai_application","switch_bank")

    print(hsd_link.set_property(0,False,"stts22h_temp","enable"))
    print(hsd_link.get_component_status(0,"stts22h_temp"))

    print(hsd_link.set_property(0,"New_Tag_Class","tags_info","sw_tag2","label"))
    print(hsd_link.get_component_status(0,"tags_info"))
    print(hsd_link.set_property(0,True,"tags_info","sw_tag2","enabled"))
    print(hsd_link.get_component_status(0,"tags_info"))
    
    hsd_link.set_rtc_time(0)   
    
    import time
    import os
    from threading import Thread, Event

    class SensorAcquisitionThread(Thread):
        def __init__(self, event, sensor_data_file, d_id, comp_name):
            Thread.__init__(self)
            self.stopped = event
            self.d_id = d_id
            self.comp_name = comp_name
            self.sensor_data_file = sensor_data_file

        def run(self):
            while not self.stopped.wait(0.1):
                sensor_data = hsd_link.get_sensor_data(self.d_id, self.comp_name)
                if sensor_data is not None:
                    res = self.sensor_data_file.write(sensor_data[1])
                    print("{}".format(sensor_data))

    ## Enabling 3 test sensors
    print(hsd_link.set_sensor_enable(0,True,"stts22h_temp"))
    print(hsd_link.set_sensor_enable(0,True,"ism330dhcx_acc"))
    # print(hsd_link.set_sensor_enable(0,True,"imp34dt05_mic"))
    sensors = ["stts22h_temp",
                "ism330dhcx_acc"]
                # "imp34dt05_mic"]

    threads_stop_flags = []
    sensor_data_files = []

    print(hsd_link.start_log(0))
    
    for s in sensors:
        is_enabled = hsd_link.get_sensor_enable(0, s)
        if(is_enabled):
            sensor_data_file_path = os.path.join(hsd_link.get_acquisition_folder(),(s + ".dat"))
            sensor_data_file = open(sensor_data_file_path, "wb+")
            sensor_data_files.append(sensor_data_file)
            stopFlag = Event()
            threads_stop_flags.append(stopFlag)
            thread = SensorAcquisitionThread(stopFlag, sensor_data_file, 0, s)
            thread.start()
        else:
            click.secho("[WARNING]: Sensor {} is not enabled.".format(s))

    print(hsd_link.set_sw_tag_on(0,0))
    time.sleep(1)
    print(hsd_link.set_sw_tag_off(0,0))
    time.sleep(1)
    print(hsd_link.set_sw_tag_on(0,1))
    time.sleep(1)
    print(hsd_link.set_sw_tag_off(0,1))
    
    time.sleep(12)
    for sf in threads_stop_flags:
        sf.set()
    for f in sensor_data_files:
        f.close()

    print(hsd_link.stop_log(0))

    #SAVE LOG info files
    click.secho("save device_config json file in acquisition folder: {}".format(hsd_link.save_json_device_status_file(0)))
    click.secho("save acquisition_info json file in acquisition folder: {}".format(hsd_link.save_json_acq_info_file(0)))

    print("TEST END")

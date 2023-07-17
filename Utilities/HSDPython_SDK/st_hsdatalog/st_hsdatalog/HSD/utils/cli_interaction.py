
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

from st_hsdatalog.HSD.model.DeviceConfig import Device, DeviceInfo, Sensor, HwTag, SwTag
from st_hsdatalog.HSD.model.AcquisitionInfo import Tag
import st_hsdatalog.HSD_utils.logger as logger

log = logger.get_logger(__name__)

class CLIInteraction:

    @staticmethod
    def select_item(what: str, a_list: list):
        choice_not_in_range = True
        choice = 0
        item_id = ""
        while choice_not_in_range:
            for i, c in enumerate(a_list):
                if isinstance(c , Device):
                    item_id = c.device_info.alias + " [" + c.device_info.part_number + "]"
                elif isinstance(c , Sensor):
                    item_id = c.name
                elif isinstance(c, Tag):
                    item_id = c.label
                else:
                    if what == "PnPL_Component":
                        item_id = list(c.keys())[0]
                    else:
                        item_id = str(c)
                print(str(i) + ' - ' + item_id)

            if len(a_list) == 0:
                log.error('==> No {w} in list'.format(w=what))
                return None

            print('q - Quit')
            choice = input('Select one {} (''q'' to quit) ==> '.format(what))
            try:
                choice_not_in_range = int(choice) not in range(len(a_list))
                if choice_not_in_range:
                    print('please input a correct value')
            except ValueError:
                if choice != 'q':
                    print('please input a correct value')
                else:
                    #choice_not_in_range = False
                    break
        if choice == 'q':
            print('Bye!')
            return None
        else:
            return a_list[int(choice)]

    @staticmethod
    def select_items(what: str, a_list: list):
        selected_elements = []
        print("Enter the indices of the elements you want to select (space-separated).")
        print("Type 'done' when you're finished. 'all' to select all elements")
        done_flag = False

        while not done_flag:
            for i, c in enumerate(a_list):
                if what == "PnPL_Component":
                    item_id = list(c.keys())[0]
                else:
                    item_id = str(c)
                print(str(i) + ' - ' + item_id)
            index_input = input("Index: ")

            ids = index_input.split(" ")
            for id in ids:
                if id == "all":
                    for e in a_list:
                        selected_elements.append(list(e.keys())[0])
                    done_flag = True
                if id == "done":
                    done_flag = True
                else:
                    try:
                        id = int(id)
                        if 0 <= id < len(a_list):
                            selected_elements.append(list(a_list[id].keys())[0])
                        else:
                            print("Invalid index! Try again.")
                    except ValueError:
                        print("Invalid input! Please enter a valid index or 'done'.")
        print("Selected elements:", selected_elements)
        return selected_elements
    
    @staticmethod
    def present_items(item_list: list):
        for item in item_list:
            if isinstance(item , Sensor):
                print("{}) {}".format(item.id, item.name))
                sd_list = item.sensor_descriptor.sub_sensor_descriptor
                ss_list = item.sensor_status.sub_sensor_status
                for i, ssd in enumerate(sd_list):
                    if ss_list[i].is_active:
                        print("    {}) {}".format(ssd.id, ssd.sensor_type))
                    else:
                        print("    {}) {} --> [INACTIVE]".format(ssd.id, ssd.sensor_type))
            elif isinstance(item , HwTag):
                print("- {} [{}]".format(item.label, "ENABLED" if item.enabled else "DISABLED"))
            elif isinstance(item , SwTag):
                print("- {}".format(item.label))
            else:
                if isinstance(item, str):
                    component_status = item_list[item]
                    if "c_type" in item_list[item]:
                        if component_status["c_type"] == 0: #SENSOR
                            if "enable" in component_status:
                                if component_status["enable"]:
                                    print("- {}".format(item))
                                else:
                                    print("- {} --> [INACTIVE]".format(item))
                    else:
                        if "sw_tag" in item or "hw_tag" in item:
                            print("- {} label: {}".format(item, component_status["label"]))
                        else:
                            print("- {}".format(item))
                else:
                    item_key = list(item.keys())[0]
                    component_status = item[item_key]
                    if "c_type" in item[item_key]:
                        if component_status["c_type"] == 0: #SENSOR
                            if "enable" in component_status:
                                if component_status["enable"]:
                                    print("- {}".format(item_key))
                                else:
                                    print("- {} --> [INACTIVE]".format(item_key))
                    else:
                        if "sw_tag" in item or "hw_tag" in item_key:
                            print("- {} label: {}".format(item_key, component_status["label"]))
                        else:
                            print("- {}".format(item_key))

    @staticmethod
    def present_item(item):
        if isinstance(item , DeviceInfo):
            print("Serial number: {}".format(item.serial_number))
            if item.model is not None:
                print("Model: {}".format(item.model))
            print("Device alias: {}".format(item.alias))
            print("Part number: {}".format(item.part_number))
            print("Website: {}".format(item.url))
            print("FW name: {}".format(item.fw_name))
            print("FW version: {}".format(item.fw_version))
            print("Data file extension: {}".format(item.data_file_ext))
            print("Data file format: {}".format(item.data_file_format))
            if item.ble_mac_address is not None:
                print("BLE MAC address: {}".format(item.ble_mac_address))
        elif isinstance(item , Sensor):
            s_descriptor_list = item.sensor_descriptor.sub_sensor_descriptor
            s_status_list = item.sensor_status.sub_sensor_status
            for i, ssd in enumerate(s_descriptor_list):
                print("\n[{}] - ID: {}".format(item.name, ssd.id))
                print("- DESCRIPTOR:")
                print(" -- Sensor Type: {}".format(ssd.sensor_type))
                print(" -- Dimensions: {}".format(ssd.dimensions))
                print(" -- Dimensions Labels: {}".format(ssd.dimensions_label))
                print(" -- Unit: {}".format(ssd.unit))
                print(" -- Data Type: {}".format(ssd.data_type))
                print(" -- Supported FS: {}".format(ssd.fs))
                print(" -- Supported ODR: {}".format(ssd.odr))
                print(" -- Samples per Timestamp info:")
                print("   - Min: {}".format(ssd.samples_per_ts.min))
                print("   - Max: {}".format(ssd.samples_per_ts.max))
                print("   - Data Type: {}".format(ssd.samples_per_ts.data_type))
                print("- STATUS:")
                print(" -- isActive: {}".format(s_status_list[i].is_active))
                print(" -- ODR: {}".format(s_status_list[i].odr))
                print(" -- FS: {}".format(s_status_list[i].fs))
                print(" -- samples per timestamp: {}".format(s_status_list[i].samples_per_ts))
                print(" -- sensitivity: {}".format(s_status_list[i].samples_per_ts))
                print(" -- initial_offset: {}".format(s_status_list[i].initial_offset))
        else:
            item_key = list(item.keys())[0]
            # if "c_type" in item[item_key]:
            print("\n[{} PnPL Component]".format(item_key))
            component_status = item[item_key]
            for p in component_status:
                print("-- {}: {}".format(p, component_status[p]))
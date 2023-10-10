
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
import sys
import json
from datetime import datetime
# import requests #NOTE for next version
from st_pnpl.DTDL import device_template_model as DTM

def generate_datetime_string():
    now = datetime.now()
    datetime_string = now.strftime("%Y-%m-%d %H:%M:%S,%f")[:-3]
    return datetime_string

def print_error(text):
    print("\033[31m{}\033[0m".format(text))

def print_warning(text):
    print("\033[33m{}\033[0m".format(text))

class DeviceTemplateManager:

    def __init__(self, device_template_json: dict) -> None:
        self.device_template_model = device_template_json

    def __get_interface_list(self):
        interfaces = []
        for d in self.device_template_model:
            if "contents" in d:
                interfaces.append(DTM.InterfaceElement.from_dict(d)) 
        return interfaces
    
    def __is_root_interface(self, interface):
        return interface.contents[0].type == DTM.ContentType.COMPONENT

    def get_root_component(self):
        for interface in self.__get_interface_list():
            if self.__is_root_interface(interface):
                return interface
    
    def get_components(self):    
        component_dict = {}
        for content in self.get_root_component().contents:
            if content.type == DTM.ContentType.COMPONENT:
                for interface in self.__get_interface_list():
                    if(content.schema == interface.id):
                        component_dict[content.name] = interface
        return component_dict

    def get_components_name_list(self):
        comp_names_list = []
        for c in self.get_components():
            comp_names_list.append(c.name)
        return comp_names_list

    def get_component(self, comp_name:str):
        try:
            res = self.get_components()[comp_name]
            return res
        except:
            print_error("{} - HSDatalogApp.{} - ERROR - Component \'{}\' doesn't exist in your selected Device Template.".format(generate_datetime_string(), __name__, comp_name))
            # print("DeviceTemplateManager - ERROR - Component \'{}\' doesn't exist in your selected Device Template".format(comp_name))

    @staticmethod
    def remove_custom_dtdl_model(board_id, fw_id):
        usb_device_catalog_path = os.path.join(os.path.dirname(sys.modules[__name__].__file__),"usb_device_catalog.json")
        with open(usb_device_catalog_path, "r") as catalog:
            catalog_dict = json.load(catalog)
            for entry in catalog_dict: 
                if entry["board_id"] == board_id and entry["fw_id"] == fw_id:
                    target_file_path = entry["custom_dtmi"]
                    if os.path.exists(target_file_path):
                        os.remove(target_file_path)
                    entry["custom_dtmi"] = ""
        
        with open(usb_device_catalog_path, "w") as catalog:
            json.dump(catalog_dict, catalog, indent=4)

    @staticmethod
    def add_dtdl_model(board_id:int, fw_id:int, dtdl_model_name, dtdl_model_json):
        board_id = hex(board_id)
        fw_id = hex(fw_id)    
        new_dtdl = {}
        usb_device_catalog_path = os.path.join(os.path.dirname(sys.modules[__name__].__file__),"usb_device_catalog.json")
        with open(usb_device_catalog_path, "r") as catalog:
            catalog_dict = json.load(catalog)
            dtm_updated = False
            target_folder = os.path.join(os.path.dirname(sys.modules[__name__].__file__), "dtmi", "custom")
            target_file_path = os.path.join( target_folder, os.path.basename(dtdl_model_name) + ".json")
            for entry in catalog_dict: 
                if entry["board_id"] == board_id and entry["fw_id"] == fw_id:
                    # entry["local_dtmi"] = target_file_path
                    entry["custom_dtmi"] = target_file_path
                    # print("Local version of exixting Device Template Updated [{},{}]".format(board_id, fw_id))
                    print("{} - HSDatalogApp.{} - INFO - Local version of exixting Device Template Updated [{},{}]".format(generate_datetime_string(), __name__, board_id, fw_id))
                    dtm_updated = True
                    break
            if dtm_updated == False:
                new_dtdl["board_id"] = board_id
                new_dtdl["fw_id"] = fw_id
                # new_dtdl["az_cloud_dtmi"] = "" #NOTE for next version
                # new_dtdl["st_cloud_dtmi"] = "" #NOTE for next version
                new_dtdl["custom_dtmi"] = target_file_path
                catalog_dict.append(new_dtdl)
                # print("Added new Device Template [{},{}]".format(board_id, fw_id))
                print("{} - HSDatalogApp.{} - INFO - Added new Device Template [{},{}]".format(generate_datetime_string(), __name__, board_id, fw_id))

        if not os.path.exists(target_folder):
            os.makedirs(target_folder)
        file = open(target_file_path, "w+")
        file.write(dtdl_model_json)
        
        with open(usb_device_catalog_path, "w") as catalog:
            json.dump(catalog_dict, catalog, indent=4)

    @staticmethod
    def query_dtdl_model(board_id, fw_id):
        usb_device_catalog_path = os.path.join(os.path.dirname(sys.modules[__name__].__file__),"usb_device_catalog.json")
        with open(usb_device_catalog_path, "r") as catalog:
            temp = json.load(catalog)
            dtdl_model_ids = []
            for entry in temp:
                if entry["board_id"] == board_id and entry["fw_id"] == fw_id:
                    if "custom_dtmi" in entry and entry["custom_dtmi"] != "":
                        # print("DeviceTemplateManager - ALERT - CUSTOM User dtmi Overwrites the base supported model. Call remove_custom_dtdl_model(...) to restore the original one.")
                        print_warning("{} - HSDatalogApp.{} - WARNING - CUSTOM User dtmi Overwrites the base supported model. Call remove_custom_dtdl_model(...) to restore the original one.".format(generate_datetime_string(), __name__))
                        dtdl_model_id = entry["custom_dtmi"]
                        dtdl_model_ids.append(dtdl_model_id)
                        # print("dtmi: {}".format(dtdl_model_id))
                        print("{} - HSDatalogApp.{} - INFO - dtmi: {}".format(generate_datetime_string(), __name__, dtdl_model_id))
                    elif "local_dtmi" in entry and entry["local_dtmi"] != "":
                        # print("dtmi found in locally in base supported models")
                        print("{} - HSDatalogApp.{} - INFO - dtmi found in locally in base supported models".format(generate_datetime_string(), __name__))
                        dtdl_model_id = entry["local_dtmi"]
                        dtdl_model_ids.append(dtdl_model_id)
                        # print("dtmi: {}".format(dtdl_model_id))
                        print("{} - HSDatalogApp.{} - INFO - dtmi: {}".format(generate_datetime_string(), __name__, dtdl_model_id))
                    #NOTE for the next version
                    # print("Searching the corresponding Device Template model in Azure Device Models repository...")
                    # dtdl_model_id = ""
                    # if "az_cloud_dtmi" in entry and entry["az_cloud_dtmi"] != "":
                    #     print("dtmi found in Azure Device Models repository")
                    #     dtdl_model_id = entry["az_cloud_dtmi"]
                    #     print("dtmi: {}".format(dtdl_model_id))
                    # else:
                    #     print("no dtmi found in Azure Device Models repository. Searching in ST Device Models repository...")
                    #     if "st_cloud_dtmi" in entry and entry["st_cloud_dtmi"] != "":
                    #         print("dtmi found in ST Device Models repository")
                    #         dtdl_model_url = entry["st_cloud_dtmi"]
                    #         try:
                    #             response = requests.get(dtdl_model_url)
                    #             dtdl_json = json.loads(response.text)                            
                    #             return dtdl_json
                    #         except requests.exceptions.ConnectionError:
                    #             print("no dtmi found in ST Device Models repository. Searching in local base supported models...")
                    #             if "local_dtmi" in entry and entry["local_dtmi"] != "":
                    #                 print("dtmi found in locally in base supported models")
                    #                 dtdl_model_id = entry["local_dtmi"]
                    #                 print("dtmi: {}".format(dtdl_model_id))
                    #     else:
                    #         print("no dtmi found in ST Device Models repository. Searching in local base supported models...")
                    #         if "local_dtmi" in entry and entry["local_dtmi"] != "":
                    #             print("dtmi found in locally in base supported models")
                    #             dtdl_model_id = entry["local_dtmi"]
                    #             print("dtmi: {}".format(dtdl_model_id))
            if len(dtdl_model_ids) == 1:
                dtdl_json_path = os.path.join(os.path.dirname(sys.modules[__name__].__file__), dtdl_model_id)
                with open(dtdl_json_path, "r") as device_model:
                    return json.load(device_model)
            else:
                device_models = {}
                for dtm_id in dtdl_model_ids:
                    dtdl_json_path = os.path.join(os.path.dirname(sys.modules[__name__].__file__), dtm_id)
                    with open(dtdl_json_path, "r") as device_model:
                        device_models[dtm_id] = json.load(device_model)
                return device_models
        return ""

import json

class PnPLCmdField:
    def __init__(self):
        self.name
        self.value

class PnPLCMDManager:

    def __init__(self):
        pass

    # @staticmethod #OLD till HSDatalog2 v1.0.1
    # def create_get_presentation_string_cmd():
    #     message = {"system_info":""}
    #     return json.dumps(message)
    
    @staticmethod #NEW from HSDatalog2  v1.1.0
    def create_get_presentation_string_cmd():
        message = {"get_presentation":""}
        return json.dumps(message)

    @staticmethod
    def create_get_device_status_cmd():
        message = {"get_status":"all"}
        return json.dumps(message)
    
    @staticmethod
    def create_get_component_status_cmd(comp_name: str):
        message = {"get_status": comp_name }
        return json.dumps(message)

    @staticmethod
    def create_set_property_cmd(comp_name, prop_name, prop_value):
        message = {
            comp_name: {
                prop_name : prop_value
                }
            }
        return json.dumps(message)

    #TODO
    # @staticmethod
    # def create_set_property_cmd(comp_name, prop_list:dict):
    #     message = {
    #         comp_name: prop_list
    #         }
    #     return json.dumps(message)
    
    @staticmethod
    def create_command_cmd(comp_name: str, command_name: str, req_name: str = None, req_value = None):
        if req_name is None:
            message = {
                comp_name + "*" + command_name: ""
                }
        elif req_name is None and req_value is not None:
            message = {
                comp_name + "*" + command_name: req_value
                }
        elif req_name is not None and req_value is not None:
            message = {
            comp_name + "*" + command_name: {
                    req_name: req_value
                }
            }
        return json.dumps(message)

    #TODO
    # @staticmethod
    # def create_command_cmd(comp_name: str, command_name: str, req_name: str, cmd_fields:dict):
    #     message = {
    #         comp_name + "*" + command_name: {
    #                 req_name: cmd_fields
    #             }
    #         }
    #     return json.dumps(message)
        
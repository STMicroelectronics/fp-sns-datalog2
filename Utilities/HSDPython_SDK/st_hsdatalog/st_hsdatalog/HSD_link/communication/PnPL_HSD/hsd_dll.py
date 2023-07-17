
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

import sys
import os
import ctypes
import platform
from ctypes import util, cdll

ST_HS_DATALOG_OK = 0
ST_HS_DATALOG_ERROR = -1
ptrs = []

def wrap_hsd_function(lib, funcname, restype, argtypes):
    func = lib.__getattr__(funcname)
    func.restype = restype
    func.argtypes = argtypes
    return func

class hsd_data_ready_callback(ctypes.Structure):
    _fields_ = [
        ('callback', ctypes.CFUNCTYPE(
                ctypes.c_int,
                ctypes.c_int,
                ctypes.c_int,
                ctypes.c_int,
                ctypes.POINTER(ctypes.c_uint8),
                ctypes.c_int,))
    ]

class HSD_Dll_Wrapper:

    def __init__(self):
        
        print(platform.architecture()[0])
        dll_name = ""
        if platform.system() == 'Linux':
            if platform.machine() == 'armv7l' or platform.machine() == 'aarch64':
                dll_name = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + "/libhs_datalog/raspberryPi/libhs_datalog_v2.so"
            else:
                dll_name = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + "/libhs_datalog/linux/libhs_datalog_v2.so"
            self._hsd_dll = cdll.LoadLibrary(dll_name)
        elif platform.system() == 'Windows':
            dll_subfolder = platform.architecture()[0]
            dll_name = "libhs_datalog_v2"
            dllabspath = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + os.path.sep + os.path.join("libhs_datalog", dll_subfolder)
            os.environ['PATH'] = dllabspath + os.pathsep + os.environ['PATH']
            self._hsd_dll = cdll.LoadLibrary(util.find_library(dll_name))

        callaback_type = ctypes.CFUNCTYPE(None)

        self.hs_datalog_register_usb_hotplug_callback = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_register_usb_hotplug_callback',
            ctypes.c_int,
            (callaback_type, callaback_type)
        )

        ## WP2 [OK]
        self.hs_datalog_open = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_open',
            ctypes.c_int,
            None
        )

        ## WP2 [OK]
        self.hs_datalog_close = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_close',
            ctypes.c_int,
            None
        )

        ## WP2 [OK]
        self.hs_datalog_free = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_free',
            ctypes.c_int,
            [ctypes.c_char_p]
        )

        ## WP2 [OK]
        self.hs_datalog_get_device_number = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_device_number',
            ctypes.c_int,
            [ctypes.POINTER(ctypes.c_int)]
        )

        ## WP2 [OK]
        self.hs_datalog_get_version = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_version',
            ctypes.c_int,
            [ctypes.POINTER(ctypes.c_char_p)]
        )

        ## WP2 [OK]
        self.hs_datalog_send_message = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_send_message',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_char_p)]
        )

        ## WP2 [TODO] missing implementation
        self.hs_datalog_send_data = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_send_data',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int]
        )

        ## WP2 [OK]
        self.hs_datalog_get_available_data_size = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_available_data_size',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p, ctypes.POINTER(ctypes.c_int)]
        )

        ## WP2 [OK]
        self.hs_datalog_get_data = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_data',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        ## WP2 [OK]
        self.hs_datalog_get_presentation = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_presentation',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
        )

        ## Only from HSD2 v >= 1.2.0
        self.hs_datalog_get_identity = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_identity',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
        )

        ## WP2 [TODO]
        self.hs_datalog_get_device_alias = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_device_alias',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        ## WP2 [TODO]
        self.hs_datalog_set_device_alias = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_device_alias',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p]
        )
        
        ## WP2 [OK]
        self.hs_datalog_start_log = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_start_log',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int]
        )

        ## WP2 [OK]
        self.hs_datalog_stop_log = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_stop_log',
            ctypes.c_int,
            [ctypes.c_int]
        )

        ## WP2 [OK]
        self.hs_datalog_get_device_status = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_device_status',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        ## WP2 [TODO]
        self.hs_datalog_set_device_status = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_device_status',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p]
        )

        ## WP2 [OK]
        self.hs_datalog_get_component_status = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_component_status',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p), ctypes.c_char_p]
        )

        ## WP2 [TODO]
        self.hs_datalog_get_components_number = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_components_number',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        ## WP2 [TODO]
        self.hs_datalog_get_sensor_components_number = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sensor_components_number',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.c_bool]
        )

        ## WP2 [TODO]
        self.hs_datalog_get_algorithm_components_number = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_algorithm_components_number',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.c_bool]
        )

        # ## WP2 reworked
        # self.hs_datalog_get_sensor_components_names = wrap_hsd_function(
        #     self._hsd_dll,
        #     'hs_datalog_get_sensor_components_names',
        #     ctypes.c_int,
        #     [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p), ctypes.c_bool]
        # )
        
        # ## WP2 reworked
        # self.hs_datalog_get_algorithm_components_names = wrap_hsd_function(
        #     self._hsd_dll,
        #     'hs_datalog_get_algorithm_components_names',
        #     ctypes.c_int,
        #     [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p), ctypes.c_bool]
        # )

        ## WP2 [TODO]
        self.hs_datalog_get_boolean_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_boolean_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_bool), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )

        ## WP2 [TODO]
        self.hs_datalog_set_boolean_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_boolean_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_bool, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )

        ## WP2 [TODO]
        self.hs_datalog_get_integer_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_integer_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )

        ## WP2 [TODO]
        self.hs_datalog_set_integer_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_integer_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )
        
        ## WP2 [TODO]
        self.hs_datalog_get_float_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_float_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_float), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )

        ## WP2 [TODO]
        self.hs_datalog_set_float_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_float_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_float, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )

        ## WP2 [TODO]
        self.hs_datalog_get_string_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_string_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )

        ## WP2 [TODO]
        self.hs_datalog_set_string_property = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_string_property',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
        )
        
        ## WP2 [TODO] missing implementation
        self.hs_datalog_set_data_ready_callback = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_data_ready_callback',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p, ctypes.POINTER(ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int))]
        )

class HSD_Dll:

    def __init__(self):
        self.hsd_wrapper = HSD_Dll_Wrapper()
        self.plug_callaback_ptr = None
        self.unplug_callaback_ptr = None

    def hs_datalog_register_usb_hotplug_callback(self, plug_callback, unplug_callback) -> bool:
        try:
            callaback_type = ctypes.CFUNCTYPE(None)
            self.plug_callaback_ptr = callaback_type(plug_callback)
            self.unplug_callaback_ptr = callaback_type(unplug_callback)
            return self.hsd_wrapper.hs_datalog_register_usb_hotplug_callback(self.plug_callaback_ptr, self.unplug_callaback_ptr) == ST_HS_DATALOG_OK
        except OSError:
            return False
    
    def hs_datalog_open(self) -> bool:
        res = None
        try:
            res = self.hsd_wrapper.hs_datalog_open()
            return res == ST_HS_DATALOG_OK
        except OSError:
            return False

    def hs_datalog_close(self) -> bool:
        return self.hsd_wrapper.hs_datalog_close() == ST_HS_DATALOG_OK
    
    def __hs_datalog_free(self, ptr) -> bool:
        res = self.hsd_wrapper.hs_datalog_free(ptr)
        if (res != ST_HS_DATALOG_OK):
            sys.exit("Error in memory free!")
        return (res)

    def hs_datalog_get_device_number(self) -> [bool, int]:
        nof_devices = ctypes.c_int(0)
        return [self.hsd_wrapper.hs_datalog_get_device_number(ctypes.byref(nof_devices)) == ST_HS_DATALOG_OK, nof_devices.value]

    def hs_datalog_get_version(self) -> [str, int]:
        version_string = ctypes.c_char_p()
        version_sizeof = self.hsd_wrapper.hs_datalog_get_version(ctypes.byref(version_string))
        vs = version_string.value.decode('UTF-8')
        self.__hs_datalog_free(version_string)
        return [vs, version_sizeof]

    def hs_datalog_send_message(self, dId : int, msg : str, msg_len : int) -> [bool, int, str]:
        dIdC = ctypes.c_int(dId)
        msgC = ctypes.c_char_p(msg.encode('UTF-8'))
        msg_lenC = ctypes.c_int(msg_len)
        data_lenC = ctypes.c_int(0)
        dataC = ctypes.c_char_p()
        res = self.hsd_wrapper.hs_datalog_send_message(dIdC, msgC, msg_lenC, ctypes.byref(data_lenC), ctypes.byref(dataC))
        if res != ST_HS_DATALOG_ERROR:
            if dataC.value is not None:
                data = dataC.value.decode('UTF-8')
            else:
                data = "Command sent successfully!"
        else:
            return (False, 0, "[ERROR] - Command not sent correctly!")
        self.__hs_datalog_free(dataC)
        return (res == ST_HS_DATALOG_OK, data_lenC, data)

    #TODO hs_datalog_send_data missing implementation

    def hs_datalog_get_available_data_size(self, dId : int, comp_name : str) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        comp_nameC = ctypes.c_char_p(comp_name.encode('utf-8'))
        available_dataC = ctypes.c_int(0)
        res = self.hsd_wrapper.hs_datalog_get_available_data_size(dIdC, comp_nameC, available_dataC)
        return ( res == ST_HS_DATALOG_OK, available_dataC.value)

    def hs_datalog_get_data(self, dId : int, comp_name : str, size : int) -> [bool, bytes, int]:
        dIdC = ctypes.c_int(dId)
        comp_nameC = ctypes.c_char_p(comp_name.encode('utf-8'))
        data_block = (ctypes.c_uint8 * size)()
        dataC = ctypes.cast(data_block, ctypes.POINTER(ctypes.c_uint8))
        sizeC = ctypes.c_int(size)
        actual_sizeC = ctypes.c_int(0)
        res = self.hsd_wrapper.hs_datalog_get_data(dIdC, comp_nameC, dataC, sizeC, actual_sizeC)
        dataCcontent = ctypes.string_at(dataC,sizeC)
        return (res == ST_HS_DATALOG_OK, dataCcontent, sizeC.value)

    def hs_datalog_get_presentation(self, dId : int) -> [bool, int, int]:
        dIdC = ctypes.c_int(dId)
        bId = ctypes.c_int()
        fwId = ctypes.c_int()
        res = self.hsd_wrapper.hs_datalog_get_presentation(dIdC, ctypes.byref(bId), ctypes.byref(fwId))
        return ( res == ST_HS_DATALOG_OK, bId.value, fwId.value)
    
    def hs_datalog_get_identity(self, dId : int) -> [bool, int, int]:
        dIdC = ctypes.c_int(dId)
        bId = ctypes.c_int()
        fwId = ctypes.c_int()
        res = self.hsd_wrapper.hs_datalog_get_identity(dIdC, ctypes.byref(bId), ctypes.byref(fwId))
        return ( res == ST_HS_DATALOG_OK, bId.value, fwId.value)

    def hs_datalog_get_device_alias(self, dId : int) -> [bool, str]:
        device_name = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_device_alias(dIdC, device_name)
        dname = device_name.value.decode('UTF-8')
        self.__hs_datalog_free(device_name)
        return (res == ST_HS_DATALOG_OK, dname)

    def hs_datalog_set_device_alias(self, dId : int, alias : str) -> [bool, str]:       
        dIdC = ctypes.c_int(dId)
        aliasC = ctypes.c_char_p(alias.encode('utf-8'))
        return (self.hsd_wrapper.hs_datalog_set_device_alias(dIdC, aliasC) == ST_HS_DATALOG_OK)

    def hs_datalog_start_log(self, dId : int, interface : int) -> bool:
        dIdC = ctypes.c_int(dId)
        interfaceC = ctypes.c_int(interface)
        return (self.hsd_wrapper.hs_datalog_start_log(dIdC, interfaceC) == ST_HS_DATALOG_OK)

    def hs_datalog_stop_log(self, dId : int) -> bool:
        dIdC = ctypes.c_int(dId)
        return (self.hsd_wrapper.hs_datalog_stop_log(dIdC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_device_status(self, dId : int) -> [bool, str]:
        device = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_device_status(dIdC, ctypes.byref(device))
        if res != ST_HS_DATALOG_ERROR:
            d = device.value.decode('UTF-8')
        else:
            return (False, None)
        self.__hs_datalog_free(device)
        return (res == ST_HS_DATALOG_OK, d)

    def hs_datalog_set_device_status(self, dId : int, device_status : str) -> bool:
        dIdC = ctypes.c_int(dId)
        dStatusC = ctypes.c_char_p(device_status.encode('utf-8'))
        return (self.hsd_wrapper.hs_datalog_set_device_status(dIdC, dStatusC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_component_status(self, dId : int, comp_name : str) -> [bool, str]:
        component = ctypes.c_char_p()
        comp_nameC = ctypes.c_char_p(comp_name.encode('utf-8'))
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_component_status(dIdC, ctypes.byref(component), comp_nameC)
        if res != ST_HS_DATALOG_ERROR:
            if component != '' and component.value != None:
                try:
                    s = component.value.decode('UTF-8')
                except UnicodeDecodeError:
                    return (False, None)    
            else:
                return (False, None)
        else:
            return (False, None)
        self.__hs_datalog_free(component)
        return (res == ST_HS_DATALOG_OK, s)

    def hs_datalog_get_components_number(self, dId : int) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        comp_number = ctypes.c_int()
        res = self.hsd_wrapper.hs_datalog_get_components_number(dIdC, ctypes.byref(comp_number))
        return ( res == ST_HS_DATALOG_OK, comp_number.value)

    def hs_datalog_get_sensor_components_number(self, dId : int, only_enabled : bool) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        only_enabledC = ctypes.c_bool(only_enabled)
        comp_number = ctypes.c_int()
        res = self.hsd_wrapper.hs_datalog_get_sensor_components_number(dIdC, ctypes.byref(comp_number), only_enabledC)
        return ( res == ST_HS_DATALOG_OK, comp_number.value)

    def hs_datalog_get_algorithm_components_number(self, dId : int, only_enabled : bool) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        only_enabledC = ctypes.c_bool(only_enabled)
        comp_number = ctypes.c_int()
        res = self.hsd_wrapper.hs_datalog_get_algorithm_components_number(dIdC, ctypes.byref(comp_number), only_enabledC)
        return ( res == ST_HS_DATALOG_OK, comp_number.value)

    # def hs_datalog_get_sensor_components_names(self, dId : int, only_enabled : bool) -> [bool, str]: #TODO here must be a list not a str
    #     dIdC = ctypes.c_int(dId)
    #     comp_names = ctypes.c_char_p()
    #     only_enabledC = ctypes.c_bool(only_enabled)
    #     res = self.hsd_wrapper.hs_datalog_get_sensor_components_names(dIdC, ctypes.byref(comp_names), only_enabledC)
    #     if res != ST_HS_DATALOG_ERROR:
    #         if comp_names != '':
    #             try:
    #                 s = comp_names.value.decode('UTF-8')
    #             except UnicodeDecodeError:
    #                 return (False, None)    
    #         else:
    #             return (False, None)
    #     else:
    #         return (False, None)
    #     self.__hs_datalog_free(comp_names)
    #     return (res == ST_HS_DATALOG_OK, s)
    
    # def hs_datalog_get_algorithm_components_names(self, dId : int, only_enabled : bool) -> [bool, str]: #TODO here must be a list not a str
    #     dIdC = ctypes.c_int(dId)
    #     only_enabledC = ctypes.c_bool(only_enabled)
    #     comp_names = ctypes.POINTER(ctypes.c_char)
    #     res = self.hsd_wrapper.hs_datalog_get_algorithm_components_names(dIdC, ctypes.byref(comp_names), only_enabledC)
    #     #TODO manage comp_names.value in a loop???
    #     #TODO call the hsdatalog free function
    #     return ( res == ST_HS_DATALOG_OK, comp_names.value)

    def hs_datalog_get_boolean_property(self, dId : int, comp_name : str, prop_name : str, sub_prop_name : str = None) -> [bool, bool]:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_bool(0)
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        res = self.hsd_wrapper.hs_datalog_get_boolean_property(dIdC, ctypes.byref(valueC), comp_nameC, prop_nameC, sub_prop_nameC)
        return (res == ST_HS_DATALOG_OK, valueC.value)

    def hs_datalog_set_boolean_property(self, dId : int, value : bool, comp_name : str, prop_name : str, sub_prop_name : str = None) -> bool:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_bool(value)
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        return (self.hsd_wrapper.hs_datalog_set_boolean_property(dIdC, valueC, comp_nameC, prop_nameC, sub_prop_nameC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_integer_property(self, dId : int, comp_name : str, prop_name : str,  sub_prop_name : str = None) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_int(0)
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        return (self.hsd_wrapper.hs_datalog_get_integer_property(dIdC, ctypes.byref(valueC), comp_nameC, prop_nameC, sub_prop_nameC) == ST_HS_DATALOG_OK, valueC.value)

    def hs_datalog_set_integer_property(self, dId : int, value : int, comp_name : str, prop_name : str, sub_prop_name : str = None) -> bool:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_int(value)
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        return (self.hsd_wrapper.hs_datalog_set_integer_property(dIdC, valueC, comp_nameC, prop_nameC, sub_prop_nameC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_float_property(self, dId : int, comp_name : str, prop_name : str, sub_prop_name : str = None) -> [bool, float]:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_float(0)
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        return (self.hsd_wrapper.hs_datalog_get_float_property(dIdC, ctypes.byref(valueC), comp_nameC, prop_nameC, sub_prop_nameC) == ST_HS_DATALOG_OK, valueC.value)

    def hs_datalog_set_float_property(self, dId : int, value : float, comp_name : str, prop_name : str, sub_prop_name : str = None) -> bool:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_float(value)
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        return (self.hsd_wrapper.hs_datalog_set_float_property(dIdC, valueC, comp_nameC, prop_nameC, sub_prop_nameC) == ST_HS_DATALOG_OK)
    
    def hs_datalog_get_string_property(self, dId : int, comp_name : str, prop_name : str, sub_prop_name : str) -> [bool, str]:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_char_p()
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        res = self.hsd_wrapper.hs_datalog_get_string_property(dIdC, ctypes.byref(valueC), comp_nameC, prop_nameC, sub_prop_nameC)
        value = valueC.value.decode('UTF-8')
        self.__hs_datalog_free(valueC)
        return (res == ST_HS_DATALOG_OK, value)

    def hs_datalog_set_string_property(self, dId : int, value : str, comp_name : str, prop_name : str, sub_prop_name : str = None) -> bool:
        dIdC = ctypes.c_int(dId)
        valueC = ctypes.c_char_p(value.encode('UTF-8'))
        comp_nameC = ctypes.c_char_p(comp_name.encode('UTF-8'))
        prop_nameC = ctypes.c_char_p(prop_name.encode('UTF-8'))
        if sub_prop_name is None:
            sub_prop_nameC = None
        else:
            sub_prop_nameC = ctypes.c_char_p(sub_prop_name.encode('UTF-8'))
        return (self.hsd_wrapper.hs_datalog_set_string_property(dIdC, valueC, comp_nameC, prop_nameC, sub_prop_nameC) == ST_HS_DATALOG_OK)
        
    #TODO hs_datalog_set_data_ready_callback missing implementation
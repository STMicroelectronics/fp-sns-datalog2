#
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
import ctypes
import platform
from ctypes import util, cdll

from st_hsdatalog.HSD_utils.exceptions import MemoryFreeError, UnsupportedPlatformError
import st_hsdatalog.HSD_utils.logger as logger

log = logger.get_logger(__name__)

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
        
        dll_name = ""
        log.info("{} - {}".format(platform.system(), platform.architecture()[0]))
        if platform.system() == 'Linux':
            if platform.machine() == 'armv7l':
                dll_name = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + "/libhs_datalog/raspberryPi/libhs_datalog_v1.so"
            elif platform.machine() == 'aarch64':
                arch = platform.architecture()[0]
                if arch == "32bit":
                    dll_name = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + "/libhs_datalog/raspberryPi4_32bit/libhs_datalog_v1.so"
                else:
                    dll_name = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + "/libhs_datalog/raspberryPi4_64bit/libhs_datalog_v1.so"
            else:
                dll_name = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + "/libhs_datalog/linux/libhs_datalog_v1.so"
            self._hsd_dll = cdll.LoadLibrary(dll_name)
        elif platform.system() == 'Windows':
            dll_subfolder = platform.architecture()[0]
            dll_name = "libhs_datalog_v1"

            dllabspath = os.path.split(os.path.dirname(os.path.abspath(__file__)))[0] + os.path.sep + os.path.join("libhs_datalog", dll_subfolder)
            os.environ['PATH'] = dllabspath + os.pathsep + os.environ['PATH']
            self._hsd_dll = cdll.LoadLibrary(util.find_library(dll_name))
        else:
            log.error("Unsupported Platform: {}".format(platform.system()))
            raise UnsupportedPlatformError(format(platform.system()))
    
        self.hs_datalog_open = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_open',
            ctypes.c_int,
            None
        )

        self.hs_datalog_close = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_close',
            ctypes.c_int,
            None
        )

        self.hs_datalog_get_device_number = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_device_number',
            ctypes.c_int,
            [ctypes.POINTER(ctypes.c_int)]
        )

        self.hs_datalog_send_message = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_send_message',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p, ctypes.c_int, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_device = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_device',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_device_descriptor = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_device_descriptor',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_sensor_descriptor = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sensor_descriptor',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_subsensor_descriptor = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_subsensor_descriptor',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_subsensor_status = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_subsensor_status',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_sensor_number = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sensor_number',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        self.hs_datalog_get_sub_sensor_number = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sub_sensor_number',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        self.hs_datalog_get_sensor_name = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sensor_name',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )
        
        self.hs_datalog_get_sub_sensor_name = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sub_sensor_name',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_device_name = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_device_name',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )
        
        self.hs_datalog_set_ODR = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_ODR',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_float]
        )

        self.hs_datalog_get_ODR = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_ODR',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_float)]
        )

        self.hs_datalog_get_measured_ODR = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_measured_ODR',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_float)]
        )
        
        self.hs_datalog_get_initial_offset = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_initial_offset',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_float)]
        )

        self.hs_datalog_set_FS = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_FS',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_float]
        )

        self.hs_datalog_get_FS = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_FS',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_float)]
        )

        self.hs_datalog_set_samples_per_timestamp = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_samples_per_timestamp',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
        )

        self.hs_datalog_get_samples_per_timestamp = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_samples_per_timestamp',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        self.hs_datalog_set_sensor_active = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_sensor_active',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_bool]
        )

        self.hs_datalog_set_sub_sensor_active = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_sub_sensor_active',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_bool]
        )

        self.hs_datalog_get_sub_sensor_active = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sub_sensor_active',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_bool)]
        )

        self.hs_datalog_start_log = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_start_log',
            ctypes.c_int,
            [ctypes.c_int]
        )

        self.hs_datalog_stop_log = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_stop_log',
            ctypes.c_int,
            [ctypes.c_int]
        )

        self.hs_datalog_get_available_data_size = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_available_data_size',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        self.hs_datalog_get_data = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_data',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        self.hs_datalog_send_data = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_send_data',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int]
        )

        self.hs_datalog_set_data_ready_callback = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_data_ready_callback',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.CFUNCTYPE(ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_uint8), ctypes.c_int))]
        )

        self.hs_datalog_free = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_free',
            ctypes.c_int,
            [ctypes.c_char_p]
        )

        self.hs_datalog_get_version = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_version',
            ctypes.c_int,
            [ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_get_acquisition_info = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_acquisition_info',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_set_acquisition_param = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_acquisition_param',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p]
        )

        self.hs_datalog_send_UCF_to_MLC = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_send_UCF_to_MLC',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_int]
        )

        self.hs_datalog_get_max_tags = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_max_tags',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
        )

        self.hs_datalog_get_available_tags = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_available_tags',
            ctypes.c_int,
            [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_set_sw_label = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_sw_label',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
        )

        self.hs_datalog_get_sw_label = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_sw_label',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_set_hw_label = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_hw_label',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
        )

        self.hs_datalog_get_hw_label = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_get_hw_label',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
        )

        self.hs_datalog_set_on_sw_tag = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_on_sw_tag',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int]
        )

        self.hs_datalog_set_off_sw_tag = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_set_off_sw_tag',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int]
        )

        self.hs_datalog_enable_hw_tag = wrap_hsd_function(
            self._hsd_dll,
            'hs_datalog_enable_hw_tag',
            ctypes.c_int,
            [ctypes.c_int, ctypes.c_int, ctypes.c_bool]
        )

class HSD_Dll:

    def __init__(self):
        self.hsd_wrapper = HSD_Dll_Wrapper()

    def hs_datalog_open(self) -> bool:
        try:
            return self.hsd_wrapper.hs_datalog_open() == ST_HS_DATALOG_OK
        except OSError:
            return False

    def hs_datalog_close(self) -> bool:
        return self.hsd_wrapper.hs_datalog_close() == ST_HS_DATALOG_OK
    
    def hs_datalog_get_device_number(self) -> [bool, int]:
        nof_devices = ctypes.c_int(0)
        return [self.hsd_wrapper.hs_datalog_get_device_number(ctypes.byref(nof_devices)) == ST_HS_DATALOG_OK, nof_devices.value]

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

    def hs_datalog_get_device(self, dId : int) -> [bool, str]:
        device = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_device(dIdC, ctypes.byref(device))
        if res != ST_HS_DATALOG_ERROR:
            d = device.value.decode('UTF-8')
        else:
            return (False, None)
        self.__hs_datalog_free(device)
        return (res == ST_HS_DATALOG_OK, d)

    def hs_datalog_get_device_descriptor(self, dId : int) -> [bool, str]:
        device_desc = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_device_descriptor(dIdC, ctypes.byref(device_desc))
        dd = device_desc.value.decode('UTF-8')
        self.__hs_datalog_free(device_desc)
        return (res == ST_HS_DATALOG_OK, dd)

    def hs_datalog_get_sensor_descriptor(self, dId : int, sId : int) -> [bool, str]:
        sensor_desc = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        res = self.hsd_wrapper.hs_datalog_get_sensor_descriptor(dIdC, sIdC, ctypes.byref(sensor_desc))
        sd = sensor_desc.value.decode('UTF-8')
        self.__hs_datalog_free(sensor_desc)
        return (res == ST_HS_DATALOG_OK, sd)

    def hs_datalog_get_subsensor_descriptor(self, dId : int, sId : int, ssId : int) -> [bool, str]:
        sub_sensor_desc = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        res = self.hsd_wrapper.hs_datalog_get_subsensor_descriptor(dIdC, sIdC, ssIdC, ctypes.byref(sub_sensor_desc))
        ssd = sub_sensor_desc.value.decode('UTF-8')
        self.__hs_datalog_free(sub_sensor_desc)
        return (res == ST_HS_DATALOG_OK, ssd)

    def hs_datalog_get_subsensor_status(self, dId : int, sId : int, ssId : int) -> [bool, str]:
        sub_sensor_status = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        res = self.hsd_wrapper.hs_datalog_get_subsensor_status(dIdC, sIdC, ssIdC, ctypes.byref(sub_sensor_status))
        ssd = sub_sensor_status.value.decode('UTF-8')
        self.__hs_datalog_free(sub_sensor_status)
        return (res == ST_HS_DATALOG_OK, ssd)

    def hs_datalog_get_sensor_number(self, dId : int) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        nof_sensors = ctypes.c_int(0)
        return (self.hsd_wrapper.hs_datalog_get_sensor_number(dIdC, nof_sensors) == ST_HS_DATALOG_OK, nof_sensors.value)

    def hs_datalog_get_sub_sensor_number(self, dId : int, sId : int) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        nof_sub_sensors = ctypes.c_int(0)
        return (self.hsd_wrapper.hs_datalog_get_sub_sensor_number(dIdC, sIdC, nof_sub_sensors) == ST_HS_DATALOG_OK, nof_sub_sensors.value)

    def hs_datalog_get_sensor_name(self, dId : int, sId : int) -> [bool, str]:
        sensor_name = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        res = self.hsd_wrapper.hs_datalog_get_sensor_name(dIdC, sIdC, sensor_name)
        sname = sensor_name.value.decode('UTF-8')
        self.__hs_datalog_free(sensor_name)
        return (res == ST_HS_DATALOG_OK, sname)

    def hs_datalog_get_sub_sensor_name(self, dId : int, sId : int, ssId : int) -> [bool,str]:
        sub_sensor_name = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        res = self.hsd_wrapper.hs_datalog_get_sub_sensor_name(dIdC, sIdC, ssIdC, sub_sensor_name)
        ssname = sub_sensor_name.value.decode('UTF-8')
        self.__hs_datalog_free(sub_sensor_name)
        return (res == ST_HS_DATALOG_OK, ssname)

    def hs_datalog_get_device_name(self, dId : int) -> [bool, str]:
        device_name = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_device_name(dIdC, device_name)
        dname = device_name.value.decode('UTF-8')
        self.__hs_datalog_free(device_name)
        return (res == ST_HS_DATALOG_OK, dname)

    def hs_datalog_set_ODR(self, dId : int, sId : int, ssId : int, odr : float) -> bool:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        odrC = ctypes.c_float(odr)
        ret = self.hsd_wrapper.hs_datalog_set_ODR(dIdC, sIdC, ssIdC, odrC)
        return (ret == ST_HS_DATALOG_OK)

    def hs_datalog_get_ODR(self, dId : int, sId : int, ssId : int) -> [bool, float]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        odrC = ctypes.c_float(0)
        return (self.hsd_wrapper.hs_datalog_get_ODR(dIdC, sIdC, ssIdC, odrC) == ST_HS_DATALOG_OK, odrC.value)

    def hs_datalog_get_measured_ODR(self, dId : int, sId : int, ssId : int) -> [bool, float]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        odrMC = ctypes.c_float(0)
        #TODO fix this (callable only if the acquisition is started???)
        return [False, -1]
        #return (self.hsd_wrapper.hs_datalog_get_measured_ODR(dIdC, sIdC, ssIdC, odrMC) == ST_HS_DATALOG_OK, odrMC.value)

    def hs_datalog_get_initial_offset(self, dId : int, sId : int,  ssId : int) -> [bool, float]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        offsetC = ctypes.c_float(0)
        return (self.hsd_wrapper.hs_datalog_get_initial_offset(dIdC, sIdC, ssIdC, offsetC) == ST_HS_DATALOG_OK, offsetC.value)
    
    def hs_datalog_set_FS(self, dId : int, sId : int, ssId : int, fs : float) -> bool:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        fsC = ctypes.c_float(fs)
        return (self.hsd_wrapper.hs_datalog_set_FS(dIdC, sIdC, ssIdC, fsC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_FS(self, dId : int, sId : int, ssId : int) -> [bool, float]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        fsC = ctypes.c_float(0)
        return (self.hsd_wrapper.hs_datalog_get_FS(dIdC, sIdC, ssIdC, fsC) == ST_HS_DATALOG_OK, fsC.value)

    def hs_datalog_set_samples_per_timestamp(self, dId : int, sId : int, ssId : int, samples : int) -> bool:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        sptC = ctypes.c_int(samples)
        return (self.hsd_wrapper.hs_datalog_set_samples_per_timestamp(dIdC, sIdC, ssIdC, sptC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_samples_per_timestamp(self, dId : int, sId : int, ssId : int) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        sptC = ctypes.c_int(0)
        return (self.hsd_wrapper.hs_datalog_get_samples_per_timestamp(dIdC, sIdC, ssIdC, sptC) == ST_HS_DATALOG_OK, sptC.value)

    def hs_datalog_set_sensor_active(self, dId : int, sId : int, status : bool) -> bool:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        activeC = ctypes.c_bool(status)
        return (self.hsd_wrapper.hs_datalog_set_sensor_active(dIdC, sIdC, activeC) == ST_HS_DATALOG_OK)

    def hs_datalog_set_sub_sensor_active(self, dId : int, sId : int, ssId : int, status : bool) -> bool:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        activeC = ctypes.c_bool(status)
        return (self.hsd_wrapper.hs_datalog_set_sub_sensor_active(dIdC, sIdC, ssIdC, activeC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_sub_sensor_active(self, dId : int, sId : int, ssId : int) -> [bool, bool]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        activeC = ctypes.c_bool(False)
        return (self.hsd_wrapper.hs_datalog_get_sub_sensor_active(dIdC, sIdC, ssIdC, activeC) == ST_HS_DATALOG_OK, activeC.value)

    def hs_datalog_start_log(self, dId : int) -> bool:
        dIdC = ctypes.c_int(dId)
        return (self.hsd_wrapper.hs_datalog_start_log(dIdC) == ST_HS_DATALOG_OK)

    def hs_datalog_stop_log(self, dId : int) -> bool:
        dIdC = ctypes.c_int(dId)
        return (self.hsd_wrapper.hs_datalog_stop_log(dIdC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_available_data_size(self, dId : int, sId : int, ssId : int) -> [bool, int]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        available_dataC = ctypes.c_int(0)
        res = self.hsd_wrapper.hs_datalog_get_available_data_size(dIdC, sIdC, ssIdC, available_dataC)
        return ( res == ST_HS_DATALOG_OK, available_dataC.value)

    def hs_datalog_get_data(self, dId : int, sId : int, ssId : int, size : int) -> [bool, bytes, int]:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ssIdC = ctypes.c_int(ssId)
        data_block = (ctypes.c_uint8 * size)()
        dataC = ctypes.cast(data_block, ctypes.POINTER(ctypes.c_uint8))
        sizeC = ctypes.c_int(size)
        actual_sizeC = ctypes.c_int(0)
        res = self.hsd_wrapper.hs_datalog_get_data(dIdC, sIdC, ssIdC, dataC, sizeC, actual_sizeC)
        dataCcontent = ctypes.string_at(dataC,sizeC)
        return (res == ST_HS_DATALOG_OK, dataCcontent, sizeC.value)

    def __hs_datalog_free(self, ptr) -> bool:
        res = self.hsd_wrapper.hs_datalog_free(ptr)
        if (res != ST_HS_DATALOG_OK):
            log.error("Error in memory free!")
            raise MemoryFreeError
        return (res)
    
    def hs_datalog_get_version(self) -> [str, int]:
        version_string = ctypes.c_char_p()
        version_sizeof = self.hsd_wrapper.hs_datalog_get_version(ctypes.byref(version_string))
        return [version_string.value.decode('UTF-8'), version_sizeof]

    def hs_datalog_get_acquisition_info(self, dId : int) -> [bool,str]:
        acq_info = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_acquisition_info(dIdC, ctypes.byref(acq_info))
        ai = acq_info.value.decode('UTF-8')
        self.__hs_datalog_free(acq_info)
        return [res == ST_HS_DATALOG_OK, ai]
    
    def hs_datalog_set_acquisition_param(self, dId : int, name : str, description : str) -> bool:
        dIdC = ctypes.c_int(dId)
        acq_nameC = ctypes.c_char_p(name.encode('utf-8'))
        acq_descC = ctypes.c_char_p(description.encode('UTF-8'))
        return (self.hsd_wrapper.hs_datalog_set_acquisition_param(dIdC, acq_nameC, acq_descC) == ST_HS_DATALOG_OK)

    def hs_datalog_send_UCF_to_MLC(self, dId : int, sId : int, ucf_buffer : bytes, length) -> bool:
        dIdC = ctypes.c_int(dId)
        sIdC = ctypes.c_int(sId)
        ucf_bufferC = ctypes.cast(ctypes.create_string_buffer(bytes(ucf_buffer)), ctypes.POINTER(ctypes.c_ubyte)).contents
        lenC = ctypes.c_int(length)
        res = self.hsd_wrapper.hs_datalog_send_UCF_to_MLC(dIdC, sIdC, ucf_bufferC, lenC)
        return (res == ST_HS_DATALOG_OK)

    def hs_datalog_get_available_tags(self, dId : int) -> [bool, str]:
        json_tags = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        res = self.hsd_wrapper.hs_datalog_get_available_tags(dIdC, ctypes.byref(json_tags))
        if res != ST_HS_DATALOG_ERROR:
            tags = json_tags.value.decode('UTF-8')
        else:
            return ST_HS_DATALOG_ERROR
        self.__hs_datalog_free(json_tags)
        return [res == ST_HS_DATALOG_OK, tags]

    def hs_datalog_set_sw_label(self, dId : int, tId : int, label : str):
        dIdC = ctypes.c_int(dId)
        tIdC = ctypes.c_int(tId)
        labelC = ctypes.c_char_p(label.encode('utf-8'))
        return (self.hsd_wrapper.hs_datalog_set_sw_label(dIdC, tIdC, labelC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_sw_label(self, dId : int, tId : int) -> [bool,str]:
        labelC = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        tIdC = ctypes.c_int(tId)
        res = self.hsd_wrapper.hs_datalog_get_sw_label(dIdC, tIdC, ctypes.byref(labelC))
        label = labelC.value.decode('UTF-8')
        self.__hs_datalog_free(labelC)
        return [res != ST_HS_DATALOG_ERROR, label]
    
    def hs_datalog_set_hw_label(self, dId : int, tId : int, label : str):
        dIdC = ctypes.c_int(dId)
        tIdC = ctypes.c_int(tId)
        labelC = ctypes.c_char_p(label.encode('utf-8'))
        return (self.hsd_wrapper.hs_datalog_set_hw_label(dIdC, tIdC, labelC) == ST_HS_DATALOG_OK)

    def hs_datalog_get_hw_label(self, dId : int, tId : int) -> [bool,str]:
        labelC = ctypes.c_char_p()
        dIdC = ctypes.c_int(dId)
        tIdC = ctypes.c_int(tId)
        res = self.hsd_wrapper.hs_datalog_get_hw_label(dIdC, tIdC, ctypes.byref(labelC))
        label = labelC.value.decode('UTF-8')
        self.__hs_datalog_free(labelC)
        return [res != ST_HS_DATALOG_ERROR, label]

    def hs_datalog_set_on_sw_tag(self, dId : int, tId : int) -> bool:
        dIdC = ctypes.c_int(dId)
        tIdC = ctypes.c_int(tId)
        return (self.hsd_wrapper.hs_datalog_set_on_sw_tag(dIdC, tIdC) == ST_HS_DATALOG_OK)

    def hs_datalog_set_off_sw_tag(self, dId : int, tId : int) -> bool:
        dIdC = ctypes.c_int(dId)
        tIdC = ctypes.c_int(tId)
        return (self.hsd_wrapper.hs_datalog_set_off_sw_tag(dIdC, tIdC) == ST_HS_DATALOG_OK)

    def hs_datalog_enable_hw_tag(self, dId : int, tId : int, enabled : bool) -> bool:
        dIdC = ctypes.c_int(dId)
        tIdC = ctypes.c_int(tId)
        enabledC = ctypes.c_bool(enabled)
        return (self.hsd_wrapper.hs_datalog_enable_hw_tag(dIdC, tIdC, enabledC) == ST_HS_DATALOG_OK)

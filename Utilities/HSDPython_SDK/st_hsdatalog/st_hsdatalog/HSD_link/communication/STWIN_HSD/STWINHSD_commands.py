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

import abc
from dataclasses import dataclass
from typing import Any, List, TypeVar, Type, Callable, cast

T = TypeVar("T")


def from_str(x: Any) -> str:
    assert isinstance(x, str)
    return x

def from_int(x: Any) -> int:
    assert isinstance(x, int) and not isinstance(x, bool)
    return x

def from_bool(x: Any) -> bool:
    assert isinstance(x, bool)
    return x

def from_float(x: Any) -> float:
        assert isinstance(x, (float, int)) and not isinstance(x, bool)
        return float(x)

def from_list(f: Callable[[Any], T], x: Any) -> List[T]:
    assert isinstance(x, list)
    return [f(y) for y in x]

def to_class(c: Type[T], x: Any) -> dict:
    assert isinstance(x, c)
    return cast(Any, x).to_dict()

@dataclass
class STWINHSDCmd():
    command: str

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDCmd':
        assert isinstance(obj, dict)
        command = from_str(obj.get("command"))
        return STWINHSDCmd(command)

    def to_dict(self) -> dict:
        result: dict = {}
        result["command"] = from_str(self.command)
        return result


@dataclass
class STWINHSDControlCmd(STWINHSDCmd):
    pass


@dataclass
class STWINHSDStartLoggingCmd(STWINHSDControlCmd):

    def __init__(self):
        self.command = "START"


@dataclass
class STWINHSDStopLoggingCmd(STWINHSDControlCmd):
    def __init__(self):
        self.command = "STOP"


@dataclass
class STWINHSDSaveConfCmd(STWINHSDControlCmd):
    def __init__(self):
        self.command = "SAVE"


@dataclass
class STWINHSDGetCmd(STWINHSDCmd):
    request: str

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDGetCmd':
        assert isinstance(obj, dict)
        request = from_str(obj.get("request"))
        return STWINHSDGetCmd(self.command, request)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDGetCmd, self).to_dict())
        result["request"] = from_str(self.request)
        return result


@dataclass
class STWINHSDGetDeviceCmd(STWINHSDGetCmd):
    def __init__(self):
        self.command = "GET"
        self.request = "device"


@dataclass
class STWINHSDGetDeviceInfoCmd(STWINHSDGetCmd):
    def __init__(self):
        self.command = "GET"
        self.request = "deviceInfo"


@dataclass
class STWINHSDGetSensorCmd(STWINHSDGetCmd):
    sensorId: int

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDGetSensorCmd':
        assert isinstance(obj, dict)
        sensorId = from_int(obj.get("sensorId"))
        return STWINHSDGetSensorCmd(self.command, self.request, sensorId)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDGetSensorCmd, self).to_dict())
        result["sensorId"] = from_int(self.sensorId)
        return result


@dataclass
class STWINHSDGetSensorDescriptorCmd(STWINHSDGetSensorCmd):
    def __init__(self, sensor_id: int):
        self.command = "GET"
        self.request = "descriptor"
        self.sensorId = sensor_id


@dataclass
class STWINHSDGetSubSensorDescriptorCmd(STWINHSDGetSensorCmd):
    def __init__(self, sensor_id: int, sub_sensor_id: int):
        self.command = "GET"
        self.request = "descriptor"
        self.sensorId = sensor_id
        self.subSensorStatus = STWINHSDSubSensorStatusParam(sub_sensor_id)


@dataclass
class STWINHSDGetSubSensorStatusCmd(STWINHSDGetSensorCmd):
    def __init__(self, sensor_id: int, sub_sensor_id: int):
        self.command = "GET"
        self.request = "status"
        self.sensorId = sensor_id
        self.subSensorStatus = STWINHSDSubSensorStatusParam(sub_sensor_id)

@dataclass
class STWINHSDGetTagConfigCmd(STWINHSDGetCmd):
    def __init__(self):
        self.command = "GET"
        self.request = "tag_config"


@dataclass
class STWINHSDGetLogStatusCmd(STWINHSDGetCmd):
    def __init__(self):
        self.command = "GET"
        self.request = "log_status"

@dataclass
class STWINHSDGetAcquisitionInfo(STWINHSDGetCmd):
    def __init__(self):
        self.command = "GET"
        self.request = "acq_info"

@dataclass
class STWINHSDSetCmd(STWINHSDCmd):
    request: str

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetCmd':
        assert isinstance(obj, dict)
        request = from_str(obj.get("request"))
        return STWINHSDSetCmd(self.command, request)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetCmd, self).to_dict())
        result["request"] = from_str(self.request)
        return result


@dataclass
class STWINHSDSetDeviceAliasCmd(STWINHSDSetCmd):
    alias: str

    def __init__(self, alias: str):
        self.command = "SET"
        self.request = "deviceInfo"
        self.alias = alias

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetDeviceAliasCmd':
        assert isinstance(obj, dict)
        alias = from_str(obj.get("alias"))
        return STWINHSDSetDeviceAliasCmd(alias)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetDeviceAliasCmd, self).to_dict())
        result["alias"] = from_str(self.alias)
        return result


@dataclass
class STWINHSDSetSWTagCmd(STWINHSDSetCmd):
    tag_id: int
    enable: bool

    def __init__(self, tag_id: int, enable: bool):
        self.command = "SET"
        self.request = "sw_tag"
        self.tag_id = tag_id
        self.enable = enable

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetSWTagCmd':
        assert isinstance(obj, dict)
        tag_id = from_int(obj.get("ID"))
        enable = from_bool(obj.get("enable"))
        return STWINHSDSetSWTagCmd(tag_id, enable)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetSWTagCmd, self).to_dict())
        result["ID"] = from_int(self.tag_id)
        result["enable"] = from_bool(self.enable)
        return result


@dataclass
class STWINHSDSetSWTagLabelCmd(STWINHSDSetCmd):
    tag_id: int
    label: str

    def __init__(self, tag_id: int, label: str):
        self.command = "SET"
        self.request = "sw_tag_label"
        self.tag_id = tag_id
        self.label = label

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetSWTagLabelCmd':
        assert isinstance(obj, dict)
        tag_id = from_int(obj.get("ID"))
        label = from_str(obj.get("label"))
        return STWINHSDSetSWTagLabelCmd(tag_id, label)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetSWTagLabelCmd, self).to_dict())
        result["ID"] = from_int(self.tag_id)
        result["label"] = from_str(self.label)
        return result

@dataclass
class STWINHSDSetHWTagCmd(STWINHSDSetCmd):
    tag_id: int
    enable: bool

    def __init__(self, tag_id: int, enable: bool):
        self.command = "SET"
        self.request = "hw_tag"
        self.tag_id = tag_id
        self.enable = enable

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetHWTagCmd':
        assert isinstance(obj, dict)
        tag_id = from_int(obj.get("ID"))
        enable = from_bool(obj.get("enable"))
        return STWINHSDSetHWTagCmd(tag_id, enable)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetHWTagCmd, self).to_dict())
        result["ID"] = from_int(self.tag_id)
        result["enable"] = from_bool(self.enable)
        return result


@dataclass
class STWINHSDSetHWTagLabelCmd(STWINHSDSetCmd):
    tag_id: int
    label: str

    def __init__(self, tag_id: int, label: str):
        self.command = "SET"
        self.request = "hw_tag_label"
        self.tag_id = tag_id
        self.label = label

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetHWTagLabelCmd':
        assert isinstance(obj, dict)
        tag_id = from_int(obj.get("ID"))
        label = from_str(obj.get("label"))
        return STWINHSDSetHWTagLabelCmd(tag_id, label)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetHWTagLabelCmd, self).to_dict())
        result["ID"] = from_int(self.tag_id)
        result["label"] = from_str(self.label)
        return result


@dataclass
class STWINHSDSetAcquisitionInfoCmd(STWINHSDSetCmd):
    name: str
    notes: str

    def __init__(self, name: str, notes: str):
        self.command = "SET"
        self.request = "acq_info"
        self.name = name
        self.notes = notes

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetAcquisitionInfoCmd':
        assert isinstance(obj, dict)
        name = from_str(obj.get("name"))
        notes = from_str(obj.get("notes"))
        return STWINHSDSetAcquisitionInfoCmd(name, notes)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetAcquisitionInfoCmd, self).to_dict())
        result["name"] = from_str(self.name)
        result["notes"] = from_str(self.notes)
        return result


@dataclass
class STWINHSDSubSensorStatusParam:
    sensor_id: int

    def __init__(self, sensor_id: int):
        self.sensor_id = sensor_id

    def from_dict(self, obj: Any) -> 'STWINHSDSubSensorStatusParam':
        assert isinstance(obj, dict)
        sensor_id = from_int(obj.get("id"))
        return STWINHSDSubSensorStatusParam(sensor_id)

    def to_dict(self) -> dict:
        result: dict = {}
        result["id"] = from_int(self.sensor_id)
        return result

@dataclass
class IsActiveParam(STWINHSDSubSensorStatusParam):
    isActive: bool
    
    def __init__(self, ss_id, isActive: bool):
        self.sensor_id = ss_id
        self.isActive = isActive

    def from_dict(self, obj: Any) -> 'IsActiveParam':
        assert isinstance(obj, dict)
        ss_id = from_int(obj.get("id"))
        isActive = from_bool(obj.get("isActive"))
        return IsActiveParam(ss_id,isActive)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(IsActiveParam, self).to_dict())
        result["id"] = from_int(self.sensor_id)
        result["isActive"] = from_bool(self.isActive)
        return result


@dataclass
class ODRParam(STWINHSDSubSensorStatusParam):
    odr: float

    def __init__(self, ss_id, odr: float):
        self.sensor_id = ss_id
        self.odr = odr

    def from_dict(self, obj: Any) -> 'ODRParam':
        assert isinstance(obj, dict)
        ss_id = from_int(obj.get("id"))
        odr = from_float(obj.get("ODR"))
        return ODRParam(ss_id, odr)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(ODRParam, self).to_dict())
        result["id"] = from_int(self.sensor_id)
        result["ODR"] = from_float(self.odr)
        return result


@dataclass
class FSParam(STWINHSDSubSensorStatusParam):
    fs: float

    def __init__(self, ss_id, fs: float):
        self.sensor_id = ss_id
        self.fs = fs

    def from_dict(self, obj: Any) -> 'FSParam':
        assert isinstance(obj, dict)
        ss_id = from_int(obj.get("id"))
        fs = from_float(obj.get("FS"))
        return FSParam(ss_id, fs)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(FSParam, self).to_dict())
        result["id"] = from_int(self.sensor_id)
        result["FS"] = from_float(self.fs)
        return result


@dataclass
class SamplePerTSParam(STWINHSDSubSensorStatusParam):
    samplesPerTs: int

    def __init__(self, ss_id, samplesPerTs: int):
        self.sensor_id = ss_id
        self.samplesPerTs = samplesPerTs

    def from_dict(self, obj: Any) -> 'SamplePerTSParam':
        assert isinstance(obj, dict)
        ss_id = from_int(obj.get("id"))
        samplesPerTs = from_int(obj.get("samplesPerTs"))
        return SamplePerTSParam(ss_id, samplesPerTs)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(SamplePerTSParam, self).to_dict())
        result["id"] = from_int(self.sensor_id)
        result["samplesPerTs"] = from_int(self.samplesPerTs)
        return result
    
@dataclass
class UsbDataPacketSizeParam(STWINHSDSubSensorStatusParam):
    usbDataPacketSize: int

    def __init__(self, ss_id, usbDataPacketSize: int):
        self.sensor_id = ss_id
        self.usbDataPacketSize = usbDataPacketSize

    def from_dict(self, obj: Any) -> 'UsbDataPacketSizeParam':
        assert isinstance(obj, dict)
        ss_id = from_int(obj.get("id"))
        usbDataPacketSize = from_int(obj.get("usbDataPacketSize"))
        return UsbDataPacketSizeParam(ss_id, usbDataPacketSize)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(UsbDataPacketSizeParam, self).to_dict())
        result["id"] = from_int(self.sensor_id)
        result["usbDataPacketSize"] = from_int(self.usbDataPacketSize)
        return result

@dataclass
class MLCParam(STWINHSDSubSensorStatusParam):
    mlcConfigSize: int
    mlcConfigData: str

    def __init__(self, ss_id, mlcConfigSize: int, mlcConfigData: str):
        self.sensor_id = ss_id
        self.mlcConfigSize = mlcConfigSize
        self.mlcConfigData = mlcConfigData

    def from_dict(self, obj: Any) -> 'ODRParam':
        assert isinstance(obj, dict)
        ss_id = from_int(obj.get("id"))
        mlcConfigSize = from_int(obj.get("mlcConfigSize"))
        mlcConfigData = from_str(obj.get("mlcConfigData"))
        return MLCParam(ss_id, mlcConfigSize, mlcConfigData)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(MLCParam, self).to_dict())
        result["id"] = from_int(self.sensor_id)
        result["mlcConfigSize"] = from_int(self.mlcConfigSize)
        result["mlcConfigData"] = from_str(self.mlcConfigData)
        return result

@dataclass
class STWINHSDSetSensorCmd(STWINHSDSetCmd):
    sensor_id: int
    subSensorStatus: List[STWINHSDSubSensorStatusParam]

    def __init__(self, sensor_id: int, subSensorStatus: List[STWINHSDSubSensorStatusParam]):
        self.command = "SET"
        self.request = ""
        self.sensor_id = sensor_id
        self.subSensorStatus = subSensorStatus

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetSensorCmd':
        assert isinstance(obj, dict)
        sensor_id = from_int(obj.get("sensorId"))
        subSensorStatus = from_list(STWINHSDSubSensorStatusParam.from_dict, obj.get("subSensorStatus"))
        return STWINHSDSetSensorCmd(sensor_id, subSensorStatus)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetSensorCmd, self).to_dict())
        result["sensorId"] = from_int(self.sensor_id)
        result["subSensorStatus"] = from_list(lambda x: to_class(STWINHSDSubSensorStatusParam, x), self.subSensorStatus)
        return result

@dataclass
class STWINHSDSetMLCSensorCmd(STWINHSDSetCmd):
    sensor_id: int
    subSensorStatus: List[MLCParam]

    def __init__(self, sensor_id: int, subSensorStatus: List[MLCParam]):
        self.command = "SET"
        self.request = "mlc_config"
        self.sensor_id = sensor_id
        self.subSensorStatus = subSensorStatus

    @staticmethod
    def from_dict(self, obj: Any) -> 'STWINHSDSetMLCSensorCmd':
        assert isinstance(obj, dict)
        sensor_id = from_int(obj.get("sensorId"))
        subSensorStatus = from_list(MLCParam.from_dict, obj.get("subSensorStatus"))
        return STWINHSDSetMLCSensorCmd(sensor_id, subSensorStatus)

    def to_dict(self) -> dict:
        result: dict = {}
        result.update(super(STWINHSDSetMLCSensorCmd, self).to_dict())
        result["sensorId"] = from_int(self.sensor_id)
        result["subSensorStatus"] = from_list(lambda x: to_class(MLCParam, x), self.subSensorStatus)
        return result
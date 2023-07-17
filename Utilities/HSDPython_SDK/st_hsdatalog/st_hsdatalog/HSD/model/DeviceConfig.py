
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

from dataclasses import dataclass
from typing import Optional, Any, List, TypeVar, Callable, Type, cast
from uuid import UUID


T = TypeVar("T")


def from_str(x: Any) -> str:
    assert isinstance(x, str)
    return x


def from_none(x: Any) -> Any:
    assert x is None
    return x


def from_union(fs, x):
    for f in fs:
        try:
            return f(x)
        except:
            pass
    assert False


def from_int(x: Any) -> int:
    assert isinstance(x, int) and not isinstance(x, bool)
    return x


def from_list(f: Callable[[Any], T], x: Any) -> List[T]:
    assert isinstance(x, list)
    return [f(y) for y in x]


def from_float(x: Any) -> float:
    assert isinstance(x, (float, int)) and not isinstance(x, bool)
    return float(x)
    

def to_float(x: Any) -> float:
    assert isinstance(x, float)
    return x


def to_class(c: Type[T], x: Any) -> dict:
    assert isinstance(x, c)
    return cast(Any, x).to_dict()


def from_bool(x: Any) -> bool:
    assert isinstance(x, bool)
    return x


@dataclass
class DeviceInfo:
    serial_number: Optional[str] = None
    alias: Optional[str] = None
    part_number: Optional[str] = None
    url: Optional[str] = None
    fw_name: Optional[str] = None
    fw_version: Optional[str] = None
    data_file_ext: Optional[str] = None
    data_file_format: Optional[str] = None
    n_sensor: Optional[int] = None
    model: Optional[str] = None #from v1.5.0
    ble_mac_address: Optional[str] = None #from v1.5.0

    @staticmethod
    def from_dict(obj: Any) -> 'DeviceInfo':
        assert isinstance(obj, dict)
        serial_number = from_union([from_str, from_none], obj.get("serialNumber"))
        alias = from_union([from_str, from_none], obj.get("alias"))
        part_number = from_union([from_str, from_none], obj.get("partNumber"))
        url = from_union([from_str, from_none], obj.get("URL"))
        fw_name = from_union([from_str, from_none], obj.get("fwName"))
        fw_version = from_union([from_str, from_none], obj.get("fwVersion"))
        data_file_ext = from_union([from_str, from_none], obj.get("dataFileExt"))
        data_file_format = from_union([from_str, from_none], obj.get("dataFileFormat"))
        n_sensor = from_union([from_int, from_none], obj.get("nSensor"))
        model = obj.get("model","")
        model:model
        ble_mac_address = obj.get("bleMacAddress","")
        ble_mac_address:ble_mac_address
        return DeviceInfo(serial_number, alias, part_number, url, fw_name, fw_version, data_file_ext, data_file_format, n_sensor, model, ble_mac_address)

    def to_dict(self) -> dict:
        result: dict = {}
        result["serialNumber"] = from_union([from_str, from_none], self.serial_number)
        result["alias"] = from_union([from_str, from_none], self.alias)
        result["partNumber"] = from_union([from_str, from_none], self.part_number)
        result["URL"] = from_union([from_str, from_none], self.url)
        result["fwName"] = from_union([from_str, from_none], self.fw_name)
        result["fwVersion"] = from_union([from_str, from_none], self.fw_version)
        result["dataFileExt"] = from_union([from_str, from_none], self.data_file_ext)
        result["dataFileFormat"] = from_union([from_str, from_none], self.data_file_format)
        result["nSensor"] = from_union([from_int, from_none], self.n_sensor)
        result["model"] = from_union([from_str, from_none], self.model)
        result["bleMacAddress"] = from_union([from_str, from_none], self.ble_mac_address)
        return result


@dataclass
class SamplesPerTs:
    min: Optional[int] = None
    max: Optional[int] = None
    data_type: Optional[str] = None

    @staticmethod
    def from_dict(obj: Any) -> 'SamplesPerTs':
        assert isinstance(obj, dict)
        min = from_union([from_int, from_none], obj.get("min"))
        max = from_union([from_int, from_none], obj.get("max"))
        data_type = from_union([from_str, from_none], obj.get("dataType"))
        return SamplesPerTs(min, max, data_type)

    def to_dict(self) -> dict:
        result: dict = {}
        result["min"] = from_union([from_int, from_none], self.min)
        result["max"] = from_union([from_int, from_none], self.max)
        result["dataType"] = from_union([from_str, from_none], self.data_type)
        return result


@dataclass
class SubSensorDescriptor:
    id: Optional[int] = None
    sensor_type: Optional[str] = None
    dimensions: Optional[int] = None
    dimensions_label: Optional[List[str]] = None
    unit: Optional[str] = None
    data_type: Optional[str] = None
    fs: Optional[List[float]] = None
    odr: Optional[List[float]] = None
    samples_per_ts: Optional[SamplesPerTs] = None

    @staticmethod
    def from_dict(obj: Any) -> 'SubSensorDescriptor':
        assert isinstance(obj, dict)
        id = from_union([from_int, from_none], obj.get("id"))
        sensor_type = from_union([from_str, from_none], obj.get("sensorType"))
        dimensions = from_union([from_int, from_none], obj.get("dimensions"))
        dimensions_label = from_union([lambda x: from_list(from_str, x), from_none], obj.get("dimensionsLabel"))
        unit = from_union([from_str, from_none], obj.get("unit"))
        data_type = from_union([from_str, from_none], obj.get("dataType"))
        fs = from_union([lambda x: from_list(from_float, x), from_none], obj.get("FS"))
        odr = from_union([lambda x: from_list(from_float, x), from_none], obj.get("ODR"))
        samples_per_ts = from_union([SamplesPerTs.from_dict, from_none], obj.get("samplesPerTs"))
        return SubSensorDescriptor(id, sensor_type, dimensions, dimensions_label, unit, data_type, fs, odr, samples_per_ts)

    def to_dict(self) -> dict:
        result: dict = {}
        result["id"] = from_union([from_int, from_none], self.id)
        result["sensorType"] = from_union([from_str, from_none], self.sensor_type)
        result["dimensions"] = from_union([from_int, from_none], self.dimensions)
        result["unit"] = from_union([from_str, from_none], self.unit)
        result["dimensionsLabel"] = from_union([lambda x: from_list(from_str, x), from_none], self.dimensions_label)
        result["dataType"] = from_union([from_str, from_none], self.data_type)
        result["FS"] = from_union([lambda x: from_list(to_float, x), from_none], self.fs)
        result["ODR"] = from_union([lambda x: from_list(to_float, x), from_none], self.odr)
        result["samplesPerTs"] = from_union([lambda x: to_class(SamplesPerTs, x), from_none], self.samples_per_ts)
        return result


@dataclass
class SensorDescriptor:
    sub_sensor_descriptor: Optional[List[SubSensorDescriptor]] = None

    @staticmethod
    def from_dict(obj: Any) -> 'SensorDescriptor':
        assert isinstance(obj, dict)
        sub_sensor_descriptor = from_union([lambda x: from_list(SubSensorDescriptor.from_dict, x), from_none], obj.get("subSensorDescriptor"))
        return SensorDescriptor(sub_sensor_descriptor)

    def to_dict(self) -> dict:
        result: dict = {}
        result["subSensorDescriptor"] = from_union([lambda x: from_list(lambda x: to_class(SubSensorDescriptor, x), x), from_none], self.sub_sensor_descriptor)
        return result


@dataclass
class SubSensorStatus:
    is_active: Optional[bool] = None
    odr: Optional[float] = None
    odr_measured: Optional[float] = None
    initial_offset: Optional[float] = None
    samples_per_ts: Optional[int] = None
    fs: Optional[float] = None
    sensitivity: Optional[float] = None
    usb_data_packet_size: Optional[int] = None
    sd_write_buffer_size: Optional[int] = None
    wifi_data_packet_size: Optional[int] = None
    com_channel_number: Optional[int] = None
    ucf_loaded: Optional[bool] = None

    @staticmethod
    def from_dict(obj: Any) -> 'SubSensorStatus':
        assert isinstance(obj, dict)
        is_active = from_union([from_bool, from_none], obj.get("isActive"))
        odr = from_union([from_float, from_none], obj.get("ODR"))
        odr_measured = from_union([from_float, from_none], obj.get("ODRMeasured"))
        initial_offset = from_union([from_float, from_none], obj.get("initialOffset"))
        samples_per_ts = from_union([from_int, from_none], obj.get("samplesPerTs"))
        fs = from_union([from_float, from_none], obj.get("FS"))
        sensitivity = from_union([from_float, from_none], obj.get("sensitivity"))
        usb_data_packet_size = from_union([from_int, from_none], obj.get("usbDataPacketSize"))
        sd_write_buffer_size = from_union([from_int, from_none], obj.get("sdWriteBufferSize"))
        wifi_data_packet_size = from_union([from_int, from_none], obj.get("wifiDataPacketSize"))
        com_channel_number = from_union([from_int, from_none], obj.get("comChannelNumber"))
        ucf_loaded = from_union([from_bool, from_none], obj.get("ucfLoaded"))
        return SubSensorStatus(is_active, odr, odr_measured, initial_offset, samples_per_ts, fs, sensitivity, usb_data_packet_size, sd_write_buffer_size, wifi_data_packet_size, com_channel_number, ucf_loaded)

    def to_dict(self) -> dict:
        result: dict = {}
        result["isActive"] = from_union([from_bool, from_none], self.is_active)
        result["ODR"] = from_union([to_float, from_none], self.odr)
        result["ODRMeasured"] = from_union([to_float, from_none], self.odr_measured)
        result["initialOffset"] = from_union([from_float, from_none], self.initial_offset)
        result["samplesPerTs"] = from_union([from_int, from_none], self.samples_per_ts)
        result["FS"] = from_union([to_float, from_none], self.fs)
        result["sensitivity"] = from_union([to_float, from_none], self.sensitivity)
        result["usbDataPacketSize"] = from_union([from_int, from_none], self.usb_data_packet_size)
        result["sdWriteBufferSize"] = from_union([from_int, from_none], self.sd_write_buffer_size)
        result["wifiDataPacketSize"] = from_union([from_int, from_none], self.wifi_data_packet_size)
        result["comChannelNumber"] = from_union([from_int, from_none], self.com_channel_number)
        result["ucfLoaded"] = from_union([from_bool, from_none], self.ucf_loaded)
        return result


@dataclass
class SensorStatus:
    sub_sensor_status: Optional[List[SubSensorStatus]] = None

    @staticmethod
    def from_dict(obj: Any) -> 'SensorStatus':
        assert isinstance(obj, dict)
        sub_sensor_status = from_union([lambda x: from_list(SubSensorStatus.from_dict, x), from_none], obj.get("subSensorStatus"))
        return SensorStatus(sub_sensor_status)

    def to_dict(self) -> dict:
        result: dict = {}
        result["subSensorStatus"] = from_union([lambda x: from_list(lambda x: to_class(SubSensorStatus, x), x), from_none], self.sub_sensor_status)
        return result


@dataclass
class Sensor:
    id: Optional[int] = None
    name: Optional[str] = None
    sensor_descriptor: Optional[SensorDescriptor] = None
    sensor_status: Optional[SensorStatus] = None

    @staticmethod
    def from_dict(obj: Any) -> 'Sensor':
        assert isinstance(obj, dict)
        id = from_union([from_int, from_none], obj.get("id"))
        name = from_union([from_str, from_none], obj.get("name"))
        sensor_descriptor = from_union([SensorDescriptor.from_dict, from_none], obj.get("sensorDescriptor"))
        sensor_status = from_union([SensorStatus.from_dict, from_none], obj.get("sensorStatus"))
        return Sensor(id, name, sensor_descriptor, sensor_status)

    def to_dict(self) -> dict:
        result: dict = {}
        result["id"] = from_union([from_int, from_none], self.id)
        result["name"] = from_union([from_str, from_none], self.name)
        result["sensorDescriptor"] = from_union([lambda x: to_class(SensorDescriptor, x), from_none], self.sensor_descriptor)
        result["sensorStatus"] = from_union([lambda x: to_class(SensorStatus, x), from_none], self.sensor_status)
        return result


@dataclass
class HwTag:
    id: Optional[int] = None
    pin_desc: Optional[str] = None
    label: Optional[str] = None
    enabled: Optional[bool] = None

    @staticmethod
    def from_dict(obj: Any) -> 'HwTag':
        assert isinstance(obj, dict)
        id = from_union([from_int, from_none], obj.get("id"))
        pin_desc = from_union([from_str, from_none], obj.get("pinDesc"))
        label = from_union([from_str, from_none], obj.get("label"))
        enabled = from_union([from_bool, from_none], obj.get("enabled"))
        return HwTag(id, pin_desc, label, enabled)

    def to_dict(self) -> dict:
        result: dict = {}
        result["id"] = from_union([from_int, from_none], self.id)
        result["pinDesc"] = from_union([from_str, from_none], self.pin_desc)
        result["label"] = from_union([from_str, from_none], self.label)
        result["enabled"] = from_union([from_bool, from_none], self.enabled)
        return result


@dataclass
class SwTag:
    id: Optional[int] = None
    label: Optional[str] = None

    @staticmethod
    def from_dict(obj: Any) -> 'SwTag':
        assert isinstance(obj, dict)
        id = from_union([from_int, from_none], obj.get("id"))
        label = from_union([from_str, from_none], obj.get("label"))
        return SwTag(id, label)

    def to_dict(self) -> dict:
        result: dict = {}
        result["id"] = from_union([from_int, from_none], self.id)
        result["label"] = from_union([from_str, from_none], self.label)
        return result


@dataclass
class TagConfig:
    max_tags_per_acq: Optional[int] = None
    sw_tags: Optional[List[SwTag]] = None
    hw_tags: Optional[List[HwTag]] = None

    @staticmethod
    def from_dict(obj: Any) -> 'TagConfig':
        assert isinstance(obj, dict)
        max_tags_per_acq = from_union([from_int, from_none], obj.get("maxTagsPerAcq"))
        sw_tags = from_union([lambda x: from_list(SwTag.from_dict, x), from_none], obj.get("swTags"))
        hw_tags = from_union([lambda x: from_list(HwTag.from_dict, x), from_none], obj.get("hwTags"))
        return TagConfig(max_tags_per_acq, sw_tags, hw_tags)

    def to_dict(self) -> dict:
        result: dict = {}
        result["maxTagsPerAcq"] = from_union([from_int, from_none], self.max_tags_per_acq)
        result["swTags"] = from_union([lambda x: from_list(lambda x: to_class(SwTag, x), x), from_none], self.sw_tags)
        result["hwTags"] = from_union([lambda x: from_list(lambda x: to_class(HwTag, x), x), from_none], self.hw_tags)
        return result


@dataclass
class Device:
    device_info: Optional[DeviceInfo] = None
    sensor: Optional[List[Sensor]] = None
    tag_config: Optional[TagConfig] = None

    @staticmethod
    def from_dict(obj: Any) -> 'Device':
        assert isinstance(obj, dict)
        device_info = from_union([DeviceInfo.from_dict, from_none], obj.get("deviceInfo"))
        sensor = from_union([lambda x: from_list(Sensor.from_dict, x), from_none], obj.get("sensor"))
        tag_config = from_union([TagConfig.from_dict, from_none], obj.get("tagConfig"))
        return Device(device_info, sensor, tag_config)

    def to_dict(self) -> dict:
        result: dict = {}
        result["deviceInfo"] = from_union([lambda x: to_class(DeviceInfo, x), from_none], self.device_info)
        result["sensor"] = from_union([lambda x: from_list(lambda x: to_class(Sensor, x), x), from_none], self.sensor)
        result["tagConfig"] = from_union([lambda x: to_class(TagConfig, x), from_none], self.tag_config)
        return result


@dataclass
class DeviceConfig:
    uuid_acquisition: Optional[UUID] = None
    json_version: Optional[str] = None
    device: Optional[Device] = None

    @staticmethod
    def from_dict(obj: Any) -> 'DeviceConfig':
        assert isinstance(obj, dict)
        uuid_acquisition = from_union([lambda x: UUID(x), from_none], obj.get("UUIDAcquisition"))
        json_version = from_union([from_str, from_none], obj.get("JSONVersion"))
        device = from_union([Device.from_dict, from_none], obj.get("device"))
        return DeviceConfig(uuid_acquisition, json_version, device)

    def to_dict(self) -> dict:
        result: dict = {}
        result["UUIDAcquisition"] = from_union([lambda x: str(x), from_none], self.uuid_acquisition)
        result["JSONVersion"] = from_union([from_str, from_none], self.json_version)
        result["device"] = from_union([lambda x: to_class(Device, x), from_none], self.device)
        return result


def device_config_from_dict(s: Any) -> DeviceConfig:
    return DeviceConfig.from_dict(s)


def device_config_to_dict(x: DeviceConfig) -> Any:
    return to_class(DeviceConfig, x)

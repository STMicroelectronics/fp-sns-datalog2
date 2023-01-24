
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
from typing import Any, List, TypeVar, Callable, Type, cast

T = TypeVar("T")


def from_float(x: Any) -> float:
    assert isinstance(x, (float, int)) and not isinstance(x, bool)
    return float(x)


def from_str(x: Any) -> str:
    assert isinstance(x, str)
    return x


def from_bool(x: Any) -> bool:
    assert isinstance(x, bool)
    return x


def to_float(x: Any) -> float:
    assert isinstance(x, float)
    return x


def from_list(f: Callable[[Any], T], x: Any) -> List[T]:
    assert isinstance(x, list)
    return [f(y) for y in x]


def to_class(c: Type[T], x: Any) -> dict:
    assert isinstance(x, c)
    return cast(Any, x).to_dict()


@dataclass
class Tag:
    t: float
    label: str
    enable: bool

    @staticmethod
    def from_dict(obj: Any) -> 'Tag':
        assert isinstance(obj, dict)
        t = from_float(obj.get("t"))
        label = from_str(obj.get("Label"))
        enable = from_bool(obj.get("Enable"))
        return Tag(t, label, enable)

    def to_dict(self) -> dict:
        result: dict = {}
        result["t"] = to_float(self.t)
        result["Label"] = from_str(self.label)
        result["Enable"] = from_bool(self.enable)
        return result


@dataclass
class AcquisitionInfo:
    uuid_acquisition: str
    name: str
    description: str
    start_time: str
    tags: List[Tag]
    end_time: str

    @staticmethod
    def from_dict(obj: Any) -> 'Acquisition':
        assert isinstance(obj, dict)
        uuid_acquisition = from_str(obj.get("UUIDAcquisition"))
        name = from_str(obj.get("Name"))
        description = from_str(obj.get("Description"))
        if "start_time" in obj:
            start_time = from_str(obj.get("start_time"))
        else:
            start_time = '0'
        tags = from_list(Tag.from_dict, obj.get("Tags"))
        if "end_time" in obj:
            end_time = from_str(obj.get("end_time"))
        else:
            end_time = '0'
        return AcquisitionInfo(uuid_acquisition, name, description, start_time, tags, end_time)

    def to_dict(self) -> dict:
        result: dict = {}
        result["UUIDAcquisition"] = from_str(self.uuid_acquisition)
        result["Name"] = from_str(self.name)
        result["Description"] = from_str(self.description)
        result["start_time"] = from_str(self.start_time)
        result["Tags"] = from_list(lambda x: to_class(Tag, x), self.tags)
        result["end_time"] = from_str(self.end_time)
        return result


def welcome_from_dict(s: Any) -> AcquisitionInfo:
    return AcquisitionInfo.from_dict(s)


def welcome_to_dict(x: AcquisitionInfo) -> Any:
    return to_class(AcquisitionInfo, x)

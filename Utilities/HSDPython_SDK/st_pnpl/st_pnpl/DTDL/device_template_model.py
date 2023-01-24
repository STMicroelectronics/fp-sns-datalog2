# To use this code, make sure you
#
#     import json
#
# and then, to convert JSON from a string, do
#
#     result = interface_from_dict(json.loads(json_string))

from dataclasses import dataclass
from typing import Optional, Any, List, Union, TypeVar, Type, cast, Callable
from enum import Enum


T = TypeVar("T")
EnumT = TypeVar("EnumT", bound=Enum)


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


def to_class(c: Type[T], x: Any) -> dict:
    assert isinstance(x, c)
    return cast(Any, x).to_dict()


def to_enum(c: Type[EnumT], x: Any) -> EnumT:
    assert isinstance(x, c)
    return x.value


def from_list(f: Callable[[Any], T], x: Any) -> List[T]:
    assert isinstance(x, list)
    return [f(y) for y in x]


def from_bool(x: Any) -> bool:
    assert isinstance(x, bool)
    return x

def is_type(t: Type[T], x: Any) -> T:
    assert isinstance(x, t)
    return x


@dataclass
class DisplayName:
    en: Optional[str] = None

    @staticmethod
    def from_dict(obj: Any) -> 'DisplayName':
        assert isinstance(obj, dict)
        en = from_union([from_str, from_none], obj.get("en"))
        return DisplayName(en)

    def to_dict(self) -> dict:
        result: dict = {}
        result["en"] = from_union([from_str, from_none], self.en)
        return result


class SchemaEnum(Enum):
    BOOLEAN = "boolean"
    DOUBLE = "double"
    INTEGER = "integer"
    STRING = "string"


@dataclass
class EnumValue:
    display_name: Optional[DisplayName] = None
    enum_value: Optional[int] = None
    name: Optional[str] = None
    id: Optional[str] = None
    schema: Optional[SchemaEnum] = None

    @staticmethod
    def from_dict(obj: Any) -> 'EnumValue':
        assert isinstance(obj, dict)
        display_name = from_union([DisplayName.from_dict, from_none], obj.get("displayName"))
        # enum_value = from_union([from_int, from_none], obj.get("enumValue"))
        enum_value = from_union([from_none, from_int, lambda x: int(from_str(x))], obj.get("enumValue"))
        name = from_union([from_str, from_none], obj.get("name"))
        id = from_union([from_str, from_none], obj.get("@id"))
        schema = from_union([SchemaEnum, from_none], obj.get("schema"))
        return EnumValue(display_name, enum_value, name, id, schema)

    def to_dict(self) -> dict:
        result: dict = {}
        result["displayName"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_name)
        # result["enumValue"] = from_union([from_int, from_none], self.enum_value)
        result["enumValue"] = from_union([lambda x: from_none((lambda x: is_type(type(None), x))(x)), lambda x: from_int((lambda x: is_type(int, x))(x))], self.enum_value)
        result["name"] = from_union([from_str, from_none], self.name)
        result["@id"] = from_union([from_str, from_none], self.id)
        result["schema"] = from_union([lambda x: to_enum(SchemaEnum, x), from_none], self.schema)
        return result


@dataclass
class Response:
    display_name: Optional[DisplayName] = None
    name: Optional[str] = None
    schema: Optional[SchemaEnum] = None
    type: Optional[str] = None

    @staticmethod
    def from_dict(obj: Any) -> 'Response':
        assert isinstance(obj, dict)
        display_name = from_union([DisplayName.from_dict, from_none], obj.get("displayName"))
        name = from_union([from_str, from_none], obj.get("name"))
        schema = from_union([SchemaEnum, from_none], obj.get("schema"))
        type = from_union([from_str, from_none], obj.get("@type"))
        return Response(display_name, name, schema, type)

    def to_dict(self) -> dict:
        result: dict = {}
        result["displayName"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_name)
        result["name"] = from_union([from_str, from_none], self.name)
        result["schema"] = from_union([lambda x: to_enum(SchemaEnum, x), from_none], self.schema)
        result["@type"] = from_union([from_str, from_none], self.type)
        return result


class SchemaType(Enum):
    ENUM = "Enum"
    OBJECT = "Object"


@dataclass
class RequestSchema:
    type: Optional[SchemaType] = None
    display_name: Optional[DisplayName] = None
    fields: Optional[List[Response]] = None
    enum_values: Optional[List[EnumValue]] = None
    value_schema: Optional[SchemaEnum] = None

    @staticmethod
    def from_dict(obj: Any) -> 'RequestSchema':
        assert isinstance(obj, dict)
        type = from_union([SchemaType, from_none], obj.get("@type"))
        display_name = from_union([DisplayName.from_dict, from_none], obj.get("displayName"))
        fields = from_union([lambda x: from_list(Response.from_dict, x), from_none], obj.get("fields"))
        enum_values = from_union([lambda x: from_list(EnumValue.from_dict, x), from_none], obj.get("enumValues"))
        value_schema = from_union([SchemaEnum, from_none], obj.get("valueSchema"))
        return RequestSchema(type, display_name, fields, enum_values, value_schema)

    def to_dict(self) -> dict:
        result: dict = {}
        result["@type"] = from_union([lambda x: to_enum(SchemaType, x), from_none], self.type)
        result["displayName"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_name)
        result["fields"] = from_union([lambda x: from_list(lambda x: to_class(Response, x), x), from_none], self.fields)
        result["enumValues"] = from_union([lambda x: from_list(lambda x: to_class(EnumValue, x), x), from_none], self.enum_values)
        result["valueSchema"] = from_union([lambda x: to_enum(SchemaEnum, x), from_none], self.value_schema)
        return result


@dataclass
class Request:
    schema: Union[RequestSchema, SchemaEnum, None]
    type: Optional[str] = None
    display_name: Optional[DisplayName] = None
    name: Optional[str] = None
    description: Optional[DisplayName] = None

    @staticmethod
    def from_dict(obj: Any) -> 'Request':
        assert isinstance(obj, dict)
        schema = from_union([RequestSchema.from_dict, SchemaEnum, from_none], obj.get("schema"))
        type = from_union([from_str, from_none], obj.get("@type"))
        display_name = from_union([DisplayName.from_dict, from_none], obj.get("displayName"))
        name = from_union([from_str, from_none], obj.get("name"))
        description = from_union([DisplayName.from_dict, from_none], obj.get("description"))
        return Request(schema, type, display_name, name, description)

    def to_dict(self) -> dict:
        result: dict = {}
        result["schema"] = from_union([lambda x: to_class(RequestSchema, x), lambda x: to_enum(SchemaEnum, x), from_none], self.schema)
        result["@type"] = from_union([from_str, from_none], self.type)
        result["displayName"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_name)
        result["name"] = from_union([from_str, from_none], self.name)
        result["description"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.description)
        return result


@dataclass
class ContentSchema:
    id: Optional[str] = None
    type: Optional[SchemaType] = None
    display_name: Optional[DisplayName] = None
    enum_values: Optional[List[EnumValue]] = None
    value_schema: Optional[SchemaEnum] = None
    fields: Optional[List[EnumValue]] = None
    writable: Optional[bool] = None

    @staticmethod
    def from_dict(obj: Any) -> 'ContentSchema':
        assert isinstance(obj, dict)
        id = from_union([from_str, from_none], obj.get("@id"))
        type = from_union([SchemaType, from_none], obj.get("@type"))
        display_name = from_union([DisplayName.from_dict, from_none], obj.get("displayName"))
        enum_values = from_union([lambda x: from_list(EnumValue.from_dict, x), from_none], obj.get("enumValues"))
        value_schema = from_union([SchemaEnum, from_none], obj.get("valueSchema"))
        fields = from_union([lambda x: from_list(EnumValue.from_dict, x), from_none], obj.get("fields"))
        writable = from_union([from_bool, from_none], obj.get("writable"))
        return ContentSchema(id, type, display_name, enum_values, value_schema, fields, writable)

    def to_dict(self) -> dict:
        result: dict = {}
        result["@id"] = from_union([from_str, from_none], self.id)
        result["@type"] = from_union([lambda x: to_enum(SchemaType, x), from_none], self.type)
        result["displayName"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_name)
        result["enumValues"] = from_union([lambda x: from_list(lambda x: to_class(EnumValue, x), x), from_none], self.enum_values)
        result["valueSchema"] = from_union([lambda x: to_enum(SchemaEnum, x), from_none], self.value_schema)
        result["fields"] = from_union([lambda x: from_list(lambda x: to_class(EnumValue, x), x), from_none], self.fields)
        result["writable"] = from_union([from_bool, from_none], self.writable)
        return result

# class ContentType(Enum):
#     COMMAND = "Command"
#     COMPONENT = "Component"
#     PROPERTY = "Property"
#     TELEMETRY = "Telemetry"
    
class TypeElement(Enum):
    BOOLEAN_VALUE = "BooleanValue"
    NUMBER_VALUE = "NumberValue"
    STRING_VALUE = "StringValue"
    INITIALIZED = "Initialized"
    PROPERTY = "Property"
    TELEMETRY = "Telemetry"
    STATE = "State"
    #TODO add here Semantic Types? State is one of them
    

class ContentType(Enum):
    COMMAND = "Command"
    COMPONENT = "Component"
    PROPERTY = "Property"
    TELEMETRY = "Telemetry"


@dataclass
class Content:
    type: Union[List[TypeElement], ContentType, None]
    schema: Union[ContentSchema, None, str]
    id: Optional[str] = None
    # type: Optional[ContentType] = None
    display_name: Optional[DisplayName] = None
    name: Optional[str] = None
    unit: Optional[str] = None
    writable: Optional[bool] = None
    display_unit: Optional[DisplayName] = None
    command_type: Optional[str] = None
    request: Optional[Request] = None
    response: Optional[Response] = None
    description: Optional[DisplayName] = None
    comment: Optional[str] = None

    @staticmethod
    def from_dict(obj: Any) -> 'Content':
        assert isinstance(obj, dict)
        type = from_union([lambda x: from_list(TypeElement, x), ContentType, from_none], obj.get("@type"))
        schema = from_union([ContentSchema.from_dict, from_str, from_none], obj.get("schema"))
        id = from_union([from_str, from_none], obj.get("@id"))
        # type = from_union([ContentType, from_none], obj.get("@type"))
        display_name = from_union([DisplayName.from_dict, from_none], obj.get("displayName"))
        name = from_union([from_str, from_none], obj.get("name"))
        unit = from_union([from_str, from_none], obj.get("unit"))
        writable = from_union([from_bool, from_none], obj.get("writable"))
        display_unit = from_union([DisplayName.from_dict, from_none], obj.get("displayUnit"))
        command_type = from_union([from_str, from_none], obj.get("commandType"))
        request = from_union([Request.from_dict, from_none], obj.get("request"))
        response = from_union([Response.from_dict, from_none], obj.get("response"))
        description = from_union([DisplayName.from_dict, from_none], obj.get("description"))
        comment = from_union([from_str, from_none], obj.get("comment"))
        return Content(type, schema, id, display_name, name, unit, writable, display_unit, command_type, request, response, description, comment)

    def to_dict(self) -> dict:
        result: dict = {}
        result["@type"] = from_union([lambda x: from_list(lambda x: to_enum(TypeElement, x), x), lambda x: to_enum(ContentType, x), from_none], self.type)
        result["schema"] = from_union([lambda x: to_class(ContentSchema, x), from_str, from_none], self.schema)
        result["@id"] = from_union([from_str, from_none], self.id)
        # result["@type"] = from_union([lambda x: to_enum(ContentType, x), from_none], self.type)
        result["displayName"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_name)
        result["name"] = from_union([from_str, from_none], self.name)
        result["unit"] = from_union([from_str, from_none], self.unit)
        result["writable"] = from_union([from_bool, from_none], self.writable)
        result["displayUnit"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_unit)
        result["commandType"] = from_union([from_str, from_none], self.command_type)
        result["request"] = from_union([lambda x: to_class(Request, x), from_none], self.request)
        result["response"] = from_union([lambda x: to_class(Response, x), from_none], self.response)
        result["description"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.description)
        result["comment"] = from_union([from_str, from_none], self.comment)
        return result


class Context(Enum):
    DTMI_DTDL_CONTEXT_2 = "dtmi:dtdl:context;2"
    DTMI_IOTCENTRAL_CONTEXT_2 = "dtmi:iotcentral:context;2"


class InterfaceType(Enum):
    INTERFACE = "Interface"


@dataclass
class InterfaceElement:
    id: Optional[str] = None
    type: Optional[InterfaceType] = None
    contents: Optional[List[Content]] = None
    display_name: Optional[DisplayName] = None
    context: Optional[List[Context]] = None

    @staticmethod
    def from_dict(obj: Any) -> 'InterfaceElement':
        assert isinstance(obj, dict)
        id = from_union([from_str, from_none], obj.get("@id"))
        type = from_union([InterfaceType, from_none], obj.get("@type"))
        contents = from_union([lambda x: from_list(Content.from_dict, x), from_none], obj.get("contents"))
        display_name = from_union([DisplayName.from_dict, from_none], obj.get("displayName"))
        context = from_union([lambda x: from_list(Context, x), from_none], obj.get("@context"))
        return InterfaceElement(id, type, contents, display_name, context)

    def to_dict(self) -> dict:
        result: dict = {}
        result["@id"] = from_union([from_str, from_none], self.id)
        result["@type"] = from_union([lambda x: to_enum(InterfaceType, x), from_none], self.type)
        result["contents"] = from_union([lambda x: from_list(lambda x: to_class(Content, x), x), from_none], self.contents)
        result["displayName"] = from_union([lambda x: to_class(DisplayName, x), from_none], self.display_name)
        result["@context"] = from_union([lambda x: from_list(lambda x: to_enum(Context, x), x), from_none], self.context)
        return result


def interface_from_dict(s: Any) -> List[InterfaceElement]:
    return from_list(InterfaceElement.from_dict, s)


def interface_to_dict(x: List[InterfaceElement]) -> Any:
    return from_list(lambda x: to_class(InterfaceElement, x), x)
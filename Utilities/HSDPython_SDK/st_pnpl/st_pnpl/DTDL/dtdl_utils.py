
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

from enum import Enum

DTDL_SENSORS_ID_COMP_KEY = "sensors"
DTDL_ALGORITHMS_ID_COMP_KEY = "algorithms"
DTDL_ACTUATORS_ID_COMP_KEY = "actuators"
DTDL_OTHER_ID_COMP_KEY = "other"

class UnitMap():
    
    def __init__(self) -> None:
        self.unit_dict = {
            "gForce":"g",
            "gauss":"G",
            "decibel":"dB",
            "degreePerSecond":"dps",
            "mdps":"mdps",
            "degreeCelsius":"°C",
            "hertz":"Hz",
            "second":"s",
            "Waveform":"Waveform",
            "percent":"%",
            "millisecond":"ms",
            "microsecond":"μs"
        }

class ComponentTypeEnum(Enum):
    SENSOR = 0
    ALGORITHM = 1
    ACTUATOR = 3
    OTHER = 2

ALGORITHM_TYPE_STRING = "algorithm_type"
class AlgorithmTypeEnum(Enum):
    IALGORITHM_TYPE_FFT = 0
    IALGORITHM_TYPE_ANOMALY_DETECTOR = 1
    IALGORITHM_TYPE_CLASSIFIER = 2

ENABLED_STRING = "enabled"
MIN_STRING = "min"
MAX_STRING = "max"
UNIT_STRING = "unit"
GRAPH_TYPE_STRING = "graph_type"

class GraphTypeEnum(Enum):
    LINE = 0
    V_BAR = 1
    H_BAR = 2    
    V_LEVEL = 3
    H_LEVEL = 4
    FFT = 5
    GAUGE = 6
    CLASSIFIER = 7
    ANOMALY_DETECT = 8
    LINE_WAV = 9
    PLOT_ARRAY = 10
    LABEL = 11
    CHECKBOX = 12
    UNKWONW = -1

#DatalogMC Utilities 
MC_SLOW_TELEMETRY_STRING = "slow_mc_telemetry"
MC_FAST_TELEMETRY_STRING = "fast_mc_telemetry"
MC_SLOW_TELEMETRY_COMP_NAME = "slow_mc_telemetries"
MC_FAST_TELEMETRY_COMP_NAME = "fast_mc_telemetries"
AI_CLASSIFIER_COMP_NAME = "ai_motor_classifier"

ST_BLE_STREAM = "st_ble_stream"

class SensorCategoryEnum(Enum):
    ISENSOR_CLASS_MEMS = 0
    ISENSOR_CLASS_AUDIO = 1
    ISENSOR_CLASS_RANGING = 2
    ISENSOR_CLASS_LIGHT = 3
    ISENSOR_CLASS_CAMERA = 4
    ISENSOR_CLASS_PRESENCE = 5
    ISENSOR_CLASS_POWERMETER = 6
    

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

class TypeConversion:

    @staticmethod
    def check_type(check_type):
        switcher = {
            'uint8_t': 'uint8',
            'uint8': 'uint8',
            'uint16_t': 'uint16',
            'uint16': 'uint16',
            'uint32_t': 'uint32',
            'uint32': 'uint32',
            'int8_t': 'int8',
            'int8': 'int8',
            'int16_t': 'int16',
            'int16': 'int16',
            'int32_t': 'int32',
            'int32': 'int32',
            'float': 'float32',
            'double': 'double',
        }
        return switcher.get(check_type, "error")

    @staticmethod
    def check_type_length(check_type):
        switcher = {
            'uint8_t': 1,
            'uint8': 1,
            'int8_t': 1,
            'int8': 1,
            'uint16_t': 2,
            'uint16': 2,
            'int16_t': 2,
            'int16': 2,
            'uint32_t': 4,
            'uint32': 4,
            'int32_t': 4,
            'int32': 4,
            'float': 4,
            'float32': 4,
            'double': 8,
        }
        return switcher.get(check_type, "error")
    
    @staticmethod
    def get_format_char(check_type):
        switcher = {
            'uint8_t': 'B',
            'uint8': 'B',
            'int8_t': 'b',
            'int8': 'b',
            'uint16_t': 'H',
            'uint16': 'H',
            'int16_t': 'h',
            'int16': 'h',
            'uint32_t': 'I',
            'uint32': 'I',
            'int32_t': 'i',
            'int32': 'i',
            'float': 'f',
            'float32': 'f',
            'double': 'd',
        }
        return switcher.get(check_type, "error")
    
    @staticmethod
    def get_str_format(check_type):
        switcher = {
            'uint8_t': '{:.0f}',
            'uint8': '{:.0f}',
            'int8_t': '{:.0f}',
            'int8': '{:.0f}',
            'uint16_t': '{:.0f}',
            'uint16': '{:.0f}',
            'int16_t': '{:.0f}',
            'int16': '{:.0f}',
            'uint32_t': '{:.0f}',
            'uint32': '{:.0f}',
            'int32_t': '{:.0f}',
            'int32': '{:.0f}',
            'float': '{:.6f}',
            'float32': '{:.6f}',
            'double': '{:.6f}',
        }
        return switcher.get(check_type, "error")

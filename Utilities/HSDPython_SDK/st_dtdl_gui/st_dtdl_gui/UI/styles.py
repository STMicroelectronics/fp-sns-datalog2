
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

import enum

class STDTDL_MenuButton():

    class STDTDL_Page(enum.Enum):
        PAGE_CONNECTION = 1
        PAGE_DEVICE_CONFIG = 2
        PAGE_EXPERIMENTAL_FEATURES = 3
        PAGE_LOG_INFO = 4
        PAGE_ABOUT = 5

    def get_stylesheet(page:STDTDL_Page, status:bool):
        
        icon = ""
        if status:
            status_text = "border-right: 5px solid rgb(44, 49, 60);"
        else:
            status_text = ""
        if page == STDTDL_MenuButton.STDTDL_Page.PAGE_CONNECTION:
            icon = "outline_settings_input_com_white_18dp.png"
            if status:
                status_text = "border-right: 5px solid rgb(32, 133, 25);"
            else:
                status_text = "border-right: 5px solid rgb(134, 26, 34);"
        elif page == STDTDL_MenuButton.STDTDL_Page.PAGE_DEVICE_CONFIG:
            icon = "baseline_tune_white_18dp.png"
        elif page == STDTDL_MenuButton.STDTDL_Page.PAGE_EXPERIMENTAL_FEATURES:
            icon = "science_18dp_E8EAED.svg"
        elif page == STDTDL_MenuButton.STDTDL_Page.PAGE_LOG_INFO:
            icon = "outline_description_white_18dp.png"
        elif page == STDTDL_MenuButton.STDTDL_Page.PAGE_ABOUT:
            icon = "outline_info_white_18dp.png"
        else:
            return None

        return f"""
            QPushButton {{	
            background-image: url(:/icons/icons/{icon});
            background-position: center;
            background-repeat: no-repeat;
            border: none;
            {status_text}
            background-color: rgb(27, 29, 35);
            text-align: left;
        }}
        QPushButton:hover {{
            background-color: rgb(33, 37, 43);
        }}
        QPushButton:pressed {{
            background-color: rgb(32, 178, 170);
        }}
        """

class STDTDL_LineEdit():

    valid = '''
        QLineEdit {
            border: 2px solid rgb(27, 29, 35);
        }
        QLineEdit:hover {
            border: 2px solid rgb(64, 71, 88);
        }
        QLineEdit:focus {
            border: 2px solid rgb(91, 101, 124);
        }
        QLineEdit:disabled {
            border: 2px solid rgb(32, 32, 32);
        }
    '''

    invalid = '''
        QLineEdit {
            border: 2px solid rgb(128, 30, 30);
        }
        QLineEdit:hover {
            border: 2px solid rgb(148, 30, 30);
        }
        QLineEdit:focus {
            border: 2px solid rgb(168, 30, 30);
        }
        QLineEdit:disabled {
            border: 2px solid rgb(32, 32, 32);
        }
    '''

class STDTDL_Label():
    valid = '''
        QLabel {
            color: #99FF33;
        }
    '''

    invalid = '''
        QLabel {
            color: #FF5050;
        }
    '''
    
class STDTDL_SpinBox():
    valid = '''
        QSpinBox {
            border: 2px solid rgb(27, 29, 35);
        }
        QSpinBox:hover {
            border: 2px solid rgb(64, 71, 88);
        }
        QSpinBox:focus {
            border: 2px solid rgb(91, 101, 124);
        }
        QSpinBox:disabled {
            border: 2px solid rgb(32, 32, 32);
        }
    '''

    invalid = '''
        QSpinBox {
            border: 2px solid rgb(128, 30, 30);
        }
        QSpinBox:hover {
            border: 2px solid rgb(148, 30, 30);
        }
        QSpinBox:focus {
            border: 2px solid rgb(168, 30, 30);
        }
        QSpinBox:disabled {
            border: 2px solid rgb(32, 32, 32);
        }
    '''
class STDTDL_Chip():

    def color(color):
        return f"""
            QPushButton {{
                background-color: {color.darker(300).name()};
                color: "#1b1d23";
                border-radius: 12px;
                padding: 4px 8px;
                font-size: 12px;
                font: 700
            }}
            QPushButton:hover {{
                background-color: {color.lighter(120).name()};
            }}
            QPushButton:checked {{
                background-color: {color.name()};
            }}
            QPushButton:checked:hover {{
                background-color: {color.lighter(120).name()};
            }}
        """
class STDTDL_PushButton():
    
    valid = '''
        QPushButton {
            color: rgb(210,210,210);
            border: 2px solid rgb(52, 59, 72);
            border-radius: 5px;	
            background-color: rgb(52, 59, 72);
            padding: 3px;
        }
        QPushButton:hover {
            background-color: rgb(57, 65, 80);
            border: 2px solid rgb(61, 70, 86);
        }
        QPushButton:pressed {	
            background-color: rgb(35, 40, 49);
            border: 2px solid rgb(43, 50, 61);
        }
    
    '''

    invalid = '''
        QPushButton {
            border: 2px solid rgb(71, 51, 51);
            border-radius: 5px;	
            background-color: rgb(29,33,41);
            font: 700 9pt "Segoe UI";
        }
    '''

    selected = '''
        QPushButton {
            color: rgb(120,120,120);
            border: 2px solid rgb(42, 60, 42);
            border-radius: 7px;	
            background-color: rgb(32, 178, 170);
            padding: 3px;
        }
    '''
    
    green = '''
        QPushButton {
            border: 2px solid rgb(51, 71, 51);
            border-radius: 5px;	
            background-color: rgb(51, 71, 51);    
        }
        QPushButton:hover {
            background-color: rgb(57, 81, 57);
            border: 2px solid rgb(61, 87, 61);
        }
        QPushButton:pressed {	
            background-color: rgb(34, 48, 34);
            border: 2px solid rgb(42, 60, 42);
        }
        QPushButton:disabled {
            background-color: rgb(29,33,41);
            border: 2px solid rgb(29,33,41);
        }
    '''

    red = '''
        QPushButton {
            border: 2px solid rgb(71, 51, 51);
            border-radius: 5px;	
            background-color: rgb(71, 51, 51);
        }
        QPushButton:hover {
            background-color: rgb(81, 57, 57);
            border: 2px solid rgb(87, 61, 61);
        }
        QPushButton:pressed {	
            background-color: rgb(48, 34, 34);
            border: 2px solid rgb(60, 42, 42);
        }
        QPushButton:disabled {
            background-color: rgb(29,33,41);
            border: 2px solid rgb(29,33,41);
        }
    '''

class STDTDL_RadioButton():

    valid = '''
    QRadioButton::indicator {
        border: 3px solid rgb(52, 59, 72);
        width: 15px;
        height: 15px;
        border-radius: 10px;
        background: rgb(44, 49, 60);
    }
    QRadioButton::indicator:hover {
        border: 3px solid rgb(58, 66, 81);
    }
    QRadioButton::indicator:checked {
        background: 3px solid rgb(94, 106, 130);
        border: 3px solid rgb(52, 59, 72);	
    }
    QRadioButton::indicator:checked:hover {
        background: 3px solid rgb(104, 116, 140);
        border: 3px solid rgb(72, 79, 92);	
    }
    '''
    def get_style(color):
        style = "QRadioButton{ color:" + color.name() + "; font: 700;}"
        style += "QRadioButton::indicator { \
                border: 3px solid rgb(52, 59, 72); \
                width: 15px; \
                height: 15px; \
                border-radius: 10px; \
                background: rgb(44, 49, 60); \
            } \
            QRadioButton::indicator:hover { \
                border: 3px solid rgb(58, 66, 81); \
            } \
            QRadioButton::indicator:checked { \
                background: 3px solid rgb(94, 106, 130); \
                border: 3px solid rgb(52, 59, 72);\
            } \
            QRadioButton::indicator:checked:hover { \
            background: 3px solid rgb(104, 116, 140); \
            border: 3px solid rgb(72, 79, 92); \
            }"
        return style
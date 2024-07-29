
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

from PySide6.QtWidgets import QLabel, QLineEdit, QComboBox, QRadioButton, QPushButton, QHBoxLayout, QVBoxLayout, QWidget, QFrame, QGridLayout
from PySide6.QtGui import QValidator, QPixmap, QDoubleValidator, QIntValidator
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection
from st_dtdl_gui.UI.styles import STDTDL_RadioButton
from st_dtdl_gui.Utils.DataClass import TypeEnum

from st_pnpl.DTDL.dtdl_utils import UnitMap
from st_pnpl.DTDL.device_template_model import Content, ContentSchema, DisplayName

import st_dtdl_gui.UI.icons #NOTE don't delete this! it is used from resource_filename (@row 35..38)
from pkg_resources import resource_filename
info_img_path = resource_filename('st_dtdl_gui.UI.icons', 'outline_info_white_18dp.png')

class CharCounterValidator(QValidator):
    def __init__(self, min_length, max_length):
        super().__init__()
        if min_length is not None:
            self.min_length = min_length
        else:
            self.min_length = 0
        self.max_length = max_length

    def validate(self, input_str, pos):
        length = len(input_str)
        if self.max_length is not None:
            if self.min_length <= length <= self.max_length:
                return QValidator.State.Acceptable
            else:
                return QValidator.State.Invalid
        else:
            if length >= self.min_length:
                return QValidator.State.Acceptable
            else:
                return QValidator.State.Invalid

class SubPropertyWidget(QWidget):
    # comp_name, self.comp_sem_type, self.prop_name, label, value, self.is_writable, self
    def __init__(self, comp_name, comp_sem_type, prop_name, label, fields, is_writable, parent=None):
        super().__init__(parent)
        QPyDesignerCustomWidgetCollection.registerCustomWidget(SubPropertyWidget, module="SubPropertyWidget")
        loader = QUiLoader()
        self.comp_name = comp_name
        self.prop_name = prop_name
        self.label = label
        comp_config_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","component_config_widget.ui"), parent)
        frame_component_config = comp_config_widget.findChild(QFrame,"frame_component_config")
        frame_component_config.setStyleSheet("QFrame { border-radius: 5px; border: 2px solid rgb(27, 29, 35);}")
        title_frame = frame_component_config.findChild(QFrame,"frame_title")
        comp_frame_contents = frame_component_config.findChild(QFrame,"frame_contents")
        
        comp_frame_contents.layout().setContentsMargins(3,0,3,0)
        comp_config_widget.layout().setContentsMargins(0,0,0,0)
        
        title_label = title_frame.findChild(QPushButton,"label_title")
        title_label.setText(self.label.upper())
        self.annotation_label = title_frame.findChild(QLabel,"label_annotation")
        self.annotation_label.setVisible(False)
        pushButton_show = title_frame.findChild(QPushButton, "pushButton_show")
        pushButton_show.setVisible(False)
        pushButton_pop_out = title_frame.findChild(QPushButton, "pushButton_pop_out")
        pushButton_pop_out.setVisible(False)
        radioButton_enable = title_frame.findChild(QRadioButton, "radioButton_enable")
        radioButton_enable.setVisible(False)
        self.contents_widget = comp_config_widget.frame_component_config.findChild(QFrame,"frame_contents")

        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        main_layout.addWidget(comp_config_widget)

        component_props_frame = QFrame()
        component_props_layout = QGridLayout()
        component_props_layout.setVerticalSpacing(3)
        sub_widgets = []
        for i,f in enumerate(fields):
            if isinstance(f.schema, ContentSchema):
                ssp_widget = SubPropertyWidget(comp_name, comp_sem_type, [prop_name, f.name], f.name, f.schema.fields, True, self)
                ssp_widget.layout().setContentsMargins(0,6,0,0)
                sub_widgets.append(ssp_widget)
                component_props_layout.addWidget(ssp_widget, i, 0)
            else:
                schema_type = f.schema.value
                field_name = f.name
                field_dname = f.display_name if isinstance(f.display_name,str) else f.display_name.en
                sub_p_content = Content(name=prop_name, type="", schema=schema_type, display_name=field_dname, writable=is_writable)
                sub_widget = PropertyWidget(comp_name, comp_sem_type, sub_p_content, field_name, parent)
                sub_widgets.append(sub_widget)
                component_props_layout.addWidget(sub_widget, i, 0)
        self.widget = MultiPropertyWidget(sub_widgets)

        component_props_frame.setLayout(component_props_layout)
        component_props_frame.setFixedHeight(component_props_layout.sizeHint().height())
        self.contents_widget.layout().addWidget(component_props_frame)

class PropertyWidget(QWidget):
    # def __init__(self, comp_name, comp_sem_type, prop_name, label, value, prop_type, is_writable, prop_unit=None, field_name=None, parent=None):
    #                    comp_name, comp_sem_type, p.name,    dname, p.schema.fields, p.schema.type.value.lower(), p.writable, unit
    def __init__(self, comp_name, comp_sem_type, p_content:Content, field_name=None, parent=None):
        super().__init__(parent)
                
        self.comp_name = comp_name
        self.comp_sem_type = comp_sem_type
        
        self.icon = QLabel()
        self.icon.setPixmap(QPixmap(info_img_path))
        self.icon.setVisible(False)
        
        ## Property name extraction
        self.prop_name = p_content.name

        ## Property type extraction
        try: #complex object schema
            if p_content.schema.type.value == "Enum":
                schema_type = p_content.schema.type.value.lower()
                # self.fields = p_content.schema.enum_values
                value = p_content.schema.enum_values

            elif p_content.schema.type.value == "Object":
                schema_type = p_content.schema.type.value.lower()
                # self.fields = p_content.schema.fields
                value = p_content.schema.fields
        except AttributeError: #primitive type schema
            schema_type = p_content.schema
            # self.fields = None #p_content.schema
            value = ""
        
        self.field_name = field_name
        
        self.prop_type = schema_type
        ## Property unit extraction
        unit = p_content.unit
        if unit is not None:
            unit = UnitMap().unit_dict.get(unit, unit)
        else:
            display_unit = p_content.display_unit if isinstance(p_content.display_unit, str) else getattr(p_content.display_unit, 'en', None)
            if display_unit is not None:
                unit = UnitMap().unit_dict.get(display_unit, display_unit)
        self.prop_unit = unit

        self.is_writable = p_content.writable
        
        self.initial_value = p_content.initial_value
        
        self.has_bounds = False
        self.min_value, self.max_value = None, None
        if hasattr(p_content, 'min_value'):
            self.min_value = p_content.min_value if p_content.min_value is not None else self.min_value
        if hasattr(p_content, 'max_value'):
            self.max_value = p_content.max_value if p_content.max_value is not None else self.max_value
        if self.min_value is not None or self.max_value is not None:
            self.has_bounds = True

        self.decimal_places = None
        if hasattr(p_content, 'decimal_places'):
            self.decimal_places = p_content.decimal_places if p_content.decimal_places is not None else self.decimal_places

        self.false_name = None
        if hasattr(p_content, 'false_name'):
            self.false_name = p_content.false_name if p_content.false_name is not None else self.false_name
        self.true_name = None
        if hasattr(p_content, 'true_name'):
            self.true_name = p_content.true_name if p_content.true_name is not None else self.true_name

        self.max_length = None
        if hasattr(p_content, 'max_length'):
            self.max_length = p_content.max_length if p_content.max_length is not None else self.max_length
        self.min_length = None
        if hasattr(p_content, 'min_length'):
            self.min_length = p_content.min_length if p_content.min_length is not None else self.min_length
        self.trim_whitespace = None
        if hasattr(p_content, 'trim_whitespace'):
            self.trim_whitespace = p_content.trim_whitespace if p_content.trim_whitespace is not None else self.trim_whitespace

        self.description = None
        if hasattr(p_content, 'description'):
            self.description = p_content.description

        label = p_content.display_name if isinstance(p_content.display_name, str) else p_content.display_name.en
        self.label = QLabel(label)
        self.label.setFixedWidth(150)
        
        # String Property
        if self.prop_type == TypeEnum.STRING.value:
            self.value = QLineEdit(value)
            if self.max_length is not None:
                self.value.setMaxLength(self.max_length)
            self.validator = CharCounterValidator(self.min_length, self.max_length)
            self.value.setValidator(self.validator)
            if self.min_length is not None:
                self.icon.setVisible(True)
                if self.max_length is None:
                    self.icon.setToolTip(f"Min: {self.min_length} characters")
                else:
                    self.icon.setToolTip(f"Min: {self.min_length} characters, Max: {self.max_length} characters")
            elif self.max_length is not None:
                self.icon.setVisible(True)
                self.icon.setToolTip(f"Max: {self.max_length} characters")
            if self.initial_value is not None:
                self.value.setText(self.initial_value)
            if self.description is not None:
                description_text = self.description.en if isinstance(self.description, DisplayName) else self.description
                self.icon.setToolTip(f"{self.icon.toolTip()}\nDescription: {self.wrap_text(description_text,60)}")     
        
        # Double Property
        elif self.prop_type == TypeEnum.DOUBLE.value or self.prop_type == TypeEnum.FLOAT.value:
            self.validator = QDoubleValidator()
            if self.decimal_places is not None:
                self.validator.setDecimals(self.decimal_places)
                decimal_places_str = f", Decimal places: {self.decimal_places}"
            else:
                decimal_places_str = ""
            if self.max_value is not None:
                self.validator.setTop(self.max_value)
            if self.min_value is not None:
                self.validator.setBottom(self.min_value)
            self.value = QLineEdit(value)
            self.value.setValidator(self.validator)
            if self.min_value is not None:
                self.icon.setVisible(True)
                if self.max_value is None:
                    self.icon.setToolTip(f"Min: {self.min_value}{decimal_places_str}")
                else:
                    self.icon.setToolTip(f"Min: {self.min_value}, Max: {self.max_value}{decimal_places_str}")
            elif self.max_value is not None:
                self.icon.setVisible(True)
                self.icon.setToolTip(f"Max: {self.max_value}{decimal_places_str}")
            if self.initial_value is not None:
                self.value.setText(str(self.initial_value))
            if self.description is not None:
                description_text = self.description.en if isinstance(self.description, DisplayName) else self.description
                self.icon.setToolTip(f"{self.icon.toolTip()}\nDescription: {self.wrap_text(description_text,60)}")     
        
        # Integer Property
        elif self.prop_type == TypeEnum.INTEGER.value:
            self.validator = QIntValidator()
            if self.max_value is not None:
                self.validator.setTop(self.max_value)
            if self.min_value is not None:
                self.validator.setBottom(self.min_value)
            self.value = QLineEdit(value)
            self.value.setValidator(self.validator)
            if self.min_value is not None:
                self.icon.setVisible(True)
                if self.max_value is None:
                    self.icon.setToolTip(f"Min: {self.min_value}")
                else:
                    self.icon.setToolTip(f"Min: {self.min_value}, Max: {self.max_value}")
            elif self.max_value is not None:
                self.value.setToolTip(f"Max: {self.max_value}")
            if self.initial_value is not None:
                self.value.setText(str(self.initial_value))
            if self.description is not None:
                description_text = self.description.en if isinstance(self.description, DisplayName) else self.description
                self.icon.setToolTip(f"{self.icon.toolTip()}\nDescription: {self.wrap_text(description_text,60)}")                     
        
        # Boolean Property
        elif self.prop_type == TypeEnum.BOOLEAN.value:
            self.value = QRadioButton(value)
            self.value.setStyleSheet(STDTDL_RadioButton.valid)
            true_false_description = ""
            if self.true_name is not None:
                self.icon.setVisible(True)
                if self.false_name is None:
                    self.icon.setToolTip(f"False label: {self.true_name}")
                else:
                    self.icon.setToolTip(f"True label: {self.true_name}, False label: {self.false_name}")
            elif self.false_name is not None:
                self.icon.setToolTip(f"False label: {self.false_name}")
            self.value.setAccessibleDescription(true_false_description)
            if self.initial_value is not None:
                self.value.setChecked(self.initial_value)
            if self.description is not None:
                description_text = self.description.en if isinstance(self.description, DisplayName) else self.description
                self.icon.setToolTip(f"{self.icon.toolTip()}\nDescription: {self.wrap_text(description_text,60)}")                
        
        # Enum Property
        elif self.prop_type == TypeEnum.ENUM.value:
            self.value = QComboBox()
            for v in value:
                self.value.addItem(v.display_name if isinstance(v.display_name,str) else v.display_name.en)
            if self.initial_value is not None:
                self.value.setCurrentIndex(int(self.initial_value))
        
        # Object Property
        elif self.prop_type == TypeEnum.OBJECT.value:
            # NOTE code to support OLD HSDatalog versions (< v1.2.0) ###############
            keys = []
            for v in value:
                keys.append(v.name)
            if set(["max","min","val"]) == set(keys) and not self.has_bounds:
                if value[0].schema.value == TypeEnum.DOUBLE.value or value[0].schema.value == TypeEnum.FLOAT.value:
                    self.field_name = "val"
                    self.validator = QDoubleValidator(0,1000,self)
                    self.value = QLineEdit("0")
                    self.has_bounds = True
                    self.value.setValidator(self.validator)
                elif value[0].schema.value == TypeEnum.INTEGER.value:
                    self.field_name = "val"
                    self.validator = QIntValidator(0,1000,self)
                    self.value = QLineEdit("0")
                    self.has_bounds = True
                    self.value.setValidator(self.validator)
            # NOTE END code to support OLD HSDatalog versions (< v1.2.0) ###############
            else:
                self.value = SubPropertyWidget(comp_name, self.comp_sem_type, self.prop_name, label, value, self.is_writable, self)
                self.value.layout().setContentsMargins(0,6,0,0)
        
        # Unknown Property Type
        else:
            self.value = QLineEdit("UNKNOWN")
        
        layout = QHBoxLayout(self)
        
        if self.prop_unit is not None and self.prop_unit != "":
            unit_dict = UnitMap().unit_dict
            unit = self.prop_unit
            if unit in unit_dict:
                unit = unit_dict[unit]
            self.label.setText(self.label.text() + " [" + unit + "]")
        
        if self.prop_type != TypeEnum.OBJECT.value:
            self.value.setFixedHeight(30)

        # layout.setContentsMargins(3, 0, 3, 0)
        layout.setContentsMargins(0, 0, 0, 0)
        
        if(self.prop_type != TypeEnum.OBJECT.value or self.has_bounds):
            layout.addWidget(self.label)
            layout.addWidget(self.icon)
        
        if not self.is_writable: #if writable is None or False --> property is read-only
            self.value.setEnabled(False)
        
        layout.addWidget(self.value)

    def wrap_text(self, text, n):
        words = text.split()
        lines = []
        current_line = ""
        for word in words:
            if len(current_line + word) <= n:
                current_line += word + " "
            else:
                lines.append(current_line.strip())
                current_line = word + " "
        if current_line:
            lines.append(current_line.strip())
        return "\n".join(lines)

class MultiPropertyWidget():
    def __init__(self, widget_list) -> None:
        self.sub_widgets = widget_list    
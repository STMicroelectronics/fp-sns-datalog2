
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

from abc import abstractmethod
import os
from functools import partial

from PySide6.QtCore import Qt, Slot
from PySide6.QtGui import QValidator, QScreen
from PySide6.QtWidgets import QApplication, QFileDialog, QLabel, QSpinBox, QRadioButton, QPushButton, QVBoxLayout, QWidget, QFrame, QGridLayout
from PySide6.QtGui import QDoubleValidator, QIntValidator
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import st_dtdl_gui
from st_dtdl_gui.Widgets.ToggleButton import ToggleButton
from st_pnpl.DTDL.device_template_model import ContentType
from st_pnpl.PnPLCmd import PnPLCMDManager
from st_dtdl_gui.Widgets.PropertyWidget import PropertyWidget
from st_dtdl_gui.Widgets.CommandWidget import CommandField, CommandWidget
from st_dtdl_gui.Widgets.TelemetryWidget import TelemetryWidget
from st_dtdl_gui.UI.styles import STDTDL_EditLine, STDTDL_SpinBox
from st_dtdl_gui.Utils.DataClass import TypeEnum, UnitMap
from st_dtdl_gui.STDTDL_Controller import ComponentType

class ComponentWidget(QWidget):
    def __init__(self, controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id = 0, parent=None):
        super().__init__(parent)
        self.parent = parent
        self.controller = controller
        self.controller.sig_component_updated.connect(self.s_component_updated)
        self.controller.sig_logging.connect(self.s_is_logging)
        self.controller.sig_detecting.connect(self.s_is_detecting)
        
        self.is_docked = True
        self.is_packed = True

        self.c_id = c_id
        self.comp_name = comp_name
        self.comp_display_name = comp_display_name
        self.comp_sem_type = comp_sem_type
        self.comp_contents = comp_contents

        self.setWindowTitle("Connection")
        
        QPyDesignerCustomWidgetCollection.registerCustomWidget(ComponentWidget, module="ComponentWidget")
        loader = QUiLoader()
        comp_config_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","component_config_widget.ui"), parent)
        self.frame_component_config = comp_config_widget.frame_component_config
        self.title_frame = comp_config_widget.frame_component_config.findChild(QFrame,"frame_title")
        self.title_label = self.title_frame.findChild(QLabel,"label_title")
        self.title_label.setText(comp_name.upper())
        self.title_label.setText(self.comp_display_name)
        self.annotation_label = self.title_frame.findChild(QLabel,"label_annotation")
        self.annotation_label.setVisible(False)
        self.pushButton_show = self.title_frame.findChild(QPushButton, "pushButton_show")
        self.pushButton_show.clicked.connect(self.clicked_show_button)
        self.pushButton_pop_out = self.title_frame.findChild(QPushButton, "pushButton_pop_out")
        self.pushButton_pop_out.clicked.connect(self.clicked_pop_out_button)
        self.radioButton_enable = self.title_frame.findChild(QRadioButton, "radioButton_enable")
        self.radioButton_enable.setVisible(False)
        self.contents_widget = comp_config_widget.frame_component_config.findChild(QFrame,"frame_contents")

        self.contents_widget.setVisible(False)

        #Main layout
        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        main_layout.addWidget(comp_config_widget)

        self.property_widgets = dict()
        self.command_widgets = dict()

        #Frame Properties
        component_props_frame = QFrame()
        component_props_layout = QGridLayout()
        component_props_layout.setVerticalSpacing(3)
        for i, p in enumerate(self.comp_contents):
            pc_display_name = p.display_name if isinstance(p.display_name, str) else p.display_name.en
            
            # unit = ""
            # if p.unit is not None:
            #     unit = p.unit
            # elif p.display_unit is not None:
            #     unit = p.display_unit if isinstance(p.display_unit, str) else p.display_unit.en
                    
            # unit_dict = UnitMap().unit_dict
            # if unit in unit_dict:
            #     unit = unit_dict[unit]
            
            cont_type = ""
            if isinstance(p.type, ContentType):
                cont_type = p.type.name
            else:
                cont_type = [x for x in p.type if x.name in ["PROPERTY", "COMMAND", "TELEMETRY"]][0].name
            
            if cont_type == 'PROPERTY':
                
                widget = PropertyWidget(comp_name, comp_sem_type, p)
                
                try: #complex object schema
                    if p.schema.type.value in ("Enum", "Object"):
                        schema_type = p.schema.type.value.lower()
                except AttributeError: #primitive type schema
                    schema_type = p.schema
                self.assign_callbacks(widget, schema_type, p.schema.value_schema if schema_type == TypeEnum.ENUM.value else None)
                
                if (comp_sem_type == ComponentType.SENSOR or comp_sem_type == ComponentType.ALGORITHM or comp_sem_type == ComponentType.ACTUATOR) and p.name == "enable":
                    self.radioButton_enable.setVisible(True)
                    self.radioButton_enable.toggled.connect(partial(self.sensor_component_enabled, widget))
                
                component_props_layout.addWidget(widget, i,0)
                #add widget to the Property widget dictionary
                self.property_widgets[p.name] = widget
            
            elif cont_type == 'COMMAND':
                fields = []
                try: #complex object schema
                    if p.request is not None:
                        if p.request.schema.type.value == "Object":
                            if "fields" in p.request.schema.to_dict(): #more than one field in command
                                for f in p.request.schema.fields:
                                    field_label = f.display_name if isinstance(f.display_name, str) else f.display_name.en
                                    if f.schema is not None:
                                        fields.append(CommandField(f.name, f.schema, field_label, ""))
                                    else:
                                        try:
                                            schema_type = f.dtmi_dtdl_property_schema_2.type.value.lower()
                                            fields.append(CommandField(f.name, schema_type, field_label, f.dtmi_dtdl_property_schema_2.enum_values))
                                        except AttributeError:
                                            print("ERROR - malformed commmand field: {}".format(field_label))
                                widget = CommandWidget(self.controller, comp_name, self.comp_sem_type, p.name, p.request.name, fields, pc_display_name)
                            else:
                                fields.append(CommandField(p.request.name, p.request.schema, pc_display_name, ""))
                                widget = CommandWidget(self.controller, comp_name, self.comp_sem_type, p.name, None, fields, pc_display_name)
                        elif p.request.schema.type.value == "Enum":
                            if "enumValues" in p.request.schema.to_dict():
                                for e in p.request.schema.enum_values:
                                    enum_label = e.display_name if isinstance(e.display_name, str) else e.display_name.en
                                    fields.append(CommandField(p.request.name, TypeEnum.ENUM.value, enum_label, e.enum_value))
                                widget = CommandWidget(self.controller, comp_name, self.comp_sem_type, p.name, None, fields, pc_display_name)
                        # else:
                        #     print("no Object, nor Enum!")
                    else:
                        widget = CommandWidget(self.controller, comp_name, self.comp_sem_type, p.name, None, fields, pc_display_name)
                except AttributeError:
                    fields = []
                    if p.request != None:
                        fields.append(CommandField(p.request.name, p.request.schema, pc_display_name, ""))
                    widget = CommandWidget(self.controller, comp_name, self.comp_sem_type, p.name, None, fields, pc_display_name)

                component_props_layout.addWidget(widget, i,0)
                #add widget to the Property widget dictionary
                self.command_widgets[p.name] = widget
            elif cont_type == 'TELEMETRY':
                try: #complex object schema
                    if p.schema.type.value == "Enum":
                        schema_type = p.schema.type.value.lower()
                        widget = TelemetryWidget(self.controller, comp_name, comp_sem_type, p.name, pc_display_name, p.schema.enum_values, p.schema.type.value.lower(), p.writable)
                    elif p.schema.type.value == "Object":
                        schema_type = p.schema.type.value.lower()
                        widget = TelemetryWidget(self.controller, comp_name, comp_sem_type, p.name, pc_display_name, p.schema.fields, p.schema.type.value.lower(), p.writable)
                except AttributeError: #primitive type schema
                    schema_type = p.schema
                    widget = TelemetryWidget(self.controller, comp_name, comp_sem_type, p.name, pc_display_name , "", p.schema, p.writable)

                component_props_layout.addWidget(widget, i,0)
                #add widget to the Property widget dictionary
                self.property_widgets[p.name] = widget

        component_props_frame.setLayout(component_props_layout)      
        component_props_frame.setFixedHeight(component_props_layout.sizeHint().height())
        self.contents_widget.layout().addWidget(component_props_frame)

    def assign_callbacks(self, widget, schema_type, schema_value=None):
        if schema_type == TypeEnum.STRING.value:
            widget.value.textChanged.connect(partial(self.validate_value, widget))
            widget.value.editingFinished.connect(partial(self.send_string_command, widget))
        elif schema_type == TypeEnum.DOUBLE.value:
            widget.value.textChanged.connect(partial(self.validate_value, widget))
            widget.value.editingFinished.connect(partial(self.send_double_command, widget))
        elif  schema_type == TypeEnum.INTEGER.value:
            widget.value.textChanged.connect(partial(self.validate_value, widget))
            if isinstance(widget.value, QSpinBox):
                widget.value.valueChanged.connect(partial(self.send_int_command, widget))
            else:
                widget.value.editingFinished.connect(partial(self.send_int_command, widget))
        elif schema_type == TypeEnum.BOOLEAN.value:
            widget.value.toggled.connect(partial(self.boolean_prop_triggered, widget))
        elif schema_type == TypeEnum.ENUM.value:
            if schema_value.value == TypeEnum.INTEGER.value:
                widget.value.activated.connect(partial(self.send_enum_number_command, widget))
            if schema_value.value == TypeEnum.STRING.value:
                widget.value.activated.connect(partial(self.send_enum_string_command, widget))
        elif schema_type == TypeEnum.OBJECT.value:
            if widget.has_bounds:
                if type(widget.validator) == QDoubleValidator:
                    widget.value.textChanged.connect(partial(self.validate_value, widget))
                    widget.value.editingFinished.connect(partial(self.send_double_command, widget))
                elif type(widget.validator) == QIntValidator:
                    widget.value.textChanged.connect(partial(self.validate_value, widget))
                    widget.value.editingFinished.connect(partial(self.send_int_command, widget))
            else:
                for w in widget.value.widget.sub_widgets:
                    self.assign_callbacks(w, w.prop_type)

    @Slot()
    def clicked_browse_dt_button(self):
        json_filter = "JSON Device Template files (*.json *.JSON)"
        filepath = QFileDialog.getOpenFileName(filter=json_filter)
        self.input_file_path = filepath[0]
        self.dt_value.setText(self.input_file_path)
        self.controller.load_local_device_template(self.input_file_path)

    @Slot()
    def clicked_show_button(self):
        if self.is_packed:
            self.unpack_contents_widget()
            self.is_packed = False
        else:
            self.pack_contents_widget()
            self.is_packed = True
    
    @Slot()
    def clicked_pop_out_button(self):
        if self.is_docked:
            self.pop_out_widget()
            self.is_docked = False
        else:
            self.pop_in_widget()
            self.is_docked = True

    @Slot(int, str, dict)
    def s_component_updated(self, comp_name: str, comp_status: dict):
        if comp_name == self.comp_name:
            # print("Component:", comp_name)
            if comp_status is not None:
                for cont_name, cont_value in comp_status.items():
                    if type(cont_value) is dict:
                        # print(" - Content:", cont_name)
                        for key in cont_value:
                            # print('  -- ' + key + ':', cont_value[key])
                            self.update_property_widget(comp_name, self.comp_sem_type, cont_name, key, cont_value[key])
                    elif type(cont_value) is list:
                        print("ComponentWidget - WARNING - Property type not supported. (comp: {}, cont:{}) status not updated".format(comp_name, cont_name))
                    else:
                        # print('- {}: {}'.format(cont_name, cont_value))
                        self.update_property_widget(comp_name, self.comp_sem_type, cont_name, None, cont_value)
                # self.controller.components_status[comp_name] = comp_status
                print("ComponentWidget - INFO - Component {} Updated correctly".format(comp_name))                
            else:
                print("ComponentWidget - WARNING - No status to update for {} Component".format(comp_name))
                
    @Slot(bool, int)
    @abstractmethod
    def s_is_logging(self, status:bool, interface:int):
        '''to override in inherithed components which need to react to a logging state change event'''
        self.radioButton_enable.setEnabled(not status)
    
    @Slot(bool, int)
    @abstractmethod
    def s_is_detecting(self, status:bool):
        '''to override in inherithed components which need to react to a detecting state change event'''
        pass
    
    def update_property_widget(self, comp_name, comp_sem_type, prop_name, sub_prop_name, cont_value):
        if prop_name in self.property_widgets.keys():
            w = self.property_widgets[prop_name]
            
            if (comp_sem_type == ComponentType.SENSOR):
                    if prop_name == "sensor_annotation":
                        self.__set_component_annotation(cont_value)
                        
            if type(w) is PropertyWidget:
                if {w.comp_name, w.prop_name} == {comp_name, prop_name}:
                    if w.prop_type != TypeEnum.OBJECT.value:
                        self.__update_Property_widget_value(w,cont_value)
                        if w.prop_type == TypeEnum.BOOLEAN.value:
                            if prop_name == "enable" and (comp_sem_type == ComponentType.SENSOR or comp_sem_type == ComponentType.ALGORITHM or comp_sem_type == ComponentType.ACTUATOR):
                                self.radioButton_enable.blockSignals(True)
                                self.radioButton_enable.setChecked(cont_value)
                                self.radioButton_enable.blockSignals(False)
                    else:
                        if w.has_bounds:
                            if sub_prop_name == "min":
                                w.validator.setBottom(cont_value)
                            elif sub_prop_name == "max":
                                w.validator.setTop(cont_value)
                            elif sub_prop_name == "val":
                                self.__update_Property_widget_value(w,cont_value)
                        else:
                            for ww in w.value.widget.sub_widgets:
                                if ww.field_name == sub_prop_name:
                                    self.__update_Property_widget_value(ww,cont_value)

    def __update_Property_widget_value(self, widget, value):
        if widget.prop_type == TypeEnum.STRING.value:
            widget.value.setText(value)
        elif widget.prop_type == TypeEnum.INTEGER.value or widget.prop_type == TypeEnum.DOUBLE.value:
            if isinstance(widget.value, QSpinBox):
                widget.value.setValue(value)
            else:
                widget.value.setText(str(value))
        elif widget.prop_type == TypeEnum.ENUM.value:
            if value < widget.value.count():
                widget.value.setCurrentIndex(value)
        elif widget.prop_type == TypeEnum.BOOLEAN.value:
            try:
                widget.value.blockSignals(True)
                if isinstance(widget.value, ToggleButton):
                    widget.value.setCheckState(Qt.CheckState.Checked if value else Qt.CheckState.Unchecked)
                    widget.value.start_transition(value)
                else:
                    widget.value.setChecked(value)
                widget.value.blockSignals(False)
            except:
                pass
        elif widget.prop_type == TypeEnum.OBJECT.value:
            if widget.has_bounds:
                widget.value.blockSignals(True)
                widget.value.setText(str(value))
                widget.value.blockSignals(False)
        else:
            print("ComponentWidget - WARNING - Unrecognized Property Type")
    
    def send_string_command(self, widget: PropertyWidget):
        json_string = PnPLCMDManager.create_set_property_cmd(widget.comp_name, widget.prop_name, widget.value.text() if widget.field_name is None else { widget.field_name: widget.value.text()})
        self.controller.send_command(json_string)
        if widget.comp_sem_type == ComponentType.SENSOR:
            comp_sensor_name = widget.comp_name.split('_')[0]
            for cn in list(self.controller.components_dtdl.keys()):
                if comp_sensor_name in cn:
                    self.controller.update_component_status(cn, ComponentType.SENSOR)
        else:
            self.controller.update_component_status(widget.comp_name, widget.comp_sem_type)
    
    def send_int_command(self, widget: PropertyWidget, value=None):
        json_string = PnPLCMDManager.create_set_property_cmd(widget.comp_name, widget.prop_name, int(widget.value.text()) if widget.field_name is None else { widget.field_name: int(widget.value.text())})
        self.controller.send_command(json_string)
        if widget.comp_sem_type == ComponentType.SENSOR:
            comp_sensor_name = widget.comp_name.split('_')[0]
            for cn in list(self.controller.components_dtdl.keys()):
                if comp_sensor_name in cn:
                    self.controller.update_component_status(cn, ComponentType.SENSOR)
        else:
            self.controller.update_component_status(widget.comp_name, widget.comp_sem_type)

    def validate_value(self, widget, text_value):
        # print("res: {}".format(widget.validator.validate(text_value,0)[0]))
        
        validation_res = widget.validator.validate(text_value,0)
        if isinstance(validation_res, tuple):
            validation_res = validation_res[0]

        if validation_res == QValidator.State.Acceptable:
            if isinstance(widget.value, QSpinBox):
                # widget.value.setStyleSheet(STDTDL_SpinBox.valid)#TODO build a proper css class
                pass
            else:
                widget.value.setStyleSheet(STDTDL_EditLine.valid)
        else:
            if isinstance(widget, QSpinBox):
                # widget.value.setStyleSheet(STDTDL_SpinBox.invalid)#TODO build a proper css class
                pass
            else:
                widget.value.setStyleSheet(STDTDL_EditLine.invalid)
    
    def send_double_command(self, widget: PropertyWidget):
        json_string = PnPLCMDManager.create_set_property_cmd(widget.comp_name, widget.prop_name, float(widget.value.text()) if widget.field_name is None else { widget.field_name: float(widget.value.text())})
        self.controller.send_command(json_string)
        if widget.comp_sem_type == ComponentType.SENSOR:
            comp_sensor_name = widget.comp_name.split('_')[0]
            for cn in list(self.controller.components_dtdl.keys()):
                if comp_sensor_name in cn:
                    self.controller.update_component_status(cn, ComponentType.SENSOR)
        else:
            self.controller.update_component_status(widget.comp_name, widget.comp_sem_type)

    def sensor_component_enabled(self, widget: PropertyWidget, status):
        get_logging_status = getattr(self.controller, "get_logging_status", None)
        if get_logging_status is not None and callable(get_logging_status):
            is_logging = self.controller.get_logging_status()
        if not is_logging:
            widget.value.setChecked(status)

    def boolean_prop_triggered(self, widget: PropertyWidget, status):
        if widget.prop_name == "enable" and self.radioButton_enable.isVisible():
            self.radioButton_enable.blockSignals(True)
            self.radioButton_enable.setChecked(status)
            self.radioButton_enable.blockSignals(False)
        self.send_bool_command(widget, status)
        
    def send_bool_command(self, widget: PropertyWidget, status):
        json_string = PnPLCMDManager.create_set_property_cmd(widget.comp_name, widget.prop_name, status if widget.field_name is None else { widget.field_name: status})
        self.controller.send_command(json_string)
        if widget.comp_sem_type == ComponentType.SENSOR:
            comp_sensor_name = widget.comp_name.split('_')[0]
            for cn in list(self.controller.components_dtdl.keys()):
                if comp_sensor_name in cn:
                    self.controller.update_component_status(cn, ComponentType.SENSOR)
        else:
            self.controller.update_component_status(widget.comp_name, widget.comp_sem_type)

    def send_enum_number_command(self, widget: PropertyWidget, index):
        json_string = PnPLCMDManager.create_set_property_cmd(widget.comp_name, widget.prop_name, index if widget.field_name is None else { widget.field_name: index})
        self.controller.send_command(json_string)
        if widget.comp_sem_type == ComponentType.SENSOR:
            comp_sensor_name = widget.comp_name.split('_')[0]
            for cn in list(self.controller.components_dtdl.keys()):
                if comp_sensor_name in cn:
                    self.controller.update_component_status(cn, ComponentType.SENSOR)
        else:
            self.controller.update_component_status(widget.comp_name, widget.comp_sem_type)
    
    def send_enum_string_command(self, widget: PropertyWidget, index):
        json_string = PnPLCMDManager.create_set_property_cmd(widget.comp_name, widget.prop_name, index if widget.field_name is None else { widget.field_name: index})
        self.controller.send_command(json_string)
        if widget.comp_sem_type == ComponentType.SENSOR:
            comp_sensor_name = widget.comp_name.split('_')[0]
            for cn in list(self.controller.components_dtdl.keys()):
                if comp_sensor_name in cn:
                    self.controller.update_component_status(cn, ComponentType.SENSOR)
        else:
            self.controller.update_component_status(widget.comp_name, widget.comp_sem_type)
    
    def closeEvent(self, event):
        self.pop_in_widget()
        self.is_docked = True

    def pop_out_widget(self):
        self.setWindowFlags(Qt.Dialog | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint)
        center = QScreen.availableGeometry(QApplication.primaryScreen()).center()
        geo = self.frameGeometry()
        geo.moveCenter(center)
        self.move(geo.topLeft())
        self.show()
        self.unpack_contents_widget()
        self.is_packed = False

    def pop_in_widget(self):
        self.setWindowFlags(Qt.Widget)
        self.parent.layout().insertWidget(self.c_id, self)

    def unpack_contents_widget(self):
        self.contents_widget.setVisible(True)

    def pack_contents_widget(self):
        self.contents_widget.setVisible(False)
    
    def __set_component_annotation(self, note):
        self.annotation_label.setText(note)
        self.annotation_label.setVisible(True)
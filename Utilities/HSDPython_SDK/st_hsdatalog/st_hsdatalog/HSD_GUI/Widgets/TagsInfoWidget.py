
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
from functools import partial

from PySide6.QtCore import Qt, Slot
from PySide6.QtWidgets import QFrame, QGridLayout
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget

import st_hsdatalog
import st_hsdatalog.HSD_utils.logger as logger
from st_dtdl_gui.Widgets.ToggleButton import ToggleButton
log = logger.get_logger(__name__)

class TagsInfoWidget(ComponentWidget):
    def __init__(self, controller, comp_contents, comp_name="tags_info", comp_display_name = "Tags Information" , comp_sem_type="other", c_id=0, parent=None):
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)

        self.app = self.controller.qt_app
        self.is_logging = False
        self.parent_widget = parent
        self.comp_sem_type = comp_sem_type
        
        self.sw_tags_wgt_dict = dict()
        self.hw_tags_wgt_dict = dict()
        
        # clear all widgets in contents_widget layout (contents)
        for i in reversed(range(self.contents_widget.layout().count())):
            self.contents_widget.layout().itemAt(i).widget().deleteLater()

        self.setWindowTitle(comp_display_name)

        QPyDesignerCustomWidgetCollection.registerCustomWidget(TagsInfoWidget, module="TagsInfoWidget")
        loader = QUiLoader()
        tags_info_widget = loader.load(os.path.join(os.path.dirname(st_hsdatalog.__file__),"HSD_GUI","UI","tags_info_widget.ui"))
        frame_contents = tags_info_widget.frame_tags_info.findChild(QFrame,"frame_contents")
        frame_sw_tags_contents = tags_info_widget.frame_tags_info.findChild(QFrame,"frame_sw_tags_contents")
        frame_hw_tags_title = tags_info_widget.frame_tags_info.findChild(QFrame,"frame_hw_tags_title")
        frame_hw_tags_title.setVisible(False)
        frame_hw_tags_contents = tags_info_widget.frame_tags_info.findChild(QFrame,"frame_hw_tags_contents")
        frame_hw_tags_contents.setVisible(False)

        sw_tags_grid_layout = QGridLayout()
        sw_tags_grid_layout.setContentsMargins(0,0,0,0)
        sw_tags_grid_layout.setHorizontalSpacing(0)
        sw_tags_grid_layout.setVerticalSpacing(0)
        
        hw_tags_grid_layout = QGridLayout()
        hw_tags_grid_layout.setContentsMargins(0,0,0,0)
        hw_tags_grid_layout.setHorizontalSpacing(0)
        hw_tags_grid_layout.setVerticalSpacing(0)
        
        w_on_col = 2
        r_id = 0
        c_id = 0
        
        hw_tags = [c for c in comp_contents if "hw_tag" in c.name]
        sw_tags = [c for c in comp_contents if "sw_tag" in c.name]
        
        for st in sw_tags:
            sw_tag_widget = loader.load(os.path.join(os.path.dirname(st_hsdatalog.__file__),"HSD_GUI","UI","sw_tag.ui"))
            tag_toggle_button = ToggleButton()
            frame_tag_toggle_button = sw_tag_widget.findChild(QFrame,"frame_tag_toggle_button")
            frame_tag_toggle_button.layout().addWidget(tag_toggle_button)
            sw_tag_widget.tag_label.editingFinished.connect(partial(self.tag_label_changed, st.name))
            tag_toggle_button.stateChanged.connect(partial(self.tag_button_toggled, st.name))
            self.sw_tags_wgt_dict[st.name] = {"tag_label":sw_tag_widget.tag_label,"tag_button":tag_toggle_button,"tag_status":False}
            sw_tags_grid_layout.addWidget(sw_tag_widget,r_id,c_id)
            c_id += 1
            if c_id == w_on_col:
                c_id = 0
                r_id += 1
                
        r_id += 1
        c_id = 0
                
        for ht in hw_tags:
            hw_tag_widget = loader.load(os.path.join(os.path.dirname(st_hsdatalog.__file__),"HSD_GUI","UI","hw_tag.ui"))
            hw_tag_widget.tag_label.editingFinished.connect(partial(self.tag_label_changed, ht.name))
            self.hw_tags_wgt_dict[ht.name] = {"tag_label":hw_tag_widget.tag_label}
            hw_tags_grid_layout.addWidget(hw_tag_widget,r_id,c_id)
            c_id += 1
            if c_id == w_on_col:
                c_id = 0
                r_id += 1
        
        frame_sw_tags_contents.layout().addLayout(sw_tags_grid_layout)
        self.contents_widget.layout().addWidget(frame_contents)
    
    def tag_button_toggled(self, tag_name, a=None):
        self.toggle_tag_status(tag_name)
        tag_curr_status = self.sw_tags_wgt_dict[tag_name]["tag_status"]
        if self.is_logging:
            self.send_tag_command(tag_name, tag_curr_status)

    def toggle_tag_status(self, tag_name):
        self.sw_tags_wgt_dict[tag_name]["tag_status"] = not self.sw_tags_wgt_dict[tag_name]["tag_status"]
        
    def send_tag_command(self, tag_name, tag_status):
        self.controller.doTag(tag_name, tag_status)
        
    def tag_label_changed(self, tag_name):
        if "sw_tag" in tag_name:
            tag_label = self.sw_tags_wgt_dict[tag_name]["tag_label"].text()
            self.controller.changeSWTagClassLabel(tag_name, tag_label)
        elif "hw_tag" in tag_name:
            tag_label = self.hw_tags_wgt_dict[tag_name]["tag_label"].text()
            self.controller.changeHWTagClassLabel(tag_name, tag_label)
    
    @Slot(bool)
    def s_is_logging(self, status:bool, interface:int):     
        self.is_logging = status
        if status:
            for st in self.sw_tags_wgt_dict:
                if self.sw_tags_wgt_dict[st]["tag_status"]:
                    self.send_tag_command(st, True)
                self.sw_tags_wgt_dict[st]["tag_label"].setEnabled(False)
                self.sw_tags_wgt_dict[st]["tag_button"].update()
            for ht in self.hw_tags_wgt_dict:
                self.hw_tags_wgt_dict[ht]["tag_label"].setEnabled(False)
        else:
            for st in self.sw_tags_wgt_dict:
                if self.sw_tags_wgt_dict[st]["tag_status"]:
                    self.send_tag_command(st, False)
                    self.sw_tags_wgt_dict[st]["tag_button"].setCheckState(Qt.CheckState.Unchecked)
                self.sw_tags_wgt_dict[st]["tag_label"].setEnabled(True)
                self.sw_tags_wgt_dict[st]["tag_button"].update()
            for ht in self.hw_tags_wgt_dict:
                self.hw_tags_wgt_dict[ht]["tag_label"].setEnabled(True)
            
    @Slot(int, str, dict)
    def s_component_updated(self, comp_name: str, comp_status: dict):
        if comp_name == "tags_info":
            # log.debug("Component: {}".format(comp_name))
            if comp_status is not None:
                # for cont_name, cont_value in comp_status.items():
                #     if type(cont_value) is dict:
                #         log.debug(" - Content: {}".format(cont_name))
                #         for key in cont_value:
                #             log.debug('  -- {} : {}'.format(key, cont_value[key]))
                for cs in comp_status:
                    if "sw_tag" in cs:
                        tag_label = comp_status[cs]["label"]
                        self.sw_tags_wgt_dict[cs]["tag_label"].setText(tag_label)
                    elif "hw_tag" in cs:
                        tag_label = comp_status[cs]["label"]
                        self.hw_tags_wgt_dict[cs]["tag_label"].setText(tag_label)
                self.controller.components_status[comp_name] = comp_status
                log.debug("Component: {}".format(comp_name))
                log.info("Component tags_info Updated correctly")
    
    @Slot()
    def plot_window_time_change(self):
        self.controller.plot_window_changed(self.time_spinbox.value())
    

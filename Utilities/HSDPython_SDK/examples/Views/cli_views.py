import os
import click
from datetime import datetime, timedelta
from asciimatics.exceptions import ResizeScreenError, NextScene, StopApplication
from asciimatics.widgets import Frame, ListBox, Layout, Divider, VerticalDivider, Text, Label, \
    MultiColumnListBox, Button, TextBox, DropdownList, Widget

from st_hsdatalog.HSD_link import HSDLink_v2

class HSDMainView(Frame):
    def __init__(self, screen, hsd_info):
        super(HSDMainView, self).__init__(screen,
                                        screen.height * 7 // 8,
                                        screen.width * 7 // 8,
                                        hover_focus=False,
                                        can_scroll=False,
                                        title="STWIN HSDatalog",
                                        reduce_cpu=True)
        
        self.set_theme("bright")
        self._hsd_info_model = hsd_info

        # Layout Header -------------------------------------------------------------------------------------------------
        # Create the form for displaying the hsdatalog info.
        layout_header = Layout([100])
        self.add_layout(layout_header)
        layout_header.add_widget(Divider())
        layout_header.add_widget(Label("STWIN HSDatalog Command Line Interface", align=u'^'))
        layout_header.add_widget(Label("Based on: {}".format(self._hsd_info_model.version),align=u'^'))
        layout_header.add_widget(Divider())    

        # Layout CLI Flags ----------------------------------------------------------------------------------------------
        layout_cli_flags = Layout([100])
        self.add_layout(layout_cli_flags)
        layout_cli_flags.add_widget(Label("- Output folder: {}".format(self._hsd_info_model.cli_flags.output_folder)))

        if self._hsd_info_model.cli_flags.acq_name is not None and self._hsd_info_model.cli_flags.acq_name != "":
            layout_cli_flags.add_widget(Label("- Acquisition name: {}".format(self._hsd_info_model.cli_flags.acq_name)))
        else:
            layout_cli_flags.add_widget(Label("- Acquisition description: STWIN_Acq"))

        if self._hsd_info_model.cli_flags.acq_desc is not None and self._hsd_info_model.cli_flags.acq_desc != "":
            layout_cli_flags.add_widget(Label("- Acquisition description: {}".format(self._hsd_info_model.cli_flags.acq_desc)))
        else:
            layout_cli_flags.add_widget(Label("- Acquisition description: "))

        if self._hsd_info_model.cli_flags.file_config != '' and os.path.exists(self._hsd_info_model.cli_flags.file_config):
            layout_cli_flags.add_widget(Label("- Device configuration file: {}".format(self._hsd_info_model.cli_flags.file_config)))
        else:
            layout_cli_flags.add_widget(Label("- Device configuration file: Default DeviceConfig from the device"))

        if self._hsd_info_model.cli_flags.ucf_file != '' and os.path.exists(self._hsd_info_model.cli_flags.ucf_file):
            self.mlc_msg_lbl = Label("- Selected UCF file [MLC]: {}".format(self._hsd_info_model.cli_flags.ucf_file))
            layout_cli_flags.add_widget(self.mlc_msg_lbl)

        else:
            self.mlc_msg_lbl = Label("- Selected UCF file [MLC]: No UCF file selected")
            self.mlc_msg_lbl.custom_colour = "control"
            layout_cli_flags.add_widget(self.mlc_msg_lbl)

        if self._hsd_info_model.cli_flags.time_sec != -1:
            layout_cli_flags.add_widget(Label("- Duration: {}".format(timedelta(seconds = self._hsd_info_model.cli_flags.time_sec))))
        else:
            layout_cli_flags.add_widget(Label("- Duration: Endless log mode"))
        layout_cli_flags.add_widget(Divider())

        # Layout Device List --------------------------------------------------------------------------------------------
        layout_device_list = Layout([100], fill_frame=True)
        self.add_layout(layout_device_list)

        layout_device_list.add_widget(Label("Connected device list (double click, or Enter to select a device)"))

        no_device_msg = ""
        dev_list = self._hsd_info_model.device_list
        self._hsd_info_model.selected_device_id = 0
        if self._hsd_info_model.device_list is None:
            # No devices connected.
            no_device_msg = "No device connected!"
            dev_list = []
            self._hsd_info_model.selected_device_id = None
        
        # Create the form for displaying the list of devices.
        self._list_view = ListBox(
            Widget.FILL_FRAME,
            [("{}) [{}] - {}".format(i,dev.fw_info.alias, dev.fw_info.part_number), i) for i,dev in enumerate(dev_list)],
            name="device_list",
            add_scroll_bar=True,
            on_change=self._on_pick,
            on_select=self._on_select)

        self._no_device_label = Label(no_device_msg)
        self._no_device_label.custom_colour = "invalid"
        layout_device_list.add_widget(self._no_device_label)
        layout_device_list.add_widget(self._list_view)
        layout_device_list.add_widget(Divider())

        # Layout Device Info --------------------------------------------------------------------------------------------
        self._hsd_info_model.update_fw_info()
        layout_fw_info = Layout([100])
        self.add_layout(layout_fw_info)
        if self._hsd_info_model.selected_fw_info is None:
            dev_id = dev_sn = dev_al = dev_pn = dev_url = dev_fwn = dev_fwv = dev_dfe = dev_dff = dev_ns = ""
        else:
            dev_id = "- id: {}".format(self._hsd_info_model.selected_device_id)
            dev_sn = "- sn: {}".format(self._hsd_info_model.selected_fw_info.serial_number)
            dev_al = "- alias: {}".format(self._hsd_info_model.selected_fw_info.alias)
            dev_pn = "- pn: {}".format(self._hsd_info_model.selected_fw_info.part_number)
            dev_url = "- url: {}".format(self._hsd_info_model.selected_fw_info.url)
            dev_fwn = "- fw_name: {}".format(self._hsd_info_model.selected_fw_info.fw_name)
            dev_fwv = "- fw_version: {}".format(self._hsd_info_model.selected_fw_info.fw_version)
            dev_dfe = "- data_file_ext: {}".format(self._hsd_info_model.selected_fw_info.data_file_ext)
            dev_dff = "- data_file_format: {}".format(self._hsd_info_model.selected_fw_info.data_file_format)
            dev_ns = "- n_sensor: {}".format(self._hsd_info_model.selected_fw_info.n_sensor)

        self.dev_id_lbl = Label(dev_id)
        self.dev_sn_lbl = Label(dev_sn)
        self.dev_al_lbl = Label(dev_al)
        self.dev_pn_lbl = Label(dev_pn)
        self.dev_url_lbl = Label(dev_url)
        self.dev_fwn_lbl = Label(dev_fwn)
        self.dev_fwv_lbl = Label(dev_fwv)
        self.dev_dfe_lbl = Label(dev_dfe)
        self.dev_dff_lbl = Label(dev_dff)
        self.dev_ns_lbl = Label(dev_ns)
        layout_fw_info.add_widget(self.dev_id_lbl)
        layout_fw_info.add_widget(self.dev_sn_lbl)
        layout_fw_info.add_widget(self.dev_al_lbl)
        layout_fw_info.add_widget(self.dev_pn_lbl)
        layout_fw_info.add_widget(self.dev_url_lbl)
        layout_fw_info.add_widget(self.dev_fwn_lbl)
        layout_fw_info.add_widget(self.dev_fwv_lbl)
        layout_fw_info.add_widget(self.dev_dfe_lbl)
        layout_fw_info.add_widget(self.dev_dff_lbl)
        layout_fw_info.add_widget(self.dev_ns_lbl)
        layout_fw_info.add_widget(Divider())

        # Layout Footer -------------------------------------------------------------------------------------------------
        layout2 = Layout([1, 1, 1])
        self.add_layout(layout2)
        self._refresh_button = Button("Refresh", self._reload_device_list)
        self._quit_button = Button("Quit", self._quit)
        layout2.add_widget(self._refresh_button, 0)
        layout2.add_widget(self._quit_button, 2)

        self.fix()

    def _on_pick(self):
        self.save()
        if self._list_view.options is not None :
            if self._hsd_info_model.selected_device_id != self._list_view.value:
                pass
                self._hsd_info_model.selected_device_id = self._list_view.value

                self._hsd_info_model.update_fw_info()
                self.dev_id_lbl.text = "- id: {}".format(self._hsd_info_model.selected_device_id)
                self.dev_sn_lbl.text = "- sn: {}".format(self._hsd_info_model.selected_fw_info.serial_number)
                self.dev_al_lbl.text = "- alias: {}".format(self._hsd_info_model.selected_fw_info.alias)
                self.dev_pn_lbl.text = "- pn: {}".format(self._hsd_info_model.selected_fw_info.part_number)
                self.dev_url_lbl.text = "- url: {}".format(self._hsd_info_model.selected_fw_info.url)
                self.dev_fwn_lbl.text = "- fw_name: {}".format(self._hsd_info_model.selected_fw_info.fw_name)
                self.dev_fwv_lbl.text = "- fw_version: {}".format(self._hsd_info_model.selected_fw_info.fw_version)
                self.dev_dfe_lbl.text = "- data_file_ext: {}".format(self._hsd_info_model.selected_fw_info.data_file_ext)
                self.dev_dff_lbl.text = "- data_file_format: {}".format(self._hsd_info_model.selected_fw_info.data_file_format)
                self.dev_ns_lbl.text = "- n_sensor: {}".format(self._hsd_info_model.selected_fw_info.n_sensor)

    def _on_select(self):
        self.save()
        self._hsd_info_model.selected_device_id = self._list_view.value
        raise NextScene("Logger")

    def _reload_device_list(self):
        self.save()
        self._hsd_info_model.update_device_list()
        if self._hsd_info_model.device_list is not None:
            self._no_device_label.text = ""
            self._list_view.options = [("{}) [{}] - {}".format(i,dev.fw_info.alias, dev.fw_info.part_number), i) for i,dev in enumerate(self._hsd_info_model.device_list)]
        else:
            self._no_device_label.text = "No device connected!"
            self._no_device_label.custom_colour = "invalid"

            self.dev_id_lbl = Label("")
            self.dev_sn_lbl = Label("")
            self.dev_al_lbl = Label("")
            self.dev_pn_lbl = Label("")
            self.dev_url_lbl = Label("")
            self.dev_fwn_lbl = Label("")
            self.dev_fwv_lbl = Label("")
            self.dev_dfe_lbl = Label("")
            self.dev_dff_lbl = Label("")
            self.dev_ns_lbl = Label("")

            self._list_view.options = []
        raise NextScene("Main")

    def _update(self, frame_no):
        pass
    
    @staticmethod
    def _quit():
        raise StopApplication("User pressed quit")

class HSDLoggingView(Frame):
        
    def __init__(self, screen, hsd_info):
        super(HSDLoggingView, self).__init__(screen,
                                            screen.height * 7 // 8,
                                            screen.width * 7 // 8,
                                            hover_focus=False,
                                            can_scroll=False,
                                            title="STWIN HSDatalog",
                                            reduce_cpu=True)
        self.set_theme("bright")
        self._hsd_info_model = hsd_info
        self._last_frame = 0

        # Layout Header -------------------------------------------------------------------------------------------------
        # Create the form for displaying the hsdatalog info.
        layout_header = Layout([100])
        self.add_layout(layout_header)
        layout_header.add_widget(Divider())
        layout_header.add_widget(Label("STWIN HSDatalog Command Line Interface", align=u'^'))
        layout_header.add_widget(Label("Based on: {}".format(self._hsd_info_model.version),align=u'^'))
        layout_header.add_widget(Divider())

        # Layout CLI Flags ----------------------------------------------------------------------------------------------
        layout_cli_flags = Layout([100])
        self.add_layout(layout_cli_flags)
        layout_cli_flags.add_widget(Label("- Output folder: {}".format(self._hsd_info_model.cli_flags.output_folder)))
        if self._hsd_info_model.cli_flags.acq_name is not None and self._hsd_info_model.cli_flags.acq_name != "":
            layout_cli_flags.add_widget(Label("- Acquisition name: {}".format(self._hsd_info_model.cli_flags.acq_name)))
        else:
            layout_cli_flags.add_widget(Label("- Acquisition description: STWIN_Acq"))

        if self._hsd_info_model.cli_flags.acq_desc is not None and self._hsd_info_model.cli_flags.acq_desc != "":
            layout_cli_flags.add_widget(Label("- Acquisition description: {}".format(self._hsd_info_model.cli_flags.acq_desc)))
        else:
            layout_cli_flags.add_widget(Label("- Acquisition description: "))

        if self._hsd_info_model.cli_flags.file_config != '' and os.path.exists(self._hsd_info_model.cli_flags.file_config):
            layout_cli_flags.add_widget(Label("- Device configuration file: {}".format(self._hsd_info_model.cli_flags.file_config)))
        else:
            layout_cli_flags.add_widget(Label("- Device configuration file: Default DeviceConfig from the device"))

        if self._hsd_info_model.cli_flags.ucf_file != '' and os.path.exists(self._hsd_info_model.cli_flags.ucf_file):
            self.mlc_msg_lbl = Label("- Selected UCF file [MLC]: {}".format(self._hsd_info_model.cli_flags.ucf_file))
            layout_cli_flags.add_widget(self.mlc_msg_lbl)

        else:
            self.mlc_msg_lbl = Label("- Selected UCF file [MLC]: No UCF file selected")
            self.mlc_msg_lbl.custom_colour = "control"
            layout_cli_flags.add_widget(self.mlc_msg_lbl)

        if self._hsd_info_model.cli_flags.time_sec != -1:
            layout_cli_flags.add_widget(Label("- Duration: {}".format(timedelta(seconds = self._hsd_info_model.cli_flags.time_sec))))
        else:
            layout_cli_flags.add_widget(Label("- Duration: Endless log mode"))
        layout_cli_flags.add_widget(Divider())
        layout_cli_flags.add_widget(Divider())

        # Log progress --------------------------------------------------------------------------------------------------
        layout_progress = Layout([100])
        self.add_layout(layout_progress)
        if self._hsd_info_model.cli_flags.time_sec == -1:
            self.remaining_time_lbl = Label(" Endless Log mode selected. Press the stop button to end the session")
        else:
            self.remaining_time_lbl = Label(" Remaining time: {}".format(self._hsd_info_model.cli_flags.time_sec))
        self.remaining_time_lbl.custom_colour = "control"
        layout_progress.add_widget(self.remaining_time_lbl)
        layout_progress.add_widget(Divider())

        # Layout Sensor/Tag List --------------------------------------------------------------------------------------------
        layout_sensor_list = Layout([1,10,1,10], fill_frame=True)       
        self.add_layout(layout_sensor_list)
        
        layout_sensor_list.add_widget(Label("Active Sensor list"),1)        
        self._sensor_list_view = MultiColumnListBox(
            Widget.FILL_FRAME,
            ["<30%", "<30%", "<40%"],
            [],
            titles=["Sensor name", "Sensor Type", "Data Count"],
            name="sensor_list"
            )

        layout_sensor_list.add_widget(Label("Tag classes list"),3)
        self._tag_list_view = MultiColumnListBox(
                        Widget.FILL_FRAME,
                        ["<35%", "<65%"],
                        [],
                        titles=["Tag name", "Tag Status"],
                        name="tag_list",
                        on_select=self._on_tag_select,
                        )
        layout_sensor_list.add_widget(VerticalDivider(),0)
        layout_sensor_list.add_widget(self._sensor_list_view,1)
        layout_sensor_list.add_widget(VerticalDivider(),2)
        layout_sensor_list.add_widget(self._tag_list_view,3)
        for i in range(0,4):
            layout_sensor_list.add_widget(Divider(),i)
        
        # Layout Footer -------------------------------------------------------------------------------------------------
        layout_footer = Layout([1, 1, 1, 1])
        self.add_layout(layout_footer)
        
        if self._hsd_info_model.cli_flags.interactive_mode:
            self._back_button = Button("Back", self._back)
            layout_footer.add_widget(self._back_button, 0)
        
        self._start_button = Button("Start", self._restart)
        layout_footer.add_widget(self._start_button, 1)
        self._start_button.disabled = True
        
        self._stop_button = Button("Stop", self._stop)
        layout_footer.add_widget(self._stop_button, 2)
        
        self._quit_button = Button("Quit", self._stop_and_quit)
        layout_footer.add_widget(self._quit_button, 3)

        self.fix()

    def _back(self):
        if self._hsd_info_model.is_log_started:
            self._hsd_info_model.stop_log()
        self._start_button.disabled = True
        self._stop_button.disabled = False
        self._last_frame = 0
        self._hsd_info_model.is_log_manually_stopped = False
        if self._hsd_info_model.cli_flags.time_sec == -1:
                self.remaining_time_lbl.text = ("")
                self.remaining_time_lbl.custom_colour = "control"
        raise NextScene("Main")

    def _restart(self):
        if not self._start_button.disabled:
            self._start_button.disabled = True
            self._stop_button.disabled = False
            self._hsd_info_model.is_log_manually_stopped = False
            self._hsd_info_model.is_log_started = False # it will unlock the "log init" case in update function. After the unlocked case, the function starts the log and unlock this flag itselfS

    def _stop(self):
        if not self._stop_button.disabled:
            self._hsd_info_model.stop_log()
            self._start_button.disabled = False
            self._stop_button.disabled = True
            self._hsd_info_model.is_log_manually_stopped = True
            if self._hsd_info_model.cli_flags.time_sec == -1:
                self.remaining_time_lbl.text = ("---> Log completed successfully!")
                self.remaining_time_lbl.custom_colour = "label"

    def _stop_and_quit(self):
        if self._hsd_info_model.is_log_started:
            self._hsd_info_model.stop_log()
        self._hsd_info_model.is_log_manually_stopped = False
        del self._hsd_info_model.hsd_link
        self._quit()

    def _on_tag_select(self):
        self._hsd_info_model.do_tag(self._tag_list_view.value)
        last_tag_selection = self._tag_list_view.value
        last_tag_start = self._tag_list_view.start_line
        tag_data = []
        for t in self._hsd_info_model.tag_list:
            if type(t) == str:
                item = [[self._hsd_info_model.tag_list[t]['label'], "[x]" if self._hsd_info_model.tag_status_list[int(t[-1])] else "[ ]"], int(t[-1])]
            else:
                item = [[t.label, "[x]" if self._hsd_info_model.tag_status_list[t.id] else "[ ]"], t.id]
            tag_data.append(item)
        self._tag_list_view.options = tag_data
        self._tag_list_view.value = last_tag_selection
        self._tag_list_view.start_line = last_tag_start

    def _update(self, frame_no):
        if frame_no - self._last_frame >= self.frame_update_count or self._last_frame == 0:
            self._last_frame = frame_no
            last_selection = self._sensor_list_view.value
            last_start = self._sensor_list_view.start_line

            last_tag_selection = self._tag_list_view.value
            last_tag_start = self._tag_list_view.start_line

            if self._hsd_info_model.is_log_manually_stopped is not True:
                if not self._hsd_info_model.is_log_started:

                    self._hsd_info_model.check_output_folder()
                    self._hsd_info_model.update_acq_params()

                    self._hsd_info_model.upload_device_conf_file()

                    self._hsd_info_model.update_sensor_list()
                    self._hsd_info_model.init_sensor_data_counters()
                    
                    #MLC UCF file upload
                    self._hsd_info_model.update_mlc_sensor_list()
                    self._hsd_info_model.upload_mlc_ucf_file()

                    self._hsd_info_model.update_tag_list()
                    self._hsd_info_model.init_tag_status_list()
                    
                    self._hsd_info_model.start_log()
                    self._hsd_info_model.start_time = datetime.now()

                if self._hsd_info_model.is_log_started:
                    
                    #MLC sensor GUI update
                    if self._hsd_info_model.cli_flags.ucf_file != '' and os.path.exists(self._hsd_info_model.cli_flags.ucf_file):
                        self.mlc_msg_lbl.text = "- Selected UCF file [MLC]: {}".format(self._hsd_info_model.cli_flags.ucf_file)
                        self.mlc_msg_lbl.custom_colour = "label"
                        if self._hsd_info_model.mlc_sensor_list is not None and len(self._hsd_info_model.mlc_sensor_list) == 0:
                            self.mlc_msg_lbl = Label("- Selected UCF file [MLC]: Ok, but No active MLC sensors!")
                            self.mlc_msg_lbl.custom_colour = "invalid"
                    else:
                        self.mlc_msg_lbl.text = "- Selected UCF file [MLC]: No UCF file selected"
                        self.mlc_msg_lbl.custom_colour = "control"

                    #Log duration management
                    if self._hsd_info_model.cli_flags.time_sec != -1:
                        current_time = datetime.now()
                        delta_time = current_time-self._hsd_info_model.start_time
                        remaining_time = self._hsd_info_model.cli_flags.time_sec - delta_time.seconds
                        self.remaining_time_lbl.text = (" Remaining time: {}".format(timedelta(seconds = remaining_time)))
                        self.remaining_time_lbl.custom_colour = "control"
                        if(remaining_time <= 0):
                            self.remaining_time_lbl.text = ("---> Log completed successfully!")
                            self.remaining_time_lbl.custom_colour = "label"
                            self._stop()
                    else:
                        self.remaining_time_lbl.text = (" Remaining time: Endless log mode selected")
                        self.remaining_time_lbl.custom_colour = "control"
                    
                    s_sensor_data = []
                    item_counter = 0
                    for idx, s in enumerate(self._hsd_info_model.sensor_list):
                        if self._hsd_info_model.is_hsd_link_v2():
                            item = [[s.split("_")[0].upper(), s.split("_")[-1].upper(), str(self._hsd_info_model.hsd_link.sensor_data_counts[s])] , item_counter]
                            s_sensor_data.append(item)
                            item_counter += 1
                        else:
                        # if self._hsd_info_model.version != "High Speed Datalog DLL v2":
                            #HSDLink_v1    
                            for sd in s.sensor_descriptor.sub_sensor_descriptor:
                                item = [[s.name, sd.sensor_type, str(self._hsd_info_model.hsd_link.sensor_data_counts[(s.id, sd.id)])] , item_counter]
                                s_sensor_data.append(item)
                                item_counter += 1
                        # else: 
                        #     item = [[s.split("_")[0].upper(), s.split("_")[-1].upper(), str(self._hsd_info_model.sensor_data_counts_v2[s])] , item_counter]
                        #     s_sensor_data.append(item)
                        #     item_counter += 1

                    self._sensor_list_view.options = s_sensor_data
                    self._sensor_list_view.value = last_selection
                    self._sensor_list_view.start_line = last_start

                    tag_data = []
                    for idx, t in enumerate(self._hsd_info_model.tag_list):
                        if self._hsd_info_model.is_hsd_link_v2():
                            tag_label = self._hsd_info_model.tag_list[t]["label"]
                            item = [[tag_label, "[x]" if self._hsd_info_model.tag_status_list[idx] else "[ ]"], idx]
                        # if self._hsd_info_model.version != "High Speed Datalog DLL v2":
                            #HSDLink_v1
                        else:
                            tag_label = t.label 
                            item = [[tag_label, "[x]" if self._hsd_info_model.tag_status_list[idx] else "[ ]"], idx]
                        # else:
                        #     tag_label = self._hsd_info_model.tag_list[t]["label"]
                        #     item = [[tag_label, "[x]" if self._hsd_info_model.tag_status_list[idx] else "[ ]"], idx]
                        tag_data.append(item)
                    self._tag_list_view.options = tag_data
                    self._tag_list_view.value = last_tag_selection
                    self._tag_list_view.start_line = last_tag_start

        # Now redraw as normal
        super(HSDLoggingView, self)._update(frame_no)
    
    @property
    def frame_update_count(self):
        # Refresh once every 1 second by default.
        return 20

    def _on_load_page(self):
        self.save()

    @staticmethod
    def  _quit():
        raise StopApplication("User pressed quit")
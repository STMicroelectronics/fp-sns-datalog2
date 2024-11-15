# *****************************************************************************
#  * @file    Controller.py
#  * @author  SRA
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
from collections import deque
import struct

from threading import Condition, Thread
import numpy as np
from PySide6.QtCore import Signal
from st_hsdatalog.HSD_datatoolkit.HSD_DataToolkit_Pipeline import HSD_DataToolkit_data
from st_hsdatalog.HSD.utils.type_conversion import TypeConversion
from st_dtdl_gui.Utils.DataClass import DataClass

class HSD_DataToolkit(Thread):
    def __init__(self, components_status, data_pipeline, data_ready_evt:Signal):
        Thread.__init__(self)
        self.components_status = components_status
        self.data_pipeline = data_pipeline
        self.data_queue = deque(maxlen=200000)
        self.queue_evt = Condition()
        self.data_ready_evt = data_ready_evt
        self.data_ready_evt.connect(self.add_data_to_queue)

        self.missing_bytes = {}
        self.incoming_data = {}
        self.stop_thread = False

    def extract_data(self, data):
        if data.comp_name in self.incoming_data:
            self.incoming_data[data.comp_name] += data.data
        else:
            self.incoming_data[data.comp_name] = data.data
        
        comp_status = self.components_status[data.comp_name]
        dim = comp_status.get("dim",1)
        data_type = comp_status.get("data_type")
        data_sample_bytes_len = dim * TypeConversion.check_type_length(data_type)
        spts = comp_status.get("samples_per_ts")
        if spts is None:
            spts = 1
        sensitivity = comp_status.get("sensitivity")
        if sensitivity is None:
            sensitivity = 1
        timestamp_bytes_len = 8 if spts != 0 else 0
        data_packet_len = (spts * data_sample_bytes_len) if spts != 0 else data_sample_bytes_len

        nof_cmplt_packets = len(self.incoming_data[data.comp_name]) // (data_packet_len + timestamp_bytes_len)
        
        for i in range(nof_cmplt_packets):
            if spts != 0:
                d = self.incoming_data[data.comp_name][:data_packet_len]
                timestamp = struct.unpack('<d', self.incoming_data[data.comp_name][data_packet_len:data_packet_len + timestamp_bytes_len])[0]
                self.incoming_data[data.comp_name] = self.incoming_data[data.comp_name][data_packet_len + timestamp_bytes_len:]
                data_buffer = np.frombuffer(d, dtype=TypeConversion.get_np_dtype(data_type)) * sensitivity
                self.data_pipeline.process_data(HSD_DataToolkit_data(data.comp_name, data_buffer, timestamp))
            else:
                #TODO: Implement the case when spts = 0 (no timestamps)
                # missing_bytes_num = len(data.data)%(data_packet_len)
                
                # d = data.data[:-missing_bytes_num]
                # data_buffer = np.frombuffer(d, dtype=TypeConversion.get_np_dtype(data_type))
                
                # missing_d = data.data[len(d):]
                # self.missing_bytes[data.comp_name] = missing_d
                pass

    def run(self):
        while not self.stop_thread:
            with self.queue_evt:
                self.queue_evt.wait()
            data = self._get_data_from_queue()
            if data:
                self.extract_data(data)

    def add_data_to_queue(self, data:DataClass):
        with self.queue_evt:
            self.data_queue.append(data)
            self.queue_evt.notify()

    def _get_data_from_queue(self):
        if len(self.data_queue) > 0:
            return self.data_queue.popleft()

    def start(self):
        super().start()

    def stop(self):
        # stop the consumer thread
        self.stop_thread = True
        with self.queue_evt:
            self.queue_evt.notify()

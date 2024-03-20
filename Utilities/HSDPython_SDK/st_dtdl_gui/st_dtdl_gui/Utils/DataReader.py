
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

import math
import struct
import numpy as np

from .DataClass import DataClass


class DataReader(object):
    def __init__(self, controller, comp_name, samples_per_ts, dimensions, sample_size, data_format, sensitivity=1, interleaved_data=True, flat_raw_data=False):
        self.controller = controller
        self.comp_name = comp_name
        self.samples_per_ts = samples_per_ts
        self.dimensions = dimensions
        self.sample_size = sample_size
        self.sensitivity = sensitivity
        self.interleaved_data = interleaved_data
        self.flat_raw_data = flat_raw_data
        if samples_per_ts != 0:
            self.data_size = self.sample_size * self.samples_per_ts * self.dimensions
            self.time_size = 8
            self.time_format = "d"
        else:
            self.data_size = self.sample_size * self.dimensions
            self.time_size = 0
            self.time_format = ""
        self.data_format = data_format
        self.next_is_timestamp = False

        self.data_buffer = []
        self.data_dict = dict()

        self.data_samples_counter = 0
        self.rem_dim_bytes = ""
        self.missing_time_bytes = 0

    def get_largest_mult_of_n_lower_than_m(self, n, m):
        return int((m // n) * n)  # Largest multiple of n lower than m

    def calculate_data_to_extract(self, data):
        # if data lenght is multiple of dimensions
        int_rem_data_bytes = len(data)
        data_samples = int_rem_data_bytes / self.sample_size
        # else, it is needed to select the largest multiple of dimensions lower than the length of data_samples
        if data_samples % self.dimensions != 0:
            data_samples = self.get_largest_mult_of_n_lower_than_m(self.dimensions, data_samples)
            int_rem_data_bytes = self.sample_size * data_samples
        return [int_rem_data_bytes, data_samples]
        
    def update_data_samples_counter_and_time_flag(self):
        if self.data_samples_counter == self.samples_per_ts * self.dimensions:
            self.data_samples_counter = 0
            self.next_is_timestamp = True
        else:
            self.next_is_timestamp = False

    def fill_data_buffer(self, data):
         # extract the number of complete packet (data+timestamp)      
        if len(data) <= self.data_size: # data length is less than the full packet data size (sample_size * samples_size_per_ts * dim)                           
            # calculate the right amount of data to extract (here data must be already expanded with remaining bytes)
            [int_rem_data_bytes, data_samples] = self.calculate_data_to_extract(data)
            # update the data_samples_counter
            self.data_samples_counter = int(data_samples)
            # copy data into buffer
            self.data_buffer = np.frombuffer(data[:int_rem_data_bytes], dtype=self.data_format, count=int(data_samples))
            # take the remaining bytes, if any (e.g., if data finish with a non complete sample (x,y,) z missing)
            self.rem_dim_bytes = data[int_rem_data_bytes:]
            # update data_samples_counter (remain the same) and timestamp flag (to False)
            self.update_data_samples_counter_and_time_flag()
            
        elif len(data) > self.data_size and len(data) < (self.data_size + self.time_size):
            #calculate the next timestam residual bytes
            self.missing_time_bytes = (self.data_size + self.time_size) - len(data)
            #trim and data to avoid to interpretate the piece of timestamp as a data
            data = data[:self.data_size]
            #recursively call this function with trimmed data
            self.fill_data_buffer(data)

        else:           
            n_cplt_packet = math.floor(len(data) / (self.data_size + self.time_size)) # n_cplt_packet must be always >=1, [n_cplt_packet == 0 will be catched in the previous case (elif)]
            # create the format string to unpack the entire buffer (if samples_per_ts is not 0 add also timestamp data position)
            if self.samples_per_ts != 0:
                format = "=" + str(n_cplt_packet * (str(str(self.samples_per_ts * self.dimensions) + self.data_format) + (str(self.time_format))))
            else:
                format = "=" + str(n_cplt_packet * (str(str(self.dimensions) + self.data_format)))
            # unpack data from the byte raw data buffer
            data_t = data[: n_cplt_packet * (self.data_size + self.time_size)]
            self.data_buffer = list(struct.unpack(format, data_t))
            # remove timestamps extracted
            if self.samples_per_ts != 0:
                del self.data_buffer[(self.samples_per_ts * self.dimensions) :: (self.samples_per_ts * self.dimensions) + 1]
            #else:
                #del self.data_buffer[self.dimensions :: self.dimensions + 1]
            # take the remaining bytes
            self.rem_data_bytes = data[n_cplt_packet * (self.data_size + self.time_size):]
            
            if len(self.rem_data_bytes) > self.data_size: #in this case rem_data_bytes contains a piece of the next timestmap, so:
                #calculate the next timestam residual bytes
                self.missing_time_bytes = (self.data_size + self.time_size) - len(self.rem_data_bytes)
                #trim rem_data_bytes and data in order to avoid to interpretate the piece of timestamp as a data
                self.rem_data_bytes = self.rem_data_bytes[:self.data_size]
                data = data[: n_cplt_packet * (self.data_size + self.time_size) + self.data_size]
            
            # calculate the right amount of data to extract
            [int_rem_data_bytes, data_samples] = self.calculate_data_to_extract(self.rem_data_bytes)
            # update the data_samples_counter
            self.data_samples_counter = int(data_samples)
            # add remaining extracted data into the data_buffer
            self.data_buffer.extend(np.frombuffer(data[n_cplt_packet * (self.data_size + self.time_size) : n_cplt_packet * (self.data_size + self.time_size) + int_rem_data_bytes], dtype=self.data_format, count=self.data_samples_counter))
            # cast self.data_buffer to be a numpy array
            self.data_buffer = np.array(self.data_buffer)
            # take the remaining bytes, if any (e.g., if data finish with a non complete sample (x,y,) z missing)
            self.rem_dim_bytes = data[ n_cplt_packet * (self.data_size + self.time_size) + int_rem_data_bytes :]
            # update data_samples_counter and timestamp flag
            self.update_data_samples_counter_and_time_flag()

    def feed_data(self, data):
        if self.comp_name == self.comp_name:
            if len(self.rem_dim_bytes) != 0:
                # append residual data bytes
                data.data = self.rem_dim_bytes + data.data
                # print(array.array('h', data.data))
            if self.missing_time_bytes != 0:
                # drop residual timestamp bytes
                data.data = data.data[self.missing_time_bytes:]
                self.missing_time_bytes = 0
                self.next_is_timestamp = False
            if self.data_samples_counter == 0:
                if not self.next_is_timestamp:
                    # received data starts with data
                    self.fill_data_buffer(data.data)
                else:
                    # received data starts with timestamp
                    data.data = data.data[len(self.rem_dim_bytes) + self.time_size :]  # removing the timestamp
                    if len(self.rem_dim_bytes) != 0:
                        # self.data_samples_counter += len(self.rem_dim_bytes)/self.sample_size
                        self.fill_data_buffer(self.rem_dim_bytes + data.data)# place residual bytes in head if any
                    else:
                        self.fill_data_buffer(data.data)
            else:
                #calculate the difference in order to determine missing data to obtain a complete packet (samples_per_ts * dimensions)
                diff = (self.samples_per_ts * self.dimensions) - self.data_samples_counter  # data_samples_counter counts data. In case of multiple dim data one dimension is one data
                if (len(data.data) < diff * self.sample_size):  # data is raw bytes, to get bytes from diff (samples) it is necessary to multiply it for sample size
                    # calculate the right amount of data to extract
                    [int_rem_data_bytes, data_samples] = self.calculate_data_to_extract(data.data)
                    # copy data into buffer
                    self.data_buffer = np.frombuffer(data.data[:int_rem_data_bytes], dtype=self.data_format, count=int(int_rem_data_bytes / self.sample_size))
                    # update the data_samples_counter
                    self.data_samples_counter += int(data_samples)
                    # take the remaining bytes, if any (e.g., if data finish with a non complete sample (x,y,) z missing)
                    self.rem_dim_bytes = data.data[int_rem_data_bytes:]
                else:
                    # copy data into buffer
                    self.data_buffer = np.frombuffer(data.data[: diff * self.sample_size], dtype=self.data_format, count=diff)
                    # reinit data_samples_counter
                    self.data_samples_counter = 0
                    if len(self.data_buffer) == (len(data.data) / self.sample_size):
                        #next received packet will start with a timestamp
                        self.next_is_timestamp = True
                        self.rem_dim_bytes = ""
                    else:
                        # removing the timestamp (excluding the first) 
                        if (len(self.data_buffer) * self.sample_size) + self.time_size > len(data.data):
                            #removes a piece of the timestamp (there will be the remaining bytes in the next received packet)
                            self.missing_time_bytes = ((len(self.data_buffer) * self.sample_size) + self.time_size) - len(data.data)
                            # print("Sensor Name: {}".format(self.comp_name))
                            # print("timestamp splitted -> missing_time_bytes: {}".format(self.missing_time_bytes))
                            data.data = data.data[(len(self.data_buffer) * self.sample_size) + (self.time_size - self.missing_time_bytes) :]
                        else:
                            data.data = data.data[(len(self.data_buffer) * self.sample_size) + self.time_size :]

                        # fill data buffer
                        self.fill_data_buffer(data.data)
           
            if len(self.data_buffer) > 0:
                if not self.interleaved_data:
                    #if self.samples_per_ts == 0:
                    for i in range( int(len(self.data_buffer) / self.dimensions)):
                        self.data_dict[i] = np.array(self.data_buffer[i*self.dimensions:(i*self.dimensions)+self.dimensions],dtype='f')*self.sensitivity
                else:
                    if self.flat_raw_data:
                        self.data_dict[0] = self.data_buffer * self.sensitivity
                    else:
                        for i in range(self.dimensions):
                            self.data_dict[i] = np.array(self.data_buffer[i::self.dimensions],dtype='f')*self.sensitivity
                
                #Add data to a plot
                self.controller.add_data_to_a_plot(DataClass(self.comp_name, self.data_dict))

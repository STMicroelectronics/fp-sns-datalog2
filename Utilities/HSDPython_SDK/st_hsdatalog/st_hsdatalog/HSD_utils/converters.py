# *****************************************************************************
#  * @file    converters.py
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

import os
import csv
import numpy as np
import wave
import pandas as pd

import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_utils.exceptions import NanoEdgeConversionError

log = logger.get_logger(__name__)

class HSDatalogConverter:
    @staticmethod
    def to_nanoedge_format(output_folder, sensor_name, df, signal_length, signal_increment = 0, n_files = 1):
        '''
        A function to prepare the Nano Edge AI studio compliant csv files.
        The NanoEdge AI studio expects users to provide two files, containing
        normalSegments and abnormalSegments. These segments have to be shaped 
        in a particular way for example if there are 256 samples in a segment
            x1      y1      z1
            x2      y2      z2
            :		  :       :
            :		  :       :
            :       :       :
            x256   y256     z256
        For NanoEdge AI studio this data is to be shaped as 
        x1 y1 z1 x2 y2 z2 ......... x256 y256 z256

        Inputs:
            output_folder = "Output folder (this will be created if it doesn't exist)
            sensor_name =  The sensor name e.g.'ISM330DHCX'
            signal_length  = The length of each segment when performing segmentation
            signal_increment  = parameter to control the overlap, if it is not passed to the function, signal_increment = 0 (no overlap), signal_increment = signal_length/2 is fifty % overlap
            n_files: number of ouput csv files. It is useful for separating training, test and validation Dataset
        '''
        signal_increment = int(signal_increment)
        if signal_increment is None:
            signal_increment = 0
        
        if signal_increment <= 0:
            if signal_increment < 0:
                log.warning("You have chosen a negative value [{}] for the \"signal_increment\" parameter! ".format(signal_increment) + "\nThis must be a positive or zero value. For this execution the overlap will be set to 0")
            signal_increment = signal_length

        if not os.path.exists(output_folder):
            os.makedirs(output_folder)
        
        # arrange data for cartesaim
        filtered_df = df.drop('Time', axis=1)
        dataset = filtered_df.to_numpy()
        
        timestamps_per_file = np.shape(dataset)[0]/n_files

        if timestamps_per_file < signal_length:
            log.error("Not enough timestamps per file [{}]. Chosen another signal_length value [{}]. It should be lower or equal to {}".format(timestamps_per_file, signal_length, timestamps_per_file))
            raise NanoEdgeConversionError(sensor_name)

        #rearrange number of signals to the maximum number extractable from the array
        n_signals = int(np.floor((np.shape(dataset)[0]/n_files)/signal_length))
        
        signal = []
        idx = 0
        for ii in range(0, n_files):
            filename = sensor_name + "_NanoEdge_segments_{}.csv".format(ii)
            file_path = os.path.join(output_folder,filename)
            with open(file_path , "w", newline="") as f:
                writer = csv.writer(f)
                for rr in range(0, n_signals): #rows of final dataset
                    for cc in range(idx, idx + signal_length): #cc = columns in input dataset
                        if cc >= dataset.shape[0]:
                            continue
                        for el in dataset[cc]:
                            signal.append(el)
        
                    idx += signal_increment
                    writer.writerow(list(signal))
                    signal.clear()
            log.info("--> File: \"{}\" chunk appended successfully".format(filename))
        return True

    @staticmethod
    def to_csv(df, filename, mode = 'w'):
        HSDatalogConverter.to_xsv(df, filename, '.csv', ',', mode)

    @staticmethod
    def to_tsv(df, filename, mode = 'w'):
        HSDatalogConverter.to_xsv(df, filename, '.tsv', '\t', mode)

    @staticmethod
    def to_xsv(df, filename, extension, separator, mode = 'w'):
        df.to_csv(filename + extension, separator, mode = mode, compression = None, index = False, header = True if mode == 'w' else False, float_format='%14.6f')
        log.info("--> File: \"{}\" converted chunk appended successfully".format(filename + extension))

    @staticmethod
    def to_wav(pcm_data, filename, sample_freq):
        wav_file = wave.open(filename, mode='wb')
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_freq)
        wav_file.writeframesraw(bytearray(pcm_data))
        log.info("--> Wav file: \"{}\" correctly exported".format(filename))

    @staticmethod
    def wav_create(filename, sample_freq, nchannels = 1):
        wav_file = wave.open(filename, mode='wb')
        wav_file.setnchannels(nchannels)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_freq)
        return wav_file

    @staticmethod
    def wav_append(wav_file, pcm_data):
        wav_file.writeframesraw(bytearray(pcm_data))
      
    @staticmethod
    def wav_close(wav_file):
        wav_file.close()
        log.info("--> Wav file correctly exported")

    @staticmethod
    def __write_unico_file(dataframe, file_path, out_format, mode):
        if out_format.lower() == "txt":
            HSDatalogConverter.to_xsv(dataframe, file_path, '.txt', '\t', mode)
        elif out_format.lower() == "csv":
            HSDatalogConverter.to_csv(dataframe, file_path, mode)
        else:
            HSDatalogConverter.to_tsv(dataframe, file_path, mode)

    @staticmethod
    def to_unico(output_folder, sensor_name, hsd_dfs, data_tags = None, out_format = "txt", mode = 'w', with_times = False):
        
        if not os.path.exists(output_folder):
            os.makedirs(output_folder)

        res_df = pd.DataFrame()

        if out_format is None:
            out_format  = 'txt'
        for df in hsd_dfs:
            if not with_times:
                df = df.drop('Time', axis=1)
            res_df = pd.concat([res_df, df], axis=1, sort=False)
        #removes duplicates (~: bitwise negation operator)
        res_df = res_df.loc[:,~res_df.columns.duplicated()]

        if data_tags is not None:
            tag_classes = set([tag['Label'] for tag in data_tags])

            for tag_class_label in tag_classes:
                log.info(tag_class_label)
                for y, (k, tag_df) in enumerate(res_df[res_df[tag_class_label] == 1].groupby((res_df[tag_class_label] != 1).cumsum())):
                    log.debug("[group {}]".format(y))
                    tag_df = tag_df.drop([tag['Label'] for tag in data_tags], axis=1)
                    labelFileName = "{}_{}_dataLog_{}".format(tag_class_label, sensor_name, y)
                    tag_sub_folder = os.path.join(output_folder, tag_class_label)
                    if not os.path.exists(tag_sub_folder):
                        os.makedirs(tag_sub_folder)
                    labelFilePath = os.path.join(tag_sub_folder, labelFileName)
                    HSDatalogConverter.__write_unico_file(tag_df, labelFilePath, out_format, mode)
        else:
            file_path = os.path.join(output_folder, sensor_name)
            HSDatalogConverter.__write_unico_file(res_df, file_path, out_format, mode)
        
        return True
    
    @staticmethod
    def to_txt_by_tags(output_folder, sensor_name, hsd_dfs, data_tags = None, out_format = "txt", mode = 'w'):
        HSDatalogConverter.to_unico(output_folder, sensor_name, hsd_dfs, data_tags, out_format, mode, True)
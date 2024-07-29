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
import warnings
import csv
import numpy as np
import wave
import pandas as pd

import st_hsdatalog.HSD_utils.logger as logger
from st_hsdatalog.HSD_utils.exceptions import NanoEdgeConversionError

log = logger.get_logger(__name__)

class NanoedgeCSVWriter:
    def __init__(self, output_folder, comp_name, signal_length, signal_increment = 0):
        
        self.signal_length = signal_length
        self.signal_increment = signal_increment
        if self.signal_increment <= 0:
            if self.signal_increment < 0:
                log.warning(f"You have chosen a negative value [{self.signal_increment}] for the \"signal_increment\" parameter! For this execution the signal_increment will be set equals to signal_length.")
            self.signal_increment = self.signal_length
        
        if not os.path.exists(output_folder):
            os.makedirs(output_folder)

        filename = comp_name + "_NanoEdge.csv"
        self.file_path = os.path.join(output_folder, filename)

        self.buffer = []
        self.samples_chest = np.array([])
        self.start_idx = 0

    def to_nanoedge_format_batch(self, dataframe, mode = "w"):
        """
        Writes the contents of a NumPy array to a CSV file with a fixed number of columns (M).
        The array's values are written consecutively without padding, filling up rows as needed.

        Parameters:
        - input_array: A NumPy array to be written to the CSV.
        """
        # arrange data for nanoedge
        filtered_df = dataframe.drop('Time', axis=1)
        dataset = filtered_df.to_numpy()

        if len(self.samples_chest) != 0:
            dataset = np.vstack((self.samples_chest, dataset))

        idx = self.start_idx
        #Number of rows extractable from the array (taking into account of signal_increment)
        #e.g.
        # - dataset_length (samples) = 10 [1,2,3,4,5,6,7,8,9,10]
        # - signal_length (samples) = 4
        # - signal_increment (samples) = 2
        # --> output: ((dataset_length - signal_length) // signal_increment) + 1 = ((10 - 4) // 2) + 1 = 4
        #  1, 2, 3, 4   ┐
        #  3, 4, 5, 6   | n_signals = 4
        #  5, 6, 7, 8   |
        #  7, 8, 9, 10  ┘
        #  8, 10, X, X
        n_signals = ((np.shape(dataset[idx:])[0] - self.signal_length) // self.signal_increment) + 1
        
        with open(self.file_path , mode, newline="") as f:
            writer = csv.writer(f)
            for _ in range(0, n_signals): #rows of final dataset
                for cc in range(idx, idx + self.signal_length): #cc = columns in input dataset
                    if cc >= dataset.shape[0]:
                        continue
                    for el in dataset[cc]:
                        self.buffer.append(el)
    
                idx += self.signal_increment
                writer.writerow(list(self.buffer))
                self.buffer.clear()

        if n_signals > 0:
            a = self.signal_length - self.signal_increment
            if a >= 0: #To manage: signal_increment >= signal_length
                try:
                    self.samples_chest = dataset[(cc+1)-(a):]
                except Exception as e:
                    pass
            else: #To manage: signal_increment > signal_length
                try:
                    self.samples_chest = dataset[(cc+1):]
                    self.start_idx = -a
                except Exception as e:
                    pass

        return True

    def flush(self):
        """
        Writes any remaining values in the buffer to the CSV file as a partial row.
        This should be called after all arrays have been processed.
        """
        if self.buffer:
            with open(self.file_path, 'a', newline='') as csvfile:
                csv_writer = csv.writer(csvfile)
                csv_writer.writerow(self.buffer)
            self.buffer = []

class HSDatalogConverter:

    @staticmethod
    def to_csv(df, filename, mode = 'w'):
        """
        Converts a DataFrame to a CSV file.

        :param df: The DataFrame to convert.
        :param filename: The base name of the file to write to.
        :param mode: The file writing mode ('w' for write, 'a' for append).
        """
        # Delegate the conversion to the generic 'to_xsv' method for CSV format.
        HSDatalogConverter.to_xsv(df, filename, '.csv', ',', mode)
        # HSDatalogConverter.my_to_xsv(df, filename, '.csv', ',', mode)

    @staticmethod
    def to_tsv(df, filename, mode = 'w'):
        """
        Converts a DataFrame to a TSV file.

        :param df: The DataFrame to convert.
        :param filename: The base name of the file to write to.
        :param mode: The file writing mode ('w' for write, 'a' for append).
        """
        # Delegate the conversion to the generic 'to_xsv' method for TSV format.
        HSDatalogConverter.to_xsv(df, filename, '.tsv', '\t', mode)

    def to_hdf5(df:pd.DataFrame, filename, mode = 'w'):
        """
        Converts a DataFrame to a HDF5 file.

        :param df: The DataFrame to convert.
        :param filename: The base name of the file to write to.
        :param mode: The file writing mode ('w' for write, 'a' for append).
        """
        log.warning("The 'to_hdf5' method is not implemented yet. Try a different conversion method.")
        # # Use pandas to_hdf method to write the DataFrame to a h5 file.
        # df.to_hdf(filename + ".h5", key="data", mode = mode)
        # log.info(f"--> File: \"{filename +'.h5'}\" converted chunk appended successfully")
    
    def to_parquet(df:pd.DataFrame, filename, mode = 'w'):
        """
        Converts a DataFrame to a Parquet file.

        :param df: The DataFrame to convert.
        :param filename: The base name of the file to write to.
        :param mode: The file writing mode ('w' for write, 'a' for append).
        """        
        parquet_file_path = filename + ".parquet"
        if mode == 'a':
            # Append to the existing Parquet file
            df.to_parquet(parquet_file_path, engine='fastparquet', append=True)
        else:
            # Write a new Parquet file
            df.to_parquet(parquet_file_path, engine='fastparquet')
        log.debug(f"--> File: \"{parquet_file_path}\" converted chunk appended successfully")

    @staticmethod
    def to_xsv_numpy(df, filename, extension, separator, mode = 'w'):
        """
        Converts a DataFrame to a delimited text file (e.g., CSV, TSV) using NumPy's savetxt method.

        :param df: The DataFrame to convert.
        :param filename: The base name of the file to write to.
        :param extension: The file extension to use (e.g., '.csv' for CSV files).
        :param separator: The delimiter to use between values (e.g., ',' for CSV).
        :param mode: The file writing mode ('w' for write, 'a' for append).
        """
        # Construct the full file path by appending the extension to the filename.
        filepath = filename + extension
        # Determine whether to write headers based on the file mode and existence of the file.
        header = True # Default is to write headers.
        if mode == 'a' and os.path.exists(filename+extension):
            # If appending to an existing file, do not write headers.
            header = False
        # Define the format for each column based on its data type.
        formats = ['%s' if dtype == object else '%d'if dtype == np.int_ else '%.12g' for dtype in df.dtypes]
        # Open the file with the specified mode and write the DataFrame content.
        with open(filepath, mode) as f:
            # Write the header if required.
            if header:
                f.write(separator.join(df.columns)+'\n')
            # Write the DataFrame data using NumPy's savetxt function with the specified formats and delimiter.
            np.savetxt(f, df.values, delimiter=separator, fmt=formats,comments='')

    @staticmethod
    def to_xsv(df, filename, extension, separator, mode = 'w'):
        """
        Converts a DataFrame to a delimited text file (e.g., CSV, TSV) using pandas to_csv method.

        :param df: The DataFrame to convert.
        :param filename: The base name of the file to write to.
        :param extension: The file extension to use.
        :param separator: The delimiter to use between values.
        :param mode: The file writing mode ('w' for write, 'a' for append).
        """
        # Convert all columns to string type to ensure consistent formatting (and to improve conversion performance).
        for c in df.columns:
            df[c] = df[c].astype(str) 
        # Use pandas to_csv method to write the DataFrame to a file with the specified delimiter and mode.
        df.to_csv(filename + extension, sep = separator, mode = mode, compression = None, index = False, header = True if mode == 'w' else False, float_format='%.6f')
        log.debug("--> File: \"{}\" converted chunk appended successfully".format(filename + extension))

    @staticmethod
    def to_wav(pcm_data, filename, sample_freq):
        """
        Converts PCM data to a WAV file.

        DEPRECATED: This method is deprecated and will be removed in a future version.
        Use 'wav_create', 'wav_append', and 'wav_close' methods instead for more control over WAV file creation.

        :param pcm_data: The PCM data to convert.
        :param filename: The name of the WAV file to create.
        :param sample_freq: The sample frequency of the audio data.
        """
        # Issue a deprecation warning to the user.
        warnings.warn(
            "The 'to_wav' method is deprecated and will be removed in a future version. "
            "Use 'wav_create', 'wav_append', and 'wav_close' methods instead.",
            DeprecationWarning,
            stacklevel=2
        )
        wav_file = wave.open(filename, mode='wb')
        wav_file.setnchannels(1)
        wav_file.setsampwidth(2)
        wav_file.setframerate(sample_freq)
        wav_file.writeframesraw(bytearray(pcm_data))
        log.info("--> Wav file: \"{}\" correctly exported".format(filename))

    @staticmethod
    def wav_create(filename, sample_freq, nchannels = 1):
        """
        Creates a new WAV file and sets its parameters.

        :param filename: The name of the WAV file to create.
        :param sample_freq: The sample frequency of the audio data.
        :param nchannels: The number of channels in the audio data (default is 1).
        :return: The wave.Wave_write object for the created WAV file.
        """
        # Open a new WAV file for writing.
        wav_file = wave.open(filename, mode='wb')
        # Set the WAV file parameters.
        wav_file.setnchannels(nchannels)
        wav_file.setsampwidth(2) # Assuming 16-bit samples (2 bytes per sample).
        wav_file.setframerate(sample_freq)
        # Return the wave.Wave_write object to allow appending data later.
        return wav_file

    @staticmethod
    def wav_append(wav_file, pcm_data):
        """
        Appends PCM data to an open WAV file.

        :param wav_file: The wave.Wave_write object representing the open WAV file.
        :param pcm_data: The PCM data to append to the WAV file.
        """
        # Write the raw PCM data to the WAV file.
        wav_file.writeframesraw(bytearray(pcm_data))
      
    @staticmethod
    def wav_close(wav_file):
        """
        Closes the WAV file and logs the successful export.

        :param wav_file: The wave.Wave_write object representing the open WAV file.
        """
        # Close the WAV file.
        wav_file.close()
        # Log the successful export of the WAV file.
        log.info("--> Wav file correctly exported")

    @staticmethod
    def __write_unico_file(dataframe, file_path, out_format, mode):
        """
        Writes a DataFrame to a file in a specified ST Unico format (txt, csv, tsv).

        :param dataframe: The DataFrame to write.
        :param file_path: The base path for the output file.
        :param out_format: The output format ('txt', 'csv', 'tsv').
        :param mode: The file writing mode ('w' for write, 'a' for append).
        """
        # Choose the appropriate method to write the DataFrame based on the output format.
        if out_format.lower() == "txt":
            HSDatalogConverter.to_xsv(dataframe, file_path, '.txt', '\t', mode)
        elif out_format.lower() == "csv":
            HSDatalogConverter.to_csv(dataframe, file_path, mode)
        else:
            HSDatalogConverter.to_tsv(dataframe, file_path, mode)
    
    @staticmethod
    def to_unico(file_path, df, out_format = "txt", mode = 'w', with_times = False, columns_labels = "default"):
        """
        Writes a DataFrame to a file in a specified ST Unico format with an optional time column.

        :param file_path: The base path for the output file.
        :param df: The DataFrame to write.
        :param out_format: The output format ('txt', 'csv', 'tsv').
        :param mode: The file writing mode ('w' for write, 'a' for append).
        :param with_times: A boolean indicating whether to include the 'Time' column.
        :param columns_labels: [Optional] Column labels for the output file. Default is "default".
        :return: True if the operation is successful.
        """
        if not with_times:
            if columns_labels == "mlc_tool":
                df = df.drop('Time[s]', axis=1)
            else:
                df = df.drop('Time', axis=1)
        HSDatalogConverter.__write_unico_file(df, file_path, out_format, mode)
        return True

    @staticmethod
    def merge_dataframes(dataframes, comp_names, columns_labels = None, tags_columns_names = []):
        """
        Merges multiple DataFrames based on their 'Time' columns and renames the columns.

        :param dataframes: A list of DataFrames to merge.
        :param comp_names: A list of component names corresponding to each DataFrame.
        :param columns_labels: A labeling scheme for the columns (e.g., 'mlc_tool').
        :param tags_columns_names: A list of column names to exclude from renaming.
        :return: The merged DataFrame.
        """
        # Define a list of columns to exclude
        cols_to_exclude = ["Time"]
        if len(tags_columns_names) > 0:
            cols_to_exclude.extend(tags_columns_names)
        # Rename columns and merge DataFrames.
        for i, cn in enumerate(comp_names):
            if columns_labels is not None:
                # Apply a specific labeling scheme to the columns based on the provided columns_labels argument.
                if columns_labels == "mlc_tool":
                    # Define a prefix to add to column names based on the component name.
                    prefix = f"{cn}"
                    new_names = {}
                    for col in dataframes[i].columns:
                        if col not in cols_to_exclude:
                            sensor_type_str = col.split(' ')[0]
                            # Create a dictionary of column name mappings with the new prefix.
                            if "_" in sensor_type_str:
                                new_col_name = prefix + "_" + sensor_type_str.split('_')[-1] + " " + dataframes[i].columns[1].split(' ')[-1]
                            else:
                                new_col_name = prefix + " " + dataframes[i].columns[1].split(' ')[-1]
                            new_names[col] = new_col_name
                else:
                    # Define a suffix to add to column names based on the component name.
                    suffix = f"_{cn}"
                    # Create a dictionary of column name mappings with the new suffix.
                    new_names = {col: col + suffix for col in dataframes[i].columns if col not in cols_to_exclude}
                # Rename columns in the dataframe according to the new names mapping.
                dataframes[i] = dataframes[i].rename(columns=new_names)

        # Convert non-numeric 'Time' column to numeric for all dataframes to ensure proper merging.
        for df in dataframes:
            df['Time'] = df['Time'].astype(float)

        # Merge dataframes using an 'asof' merge, which is a sort of fuzzy merge suitable for time series data.
        # It merges on the nearest key rather than requiring an exact match.
        merged_df = dataframes[0]
        for i in range(1, len(dataframes)):
            merged_df = pd.merge_asof(merged_df, dataframes[i], on="Time", direction="nearest")
        
        # Return the merged DataFrame.
        return merged_df
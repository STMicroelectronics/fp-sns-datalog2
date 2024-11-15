
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

from functools import partial
import os
import json
from datetime import datetime, timedelta, timezone

from PySide6.QtCore import QThread, Signal

from st_hsdatalog.HSD.HSDatalog import HSDatalog
from ..segmentation.controller import Controller
from ..segmentation.losses import F1Loss
from ..segmentation.optimizers import MangoOptimizer
from ..segmentation.segmenters import Segmenter
from ..segmentation.scoremodels import WaveletDecompositionModel

class UI_Controller:
    class SegmentationThread(QThread):
        sig_finished = Signal(Controller) # Signal emitted when the segmentation thread finishes

        def __init__(self, samples):
            """
            Initializes the SegmentationThread object.
            
            Args:
                samples (list): The samples to segment using the segmentation algorithm thread.
            """
            super().__init__()
            self.samples = samples

        def run(self):
            """
            Runs the segmentation algorithm thread.
            """
            print("Segmentation Thread started...")
            mu = self.samples.mean() # Mean of the samples
            sigma = self.samples.std() # Standard deviation of the samples
            samples = (self.samples - mu)/(3*sigma) # Normalize the samples
            # Create the segmentation algorithm objects
            score_model = WaveletDecompositionModel(samples) # Create the score model
            segmenter = Segmenter(score_model=score_model) # Create the segmenter
            optimizer = MangoOptimizer(max_calls=50) # Create the optimizer
            loss = F1Loss(peak_tolerance=500) # Create the loss function
            segmenter.compile(loss, optimizer) # Compile the segmenter
            # Create the controller object with the segmenter and the break point tolerance
            controller = Controller(segmenter=segmenter, break_point_tolerance=500)
            print("Segmentation Thread completed")
            # Emit the finished signal with the controller object as an argument to be used in the finish callback
            self.sig_finished.emit(controller)

    def __init__(self):
        """
        Initializes the UI_Controller object.
        """
        self.hsd_factory = HSDatalog()
        self.hsd = None
        self.selected_component = None
        self.controller:Controller = None
        self.tagging_params = "create"

    def get_gt_break_points(self):
        """
        Returns the ground truth breakpoints.

        Returns:
            list: A list of ground truth breakpoints.
        """
        return self.controller.segmenter.gt_break_points
    
    def get_candidate_break_points(self):
        """
        Returns the candidate breakpoints.
        
        Returns:
            list: A list of candidate breakpoints.
        """
        return self.controller.get_candidate_break_points()

    def get_samples(self):
        """
        Returns the samples.
        
        Returns:
            list: A list of samples.
        """
        return self.controller.get_samples()
    
    def get_scores(self):
        """
        Returns the scores.

        Returns:
            list: A list of scores.
        """
        return self.controller.get_scores()

    def get_nsamples(self):
        """
        Returns the number of samples.

        Returns:
            int: The number of samples.
        """
        return self.controller.get_nsamples()
    
    def get_segmenter_threshold(self):
        """
        Returns the segmenter threshold.

        Returns:
            float: The segmenter threshold.
        """
        return self.controller.segmenter.threshold
    
    def add_gt_break_point(self, bkp_index):
        """
        Adds a ground truth breakpoint.

        Args:
            bkp_index (int): The breakpoint index.
        """
        self.controller.add_gt_break_point(bkp_index)

    def remove_candidate_break_point(self, bkp_index):
        """
        Removes a candidate breakpoint.
        
        Args:
            bkp_index (int): The breakpoint index.
        """
        self.controller.remove_candidate_break_point(bkp_index)
    
    def validate_acquisition_folder(self, acquisition_folder_path):
        """
        Validates the acquisition folder.
        
        Args:
            acquisition_folder_path (str): The acquisition folder path.
            
        Returns:
            bool: True if the acquisition folder is valid, False otherwise.
        """
        # Validate the acquisition folder
        hsd_version = HSDatalog.validate_hsd_folder(acquisition_folder_path)
        if hsd_version != HSDatalog.HSDVersion.INVALID: # If the acquisition folder is valid
            self.hsd = self.hsd_factory.create_hsd(acquisition_folder_path) # Create the HSD object
            self.acquisition_folder_path = acquisition_folder_path # Set the acquisition folder path
            return True
        return False
    
    def get_file_dimension(self, component_name):
        """
        Returns the file dimension.
        
        Args:
            component_name (str): The component name.

        Returns:
            int: The file dimension.
        """
        return HSDatalog.get_file_dimension(self.hsd, component_name) # Get the file dimension
    
    def get_component_list(self):
        """
        Returns the component list.
        
        Returns:
            list: A list of components.
        """
        return HSDatalog.get_sensor_list(self.hsd) # Get the component list

    def set_selected_component(self, selected_component):
        """
        Sets the selected component.
        
        Args:
            selected_component (str): The selected component.
        """
        self.selected_component = selected_component # Set the selected component

    def set_tagging_params(self, tagging_params):
        """
        Sets the tagging parameters.
        
        Args:
            tagging_params (str): The tagging parameters.
        """
        self.tagging_params = tagging_params

    def start_segmentation_thread(self, finish_callback):
        """
        Starts the segmentation thread.
        
        Args:
            finish_callback (function): The finish callback.
        """
        # Get the data and timestamps
        data_times = HSDatalog.get_data_and_timestamps(self.hsd, self.selected_component, start_time=0, end_time=-1, raw_data=True)
        self.component_samples = data_times[0][0] # Get the component samples
        self.component_times = data_times[0][1] # Get the component times
        # Start the segmentation thread
        self.worker_thread = UI_Controller.SegmentationThread(self.component_samples) # Create the segmentation thread
        self.worker_thread.sig_finished.connect(partial(self.__inner_finish_callback, finish_callback)) # Connect the finish callback
        self.worker_thread.start() # Start the segmentation thread

    def __inner_finish_callback(self, finish_callback, controller):
        """
        The inner segmentation finish callback.

        Args:
            finish_callback (function): The finish callback.
            controller (Controller): The controller
        """
        self.controller = controller # Set the returned controller (created within the segmentation thread)
        finish_callback() # Call the finish callback

    def tag_acquisition_with_break_points(self, non_empty_tags_alert_callback):
        """
        Tags the acquisition data with segmenter output breakpoints.
        
        Args:
            non_empty_tags_alert_callback (function): The non-empty tags alert callback.
            
        Returns:
            bool: True if the acquisition data is tagged with segmenter output breakpoints, False otherwise.
        """
        breakpoints = self.get_gt_break_points() # Get the ground truth breakpoints
        if breakpoints != []: # If there are ground truth breakpoints
            acquisition_info = HSDatalog.get_acquisition_info(self.hsd) # Get the acquisition info
            return self.__tag_acquisition_w_breakpoints(breakpoints, self.component_times, acquisition_info, non_empty_tags_alert_callback) # Tag the acquisition data with segmenter output breakpoints
        return False

    def __tag_acquisition_w_breakpoints(self, breakpoints, component_times, acquisition_info, non_empty_tags_alert_callback):
        """
        Tags the acquisition data with segmenter output breakpoints.

        Args:
            breakpoints (list): The breakpoints.
            component_times (list): The component times.
            acquisition_info (dict): The acquisition info.
            non_empty_tags_alert_callback (function): The non-empty tags alert callback.

        Returns:
            bool: True if the acquisition data is tagged with segmenter output breakpoints, False otherwise.
        """
        existing_tags = acquisition_info.get("tags", []) # Get the existing tags
        if existing_tags != []: # If there are existing tags
            non_empty_tags_alert_callback() # Call the non-empty tags alert callback
        # If the tagging parameters are set to "overwrite", then the existing tags are overwritten
        if self.tagging_params == "overwrite":
            existing_tags = [] # Clear the existing tags
        # If the tagging parameters are set to "append", then the existing tags are appended
        if self.tagging_params is not None:
            start_time_iso = acquisition_info["start_time"] # Get the aquisition start time from acquisition_info.json
            end_time_iso = acquisition_info["end_time"] # Get the aquisition end time from acquisition_info.json
            start_time = datetime.strptime(start_time_iso, "%Y-%m-%dT%H:%M:%S.%fZ").replace(tzinfo=timezone.utc) # Convert the start time to datetime object
            tag_times = [component_times[i][0] for i in breakpoints] # Get the tag times using the breakpoints indices and the component times list
            new_times = [start_time] + [start_time + timedelta(seconds=tt) for tt in tag_times] # Create a list of new times calculated from the start time and the tag times
            formatted_times =  [nt.strftime("%Y-%m-%dT%H:%M:%S.%f")[:-3] + "Z" for nt in new_times] # Format the new times to ISO 8601 format with milliseconds
            label_counter = 0 # Initialize the label counter for the tag classes
            formatted_times.append(end_time_iso) # Append the end time to the formatted times list
            # for each pair of consecutive formatted times, create a start and an end event with a unique label
            for i in range(0, len(formatted_times)-1):
                start_event = {'l': f'SW_TAG_{label_counter}', 'e': True, 'ta': formatted_times[i]} # Create a start tag event with a unique label
                end_event = {'l': f'SW_TAG_{label_counter}', 'e': False, 'ta': formatted_times[i+1]} # Create an end tag event with a unique label
                existing_tags.append(start_event) # Append the start tag event to the existing tags
                existing_tags.append(end_event) # Append the end tag event to the existing tags
                label_counter += 1 # Increment the label counter for the tag classes
            # Update the tags in the acquisition_info dictionary with the new tags (existing tags + new tags if "append" mode is selected, new tags only if "overwrite" mode is selected)
            acquisition_info["tags"] = existing_tags
            # Write the updated acquisition_info dictionary to the acquisition_info.json file
            with open(os.path.join(self.acquisition_folder_path,"acquisition_info.json"), 'w') as file:
                json.dump(acquisition_info, file, indent=4)
            # Reset the tagging parameters
            self.tagging_params = None
            return True
        else: # If the tagging parameters are not set
            return False
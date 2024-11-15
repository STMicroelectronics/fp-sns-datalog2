
""" 
This module contains the Controller class, which is responsible for managing the interaction between the user and the segmenter. 

The Controller class has the following attributes:
    - segmenter: Segmenter object
    - tolerance: int

The Controller class has the following methods:
    - get_candidate_break_points: returns a list of candidate break points
    - add_gt_break_point: adds a ground truth break point
    - remove_candidate_break_point: removes a candidate break point
    - confirm_break_points: confirms break points
    - update: updates the segmenter
    - get_samples: returns the samples used by the segmenter
    - get_n_samples: returns the number of samples used by the segmenter
"""

import numpy as np
from .segmenters import Segmenter
from typing import List

# REFACTOR: uniformize the use of @property and getters
class Controller:
    """ 
    Controller class is responsible for managing the interaction between the user and the segmenter. 

    Attributes:
        segmenter (Segmenter): Segmenter object
        tolerance (int): tolerance for break points
        gt_updated (bool): flag to indicate if ground truth was updated
    
    Methods:
        get_candiate_break_points(): returns a list of candidate break points
        add_gt_break_point(break_point: int): adds a ground truth break point
        remove_candidate_break_point(break_point: int): removes a candidate break point
        confirm_break_points(start: int, end: int): confirms break points
        update(): updates the segmenter
        get_samples(): returns the samples used by the segmenter
        get_scores(): returns the scores used by the segmenter
        get_nsamples(): returns the number of samples used by the segmenter

    """

    def __init__(self, segmenter: Segmenter, break_point_tolerance: int):
        """ 
        Initializes the Controller object.

        Args:
            segmenter (Segmenter): Segmenter object
            break_point_tolerance (int): tolerance for break points
        """
        self.segmenter = segmenter
        self.tolerance = break_point_tolerance
        self.gt_updated = False

    def get_candidate_break_points(self) -> List[int]:
        """ 
        Returns a list of candidate break points. 

        Returns:
            List[int]: list of candidate break points
        """
        candidate_break_points = self.segmenter.get_break_points()
        return [bkp for bkp in candidate_break_points if bkp not in self.segmenter.supervised_domain]
    
    def add_gt_break_point(self, break_point: int):
        """ 
        Adds a ground truth break point.

        Args:
            break_point (int): ground truth break point
        """
        candidate_break_points = self.get_candidate_break_points()
        if len(candidate_break_points) > 0:
            candidate_dist = [np.abs(bkp - break_point) for bkp in candidate_break_points]

            i_best = np.argmin(candidate_dist)
            if candidate_dist[i_best] < self.tolerance:
                break_point = candidate_break_points[i_best]
            else:
                self.gt_updated = True

            self.confirm_break_points(0, break_point - self.tolerance)

        else:
            self.gt_updated = True
        
        self.segmenter.add_gt_interval(supervised_interval=[0, break_point+self.tolerance], break_points=[break_point] + self.segmenter.gt_break_points)

        self.update() 

    def remove_candidate_break_point(self, break_point: int):
        """
        Removes a candidate break point. 

        Args:
            break_point (int): candidate break point
        """
        candidate_break_points = self.get_candidate_break_points()

        if len(candidate_break_points) > 0:

            candidate_dist = [np.abs(bkp - break_point) for bkp in candidate_break_points]

            i_best = np.argmin(candidate_dist)

            if candidate_dist[i_best] < self.tolerance:
                self.confirm_break_points(0, candidate_break_points[i_best] - self.tolerance)
                self.segmenter.add_gt_interval(supervised_interval=[0, candidate_break_points[i_best] + self.tolerance], break_points=self.segmenter.gt_break_points)
                self.gt_updated = True
        
        self.update()
    
    # ONGOING: add_gt_interval va invocato fuori dal loop perche' puo' rallentare parecchio
    def confirm_break_points(self, start: int, end: int):
        """
        Confirms break points insime a time interval, specified by start and end.

        Args:
            start (int): start of the time interval
            end (int): end of the time interval
        """ 
        candidate_break_points = self.get_candidate_break_points()
        for bkp in candidate_break_points:
            if start <= bkp <= end:
                self.segmenter.add_gt_interval(supervised_interval=[0, bkp + self.tolerance], break_points=[bkp] + self.segmenter.gt_break_points)   

    def update(self):
        """
        Updates the segmenter weights, based on the feedback provided by the user. 
        """
        if self.gt_updated:
            self.segmenter.update()
            self.gt_updated = False
    
    def get_samples(self) -> np.ndarray:
        """
        Returns the samples used by the segmenter. 

        Returns:
            np.ndarray: samples
        """
        return self.segmenter.samples
    
    def get_scores(self) -> np.ndarray:
        """
        Returns the scores used by the segmenter. 

        Returns:
            np.ndarray: scores
        """
        return self.segmenter.get_score()
    
    def get_nsamples(self) -> int:
        """
        Returns the number of samples used by the segmenter. 

        Returns:
            int: number of samples
        """
        return self.segmenter.samples.shape[0]
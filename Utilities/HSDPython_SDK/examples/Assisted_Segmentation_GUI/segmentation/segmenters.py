
# DOC modificare la documentazione dopo il refactoring
"""    
This module contains classes for segmentation tasks. The module provides classes for defining score models, segmenters, and optimizers for segmentation tasks. 

Classes:
    Segmenter: A class representing a segmenter for segmentation tasks.
    Loss: Abstract base class for defining loss functions used in segmentation.
    F1Loss: A class that calculates the F1 loss for a given set of scores and ground truth break points.
    ScoreModel: Abstract base class for score models used in segmentation.
    WaveletDecompositionModel: A class representing a wavelet decomposition model for segmentation.
    Optimizer: Abstract base class for optimizers used in segmentation.
    BayesianOptimizer: A class that performs Bayesian optimization to find the optimal weights for a segmenter.
    MangoOptimizer: A class that optimizes the weights of a segmenter using the Mango optimization algorithm.
    SupervisedDomain: A class representing a supervised domain for segmentation.

Segmenter:
    A class representing a segmenter for segmentation tasks. The segmenter is used to perform segmentation on a datastream using a score model. The segmenter can be compiled with a loss function and an optimizer to find the optimal weights for the score model. 

Loss:
    Abstract base class for defining loss functions used in segmentation.

F1Loss:
    A class that calculates the F1 loss for a given set of scores and ground truth break points. The F1 loss is defined as 1-F1, where F1 is the maximum F1 score that can be achieved by varying the threshold value.

ScoreModel:
    Abtract base class for score models used in segmentation. The score model is used to calculate the score for a segmentation task. In particular, the method get_profile() return a score profile, that has the same length of the datastream to be segmented. A peak in the score profile indicates a change point in the datastream.

 WaveletDecompositionModel:
    A class representing a wavelet decomposition model for segmentation. It performs wavelet decomposition on the input samples using the specified level and wavelet type (db3). For each subband obtained from the decomposition, it calculates a profile using the NormalDivergenceProfile, resamples it, applies a power transform, and stores it. It then computes an average score profile across all subbands and sets this as both the average and the score profile of the model. 

Optimizer:
    Abstract base class for optimizers used in segmentation. The optimizer is devoted to find the optimal weights for a segmenter.

BayesianOptimizer:
    A class that performs Bayesian optimization to find the optimal weights for a segmenter.

MangoOptimizer:
    The MangoOptimizer class is used to optimize the weights of a segmenter using the Mango optimization algorithm.

SupervisedDomain:
    Represents a supervised domain for segmentation. This class is designed to manage a domain of supervised intervals for segmentation tasks. It encapsulates the concept of a domain that is partially labeled or supervised, where certain ranges (intervals) of the domain are marked as supervised regions. This class provides a structured way to handle these intervals, check for membership, add new intervals, resolve overlaps between them, and generate a boolean array indicating the supervised samples.

"""

from typing import List, Tuple
import numpy as np
from .utils import UnionOfIntervals 
from abc import ABC, abstractmethod

# REFACTOR: the optimizer does not need the segmenter. It only needs the weights and 
# the weights contraints. The reference to the segmenter can be removed


class Loss(ABC):
    """
    Abstract base class for defining loss functions used in segmentation.
    """

    @abstractmethod
    def __call__(self, score: np.array, gt_break_points: List[int]) -> float:
        """
        Calculate the loss value given the predicted score and ground truth break points.

        Args:
            score (np.array): The predicted score.
            gt_break_points (List[int]): The ground truth break points.

        Returns:
            float: The calculated loss value.
        """

    @abstractmethod
    def get_optimal_threshold(self, score: np.array, gt_break_points: List[int]) -> float:
        """
        Calculate the optimal threshold value given the predicted score and ground truth break points.

        Args:
            score (np.array): The predicted score.
            gt_break_points (List[int]): The ground truth break points.

        Returns:
            float: The calculated optimal threshold value.
        """

    @abstractmethod
    def get_minimum_value(self) -> float:
        """
        Get the minimum possible value for the loss function.

        Returns:
            float: The minimum value for the loss function.
        """

class Optimizer:
    """
    Abstract base class for optimizers. The optimizer is devoted to find the 
    optimal weights for a segmenter

    This class defines the interface for optimizers used in the segmentation process.
    Subclasses of `Optimizer` should implement the `update`, `set_segmenter`, and `set_loss` methods.

    Attributes:
        None

    Methods:
        update: Update the weights of the score model used by the segmenter and 
            return the updated weights.
        set_segmenter: Set the segmenter for the optimizer.
        set_loss: Set the loss function and minimum loss value for the optimizer.
    """

    @abstractmethod
    def update(self) -> List[float]:
        """
        Compute the new weights of the segmenter and return the computed weights

        Returns:
            List[float]: The computed weights.
        """

    @abstractmethod
    def set_segmenter(self, segmenter: 'Segmenter'):
        """
        Sets the segmenter for the object.

        Args:
        - segmenter: An instance of the Segmenter class.

        """

    @abstractmethod
    def set_loss(self, loss: callable, minimum_loss_value: float = -np.inf):
        """
        Sets the loss function for the segmenter.

        Args:
            loss (callable): The loss function to be used for segmentation.
            minimum_loss_value (float): The minimum loss value allowed.

        """



class ScoreModel(ABC):
    """
    Abstract base class for score models used in segmentation.
    """

    @property
    @abstractmethod
    def num_samples(self) -> int:
        """
        Get the number of samples used in the score model.

        Returns:
            int: The number of samples.
        """

    @property
    @abstractmethod
    def weights(self) -> List[float]:
        """
        Get the weights used in the score model.

        Returns:
            List[float]: The weights.
        """

    @abstractmethod
    def get_score(self) -> np.array:
        """
        Calculate the score using the score model.

        Returns:
            np.array: The calculated score.
        """

    @property
    @abstractmethod
    def weights_constraints(self) -> List[Tuple[float, float]]:
        """
        Get the constraints for the weights used in the score model.

        Returns:
            List[Tuple[float, float]]: The constraints for the weights.
        """

    # DOC: ScoreModel.store_window
    @abstractmethod
    def store_window(self, start: int, end: int):
        pass
  
    # DOC: ScoreModel.get_windows
    @abstractmethod
    def get_windows(self) -> List[Tuple[int, np.array]]:
        pass


# TODO: add a method to remove a break point from the ground truth break points
# DOC: manca la documentazione dei metodi
class Segmenter:
    """
    A class representing a segmenter for segmentation tasks. The segmenter is used to perform segmentation on a datastream using a score model. The segmenter can be compiled with a loss function and an optimizer to find the optimal weights for the score model.

    Attributes:
        score_model (ScoreModel): The score model used by the segmenter.
        gt_break_points (List[int]): The ground truth break points.
        supervised_domain (UnionOfIntervals): The supervised domain for the segmenter.
        weights (List[float]): The weights assigned to the score model.
        threshold (float): The threshold value for segmentation.
        extension_window (int): The extension window for supervised intervals.
        loss (Loss): The loss function used by the segmenter.
        optimizer (Optimizer): The optimizer used by the segmenter.

    """

    def __init__(self, score_model: ScoreModel, extension_window: int = 100):
        """
        Initializes a new instance of the Segmenter class.

        Args:
            score_model (ScoreModel): The score model used by the segmenter.
            extension_window (int, optional): The extension window for supervised intervals. Defaults to 100.
        """
        
        self.score_model: ScoreModel = score_model
        self.gt_break_points = []
        self.supervised_domain = UnionOfIntervals(score_model.num_samples)
        self.weights = score_model.weights
        self.threshold = np.sort(score_model.get_score())[-10]
        # self.threshold = np.quantile(score_model.get_score(), 0.99)
        self.extension_window = extension_window
        self.loss = None
        self.optimizer = None

    def compile(self, loss: Loss, optimizer: Optimizer):
        """
        Compiles the segmenter with a loss function and an optimizer. The loss function is used to calculate the loss value for the segmenter, while the optimizer is used to find the optimal weights for the score model.

        Args:
            loss (Loss): The loss function to be used for segmentation.
            optimizer (Optimizer): The optimizer to be used for optimization.
        """
        self.loss = loss
        optimizer.set_segmenter(self)
        optimizer.set_loss(self.loss_fun, loss.get_minimum_value())
        self.optimizer = optimizer

    @property
    def num_samples(self) -> int:
        """
        Returns the number of samples used in the segmenter.

        Returns:
            int: The number of samples.
        """
        return self.score_model.num_samples

    def loss_fun(self, weights: List[float]):
        """
        Calculates the loss value for the segmenter using the given weights.

        Args:
            weights (List[float]): The weights to be used for segmentation.
        Returns:
            float: The calculated loss value.
        """
        score, gt_bkps = self.get_supervised_score(weights=weights)
        loss_value = self.loss(score, gt_bkps)
        return loss_value

    def get_optimal_threshold(self) -> float:
        """
        Calculates the optimal threshold value for the segmenter.

        Returns:
            float: The optimal threshold value.
        """
        score, gt_bkps = self.get_supervised_score()
        return self.loss.get_optimal_threshold(score, gt_bkps)

    def get_score(self, weights: List[float] = None) -> np.array:
        """
        Calculates the score for the segmenter using the given weights.

        Args:
            weights (List[float], optional): The weights to be used for segmentation. Defaults to None. If None is provided, the weights stored in the segmenter are used.
        
        Returns:
            np.array: The calculated score.
        """
        if weights is None:
            weights = self.weights
        self.score_model.weights = weights
        return self.score_model.get_score()
    
    @property
    def samples(self) -> np.array:
        """
        Returns the samples used by the segmenter.

        Returns:
            np.array: The samples.
        """
        return self.score_model.samples
        
    def get_supervised_score(self, weights: List[float] = None) -> np.array:
        """
        Calculates the score for the segmenter using the given weights, considering only the supervised regions.

        Args:
            weights (List[float], optional): The weights to be used for segmentation. Defaults to None. If None is provided, the weights stored in the segmenter are used.

        Returns:
            np.array: The calculated score.
        """
        if weights is None:
            weights = self.weights
        self.score_model.weights = weights
        stored_windows = self.score_model.get_windows()

        supervised_score = []
        gt_bkps = []
        cnt = 0
        for start, window in stored_windows:
            supervised_score.append(window)
            for bkp in self.gt_break_points:
                if start <= bkp < start + len(window):
                    gt_bkps.append(cnt + bkp - start)
            cnt += len(window)
        supervised_score = np.concatenate(supervised_score)
        return supervised_score, gt_bkps

    @property
    def weights_constraints(self) -> List[Tuple[float, float]]:
        """
        Returns the constraints for the weights used in the segmenter.

        Returns:
            List[Tuple[float, float]]: The constraints for the weights.
        """
        return self.score_model.weights_constraints

    def get_break_points(self) -> List[int]:
        """
        Returns the break points detected by the segmenter.

        Returns:
            List[int]: The detected break points.
        """
        bkps = [bkp for bkp in np.where(self.get_score() >= self.threshold)[0] if bkp not in self.supervised_domain]
        bkps.extend(self.gt_break_points)
        return bkps

    #DOC: Segmenter.add_gt_interval
    def add_gt_interval(self, supervised_interval: List[int], break_points: List[int] = None):
        gt_break_points = [bkp for bkp in self.gt_break_points if not(supervised_interval[0] <= bkp <= supervised_interval[1])]
        if break_points:
            for bkp in break_points:
                if not (supervised_interval[0] <= bkp <= supervised_interval[1]):
                    raise ValueError("The break points must be inside the supervised interval")
            gt_break_points.extend(break_points)
        self.gt_break_points = list(sorted([b for b in gt_break_points]))
        self.supervised_domain.add_interval(supervised_interval)
        self.score_model.store_window(supervised_interval[0], supervised_interval[1])

    def update(self):
        """
        Updates the segmenter using the optimizer. The weights of the score model are updated using the optimizer, and the optimal threshold value is calculated.
        """
        self.weights = self.optimizer.update()
        self.threshold = self.get_optimal_threshold()
    


    

                    




    


















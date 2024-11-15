
from typing import List, Tuple
import numpy as np
from .segmenters import Loss



class F1Loss(Loss):
    """
    F1Loss is a class that calculates the F1 loss for a given set of scores and ground truth break points. 
    The F1 loss is defined as 1-F1, where F1 is the maximum F1 score that can be achieved by varying the threshold value.
    """

    def __init__(self, peak_tolerance: int = 100):
        """
        Initializes the F1Loss object.

        Args:
            peak_tolerance (int): The tolerance value for matching peaks in the ground truth break points.
                                  Defaults to 100.
        """
        self.peak_tolerance = peak_tolerance

    def _compute_best_f1(self, scores: np.array, gt_break_points: List[int]) -> Tuple[float, float]:
        """
        Computes the best F1 score and threshold value for a given set of scores and ground truth break points.

        Args:
            scores (np.array): The array of scores.
            gt_break_points (List[int]): The list of ground truth break points.

        Returns:
            Tuple[float, float]: A tuple containing the best F1 score and the corresponding threshold value.
        """
        threshold_values = np.unique(scores)[::-1]

        best_f1 = 0
        max_fp = len(scores)
        best_index = 0

        for i_threshold, threshold in enumerate(threshold_values):

            indices = np.where(scores >= threshold)[0]

            if len(indices) == 0:
                f1 = 0
            else:
                not_matched_positive = indices.copy()
                for p in gt_break_points:
                    if len(not_matched_positive) == 0:
                        break
                    if np.min(np.abs(not_matched_positive-p)) < self.peak_tolerance:
                        i = np.argmin(np.abs(not_matched_positive-p))
                        not_matched_positive = np.delete(not_matched_positive, i)

                false_positive = len(not_matched_positive)
                true_positive = len(indices) - false_positive

                if true_positive == 0:
                    f1 = 0
                else:
                    precision = true_positive / len(indices)
                    recall = true_positive / len(gt_break_points)

                    f1 = 2*precision * recall / (precision + recall)

            if f1 > best_f1:
                best_f1 = f1
                max_fp = 2*len(gt_break_points) / best_f1 - 2*len(gt_break_points)
                best_index = i_threshold
            else:
                if false_positive > max_fp:
                    break

        best_threshold = (threshold_values[best_index] + threshold_values[best_index+1])/2
        return 1 - best_f1, best_threshold

    def __call__(self, score: np.array, gt_break_points: List[int]) -> float:
        """
        Calculates the F1 loss for a given set of scores and ground truth break points.

        Args:
            score (np.array): The array of scores.
            gt_break_points (List[int]): The list of ground truth break points.

        Returns:
            float: The F1 loss value.
        """
        f1, _ = self._compute_best_f1(score, gt_break_points)
        return f1

    def get_optimal_threshold(self, score: np.array, gt_break_points: List[int]) -> float:
        """
        Calculates the optimal threshold value for a given set of scores and ground truth break points.

        Args:
            score (np.array): The array of scores.
            gt_break_points (List[int]): The list of ground truth break points.

        Returns:
            float: The optimal threshold value.
        """
        _, threshold = self._compute_best_f1(score, gt_break_points)
        return threshold
    
    def get_minimum_value(self) -> float:
        """
        Returns the minimum possible value for the F1 loss.

        Returns:
            float: The minimum value for the F1 loss.
        """
        return 0


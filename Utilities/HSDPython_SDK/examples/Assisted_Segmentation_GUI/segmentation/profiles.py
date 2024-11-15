
"""
This module contains classes for computing score profiles. A score profile is a sequence of scores that are assigned to each sample in a datastream. These scores are used to identify significant local maxima, or peaks, which are indicative of breakpoints in the datastream. The module provides a base class `ScoreProfile` that defines the interface for computing score profiles. 

Classes:
    ScoreProfile: Abstract base class for score profiles.
    NormalDivergenceProfile: Represents a profile for computing the normal divergence of a stream of samples.
    ProminenceProfile: Represents a profile for computing the prominence of peaks in a score profile.
    AverageScoreProfile: Represents a profile for computing the average of multiple score profiles.
    PowerTransformProfile: Represents a profile for applying a power transformation to a score profile.
    ResampleProfile: Represents a profile for resampling a score profile.
    SigmoidProfile: Represents a profile for applying a sigmoid function to a score profile.

ScoreProfile:
    The `ScoreProfile` class is designed to assign scores to each sample in a datastream. These scores are used to identify significant local maxima, or peaks, which are indicative of breakpoints in the datastream. Concrete subclasses must implement the `get_profile` method to define the specific scoring strategy.

InputProfile:
    The `InputProfile` class represents a profile that wraps a numpy array as a score profile. The input samples are returned as the score profile.

NormalDivergenceProfile:
    The `NormalDivergenceProfile` class represents a profile for computing the normal divergence of a stream of samples. The divergence is computed using a sliding window of a specified size.

ProminenceProfile:
    The `ProminenceProfile` class represents a profile for computing the prominence of peaks in a score profile. The prominence of a peak is a measure of how much the peak stands out from the surrounding samples and is computed using the `peak_prominences` function from `scipy.signal`.

AverageScoreProfile:
    The `AverageScoreProfile` class represents a profile for computing the average of multiple score profiles. The average profile is computed as a weighted sum of the input profiles.

PowerTransformProfile:
    The `PowerTransformProfile` class represents a profile for applying a power transformation to a score profile. The power transformation is defined as f(x) = (x + 1)^a - 1, where a is a coefficient.

ResampleProfile:
    The `ResampleProfile` class represents a profile for resampling a score profile. The resampling is performed using linear interpolation to a specified number of target samples.

SigmoidProfile:
    The `SigmoidProfile` class represents a profile for applying a sigmoid function to a score profile. The sigmoid function is defined as f(x) = 2 / (1 + exp(-a*x)) - 1, where a is a coefficient.
"""

from abc import ABC, abstractmethod
from typing import List, Union
import numpy as np
from scipy.signal import peak_prominences
from scipy.signal import resample
from .utils import MovingCovarianceRatio


# FIX: ricalcolare i profili solo quando il profile di input e' cambiato. Mi conviene fare un
# metodo is_changed() che controlla se il profilo stesso o quello in input e' cambiato.
# in questo modo posso propagare a cascata i cambiamenti

class ScoreProfile(ABC):
    """
    Abstract base class for score profiles.

    The `ScoreProfile` class is designed to assign scores to each sample in a datastream.
    These scores are used to identify significant local maxima, or peaks, which are indicative
    of breakpoints in the datastream. Concrete subclasses must implement the `get_profile`
    method to define the specific scoring strategy.

    """

    @abstractmethod
    def get_profile(self) -> np.array:
        """
        Abstract method to get the score profile.

        Returns:
            np.array: The score profile.
        """

    def get_threshold(self, num_peaks: int) -> float:
        """
        Calculates the threshold value based on the score profile.

        Args:
            num_peaks (int): The number of peaks to consider.

        Returns:
            float: The calculated threshold value.
        """
        # indices of top-(N+1) elements
        profile = self.get_profile()
        topN = np.argpartition(profile, -(num_peaks+1))[-(num_peaks+1):]
        topN = topN[np.argsort(profile[topN])]
        # threshold = mid-point between N and N+1
        threshold = .5 * (profile[topN[0]] + profile[topN[1]])
        return threshold
    
    def get_peaks(self, threshold: float) -> List[int]:
        """
        Finds the indices of peaks in the score profile that are above the given threshold.

        Args:
            threshold (float): The threshold value.

        Returns:
            List[int]: The indices of the peaks.
        """
        profile = self.get_profile()
        return np.where(profile >= threshold)[0]


class InputProfile(ScoreProfile):
    """
    Represents a profile that wraps a numpy array as a score profile.

    Attributes:
        samples (np.array): The input samples.

    Methods:
        get_profile(): Returns the input samples as a score profile.

    """
    def __init__(self, samples: np.array):
        """
        Initializes the InputProfile with a numpy array of samples.

        Args:
            samples (np.array): The input samples.
        """
        self.samples = samples

    def get_profile(self) -> np.array:
        """
        Returns the input samples as a score profile.

        Returns:
            np.array: The input samples.
        """
        return self.samples


class NormalDivergenceProfile(ScoreProfile):
    """
    Represents a profile for computing the normal divergence of a stream of samples. The divergence is computed using a sliding window of a specified size and is based on the moving covariance ratio defined in `segmentation.utils`.

    Attributes:
        stream (np.array): The stream of samples.
        window_size (int): The size of the sliding window used for computing the divergence.
        divergence (np.array): The computed divergence profile.

    Methods:
        get_profile(): Computes and returns the divergence profile.

    """

    def __init__(self, stream: Union[ScoreProfile, np.array], window_size: int):
        """
        Initializes the NormalDivergenceProfile with a stream of samples and a window size for computing the divergence.

        Args:
            stream (np.array): The stream of samples.
            window_size (int): The size of the sliding window used for computing the divergence.
        """
        if len(stream.shape) == 1:
            stream = stream[:, None]
        self.stream = stream
        self.window_size = window_size
        self.divergence = None
    
    # REFACTOR: replace the stream field with input_profile to make it coeherent with the other ScoreProfile classes
    @property
    def input_profile(self):
        return self.stream
    
    def get_profile(self) -> np.array:
        """
        Computes and returns the divergence profile.

        Returns:
            np.array: The computed divergence profile.

        """
        if self.divergence is None:
            window_size = self.window_size
            samples = self.stream

            # initialization
            moving_cov_ratio = MovingCovarianceRatio(samples[:window_size].copy().T)
            divergence = np.zeros(self.stream.shape[0])
            divergence[:window_size // 2] = moving_cov_ratio.get_divergence()

            # score computation
            for idx, sample in enumerate(samples[window_size:], start=window_size//2 + 1):
                moving_cov_ratio.update(sample)
                divergence[idx] = moving_cov_ratio.get_divergence()

            # finalization
            divergence[-(window_size//2):] = moving_cov_ratio.get_divergence()

            # save results
            self.divergence = divergence
        
        return self.divergence
     

class ProminenceProfile(ScoreProfile):
    """
    Represents a profile for computing the prominence of peaks in a score profile. The prominence of a peak is a measure of how much the peak stands out from the surrounding samples and is computed using the `peak_prominences` function from `scipy.signal`.

    Attributes:
        input_profile (ScoreProfile): The input score profile.

    Methods:
        get_profile(): Computes and returns the prominence profile.

    """
    def __init__(self, input_profile: ScoreProfile):
        """
        Initializes the ProminenceProfile with an input ScoreProfile.

        Args:
            input_profile (ScoreProfile): An instance of ScoreProfile to compute the prominence profile.
        """

        self.input_profile = input_profile
        self.prominence = None
    
    def get_profile(self) -> np.array:
        """ 
        Computes and returns the prominence profile.

        Returns:
            np.array: The computed prominence profile.
        """
        samples = self.input_profile.get_profile()
        self.prominence, _, _ = peak_prominences(x=samples, peaks=np.arange(samples.shape[0]))    
        return self.prominence


class AverageScoreProfile(ScoreProfile):
    """ 
    Represents a profile for computing the average of multiple score profiles. The average profile is computed as a weighted sum of the input profiles.

    Attributes:
        input_profile_list (List[ScoreProfile]): A list of input score profiles.
        weights (List[float]): The weights used for computing the average profile.

    Methods:
        get_profile(): Computes and returns the average profile.
    """
    def __init__(self, input_profile_list: List[ScoreProfile], weights):
        """ 
        Initializes the AverageScoreProfile with a list of input ScoreProfiles and weights for computing the average profile.

        Args:
            input_profile_list (List[ScoreProfile]): A list of input ScoreProfiles to be averaged.
            weights (List[float]): The weights used for computing the average profile.
        """
        self.input_profile_list = input_profile_list
        self.average_profile = None
        self.weights = weights
    
    def get_profile(self) -> np.array:
        """ 
        Computes and returns the average profile.

        Returns:
            np.array: The computed average profile.
        """
        self.average_profile = None
        for weight, input_profile in zip(self.weights,self.input_profile_list):
            if self.average_profile is None:
                self.average_profile = input_profile.get_profile() * weight
            else:
                self.average_profile += input_profile.get_profile() * weight
            
        self.average_profile = self.average_profile / np.sum(self.weights)
        return self.average_profile


class PowerTransformProfile(ScoreProfile):
    """
    Represents a profile for applying a power transformation to a score profile. The power transformation is defined as f(x) = (x + 1)^a - 1, where a is a coefficient.

    Attributes:
        input_profile (ScoreProfile): The input score profile.
        coeff (float): The coefficient used in the power transformation.

    Methods:
        get_profile(): Computes and returns the power-transformed profile.

    """
    def __init__(self, input_profile: ScoreProfile, coeff: float = 1):
        """
        Initializes the PowerTransformProfile with an input ScoreProfile and a coefficient for the power transformation.
        
        Args:
            input_profile (ScoreProfile): An instance of ScoreProfile to be transformed.
            coeff (float, optional): The coefficient used in the power transformation. Defaults to 1.
        """
        self.input_profile = input_profile
        self._coeff = coeff

    @property
    def coeff(self) -> float:
        """ 
        Returns the coefficient used in the power transformation.

        Returns:
            float: The coefficient used in the power transformation.
        """
        return self._coeff
    
    @coeff.setter 
    def coeff(self, value: float):
        """ 
        Sets the coefficient used in the power transformation.

        Args:
            value (float): The new coefficient value.
        """
        self._coeff = value
    
    def get_profile(self) -> np.array:
        """
        Computes and returns the power-transformed profile.

        Returns:
            np.array: The computed power-transformed profile.
        """
        return self.compute(self.input_profile.get_profile())

    # DOC: PowerTransformProfile.compute
    def compute(self, samples):
        return (samples + 1) ** self.coeff - 1


class ResampleProfile(ScoreProfile):
    """
    Represents a profile for resampling a score profile. The resampling is performed using linear interpolation to a specified number of target samples.

    Attributes:
        input_profile (ScoreProfile): The input score profile.
        n_target_samples (int): The number of target samples for resampling.

    Methods:
        get_profile(): Computes and returns the resampled profile.

    """
    def __init__(self, input_profile: ScoreProfile, n_target_samples: int):
        """
        Initializes the ResampleProfile with an input ScoreProfile and a number of target samples for resampling.

        Args:
            input_profile (ScoreProfile): An instance of ScoreProfile to be resampled.
            n_target_samples (int): The number of target samples for resampling.
        """
        self.input_profile = input_profile
        self.n_target_samples = n_target_samples

    def get_profile(self):
        """
        Computes and returns the resampled profile.

        Returns:
            np.array: The computed resampled profile.
        """
        return resample(self.input_profile.get_profile(), self.n_target_samples)


class SigmoidProfile(ScoreProfile):
    """
    Represents a profile for applying a sigmoid function to a score profile. The sigmoid function is defined as f(x) = 2 / (1 + exp(-a*x)) - 1, where a is a coefficient.

    Attributes:
        input_profile (ScoreProfile): The input score profile.
        a (float): The coefficient used in the sigmoid function.

    Methods:
        get_profile(): Computes and returns the sigmoid-transformed profile.

    """

    def __init__(self, input_profile: ScoreProfile, a: float = 1):
        """
        Initializes the SigmoidProfile with an input ScoreProfile and a coefficient for the sigmoid function.

        Args:
            input_profile (ScoreProfile): An instance of ScoreProfile to be transformed.
            a (float, optional): The coefficient used in the sigmoid function. Defaults to 1.
        """    
        self.input_profile = input_profile
        self._a = a
        self.profile = None
    
    @property
    def a(self):
        """ 
        Returns the coefficient used in the sigmoid function.

        Returns:
            float: The coefficient used in the sigmoid function.
        """
        return self._a
    
    @a.setter 
    def a(self, value):
        """
        Sets the coefficient used in the sigmoid function.

        Args:
            value (float): The new coefficient value.
        """
        if self._a != value:
            self._a = value
    
    def get_profile(self):
        """ 
        Computes and returns the sigmoid-transformed profile.

        Returns:
            np.array: The computed sigmoid-transformed profile.
        """
        return  2 / (1 + np.exp(-self.a*self.input_profile.get_profile())) - 1


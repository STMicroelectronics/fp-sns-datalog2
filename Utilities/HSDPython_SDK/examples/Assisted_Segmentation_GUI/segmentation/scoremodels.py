
from typing import List, Tuple
import numpy as np
import pywt
from .profiles import NormalDivergenceProfile, ProminenceProfile, AverageScoreProfile
from .profiles import PowerTransformProfile, ResampleProfile, InputProfile, ScoreProfile
from .utils import UnionOfIntervals
from .segmenters import ScoreModel


# REFACTOR: dentro lo score model e' necessario usare il metodo ScoreProfile.compute() per calcolare il profilo. Conviene renderlo disponibile nella classe astratta ScoreProfile e rivedere l'implementazione dei vari profili?
# REFACTOR: attenziona a accedere a profile, nel momento in cui voglio passare a un'implementazione a chuncks
class WaveletDecompositionModel(ScoreModel):
    """
    A class representing a wavelet decomposition model for segmentation. 
    It performs wavelet decomposition on the input samples using the specified 
    level and wavelet type (db3). For each subband obtained from the decomposition, 
    it calculates a profile using the NormalDivergenceProfile, resamples it, 
    applies a power transform, and stores it. It then computes an average score 
    profile across all subbands and sets this as both the average and the score 
    profile of the model. Initial weights (that correspond to the exponents of
    the power transforms are set to 1 for each subband).

    Attributes:
        samples (np.array): The input samples for the model.
        level_wavelet (int): The level of wavelet decomposition.
        windows_size (int): The size of the windows used for profile calculations.
        average_profile (AverageScoreProfile): The average score profile of all subbands.
        score_profile (ProminenceProfile): The score profile of the model.
        _weights (List[float]): The weights assigned to each subband profile.
        _constraints (List[Tuple[float, float]]): The constraints for the weights.

    Methods:
        num_samples() -> int: Returns the number of samples.
        weights() -> List[float]: Returns the weights assigned to each subband profile.
        weights(w: List[float]): Sets the weights for each subband profile.
        get_score() -> np.array: Returns the score profile of the model.
        get_weights_constraints() -> List[Tuple[float, float]]: Returns the constraints for the weights.
    """

    def __init__(self, samples: np.array, level_wavelet: int = 7, windows_size: int = 100):
        """
        Initializes a new instance of the WaveletDecompositionModel class.

        Args:
            samples (np.array): The input samples for the model.
            level_wavelet (int, optional): The level of wavelet decomposition. Defaults to 7.
            windows_size (int, optional): The size of the windows used for profile calculations. Defaults to 100.
        """
        self.samples = samples
        self.window_size = windows_size

        dec = pywt.wavedec(samples, wavelet='db3', level=level_wavelet, axis=0)
        N = samples.shape[0]

        core_profiles = []
        weights = []
        constraints = []
        for _, subband in enumerate(dec):
            profile = NormalDivergenceProfile(subband, windows_size)
            profile = ResampleProfile(profile, N)
            core_profiles.append(profile)
            weights.append(1)
            constraints.append((0.5, 2))
            
        self._weights = weights
        self._constraints = constraints

        self.core_profiles = core_profiles
        self.enhanced_profiles = self._enhance_profiles()
        self.score_profile = self._final_processing()

        self.max_windows = 20
        self.stored_windows = []
        self.prune_factor = 10
    
    def _enhance_profiles(self, core_profiles: List[InputProfile] = None) -> List[ScoreProfile]:
        if not core_profiles:
            core_profiles = self.core_profiles
        enhanced_profiles = []
        for core_profile, w in zip(core_profiles, self.weights):
            profile = PowerTransformProfile(core_profile, coeff=w)
            enhanced_profiles.append(profile)
        return enhanced_profiles
    
    def _final_processing(self, enhanced_profiles: List[ScoreProfile] = None) -> ScoreProfile:
        if not enhanced_profiles:
            enhanced_profiles = self.enhanced_profiles 
        profile = AverageScoreProfile(enhanced_profiles, np.ones(len(enhanced_profiles)))
        profile = ProminenceProfile(profile)
        return profile

    @property
    def num_samples(self) -> int:
        """
        Returns the number of samples.

        Returns:
            int: The number of samples.
        """
        return self.samples.shape[0]

    @property
    def weights(self) -> List[float]:
        """
        Returns the weights assigned to each subband profile.

        Returns:
            List[float]: The weights assigned to each subband profile.
        """
        return self._weights
    @weights.setter
    def weights(self, w: List[float]):
        """
        Sets the weights for each subband profile.

        Args:
            w (List[float]): The weights to be assigned to each subband profile.
        """
        self._weights = w
        for ww, profile in zip(self._weights, self.enhanced_profiles):
            profile.coeff = ww

    @property
    def weights_constraints(self) -> List[Tuple[float, float]]:
        """
        Returns the constraints for the weights.

        Returns:
            List[Tuple[float, float]]: The constraints for the weights.
        """
        return self._constraints

    def get_score(self) -> np.array:
        """
        Returns the score profile of the model.

        Returns:
            np.array: The score profile of the model.
        """
        return self.score_profile.get_profile()


    # DOC WaveletDecompositionModel.store_window
    # FIX: da aggiungere pezzi di finestra prima e dopo start e end per non falsare il calcolo della prominence
    def store_window(self, start: int, end: int):

        # estraggo solo i sample che mi interessano dai core_profiles
        # FIX: qua conviene usare un'api dedicata invece di prendere tutti i sample e poi estrarre solo quelli che mi interessano
        core_profiles_window = [profile.get_profile()[start:end] for profile in self.core_profiles]
        self.stored_windows.append((start, core_profiles_window))

        self._resolve_overlaps()
        self._prune_windows()

        # num_stored_windows = len(self.stored_windows)
        # num_stored_samples = sum([len(w[0]) for _, w in self.stored_windows])

    # DOC
    # ONGOING: c'e' qualcosa che non va in questo metodo, bisogna capire cosa
    def _prune_windows(self):
        intervals_with_score = []            
        for idx_window, (start, core_profiles_window) in enumerate(self.stored_windows):
            tmp_core_profiles = [InputProfile(p) for p in core_profiles_window]
            score = self._final_processing(self._enhance_profiles(tmp_core_profiles)).get_profile()

            idx = np.argsort(score)
            idx = idx[::-1]
            idx = idx[:min(self.max_windows, len(idx))]

            for i in idx:
                start = max(0, i - self.prune_factor * self.window_size)
                end = min(len(score), i + self.prune_factor * self.window_size)
                intervals_with_score.append((idx_window, start, end, score[i]))

        intervals_with_score = sorted(intervals_with_score, key=lambda x: x[3], reverse=True)
        intervals_with_score = intervals_with_score[:min(self.max_windows, len(intervals_with_score))]
        new_stored_windows = []
        for idx, (_, core_profiles_window) in enumerate(self.stored_windows):
            union_of_intervals = UnionOfIntervals(len(core_profiles_window[0]))
            for i, (idx_window, start, end, _) in enumerate(intervals_with_score):
                if idx_window == idx:
                    union_of_intervals.add_interval([start, end])
            for interval in union_of_intervals.intervals:
                start, end = interval
                new_core_profiles_window = [w[start:end] for w in core_profiles_window]
                new_stored_windows.append((start, new_core_profiles_window))
        self.stored_windows = new_stored_windows
            
    # DOC
    # REFACTOR: la parte di codice che risolve gli overlap tra finestre e' la stessa che e' presente in SupervisedDomain. Da capire il modo migliore per riutilizzarla
    def _resolve_overlaps(self):
        self.stored_windows.sort(key=lambda x: x[0])
        new_stored_windows = []
        for start, core_profiles_window in self.stored_windows:
            end = start + len(core_profiles_window[0])
            if not new_stored_windows:
                new_stored_windows.append((start, core_profiles_window))
            else:
                last_start, last_core_profiles_window = new_stored_windows[-1]
                last_end = last_start + len(last_core_profiles_window[0])
                if start <= last_end:
                    if last_end < end:                        
                        for i, (w, lw) in enumerate(zip(core_profiles_window, last_core_profiles_window)):
                            lw = np.concatenate([lw, w[last_end-start:]])
                            new_stored_windows[-1][1][i] = lw
                else:
                    new_stored_windows.append((start, core_profiles_window))
        self.stored_windows = new_stored_windows    

    # DOC: WaveletDecompositionModel.get_windows
    def get_windows(self):
        windows = []
        for start, core_profiles_window in self.stored_windows:
            tmp_core_profiles = [InputProfile(p) for p in core_profiles_window]
            score = self._final_processing(self._enhance_profiles(tmp_core_profiles)).get_profile()
            windows.append((start, score))
        return windows
  


import numpy as np
from skopt import forest_minimize
from .segmenters import Loss, Optimizer
from .segmenters import Segmenter
import mango
from scipy.stats import uniform



class BayesianOptimizer(Optimizer):
    """
    BayesianOptimizer is a class that performs Bayesian optimization to find the optimal weights for a segmenter.

    Args:
        max_calls (int): The maximum number of function calls to be made during optimization. Default is 100.
        n_initial_points (int): The number of initial points to be used for optimization. Default is 10.
        verbose (bool): Whether to print verbose output during optimization. Default is True.
    """

    def __init__(self, max_calls: int = 100, n_initial_points: int = 10, verbose: bool = True):
        self.loss: Loss = None
        self.segmenter: Segmenter = None
        self.max_calls = max_calls
        self.stored_points = []
        self.n_initial_points = n_initial_points
        self.verbose = verbose

    def set_segmenter(self, segmenter: Segmenter):
        """
        Sets the segmenter to be used for optimization.

        Args:
            segmenter (Segmenter): The segmenter object to be used for optimization.
        """
        self.segmenter = segmenter
        self.stored_points = [self.segmenter.weights]

    def set_loss(self, loss: callable, minimum_loss_value: float = -np.inf):
        """
        Sets the loss function to be minimized during optimization.

        Args:
            loss (callable): The loss function to be minimized.
            minimum_loss_value (float): The minimum value of the loss function. Default is -inf.
        """
        self.loss = loss

    def update(self):
        """
        Performs Bayesian optimization to find the optimal weights for the segmenter.

        Returns:
            The optimal weights for the segmenter.
        """
        res = forest_minimize(self.loss,
                              self.segmenter.weights_constraints,
                              n_calls=self.max_calls,
                              verbose=self.verbose,
                              x0=self.stored_points,
                              )
        all_x = res.x_iters
        all_y = res.func_vals
        _, stored_points = zip(*sorted(zip(all_y, all_x), key=lambda t: t[0]))
        self.stored_points = [p for p in stored_points[:self.n_initial_points]]

        return res.x


class MangoOptimizer(Optimizer):
    """
    The MangoOptimizer class is used to optimize the weights of a segmenter using the Mango optimization algorithm.

    Attributes:
        loss (Loss): The loss function to be minimized.
        segmenter (Segmenter): The segmenter whose weights are being optimized.
        params_space (dict): A dictionary representing the parameter space for optimization.
        params_names (list): A list of parameter names.
        loss_to_adapt (callable): The loss function to be adapted for optimization.
        max_calls (int): The maximum number of function calls to be made during optimization.
        stored_points (list): A list of stored points for updating the optimizer.
        previuous_loss_value (float): The previous loss value.
        max_stored_points (int): The maximum number of stored points.

    Methods:
        update(): Updates the optimizer and returns the optimized weights.
        set_segmenter(segmenter: Segmenter): Sets the segmenter for optimization.
        set_loss(loss: callable, minimum_loss_value: float = -np.inf): Sets the loss function for optimization.
        objective(params_batch): Computes the objective values for a batch of parameter sets.
        early_stopping(results): Determines if early stopping criteria is met.

    """

    def __init__(self, max_calls=20):
        """
        Initializes a new instance of the MangoOptimizer class.

        Args:
            max_calls (int): The maximum number of function calls to be made during optimization. Default is 20.
        """
        self.loss: Loss = None
        self.segmenter: Segmenter = None
        self.params_space = dict()
        self.params_names = []
        self.loss_to_adapt = None
        self.max_calls = max_calls
        self.stored_points = []
        self.previuous_loss_value = None
        self.max_stored_points = max_calls // 4

    def update(self) -> np.array:
        """
        Updates the optimizer and returns the optimized weights.

        Returns:
            np.array: The optimized weights.

        """
        conf_dict = {
            'domain_size': 500,
            'num_iteration': self.max_calls,
            'initial_custom': self.stored_points,
            'early_stopping': self.early_stopping,
        }
        tuner = mango.Tuner(self.params_space, self.loss, conf_dict)
        res = tuner.minimize()

        self.previuous_loss_value = res['best_objective']

        _, stored_points = zip(*sorted(zip(res['objective_values'], res['params_tried']), key=lambda t: t[0]))
        stored_points = [p for p in stored_points]
        self.stored_points = stored_points[:min(self.max_stored_points, len(stored_points))]
        weights = []
        for k in self.params_names:
            weights.append(res['best_params'][k])
        return weights

    def set_segmenter(self, segmenter: Segmenter):
        """
        Sets the segmenter for optimization.

        Args:
            segmenter (Segmenter): The segmenter to be optimized.

        """
        self.segmenter = segmenter

        for i, c in enumerate(segmenter.weights_constraints):
            name = f'w_{i}'
            self.params_names.append(name)
            self.params_space[name] = uniform(c[0], c[1]-c[0])
        
        params = dict()
        for name, w in zip(self.params_names, segmenter.weights):
            params[name] = w
        self.stored_points = [params]

    def set_loss(self, loss: callable, minimum_loss_value: float = -np.inf):
        """
        Sets the loss function for optimization.

        Args:
            loss (callable): The loss function to be optimized.
            minimum_loss_value (float, optional): The minimum loss value. Defaults to -np.inf.

        """
        self.loss_to_adapt = loss
        self.loss = self.objective
        self.previuous_loss_value = minimum_loss_value
    
    def objective(self, params_batch):
        """
        Computes the objective values for a batch of parameter sets.

        Args:
            params_batch: A batch of parameter sets.

        Returns:
            list: The objective values for the parameter sets.

        """
        values = []
        for params in params_batch:
            weights = [params[name] for name in self.params_names]
            values.append(self.loss_to_adapt(weights))
        return values
    
    def early_stopping(self, results):
        """
        Determines if early stopping criteria is met.

        Args:
            results: The optimization results.

        Returns:
            bool: True if early stopping criteria is met, False otherwise.

        """
        return results['best_objective'] <= self.previuous_loss_value



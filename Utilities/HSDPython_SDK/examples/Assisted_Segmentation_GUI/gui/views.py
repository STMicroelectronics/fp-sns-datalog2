
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
import pyqtgraph as pg
from PySide6.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QFrame, QSlider, QPushButton, QFileDialog, QLineEdit, QButtonGroup, QDialog, QLabel
from PySide6.QtCore import Qt
from PySide6.QtUiTools import QUiLoader
import numpy as np

import Assisted_Segmentation_GUI
from Assisted_Segmentation_GUI.gui.Ui_MainWindow import Ui_MainWindow
from Assisted_Segmentation_GUI.gui.Widgets.ComponentWidget import ComponentWidget
from Assisted_Segmentation_GUI.gui.Widgets.dialogs import TagsAlertDialog, WaitingDialog
from Assisted_Segmentation_GUI.gui.ui_controller import UI_Controller
from st_hsdatalog.HSD.utils.type_conversion import TypeConversion

class DatastreamCanvas(QWidget):
    def __init__(self, view: 'AssistedSegmentationWindow', parent=None):
        """
        Initialize the DatastreamCanvas.

        Args:
            view (AssistedSegmentationWindow): The parent view containing the data and settings.
            parent (QWidget, optional): The parent widget. Defaults to None.
        """
        super().__init__(parent)
        self.view = view        
        # Create a layout for the widget
        layout = QVBoxLayout()
        self.graph_curves = {}
        # Create a pyqtgraph plot widget
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setMouseEnabled(x=True, y=False) # Enable mouse interaction only in the x direction
        self.plot_widget.showGrid(x=True, y=True) # Show grid lines
        self.plot_widget.hideAxis('left') # Hide the left axis

        # Plot each axis as a separate curve
        samples = self.view.samples[self.view.start_plot:self.view.end_plot, :]
        for i in range(samples.shape[1]):
            self.graph_curves[i] = self.plot_widget.plot() # Initialize the plot curve
            self.graph_curves[i] = pg.PlotDataItem(pen=({'color': self.view.lines_colors[i - (len(self.view.lines_colors)* int(i / len(self.view.lines_colors)))], 'width': 1}), skipFiniteCheck=True, ignoreBounds=True) # Create the curve, then set the pen color and width
            self.plot_widget.addItem(self.graph_curves[i]) # Add the curve to the plot widget

        # Add a green vertical line to indicate the x pointer position
        self.x_pointer_line = pg.InfiniteLine(pos=self.view.x_pointer, angle=90, pen=pg.mkPen({"color": "#00FF00", "width": 3}))
        self.plot_widget.addItem(self.x_pointer_line)

        # Disable mouse interaction for the plot view box
        self.plot_widget.plotItem.getViewBox().setMouseEnabled(x=False, y=False)
        self.plot_widget.plotItem.setMenuEnabled(False) # Disable the context menu
        self.plot_widget.plotItem.hideButtons() # Hide the buttons on the plot

        # Add a linear region to indicate the focus area
        self.focus_region = pg.LinearRegionItem([self.view.start_window, self.view.end_window], brush=(255, 255, 0, 30))
        self.focus_region.sigRegionChanged.connect(self.focus_region_changed) # Connect the region changed signal to a callback
        # Disable resizing by setting the handles' movable attribute to False
        for handle in self.focus_region.lines:
            handle.setMovable(False)
        self.plot_widget.addItem(self.focus_region)

        # Add the plot widget to the layout
        layout.addWidget(self.plot_widget)
        self.setLayout(layout) # Set the layout for the QWidget
    
    def update_plot(self):
        """
        Update the plot with the latest data.
        """
        tt = np.arange(self.view.start_plot, self.view.end_plot)
        samples = self.view.samples[self.view.start_plot:self.view.end_plot, :]
        for i in self.graph_curves:
            self.graph_curves[i].setData(tt, samples[:, i]) # Update the data for each curve
            # Remove padding by setting the range manually
        
        self.focus_region.setRegion([self.view.start_window, self.view.end_window]) # Update the focus region
        self.plot_widget.setXRange(self.view.start_plot, self.view.end_plot, padding=0) # Set the x range without padding  
    
    def focus_region_changed(self, region):
        """
        Callback function for when the focus region is changed.

        Args:
            region (pg.LinearRegionItem): The linear region item representing the focus area.
        """
        self.view.start_window = int(region.getRegion()[0])
        self.view.window_size = int(region.getRegion()[1]) - int(region.getRegion()[0])
        self.enforce_focus_region_boundaries(region) # Enforce the boundaries of the focus region
        self.view.score_canvas.update_focus_region() # Update the focus region in the score canvas
        self.view.focus_window_canvas.update_plot() # Update the plot in the focus window canvas

    def enforce_focus_region_boundaries(self, region: pg.LinearRegionItem):
        """
        Enforce the boundaries of the focus region.

        Args:
            region (pg.LinearRegionItem): The linear region item representing the focus area.
        """
        # Get the current region boundaries
        min_val, max_val = region.getRegion()
        width = max_val - min_val
        # Enforce the boundaries
        if min_val < 0 or max_val < min_val + width:
            min_val = 0
            max_val = min_val + width
        if min_val > self.view.nsamples - width or max_val > self.view.nsamples:
            max_val = self.view.nsamples
            min_val = max_val - width
        # Update the region with the adjusted boundaries
        region.setRegion([min_val, max_val])

    def update_x_pointer(self):
        """
        Update the position of the x pointer line.
        """
        if self.view.x_pointer:
            self.x_pointer_line.setPos(self.view.x_pointer) # Update the position of the vertical line

    def update_focus_region(self):
        """
        Update the focus region to match the current start and end window.
        """
        self.focus_region.setRegion([self.view.start_window, self.view.end_window])

class DatastreamZoomSlider:
    def __init__(self, view: 'AssistedSegmentationWindow', slider: QSlider):
        """
        Initialize the DatastreamZoomSlider.
        
        Args:
            view (AssistedSegmentationWindow): The parent view containing the data and settings.
            slider (QSlider): The slider widget.
        """
        self.view = view
        self.slider = slider

    def build(self):
        """
        Build the zoom slider.
        """
        self.slider.setRange(self.view.MIN_ZOOM, self.view.MAX_ZOOM) # Set the range of the slider
        self.slider.setValue(self.view.zoom_length) # Set the initial value of the slider
        self.slider.valueChanged.connect(self.view.update_zoom_slider_position) # Connect the value changed signal to a callback

class DatastreamScrollSlider:
    def __init__(self, view: 'AssistedSegmentationWindow', slider: QSlider):
        """
        Initialize the DatastreamScrollSlider.

        Args:
            view (AssistedSegmentationWindow): The parent view containing the data and settings.
            parent (QWidget, optional): The parent widget. Defaults to None.
        """
        self.view = view
        self.slider = slider
    
    def build(self):
        """
        Build the scroll slider.
        """
        self.slider.setRange(0, self.view.nsamples - self.view.zoom_length) # Set the range of the slider
        self.slider.setValue(self.view.start_plot) # Set the initial value of the slider
        self.slider.valueChanged.connect(self.view.update_scroll_slider_position) # Connect the value changed signal to a callback

class ScoreCanvas(QWidget):
    def __init__(self, view: 'AssistedSegmentationWindow', parent=None):
        """
        Initialize the ScoreCanvas.

        Args:
            view (AssistedSegmentationWindow): The parent view containing the data and settings.
            parent (QWidget, optional): The parent widget. Defaults to None.
        """
        super().__init__(parent)
        self.view = view
        # Create a layout for the widget
        layout = QVBoxLayout()
        # Create a pyqtgraph plot widget
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setMouseEnabled(x=True, y=False)
        self.plot_widget.showGrid(x=True, y=True)
        self.plot_widget.hideAxis('left')
        # Initialize the plot curve
        self.graph_curve = self.plot_widget.plot()
        self.graph_curve = pg.PlotDataItem(pen=({'color': self.view.lines_colors[0], 'width': 1}), skipFiniteCheck=True, ignoreBounds=True) # Create the curve, then set the pen color and width
        self.plot_widget.addItem(self.graph_curve) # Add the curve to the plot widget
        # Add a green vertical line to indicate the x pointer position
        self.x_pointer_line = pg.InfiniteLine(pos=self.view.x_pointer, angle=90, pen=pg.mkPen({"color": "#00FF00", "width": 3}))
        self.plot_widget.addItem(self.x_pointer_line) # Add the vertical line to the plot
        # Add horizontal line for threshold
        self.hline = pg.InfiniteLine(pos=self.view.controller.get_segmenter_threshold(), angle=0, pen=pg.mkPen('r', style=pg.QtCore.Qt.DashLine))
        self.plot_widget.addItem(self.hline)
        # Disable mouse interaction for the plot view box
        self.plot_widget.plotItem.getViewBox().setMouseEnabled(x=False, y=False)
        self.plot_widget.plotItem.setMenuEnabled(False) # Disable the context menu
        self.plot_widget.plotItem.hideButtons() # Hide the buttons on the plot
        # Add a linear region to indicate the focus area
        self.focus_region = pg.LinearRegionItem([self.view.start_window, self.view.end_window], brush=(255, 255, 0, 50))
        self.focus_region.sigRegionChanged.connect(self.focus_region_changed)
        # Disable resizing by setting the handles' movable attribute to False
        for handle in self.focus_region.lines:
            handle.setMovable(False) # Disable the handles
        self.plot_widget.addItem(self.focus_region) # Add the focus region to the plot
        # Add the plot widget to the layout
        layout.addWidget(self.plot_widget)
        # Set the layout for the QWidget
        self.setLayout(layout)
    
    def update_plot(self):
        """
        Update the plot with the latest data.
        """
        tt = np.arange(self.view.start_plot, self.view.end_plot) # Create a time vector
        scores = self.view.scores[self.view.start_plot:self.view.end_plot] # Get the scores
        self.graph_curve.setData(tt, scores) # Update the data for the curve

        self.focus_region.setRegion([self.view.start_window, self.view.end_window]) # Update the focus region
        self.plot_widget.setXRange(self.view.start_plot, self.view.end_plot, padding=0) # Set the x range without padding

        self.hline.setPos(self.view.controller.get_segmenter_threshold()) # Update the threshold line

    def update_x_pointer(self):
        """
        Update the position of the x pointer line.
        """
        if self.view.x_pointer:
            self.x_pointer_line.setPos(self.view.x_pointer) # Update the position of the vertical line

    def focus_region_changed(self, region):
        """
        Callback function for when the focus region is changed.
        
        Args:
            region (pg.LinearRegionItem): The linear region item representing the focus area.
        """
        self.view.start_window = int(region.getRegion()[0]) # Update the start window
        self.view.window_size = int(region.getRegion()[1]) - int(region.getRegion()[0]) # Update the window size
        self.enforce_focus_region_boundaries(region) # Enforce the boundaries of the focus region
        self.view.datastream_canvas.update_focus_region() # Update the focus region in the datastream canvas
        self.view.focus_window_canvas.update_plot() # Update the plot in the focus window canvas

    def update_focus_region(self):
        """
        Update the focus region to match the current start and end window.
        """
        self.focus_region.setRegion([self.view.start_window, self.view.end_window]) # Update the focus region

    def enforce_focus_region_boundaries(self, region: pg.LinearRegionItem):
        """
        Enforce the boundaries of the focus region.
        
        Args:
            region (pg.LinearRegionItem): The linear region item representing the focus area.
        """
        # Get the current region boundaries
        min_val, max_val = region.getRegion()
        width = max_val - min_val
        # Enforce the boundaries
        if min_val < 0 or max_val < min_val + width:
            min_val = 0
            max_val = min_val + width
        if min_val > self.view.nsamples - width or max_val > self.view.nsamples:
            max_val = self.view.nsamples
            min_val = max_val - width
        # Update the region with the adjusted boundaries
        region.setRegion([min_val, max_val])

class FocusWindowCanvas(QWidget):
    def __init__(self, view: 'AssistedSegmentationWindow', parent=None):
        """
        Initialize the FocusWindowCanvas.
        
        Args:
            view (AssistedSegmentationWindow): The parent view containing the data and settings.
            parent (QWidget, optional): The parent widget. Defaults to None.
        """
        super().__init__(parent)
        self.view = view
        # Create a layout for the widget
        layout = QVBoxLayout()
        self.graph_curves = {}
        # Create a pyqtgraph plot widget
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setMouseEnabled(x=True, y=False) # Enable mouse interaction only in the x direction
        self.plot_widget.showGrid(x=True, y=True) # Show grid lines
        self.plot_widget.hideAxis('left') # Hide the left axis
        # Plot each axis as a separate curve
        samples = self.view.samples[self.view.start_window:self.view.end_window, :]
        for i in range(samples.shape[1]):
            self.graph_curves[i] = self.plot_widget.plot() # Initialize the plot curve
            self.graph_curves[i] = pg.PlotDataItem(pen=({'color': self.view.lines_colors[i - (len(self.view.lines_colors)* int(i / len(self.view.lines_colors)))], 'width': 1}), skipFiniteCheck=True, ignoreBounds=True) # Create the curve, then set the pen color and width
            self.plot_widget.addItem(self.graph_curves[i]) # Add the curve to the plot widget
        self.plot_widget.plotItem.getViewBox().setMouseEnabled(x=False, y=False) # Disable mouse interaction for the plot view box
        self.plot_widget.plotItem.setMenuEnabled(False) # Disable the context menu
        self.plot_widget.plotItem.hideButtons() # Hide the buttons on the plot
        # Add a green vertical line to indicate the x pointer position
        self.x_pointer_line = pg.InfiniteLine(pos=self.view.x_pointer, angle=90, pen=pg.mkPen({"color": "#00FF00", "width": 3}))
        # Add the vertical line to the plot
        self.plot_widget.addItem(self.x_pointer_line)
        self.plot_widget.scene().sigMouseClicked.connect(self.on_click) # Connect the mouse clicked signal to a callback
        self.plot_widget.scene().sigMouseMoved.connect(self.on_move) # Connect the mouse moved signal to a callback
        # Add the plot widget to the layout
        layout.addWidget(self.plot_widget)
        self.setLayout(layout) # Set the layout for the QWidget
    
    def update_plot(self):
        """
        Update the plot with the latest data.
        """
        tt = np.arange(self.view.start_window, self.view.end_window) # Create a time vector
        samples = self.view.samples[self.view.start_window:self.view.end_window, :]
        for i in self.graph_curves: # Update the data for each curve
            self.graph_curves[i].setData(tt, samples[:, i]) # Update the data for each curve
        self.plot_widget.setXRange(self.view.start_window, self.view.end_window, padding=0) # Set the x range without padding
    
    def on_move(self, pos):
        """
        Callback function for when the mouse is moved.
        
        Args:
            pos (QPointF): The position of the mouse.
        """
        # Get the mouse position in plot coordinates
        mouse_point = self.plot_widget.plotItem.vb.mapSceneToView(pos)
        self.view.x_pointer = mouse_point.x()
        # Update the position of the vertical line
        self.x_pointer_line.setPos(self.view.x_pointer) # Update the position of the vertical line
        self.view.score_canvas.update_x_pointer() # Update the x pointer in the score canvas
        self.view.datastream_canvas.update_x_pointer() # Update the x pointer in the datastream canvas
    
    def on_click(self, event):
        """
        Callback function for when the mouse is clicked.
        
        Args:
            event (QMouseEvent): The mouse event.
        """
        bkp = self.view.x_pointer # Get the x pointer position
        QApplication.setOverrideCursor(Qt.WaitCursor) # Set the cursor to a wait cursor
        try:
            if event.button() == Qt.LeftButton:
                self.view.controller.add_gt_break_point(bkp) # Add a ground truth break point
            elif event.button() == Qt.RightButton:
                self.view.controller.remove_candidate_break_point(bkp) # Remove a candidate break point
        except Exception as e:
            print(e) # Print the exception
        QApplication.restoreOverrideCursor() # Restore the cursor

        gt_break_points = self.view.controller.get_gt_break_points() # Get the ground truth break points
        candidate_break_points = self.view.controller.get_candidate_break_points() # Get the candidate break points
        
        self.view.clear_break_points() # Clear the break points
        self.view.draw_break_points(self.plot_widget, gt_break_points, candidate_break_points) # Draw the break points
        self.view.draw_break_points(self.view.datastream_canvas.plot_widget, gt_break_points, candidate_break_points) # Draw the break points in the datastream canvas

        if len(candidate_break_points) == 0 and len(gt_break_points) > 1: # If there are no candidate break points and there are more than one ground truth break points
            self.view.tag_acquisition_button.setEnabled(True) # Enable the tag acquisition button
            self.view.tag_acquisition_button.setVisible(True) # Show the tag acquisition button
        else:
            self.view.tag_acquisition_button.setEnabled(False) # Disable the tag acquisition button
            self.view.tag_acquisition_button.setVisible(True) # Show the tag acquisition button
        self.view.tag_acquisition_label.setVisible(False) # Hide the tag acquisition label

class FocusWindowSizeSlider:
    def __init__(self, view: 'AssistedSegmentationWindow', slider: QSlider):
        """
        Initialize the FocusWindowSizeSlider.
        
        Args:
            view (AssistedSegmentationWindow): The parent view containing the data and settings.
            slider (QSlider): The slider widget.
        """
        self.view = view
        self.slider = slider
    
    def build(self):
        """
        Build the focus window size slider.
        """
        self.slider.setRange(self.view.MIN_WINDOW, self.view.max_window_size) # Set the range of the slider
        self.slider.setValue(self.view.window_size) # Set the initial value of the slider
        self.slider.valueChanged.connect(self.view.update_window_size_slider_position) # Connect the value changed signal to a callback

class AssistedSegmentationWindow(QMainWindow):
    def __init__(self, parent=None):
        """
        Initialize the AssistedSegmentationWindow.
        
        Args:
            parent (QWidget, optional): The parent widget. Defaults to None.
        """
        super(AssistedSegmentationWindow, self).__init__(parent)

        # Initialize the UI controller
        self.controller:UI_Controller = UI_Controller()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.lines_colors = ['#e6007e', '#a4c238', '#3cb4e6', '#ef4f4f', '#46b28e', '#e8ce0e', '#60b562', '#f99e20', '#41b3ba']

        # Breakpoints dictionaries
        self.gt_break_points_lines = {} # Ground truth break points
        self.candidate_break_points_lines = {} # Candidate break points

        self.components = None
        self.components_widgets = {}
        self.components_button_group = QButtonGroup(self)
        self.components_button_group.setExclusive(True)

        # Info
        self.menu_btn_info = self.findChild(QPushButton, "menu_btn_info")
        self.menu_btn_info.clicked.connect(self.show_app_info)

        # Settings
        self.acq_folder_button = self.findChild(QPushButton, "acq_folder_button")
        self.acq_folder_button.clicked.connect(self.select_acquisition_folder)
        self.acq_folder_textEdit:QLineEdit = self.findChild(QLineEdit, "acq_folder_textEdit")
        self.components_frame = self.findChild(QFrame, "components_frame")
        self.run_segmentation_button:QPushButton = self.findChild(QPushButton, "run_segmentation_button")
        self.run_segmentation_button.clicked.connect(self.start_segmentation_algorithm)
        self.run_segmentation_button.setEnabled(False)
        self.tag_acquisition_button:QPushButton = self.findChild(QPushButton, "tag_acquisition_button")
        self.tag_acquisition_button.clicked.connect(self.start_acquisition_tagging)
        self.tag_acquisition_button.setEnabled(False)
        self.tag_acquisition_label:QLabel = self.findChild(QLabel, "tag_acquisition_label")
        self.tag_acquisition_label.setVisible(False)
        self.message_label:QLabel = self.findChild(QLabel, "message_label")
        self.message_label.setVisible(False)

        # Datastream Widget
        self.slider_scroll_datastream = self.findChild(QSlider, 'slider_scroll_datastream')
        self.datastream_scroll_slider = DatastreamScrollSlider(self, self.slider_scroll_datastream)
        self.slider_zoom_datastream = self.findChild(QSlider, 'slider_zoom_datastream')
        self.datastream_zoom_slider = DatastreamZoomSlider(self, self.slider_zoom_datastream)
        self.datastream_canvas = None

        # Score Widget
        self.score_canvas = None

        # Focus Window Widget
        self.slider_focus_window_size = self.findChild(QSlider, 'slider_focus_window_size')
        self.focus_window_size_slider = FocusWindowSizeSlider(self, self.slider_focus_window_size)
        self.focus_window_canvas = None

        # Waiting Dialog
        self.waiting_dialog = WaitingDialog("Assisted Segmentation", "Segmentation algorithm is running...", self)

    @property
    def samples(self):
        """
        Get the samples from the controller.
        """
        return self.controller.get_samples()
    
    @property
    def scores(self):
        """
        Get the scores from the controller.
        """
        return self.controller.get_scores()

    @property    
    def nsamples(self):
        """
        Get the number of samples from the controller.
        """
        return self.controller.get_nsamples()

    @property
    def start_plot(self):
        """
        Get the start plot index.
        """
        return self._start_plot
    @start_plot.setter
    def start_plot(self, value):
        """
        Set the start plot index.
        """
        self._start_plot = value

    @property
    def end_plot(self):
        """
        Get the end plot index.
        """
        return min(self.start_plot + self.zoom_length, self.nsamples)

    @property
    def start_window(self):
        """
        Get the start window index.
        """
        return self._start_window
    @start_window.setter
    def start_window(self, value):
        """
        Set the start window index.
        """
        self._start_window = value
    
    @property
    def end_window(self):
        """
        Get the end window index.
        """
        return min(self.start_window + self.window_size, self.nsamples)
        
    @property
    def zoom_length(self):
        """
        Get the zoom length.
        """
        return self._zoom_length
    @zoom_length.setter
    def zoom_length(self, value):
        """
        Set the zoom length.
        """
        self._zoom_length = value

    @property
    def window_size(self):
        """
        Get the window size.
        """
        return self._window_size
    @window_size.setter
    def window_size(self, value):
        """
        Set the window size.
        """
        self._window_size = value

    @property
    def x_pointer(self):
        """
        Get the x pointer position.
        """
        return self._x_pointer
    @x_pointer.setter
    def x_pointer(self, value):
        """
        Set the x pointer position.
        """
        if value:
            self._x_pointer = int(round(value))

    @property
    def max_window_size(self):
        """
        Get the maximum window size.
        """
        return self.zoom_length // 2    

    def show_app_info(self):
        """
        Show the application information dialog.
        """
        loader = QUiLoader() # Create a QUiLoader instance
        info_dialog:QDialog = loader.load(os.path.join(os.path.dirname(Assisted_Segmentation_GUI.__file__),"gui","UI","info_dialog.ui"), self) # Load the info dialog UI
        info_dialog.exec() # Execute the info dialog

    def update_plots(self):
        """
        Update the plots.
        """
        self.datastream_canvas.update_plot() # Update the datastream canvas plot
        self.focus_window_canvas.update_plot() # Update the focus window canvas plot
        self.score_canvas.update_plot() # Update the score canvas plot

    def clear_break_points(self):
        """
        Clear the break points.
        """
        for item in self.datastream_canvas.plot_widget.items(): # Iterate through all items in the datastream canvas plot widget
            if isinstance(item, pg.InfiniteLine): # If the item is an infinite line
                if item != self.datastream_canvas.x_pointer_line: # If the item is not the x pointer line
                    self.datastream_canvas.plot_widget.removeItem(item) # Remove the item

        for item in self.focus_window_canvas.plot_widget.items(): # Iterate through all items in the focus window canvas plot widget
            if isinstance(item, pg.InfiniteLine): # If the item is an infinite line
                if item != self.focus_window_canvas.x_pointer_line: # If the item is not the x pointer line
                    self.focus_window_canvas.plot_widget.removeItem(item) # Remove the item
        
        # Clear the break points dictionaries
        self.gt_break_points_lines = {}
        self.candidate_break_points_lines = {}

    def draw_break_points(self, plot_widget, gt_break_points, candidate_break_points):
        """
        Draw the break points on the plot widget.
        
        Args:
            plot_widget (pg.PlotWidget): The plot widget.
            gt_break_points (list): The ground truth break points.
            candidate_break_points (list): The candidate break points.
        """
        for bkp in gt_break_points: # Iterate through the ground truth break points
            self.gt_break_points_lines[bkp] = pg.InfiniteLine(pos=bkp, angle=90, pen=pg.mkPen('w', width=2)) # Create an infinite line for the ground truth break point
            plot_widget.addItem(self.gt_break_points_lines[bkp]) # Add the ground truth break point line to the plot widget

        for bkp in candidate_break_points: # Iterate through the candidate break points
            self.candidate_break_points_lines[bkp] = pg.InfiniteLine(pos=bkp, angle=90, pen=pg.mkPen('w', width=2, style=pg.QtCore.Qt.DashLine)) # Create an infinite line for the candidate break point
            plot_widget.addItem(self.candidate_break_points_lines[bkp]) # Add the candidate break point line to the plot widget

    def update_zoom_slider_position(self, index):
        """
        Update the zoom slider position.
        
        Args:
            index (int): The index of the slider.
        """
        self.zoom_length = index
        self.window_size = min(self.window_size, self.max_window_size) # Update the window size
        self.start_plot = min(self.start_plot, self.nsamples - self.zoom_length) # Update the start plot
        self.start_window = min(self.start_window, self.end_plot - self.window_size) # Update the start window
        # Update the scroll slider range and value
        self.datastream_scroll_slider.slider.setRange(0, self.nsamples - self.zoom_length) 
        self.datastream_scroll_slider.slider.setValue(self.start_plot)
        # Update the focus window size slider range and value
        self.focus_window_size_slider.slider.setRange(self.MIN_WINDOW, self.max_window_size)
        self.focus_window_size_slider.slider.setValue(self.window_size)
        # Update the plots
        self.update_plots()
    
    def update_scroll_slider_position(self, index):
        """
        Update the scroll slider position.
        
        Args:
            index (int): The index of the slider.
        """
        self.start_plot = index
        self.start_window = min(max(self.start_window, self.start_plot), self.end_plot - self.window_size) # Update the start window
        # Update the plots
        self.update_plots()
    
    def update_window_size_slider_position(self, index):
        """
        Update the window size slider position.
        """
        self.window_size = index
        self.update_plots() # Update the plots

    def clear_components_list(self):
        """
        Clear the components list.
        """
        # Iterate through all items in the components widgets dictionary
        for cw in self.components_widgets:
            self.components_widgets[cw][0].setParent(None) # Set the parent of the component widget to None
            self.components_widgets[cw][0].deleteLater() # Delete the component widget
            self.components_button_group = QButtonGroup(self) # Create a new button group
        # Iterate through all items in the components_frame layout
        while self.components_frame.layout().count():
            item = self.components_frame.layout().takeAt(0) # Take the first item in the layout
            self.remove_widget_from_layout(self.components_frame.layout(), item.widget()) # Remove the widget from the layout
        # Clear the components widgets dictionary
        self.components_widgets = {}
        # Disable the run segmentation button
        self.run_segmentation_button.setEnabled(False)

    def remove_widget_from_layout(self, layout, widget):
        """
        Remove a widget from a layout.
        
        Args:
            layout (QLayout): The layout.
            widget (QWidget): The widget to remove.
        
        Returns:
            bool: True if the widget was removed, False otherwise.
        """
        for i in range(layout.count()): # Iterate through the items in the layout
            item = layout.itemAt(i) # Get the item at the index
            if item.widget() == widget: # If the item is the widget
                # Remove the widget from the layout
                layout.takeAt(i)
                widget.deleteLater()
                return True
        return False

    def select_acquisition_folder(self):
        """
        Select the acquisition folder.
        """
        # Open a file dialog to select the acquisition folder
        self.acquisition_folder_path = QFileDialog.getExistingDirectory(None, 'Select Folder')
        if self.acquisition_folder_path: # If a folder was selected
            self.acq_folder_textEdit.setText(self.acquisition_folder_path) # Set the text of the acquisition folder text edit
            self.acq_folder_textEdit.setStyleSheet("color: rgb(90,90,90);") # Set the text color
            # Clear the components list
            self.clear_components_list()
            # Validate the acquisition folder
            ret = self.controller.validate_acquisition_folder(self.acquisition_folder_path)
            if ret: # If the folder is valid
                if not self.message_label.isVisible(): # If the message label is not visible
                    self.message_label.setVisible(True) # Show the message label
                self.components = self.controller.get_component_list() # Get the component list
                for i, c in enumerate(self.components): # Iterate through the components
                    c_name = list(c.keys())[0] # Get the component name
                    odr = c[c_name].get("odr", "N/A") # Get the component ODR
                    file_size = self.controller.get_file_dimension(c_name) # Get the file size
                    dim = c[c_name]["dim"] # Get the component dimensions (number of axis)
                    data_sample_bytes_length = TypeConversion.check_type_length(c[c_name]["data_type"]) # Get the data sample bytes length
                    comp_contents = {"odr": odr, "axes": dim, "data_type": c[c_name]["data_type"], "file_size": file_size, "data_sample_bytes_length": data_sample_bytes_length} # Create the component contents dictionary
                    # Create a component widget
                    cw = ComponentWidget(c_name, c_name.upper(), comp_contents, self.components_frame)
                    cw.title_label.setCheckable(True) # Set the title label checkable
                    cw.setEnabled(c[c_name]["enable"]) # Set the component widget enabled if the component is enabled in the acquisition folder, otherwise disable it
                    if not c[c_name]["enable"]: # If the component is disabled
                        cw.title_label.setToolTip("Component was disabled in the selected acquisition")
                    self.components_button_group.addButton(cw.title_label, i) # Add the title label to the button group
                    self.components_frame.layout().addWidget(cw) # Add the component widget to the components frame layout
                    self.components_widgets[c_name] = [cw, False] # Add the component widget to the components widgets dictionary
                self.components_button_group.buttonClicked.connect(self.on_component_selected) # Connect the button clicked signal to a callback
            else: # If the folder is invalid
                self.acq_folder_textEdit.setText(f"Invalid folder selected: {self.acquisition_folder_path}") # Set the text of the acquisition folder text edit
                self.acq_folder_textEdit.setStyleSheet("color: red") # Set the text color to red
                if self.message_label.isVisible(): # If the message label is visible
                    self.message_label.setVisible(False) # Hide the message label

    def on_component_selected(self, button):
        """
        Callback function for when a component is selected.
        """
        # Get the selected component from the components dictionary
        button_id = self.components_button_group.id(button) # Get the button ID
        self.selected_component = self.components[button_id] # Get the selected component
        self.controller.set_selected_component(self.selected_component) # Set the selected component in the controller
        selected_component_name = list(self.selected_component.keys())[0] # Get the selected component name
        for c in self.components_widgets: # Iterate through the components widgets
            if c != selected_component_name: # If the component is not the selected component
                self.components_widgets[c][0].pack_contents_widget() # Pack the contents widget
                self.components_widgets[c][1] = False # Set the component widget to not selected
            else:
                self.components_widgets[c][1] = not self.components_widgets[c][1] # Toggle the selected component
        # Enable the run segmentation button if at least one component is selected
        self.run_segmentation_button.setEnabled(any(value[1] for value in self.components_widgets.values()))

    def start_segmentation_algorithm(self):
        """
        Start the segmentation algorithm.
        """
        self.waiting_dialog.start() # Start the waiting dialog
        self.controller.start_segmentation_thread(self.on_segmentation_finished) # Start the segmentation algorithm thread, passing the on segmentation finished callback

    def start_acquisition_tagging(self):
        """
        Start the acquisition tagging.
        """
        # Start the tag acquisition with break points function in the controller, passing the show non empty tags alert function as a callback
        ret = self.controller.tag_acquisition_with_break_points(self.show_non_empty_tags_alert)
        if ret: # If the tag acquisition was successful
            self.tag_acquisition_button.setVisible(False) # Hide the tag acquisition button
            self.tag_acquisition_label.setVisible(True) # Show the tag acquisition label

    def on_segmentation_finished(self):
        """
        Callback function for when the segmentation algorithm is finished.
        """
        self.waiting_dialog.stop() # Stop the waiting dialog
        
        self.MIN_ZOOM = min(100, self.nsamples // 10) # Set the minimum zoom 
        self.MAX_ZOOM = self.nsamples # Set the maximum zoom
        self.MIN_WINDOW = min(50, self.nsamples // 200) # Set the minimum window

        self._start_plot = 0 # Set the start plot index
        self._zoom_length = (self.MIN_ZOOM + self.MAX_ZOOM) // 2 # Set the zoom length
        self._window_size = (self.MIN_WINDOW + self.max_window_size) // 2 # Set the window size
        self._start_window = 0 # Set the start window index
        self._x_pointer = None # Set the x pointer position
        # Update the plots
        self.update_GUI()

    def show_non_empty_tags_alert(self):
        """
        Show the non-empty tags alert.
        """
        dialog = TagsAlertDialog(self) # Create a tags alert dialog
        if dialog.exec() == QDialog.DialogCode.Accepted: # If the dialog was accepted
            # Set the tagging parameters in the controller based on the dialog settings
            self.controller.set_tagging_params("overwrite" if dialog.overwrite_tags_flag else "append") # Set the tagging parameters
        else:
            # Set the tagging parameters to None
            self.controller.set_tagging_params(None)

    def update_GUI(self):
        """
        Update the GUI.
        """
        # Update the datastream canvas
        if self.datastream_canvas is not None: # If the datastream canvas is not None
            self.remove_widget_from_layout(self.frame_datastream_plot.layout(), self.datastream_canvas) # Remove the datastream canvas from the layout
        self.datastream_canvas = DatastreamCanvas(self) # Create a new datastream canvas
        self.frame_datastream_plot = self.findChild(QFrame, 'frame_datastream_plot') # Get the datastream plot frame
        self.frame_datastream_plot.layout().addWidget(self.datastream_canvas) # Add the datastream canvas to the layout
        # Update the score canvas
        if self.score_canvas is not None: # If the score canvas is not None
            self.remove_widget_from_layout(self.frame_score_plot.layout(), self.score_canvas) # Remove the score canvas from the layout
        self.score_canvas = ScoreCanvas(self) # Create a new score canvas
        self.frame_score_plot = self.findChild(QFrame, 'frame_score_plot') # Get the score plot frame
        self.frame_score_plot.layout().addWidget(self.score_canvas) # Add the score canvas to the layout
        # Update the focus window canvas
        if self.focus_window_canvas is not None: # If the focus window canvas is not None
            self.remove_widget_from_layout(self.frame_focus_window_plot.layout(), self.focus_window_canvas) # Remove the focus window canvas from the layout
        self.focus_window_canvas = FocusWindowCanvas(self) # Create a new focus window canvas
        self.frame_focus_window_plot:QFrame = self.findChild(QFrame, 'frame_focus_window_plot') # Get the focus window plot frame
        self.frame_focus_window_plot.setStyleSheet("background-color: rgba(255, 255, 0, 0.3);") # Set the background color of the focus window plot frame
        self.frame_focus_window_plot.layout().addWidget(self.focus_window_canvas) # Add the focus window canvas to the layout
        # Build the sliders
        self.datastream_scroll_slider.build() # Build the datastream scroll slider
        self.datastream_zoom_slider.build() # Build the datastream zoom slider
        self.focus_window_size_slider.build() # Build the focus window size slider
        # Update the plots
        self.update_plots()
        # Clear the break points already drawn
        self.clear_break_points()
        # Draw the new break points on the plots
        gt_break_points = self.controller.get_gt_break_points() # Get the ground truth break points
        candidate_break_points = self.controller.get_candidate_break_points() # Get the candidate break points
        self.draw_break_points(self.focus_window_canvas.plot_widget, gt_break_points, candidate_break_points) # Draw the break points in the focus window canvas
        self.draw_break_points(self.datastream_canvas.plot_widget, gt_break_points, candidate_break_points) # Draw the break points in the datastream canvas
        # Check if the tag acquisition button should be enabled
        if len(candidate_break_points) == 0 and len(gt_break_points) > 1: # If there are no candidate break points and there are more than one ground truth break points
            self.tag_acquisition_button.setEnabled(True) # Enable the tag acquisition button
        else:
            self.tag_acquisition_button.setEnabled(False) # Disable the tag acquisition button
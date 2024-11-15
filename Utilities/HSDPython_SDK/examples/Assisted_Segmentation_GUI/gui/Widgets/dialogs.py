
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

from PySide6.QtCore import Qt, QSize
from PySide6.QtWidgets import QPushButton, QVBoxLayout, QDialog, QLabel
from PySide6.QtGui import QMovie
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import Assisted_Segmentation_GUI

class TagsAlertDialog(QDialog):
    def __init__(self, parent=None):
        """
        Constructor for the TagsAlertDialog class.
        This class is a custom QDialog that is used to display a dialog to the user when the user tries to tag an acquisition that already has tags.
        """
        super().__init__(parent)
        self.parent = parent
        # Flag to indicate if the user wants to overwrite the existing tags
        self.overwrite_tags_flag = False
        # Set the dialog title
        self.setWindowTitle("Existing Tags Alert Dialog")
        # Register the custom widget
        QPyDesignerCustomWidgetCollection.registerCustomWidget(TagsAlertDialog, module="TagsAlertDialog")
        loader = QUiLoader() # Create a QUiLoader object
        main_dialog_widget = loader.load(os.path.join(os.path.dirname(Assisted_Segmentation_GUI.__file__),"gui","UI","tags_alert_dialog.ui"), parent) # Load the UI file
        # Get buttons from the UI file and connect them to the respective slots
        self.overwrite_button:QPushButton = main_dialog_widget.findChild(QPushButton,"overwrite_button")
        self.overwrite_button.clicked.connect(self.clicked_overwrite_button)
        self.append_button:QPushButton = main_dialog_widget.findChild(QPushButton,"append_button")
        self.append_button.clicked.connect(self.clicked_append_button)
        self.cancel_button:QPushButton = main_dialog_widget.findChild(QPushButton,"cancel_button")
        self.cancel_button.clicked.connect(self.reject)
        #Main layout
        main_layout = QVBoxLayout() # Create a QVBoxLayout object
        self.setLayout(main_layout) # Set the layout
        main_layout.addWidget(main_dialog_widget) # Add the main dialog widget to the layout

    def clicked_overwrite_button(self):
        """
        Slot for the clicked signal of the overwrite button.
        """
        self.overwrite_tags_flag = True # Set the flag to True
        self.accept() # Accept the dialog

    def clicked_append_button(self):
        """
        Slot for the clicked signal of the append button.
        """
        self.overwrite_tags_flag = False # Set the flag to False
        self.accept() # Accept the dialog

class WaitingDialog(QDialog):
    def __init__(self, title, text, parent=None):
        """
        Constructor for the WaitingDialog class.
        This class is a custom QDialog that is used to display a waiting dialog to the user when the application is processing some data.
        """
        super().__init__(parent)
        self.setWindowFlags(Qt.WindowType.Dialog | Qt.WindowType.CustomizeWindowHint | Qt.WindowType.WindowTitleHint) # Set the window flags
        self.setWindowTitle(title) # Set the window title
        self.setModal(True) # Set the dialog to be modal
        # Set the background color using setStyleSheet
        self.setStyleSheet("background-color: rgb(44, 49, 60); color: rgb(210, 210, 210);")
        layout = QVBoxLayout()
        self.message_label = QLabel(text) # Create a QLabel object with the message text passed as an argument
        layout.addWidget(self.message_label, alignment=Qt.AlignmentFlag.AlignCenter) # Add the message label to the layout
        layout.setSpacing(24)
        layout.setContentsMargins(24, 24, 24, 24)
        self.movie_label = QLabel(text) # Create a QLabel object to display the loading icon
        loading_gif_path = os.path.join(os.path.dirname(Assisted_Segmentation_GUI.__file__),"gui","UI","loading_icon.gif") # Get the path to the loading icon
        self.movie = QMovie(loading_gif_path) # Create a QMovie object with the loading icon
        self.movie.setScaledSize(QSize(64,64)) # Set the size of the loading icon
        self.movie_label.setMovie(self.movie) # Set the movie to the movie label
        layout.addWidget(self.movie_label, alignment=Qt.AlignmentFlag.AlignCenter) # Add the movie label to the layout
        # Set the layout
        self.setLayout(layout)

    def start(self):
        """
        Method to show the dialog and start the loading animation.
        """
        self.movie.start() # Start the movie
        self.show() # Show the dialog

    def stop(self):
        """
        Method to stop the loading animation and close the dialog.
        """
        self.movie.stop() # Stop the movie
        self.close() # Close the dialog
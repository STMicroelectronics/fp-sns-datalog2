
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

from PySide6.QtGui import QPixmap, QPainter, QColor
from PySide6.QtCore import Slot, Qt
from PySide6.QtWidgets import QPushButton, QVBoxLayout, QWidget, QFrame, QLineEdit, QLabel
from PySide6.QtUiTools import QUiLoader
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import Assisted_Segmentation_GUI

# CSS styles for the component widget
green_style = '''
        QFrame {
            border: transparent;
            background-color: rgb(51, 71, 51);
        }
        QFrame:hover {
            background-color: rgb(57, 81, 57);
            border: transparent;
        }
        QFrame:pressed {	
            background-color: rgb(34, 48, 34);
            border: 1px solid rgb(42, 60, 42);
        }
        QPushButton {
            background-color: transparent;
        }
        QLabel {
            background-color: transparent;
        }
    '''

default_style = '''
        QFrame {
            border: transparent;
            background-color: rgb(39, 44, 54);
        }
        QFrame:hover {
            border: transparent;
            background-color: rgb(57, 65, 80);
        }
        QFrame:pressed {	
            background-color: rgb(35, 40, 49);
            border: 1px solid rgb(43, 50, 61);
        }
        QFrame:disabled {
            background-color: rgb(60, 60, 60);
            color: rgb(90,90,90);
        }
        QPushButton {
            background-color: transparent;
        }
        QLabel {
            background-color: transparent;
        }
    '''

class ComponentWidget(QWidget):
    def __init__(self, comp_name, comp_display_name, comp_contents, parent=None):
        """
        Constructor for the ComponentWidget class.
        This class is a custom QWidget that is used to display the details of a component in the main window.
        
        Args:
            comp_name (str): The name of the component.
            comp_display_name (str): The display name of the component.
            comp_contents (dict): The contents of the component.
            parent (QWidget): The parent widget. Default is None.
        """
        super().__init__(parent)
        self.parent = parent
        self.is_packed = True # Flag to check if the contents are packed or unpacked
        self.comp_name = comp_name # The name of the component
        self.comp_display_name = comp_display_name # The display name of the component
        self.comp_contents = comp_contents # The contents of the component
        self.setWindowTitle(self.comp_display_name) # Set the window title
        # Register the custom widget
        QPyDesignerCustomWidgetCollection.registerCustomWidget(ComponentWidget, module="ComponentWidget") 
        loader = QUiLoader() # Create a QUiLoader object
        comp_config_widget = loader.load(os.path.join(os.path.dirname(Assisted_Segmentation_GUI.__file__),"gui","UI","component_widget.ui"), parent) # Load the UI file
        # Get the widgets from the UI file
        self.frame_title:QFrame = comp_config_widget.findChild(QFrame,"frame_title")
        self.title_label:QPushButton = self.frame_title.findChild(QPushButton,"label_title")
        self.title_label.setText(self.comp_display_name)
        self.title_label.clicked.connect(self.clicked_show_contents_button) # Connect the clicked signal of the title label to the slot
        self.label_alert_icon:QLabel = self.frame_title.findChild(QLabel,"label_alert_icon")
        self.label_alert_icon.setVisible(False)
        self.label_alert_icon.setStyleSheet("""
            QToolTip {
                color: #ffffff;
                background-color: rgb(44, 49, 60);
            }
        """)
        # Get the frame_contents widget
        self.frame_contents = comp_config_widget.findChild(QFrame,"frame_contents")
        self.frame_contents.setVisible(False) # Set the frame_contents widget to be invisible (initially packed)
        # Get component properties from the comp_contents dictionary
        odr = comp_contents["odr"] # Get the ODR value
        data_sample_bytes_length = comp_contents["data_sample_bytes_length"] # Get the data sample bytes length
        file_size = comp_contents["file_size"] # Get the file size
        data_type = comp_contents["data_type"] # Get the data type
        dim = comp_contents["axes"] # Get the axes value
        num_of_samples_to_display = 0 # Initialize the number of samples to display
        if file_size is not None: # Check if the file size is not None
            num_of_samples_to_display = ((file_size//dim)//data_sample_bytes_length) # Calculate the number of samples to display
        # Get the frame_contents inner widgets from the UI file
        self.odr_textEdit = self.frame_contents.findChild(QLineEdit,"odr_textEdit")
        self.odr_textEdit.setText(str(odr)) # Set the ODR value
        self.axes_textEdit = self.frame_contents.findChild(QLineEdit,"axes_textEdit")
        self.axes_textEdit.setText(str(dim)) # Set the axes value
        self.data_type_textEdit = self.frame_contents.findChild(QLineEdit,"data_type_textEdit")
        self.data_type_textEdit.setText(str(data_type)) # Set the data type
        self.file_size_textEdit = self.frame_contents.findChild(QLineEdit,"file_size_textEdit")
        self.file_size_textEdit.setText(str(file_size)) # Set the file size
        # Set the alert icon color based on the previous calculated number of samples to display
        if num_of_samples_to_display >= 140000 and num_of_samples_to_display < 200000:
            self.label_alert_icon.setVisible(True) # Set the alert icon to be visible
            self.change_pixmap_color(self.label_alert_icon, QColor(253, 253, 150)) # Change the pixmap color to yellow
        if num_of_samples_to_display >= 200000:
            self.label_alert_icon.setVisible(True) # Set the alert icon to be visible
            self.change_pixmap_color(self.label_alert_icon, QColor(255, 105, 97)) # Change the pixmap color to red
        #Main layout
        main_layout = QVBoxLayout()
        self.setLayout(main_layout) # Set the layout
        main_layout.addWidget(comp_config_widget) # Add the component config widget to the layout

    @Slot()
    def clicked_show_contents_button(self):
        """
        Slot for the clicked signal of the title label.
        """
        if self.is_packed: # Check if the contents are packed
            self.unpack_contents_widget() # Unpack the contents
        else:
            self.pack_contents_widget() # Pack the contents
    
    def unpack_contents_widget(self):
        """
        Method to unpack the contents widget.
        """
        if self.isEnabled(): # Check if the widget is enabled
            self.frame_title.setStyleSheet(green_style) # Set the frame title style sheet to green
            self.frame_contents.setVisible(True) # Set the frame contents to be visible
            self.is_packed = False # Set the is_packed flag to False

    def pack_contents_widget(self):
        """
        Method to pack the contents widget.
        """
        if self.isEnabled(): # Check if the widget is enabled
            self.frame_title.setStyleSheet(default_style) # Set the frame title style sheet to default
            self.frame_contents.setVisible(False) # Set the frame contents to be invisible
            self.is_packed = True # Set the is_packed flag to True

    def change_pixmap_color(self, label:QLabel, color):
        """
        Method to change the color of a pixmap in a QLabel.
        
        Args:
            label (QLabel): The QLabel object.
            color (QColor): The QColor object.
        """
        # Get the current pixmap from the QLabel
        original_pixmap = label.pixmap()
        if original_pixmap is None: # Check if the pixmap is None
            return # Return
        # Create a new pixmap with the same size as the original
        colored_pixmap = QPixmap(original_pixmap.size())
        colored_pixmap.fill(Qt.GlobalColor.transparent)  # Fill with transparent color
        # Create a QPainter to paint on the new pixmap
        painter = QPainter(colored_pixmap)
        painter.setCompositionMode(QPainter.CompositionMode.CompositionMode_Source) # Set the composition mode to source
        # Fill the pixmap with the desired color
        painter.fillRect(colored_pixmap.rect(), color)
        # Set the composition mode to only draw on the white parts of the original pixmap
        painter.setCompositionMode(QPainter.CompositionMode.CompositionMode_DestinationIn)
        painter.drawPixmap(0, 0, original_pixmap) # Draw the original pixmap
        painter.end() # End the painting
        # Set the new pixmap to the QLabel
        label.setPixmap(colored_pixmap)

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

from PySide6.QtWidgets import QDialog, QVBoxLayout, QLabel, QProgressDialog

class StaticLoadingWindow():
    def __init__(self, title, text, parent) -> None:
        self.dialog = QDialog(parent)
        
        layout = QVBoxLayout()
        message = QLabel(text)
        layout.addWidget(message)
        self.dialog.setLayout(layout)
        self.dialog.setContentsMargins(24,24,24,24)
        
        self.dialog.setWindowTitle(title)
        self.dialog.setModal(True)
        style = '''
             QDialog
             {
                 background-color: rgb(41, 45, 56);
             }
         '''
         
        self.dialog.setStyleSheet(style)
        self.dialog.show()
    
    def loadingDone(self):
        self.dialog.close()

class LoadingWindow:
    
    def __init__(self, title, text, parent) -> None:
        self.dialog = QProgressDialog(parent)
        self.dialog.setContentsMargins(24,24,24,24)
        self.dialog.setMinimum(0)
        self.dialog.setMaximum(0)
        self.dialog.setLabelText(text)
        self.dialog.setWindowTitle(title)
        self.dialog.setCancelButton(None)
        self.dialog.setModal(True)
        style = '''
             QProgressDialog
             {
                 background-color: rgb(41, 45, 56);
             }
         '''
         
        self.dialog.setStyleSheet(style)
        self.dialog.show()
    
    def loadingDone(self):
        self.dialog.close()
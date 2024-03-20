from st_dtdl_gui.UI.styles import STDTDL_PushButton
from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
from PySide6.QtWidgets import QMessageBox, QDialog, QVBoxLayout, QLabel, QPushButton

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class HSDMCLConfigurationWidget(ComponentWidget):
    def __init__(self, controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id = 0, parent=None):       
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)

        self.app = self.controller.qt_app
        self.is_logging = False
        self.parent_widget = parent
        self.comp_sem_type = comp_sem_type

        self.property_widgets["enable"].value.toggled.connect(self.enable_mlc_clicked)

    def enable_mlc_clicked(self, status):
        if status and not self.property_widgets["ucf_status"].value.isChecked():
            # create a QMessageBox object
            msg_box = QMessageBox()
            # set message box window title
            msg_box.setWindowTitle("Error enabling MLC")
            # set the icon
            msg_box.setIcon(QMessageBox.Warning)
            # set the message text
            msg_box.setText("Please, upload a valid UCF file before enabling this MLC component.")            
            # set the stylesheet
            msg_box.setStyleSheet(f"QMessageBox {{ background-color: rgb(27, 29, 35); }} QLabel {{ color: rgb(210,210,210); }} {STDTDL_PushButton.valid}")
            # show the message box
            msg_box.exec()
            log.warning("Please, upload a valid UCF file before enabling this MLC component!")
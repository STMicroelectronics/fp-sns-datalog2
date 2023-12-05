from PySide6.QtGui import QValidator
from PySide6.QtWidgets import QSpinBox

from st_dtdl_gui.UI.styles import STDTDL_LineEdit, STDTDL_SpinBox
from st_dtdl_gui.Widgets.CommandWidget import CommandWidget
from st_dtdl_gui.Widgets.PropertyWidget import PropertyWidget
from st_dtdl_gui.Widgets.TelemetryWidget import TelemetryWidget


@staticmethod
def validate_value(widget, text_value):        
    if isinstance(widget, PropertyWidget) or isinstance(widget, TelemetryWidget) or isinstance(widget, CommandWidget):
        validation_res = widget.validator.validate(text_value,0)
        widget = widget.value
    else:
        validation_res = widget.validator().validate(text_value,0)
    
    if isinstance(validation_res, tuple):
        validation_res = validation_res[0]

    if validation_res == QValidator.State.Acceptable:
        if isinstance(widget, QSpinBox):
            widget.setStyleSheet(STDTDL_SpinBox.valid)
        else:
            widget.setStyleSheet(STDTDL_LineEdit.valid)
        return True
    else:
        if isinstance(widget, QSpinBox):
            widget.setStyleSheet(STDTDL_SpinBox.invalid)
        else:
            widget.setStyleSheet(STDTDL_LineEdit.invalid)
        return False
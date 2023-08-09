from PySide6.QtCore import QEasingCurve
from st_dtdl_gui.Widgets.ToggleButton import ToggleButton

class TagToggleButton(ToggleButton):
    def __init__(self, width=60, bg_color="#777", circle_color="#DDD", active_color="#000ccf", animation_curve=QEasingCurve.Type.OutBounce):
        super().__init__(width, bg_color, circle_color, active_color, animation_curve)
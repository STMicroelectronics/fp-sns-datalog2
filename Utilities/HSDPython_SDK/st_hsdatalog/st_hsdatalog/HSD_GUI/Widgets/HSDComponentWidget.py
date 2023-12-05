
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

from st_dtdl_gui.Widgets.ComponentWidget import ComponentWidget
class HSDComponentWidget(ComponentWidget):
    def __init__(self, controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id = 0, parent=None):
        if comp_name == "DeviceInformation":
            [c for c in comp_contents if c.name == "totalStorage"][0].display_name = "Total Storage [GB]"
            [c for c in comp_contents if c.name == "totalMemory"][0].display_name = "Total Memory [KB]"
        
        super().__init__(controller, comp_name, comp_display_name, comp_sem_type, comp_contents, c_id, parent)
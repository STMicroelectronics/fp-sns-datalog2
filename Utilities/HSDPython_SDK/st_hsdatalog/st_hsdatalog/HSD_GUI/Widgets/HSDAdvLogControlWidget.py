
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

from st_hsdatalog.HSD_GUI.Widgets.HSDLogControlWidget import HSDLogControlWidget
import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class HSDAdvLogControlWidget(HSDLogControlWidget):
    def __init__(self, controller, comp_contents, comp_name="log_controller", comp_display_name="Log Controller", comp_sem_type="other", c_id=0, parent=None):
        super().__init__(controller, comp_contents, comp_name, comp_display_name, comp_sem_type, c_id, parent)

        self.frame_sub_log_info.setEnabled(True)
        self.frame_sub_log_info.setVisible(True)
        
        self.update_sd_status_label()
        
        self.refresh_sd_button.setEnabled(True)
        self.refresh_sd_button.setVisible(True)

        self.spectrum_radio.setEnabled(True)
        self.spectrum_radio.setVisible(True)

        self.debug_radio.setEnabled(True)
        self.debug_radio.setVisible(True)
        
        self.tags_label_combo.setEnabled(True)
        self.tags_label_combo.setVisible(True)
        
        self.sub_plots_radio.setEnabled(True)
        self.sub_plots_radio.setVisible(True)
        
        self.raw_data_radio.setEnabled(True)        
        self.raw_data_radio.setVisible(True)
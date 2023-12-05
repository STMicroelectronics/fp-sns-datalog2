
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
import json
import st_hsdatalog.HSD_utils.logger as logger
from st_pnpl.DTDL.device_template_manager import DeviceTemplateManager
log = logger.setup_applevel_logger(is_debug = False, file_name= "app_debug.log")

class HSDatalogDTM:
    
    @staticmethod
    def upload_custom_dtm(custom_dtm):
        if custom_dtm is not None:
            n_board_id = custom_dtm[0]
            n_fw_id = custom_dtm[1]
            n_dtm_path = custom_dtm[2]
            n_dtm_filename = os.path.basename(n_dtm_path).split('.')[0]
            n_dtm_json_str = None
            try:
                if os.path.isfile(n_dtm_path) and os.path.exists(n_dtm_path):
                    with open(n_dtm_path) as f:
                        device_json_dict = json.load(f)
                    n_dtm_json_str = json.dumps(device_json_dict)
                    f.close()
                else:
                    raise
            except:
                log.error("Wrong nput custom Device Template Model path")
                return
            if n_dtm_json_str is not None:
                DeviceTemplateManager.add_dtdl_model(n_board_id, n_fw_id, n_dtm_filename, n_dtm_json_str)
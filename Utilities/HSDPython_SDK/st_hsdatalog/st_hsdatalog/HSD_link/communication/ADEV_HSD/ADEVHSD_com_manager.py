#
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

import st_hsdatalog.HSD_utils.logger as logger
log = logger.get_logger(__name__)

class ADEVHSD_Cmd():
    def get_name(self):
        return "ADEVHSDCmd"

class ADEVHSD_CommandManager:

    def __init__(cmd_set:ADEVHSD_Cmd):
        log.info("DUMMY Communication Engine UP")
    
    def __del__(self):
        log.info("DUMMY Communication Engine DOWN")

    def get_device_presentation_string(self):
        return "ADEV"

    def get_cmd_set_presentation_string(self, cmd_set):
        return "ADEVHSD_CommandManager command_set: {}".format(cmd_set.get_name())

    def get_version(self):
        return "7.7.7"
    
    def get_nof_devices(self):
        return 0

class ADEVHSD_Creator:
    def __create_cmd_set(): return ADEVHSD_Cmd()
    def create_cmd_manager(self): return ADEVHSD_CommandManager(self.__create_cmd_set())
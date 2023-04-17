# *****************************************************************************
#  * @file    ultrasound_fft_app.py
#  * @author  SRA
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

import sys

# import cProfile
# from pstats import Stats

from PySide6.QtWidgets import QApplication
from PySide6 import QtCore

from st_hsdatalog.HSD_GUI.HSD_MainWindow import HSD_MainWindow


def main():
    QApplication.setAttribute(QtCore.Qt.AA_ShareOpenGLContexts)
    app = QApplication(sys.argv)
    
    mainWindow = HSD_MainWindow(app)
    mainWindow.setAppTitle("Ultrasound FFT Application")
    mainWindow.setAppCredits("Ultrasound FFT Application")
    mainWindow.setWindowTitle("Ultrasound FFT")
    mainWindow.setAppVersion("v1.0.0")
    mainWindow.setComponentsConfigWidth(500)
    mainWindow.show()
    app.setAttribute(QtCore.Qt.AA_Use96Dpi)
    app.exec()

if __name__ == "__main__":
    
    # do_profiling = True
    # if do_profiling:
    #     with cProfile.Profile() as pr:
    #         main()

    #     with open('profiling_stats.txt', 'w') as stream:
    #         stats = Stats(pr, stream=stream)
    #         stats.strip_dirs()
    #         stats.sort_stats('time')
    #         stats.dump_stats('.prof_stats')
    #         stats.print_stats()

    main()



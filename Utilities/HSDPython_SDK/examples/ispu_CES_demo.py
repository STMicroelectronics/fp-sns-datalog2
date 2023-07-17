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
import click

# import cProfile
# from pstats import Stats

from PySide6.QtWidgets import QApplication
from PySide6 import QtCore

from st_hsdatalog.HSD_GUI.HSD_MainWindow import HSD_MainWindow

def show_help(ctx, param, value):
    if value and not ctx.resilient_parsing:
        click.secho(ctx.get_help(), color=ctx.color)
        ctx.exit()

@click.command()
@click.argument('acq_folder', type=click.Path(exists=True))
@click.option('-oc', '--out_classes', help="Classifier Output classes. An item is a tuple (class_name:str, class_image_path:str).", type=click.Tuple([str,str]), multiple=True)
@click.option("-h"," --help", is_flag=True, is_eager=True, expose_value=False, callback=show_help, help="Show this message and exit.",)

def process_cli_flags(class_names, class_images):
    pass

def main():
    QApplication.setAttribute(QtCore.Qt.AA_ShareOpenGLContexts)
    app = QApplication(sys.argv)
    
    mainWindow = HSD_MainWindow(app)
    mainWindow.setAppTitle("ISPU Demo")
    mainWindow.setAppCredits("ISPU Demo")
    mainWindow.setWindowTitle("ISPU Demo")
    mainWindow.setAppVersion("v1.0.0")

    mainWindow.setAIAnomalyImages(["Motor_Normal_class", "Motor_Anomaly_class"])
    mainWindow.setAIClassifierImages(["Motor_Magnet_class", "Motor_Vibration_class"])
    mainWindow.setAIAnomalyTool("Nanoedge_ISPU")
    mainWindow.setAIClassifierTool("Nanoedge_STM32")
    mainWindow.showMaximized()
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



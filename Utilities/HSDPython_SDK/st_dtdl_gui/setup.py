import setuptools
import os

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="st_dtdl_gui",
    version="1.0.0",
    author="SRA-ASP",
    author_email="",
    description="STMicroelectronics Graphical Widgets from DTDL collection python package",
    long_description=long_description,
    long_description_content_type="text\\markdown",
    url="",
    packages=setuptools.find_packages(),
    package_dir={'st_dtdl_gui': 'st_dtdl_gui'},
    data_files=[
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","about_dialog.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","classifier_output_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","component_config_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","connection_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","device_template_load_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","graph_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","MainWindow.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","output_class_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","plot_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","send_command_widget.ui")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI"),[os.path.join("st_dtdl_gui","UI","resources.qrc")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","baseline_engineering_white_48dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","baseline_folder_open_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","baseline_multi_table_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","baseline_tune_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","motor_anomaly_class.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_arrow_bottom_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_autorenew_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_backup_table_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_check_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_close_white_36dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_done_outline_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_edit_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_group_work_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_home_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_info_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_lamp_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_lock_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_open_in_full_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_open_in_full_white_36dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_open_with_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_play_arrow_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_save_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_settings_input_com_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_stop_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","outline_zoom_in_white_18dp.png")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","icons"),[os.path.join("st_dtdl_gui","UI","icons","ST16418_ST-Logo-Ico_3.ico")]),
        (os.path.join("Lib","site-packages","st_dtdl_gui","UI","images"),[os.path.join("st_dtdl_gui","UI","images","st_logo.png")])
    ],
    license='BSD 3-clause',
    classifiers=[
        "License :: BSD License (BSD-3-Clause)",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: Linux",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Topic :: Software Development :: Embedded Systems"
    ],
    install_requires=[
       "st_pnpl",
       "numpy",
       "pyqtgraph",
       "PySide6"
    ]
)
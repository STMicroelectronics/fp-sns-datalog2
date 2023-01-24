import setuptools
import os

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="st_hsdatalog",
    version="2.0.0",
    author="Matteo Ronchi",
    author_email="matteo.ronchi@st.com",
    description="STMicroelectronics High Speed Datalog python package",
    long_description=long_description,
    long_description_content_type="text\\markdown",
    url="",
    packages=setuptools.find_packages(),
    package_dir={'st_hsdatalog': 'st_hsdatalog'},
    data_files=[
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","32bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","32bit","libhs_datalog_v1.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","32bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","32bit","libhs_datalog_v2.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","32bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","32bit","libgcc_s_dw2-1.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","32bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","32bit","libstdc++-6.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","32bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","32bit","libwinpthread-1.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","64bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","64bit","libhs_datalog_v1.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","64bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","64bit","libhs_datalog_v2.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","64bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","64bit","libgcc_s_seh-1.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","64bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","64bit","libstdc++-6.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","64bit"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","64bit","libwinpthread-1.dll")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","linux"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","linux","libhs_datalog_v1.so")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","linux"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","linux","libhs_datalog_v2.so")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","raspberryPi"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","raspberryPi","libhs_datalog_v1.so")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_link","communication","libhs_datalog","raspberryPi"),[os.path.join("st_hsdatalog","HSD_link","communication","libhs_datalog","raspberryPi","libhs_datalog_v2.so")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI"),[os.path.join("st_hsdatalog","HSD_GUI","UI","log_control_widget.ui")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI"),[os.path.join("st_hsdatalog","HSD_GUI","UI","app_classification_control.ui")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI"),[os.path.join("st_hsdatalog","HSD_GUI","UI","save_config_dialog.ui")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI"),[os.path.join("st_hsdatalog","HSD_GUI","UI","tags_info_widget.ui")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI"),[os.path.join("st_hsdatalog","HSD_GUI","UI","sw_tag.ui")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI"),[os.path.join("st_hsdatalog","HSD_GUI","UI","hw_tag.ui")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI","images"),[os.path.join("st_hsdatalog","HSD_GUI","UI","images","ISPU.png")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI","images"),[os.path.join("st_hsdatalog","HSD_GUI","UI","images","Motor_Anomaly_Class.png")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI","images"),[os.path.join("st_hsdatalog","HSD_GUI","UI","images","Motor_Normal_Class.png")]),
        (os.path.join("Lib","site-packages","st_hsdatalog","HSD_GUI","UI","images"),[os.path.join("st_hsdatalog","HSD_GUI","UI","images","Motor_Vibration_Class.png")])
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
       "st_dtdl_gui",
       "numpy",
       "pandas",
       "colorama",
       "click",
       "PySide6",
       "pyqtgraph",
       "matplotlib"
    ]
)
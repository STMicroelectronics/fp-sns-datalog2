
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

import setuptools
import os

with open("LICENSE.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="st_dtdl_gui",
    version="2.4.0",
    author="SRA-ASP",
    author_email="matteo.ronchi@st.com",
    description="STMicroelectronics UI Toolkit based on DTDL and PySide6.",
    long_description=long_description,
    long_description_content_type="text\\markdown",
    include_package_data=True,    
    url="",
    packages=setuptools.find_packages(),
    package_dir={'st_dtdl_gui': 'st_dtdl_gui'},
    license='BSD 3-clause',
    classifiers=[
        "License :: BSD License (BSD-3-Clause)",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: Linux",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Software Development :: Embedded Systems"
    ],
    install_requires=[
       "st_pnpl",
       "numpy==1.26.4",
       "pyqtgraph",
       "setuptools",
       "PySide6==6.6.1",
       "pyaudio"
    ]
)
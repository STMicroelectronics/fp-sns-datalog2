
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
    name="st_pnpl",
    version="2.3.0",
    author="SRA-ASP",
    author_email="matteo.ronchi@st.com",
    description="STMicroelectronics PnP-Like and DTDL Device Template Models management python package",
    long_description=long_description,
    long_description_content_type="text\\markdown",
    include_package_data=True,    
    url="",
    packages=setuptools.find_packages(),
    package_dir={'st_pnpl': 'st_pnpl'},
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
        "Programming Language :: Python :: 3.11",
        "Programming Language :: Python :: 3.12",
        "Topic :: Software Development :: Embedded Systems"
    ]
)
import setuptools
import os

with open("LICENSE.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="st_dtdl_gui",
    version="1.1.0",
    author="SRA-ASP",
    author_email="",
    description="STMicroelectronics Graphical Widgets from DTDL collection python package",
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
       "PySide6==6.4.3",
       "pyaudio"
    ]
)
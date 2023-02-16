import setuptools
import os

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="st_pnpl",
    version="1.0.1",
    author="Matteo Ronchi",
    author_email="matteo.ronchi@st.com",
    description="STMicroelectronics PnP-Like and DTDL Device Template Models management python package",
    long_description=long_description,
    long_description_content_type="text\\markdown",
    url="",
    packages=setuptools.find_packages(),
    package_dir={'st_pnpl': 'st_pnpl'},
    data_files=[
        (os.path.join("Lib","site-packages","st_pnpl","DTDL"),[os.path.join("st_pnpl","DTDL","usb_device_catalog.json")]),
        # (os.path.join("Lib","site-packages","st_pnpl","DTDL","dtmi","appconfig","steval_stbox_pro"),[os.path.join("st_pnpl","DTDL","dtmi","appconfig","steval_stbox_pro","fp_sns_datalog2.json")]),
        (os.path.join("Lib","site-packages","st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1"),[os.path.join("st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1","fp_sns_datalog2.json")]),
        (os.path.join("Lib","site-packages","st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1"),[os.path.join("st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1","fp_sns_datalog2_ispu.json")]),
        (os.path.join("Lib","site-packages","st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1"),[os.path.join("st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1","fft_ultrasound_dpu.json")]),
        (os.path.join("Lib","site-packages","st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1"),[os.path.join("st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1","ispu_ces_demo.json")]),
        (os.path.join("Lib","site-packages","st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1"),[os.path.join("st_pnpl","DTDL","dtmi","appconfig","steval_stwinbx1","pnpl_ai_dpu.json")])
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
    ]
)
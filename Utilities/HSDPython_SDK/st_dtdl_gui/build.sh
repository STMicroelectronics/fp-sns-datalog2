#!/bin/bash

rm -rf dist/ st_hsdatalog.egg-info/ && \
python3 -m build

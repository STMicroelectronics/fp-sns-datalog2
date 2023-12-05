
@echo off

REM ******************************************************************************
REM * @attention
REM *
REM * Copyright (c) 2022 STMicroelectronics.
REM * All rights reserved.
REM *
REM * This software is licensed under terms that can be found in the LICENSE file
REM * in the root directory of this software component.
REM * If no LICENSE file comes with this software, it is provided AS-IS.
REM *
REM *
REM ******************************************************************************
REM
REM Welcome to HS_DataLog Command Line Interface example
REM Usage: cli_example.exe [-COMMAND [ARGS]]
REM Commands:
REM -h Show this help
REM -f <filename>: Device Configuration file (JSON)
REM -u <filename>: UCF Configuration file for MLC
REM -t <seconds>: Duration of the current acquisition (seconds)


set PATH=%PATH%;.\bin_64\

cli_example.exe -t 100

pause

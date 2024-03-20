@echo off

SET STM32CubeProgrammerPath=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin

echo This script will perform the following operations:
echo   - Restore boot operation from flash Bank 1 (default)
echo   - Erase the flash of the microcontroller
echo   - Download DATALOG2_Release.bin to the MCU
echo   - Reset the board
echo ------------------------------------------------------

pause

SET STM32CubeProgrammerPath=C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin

SET PATH=%PATH%;%STM32CubeProgrammerPath%

STM32_Programmer_CLI.exe -c port=SWD freq=8000 ap=0 -ob SWAP_BANK=0x0 -ob displ

STM32_Programmer_CLI.exe -c port=SWD freq=8000 ap=0 -e all

STM32_Programmer_CLI.exe -c port=SWD freq=8000 ap=0 -d ..\..\..\Projects\STWIN.box\Applications\DATALOG2\Binary\DATALOG2_Release.bin 0x08000000 -rst

pause

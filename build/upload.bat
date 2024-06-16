@echo off

REM Define variables
set AVRDUDE=avrdude
set PROGRAMMER=usbasp
set MCU=attiny25
set BIN_FILE=RscDroneBadge.hex
set SPEED=93750Hz

REM Check if avrdude exists
where /q %AVRDUDE%
if ERRORLEVEL 1 (
    echo avrdude not found. Please install it or add it to your PATH.
    exit /b
)

REM Set fuses
echo Setting fuses...
echo.
%AVRDUDE% -B %SPEED% -c %PROGRAMMER% -p %MCU% -e -Uefuse:w:0xFF:m -Uhfuse:w:0b11010111:m -Ulfuse:w:0xE2:m

REM Upload firmware
echo Uploading firmware...
echo.
%AVRDUDE% -B %SPEED% -c %PROGRAMMER% -p %MCU% -U flash:w:%BIN_FILE%
echo All done!
echo.

pause
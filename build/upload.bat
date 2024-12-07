@echo off

REM Ask whether to use attiny25 or attiny85
set /p MCU_SUFFIX=Which ATTINY to use (25/85)? 

if "%MCU_SUFFIX%"=="25" (
    set MCU=attiny25
    set BIN_FILE=RscDroneBadge_attiny25.hex
) else if "%MCU_SUFFIX%"=="85" (
    set MCU=attiny85
    set BIN_FILE=RscDroneBadge_attiny85.hex
) else (
    echo Invalid ATTINY selected, write 25 or 85. Exiting.
    exit /b 1
)

REM Define variables
set AVRDUDE=avrdude
set PROGRAMMER=usbasp
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

:ask_again
set /p yn=Do you want to flash it again (yes=default/no)? 
if "%yn%"=="" set yn=yes

if /i "%yn%"=="yes" (
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
    goto ask_again
) else if /i "%yn%"=="no" (
    echo Exiting.
    exit /b
) else (
    echo Please answer yes or no.
    goto ask_again
)

pause
#!/bin/bash

# Define variables
AVRDUDE="avrdude"                  # Path to avrdude executable
PROGRAMMER="usbasp"                # Programmer type
MCU="attiny25"                     # Target microcontroller
BIN_FILE="RscDroneBadge.hex" # Firmware binary file
SPEED="93750Hz"                    # Programmer speed

# Set fuses
echo "Setting fuses..."
echo ""
$AVRDUDE -B $SPEED -c $PROGRAMMER -p $MCU -e -Uefuse:w:0xFF:m -Uhfuse:w:0b11010111:m -Ulfuse:w:0xE2:m

# Upload firmware
echo "Uploading firmware..."
echo ""
$AVRDUDE -B $SPEED -c $PROGRAMMER -p $MCU -U flash:w:$BIN_FILE
echo "All done!"
echo ""

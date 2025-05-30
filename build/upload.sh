#!/bin/bash

# Ask whether to use attiny25 or attiny85
read -p "Which ATTINY to use (25/85)? " MCU_SUFFIX

if [ "$MCU_SUFFIX" == "25" ]; then
    MCU="attiny25"
elif [ "$MCU_SUFFIX" == "85" ]; then
    MCU="attiny85"
else
    echo "Invalid ATTINY selected, write 25 or 85. Exiting."
    exit 1
fi

# Ask for variant (normal or mini)
read -p "Which variant to use (normal/mini)? " VARIANT

if [ "$VARIANT" == "normal" ]; then
    BIN_FILE="RscDroneBadge_${MCU}.hex"
elif [ "$VARIANT" == "mini" ]; then
    BIN_FILE="RscDroneBadgeMini_${MCU}.hex"
else
    echo "Invalid variant selected, write normal or mini. Exiting."
    exit 1
fi

# Define variables
AVRDUDE="avrdude"   # Path to avrdude executable
PROGRAMMER="usbasp" # Programmer type
SPEED="93750Hz"     # Programmer speed

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

# Ask whether to flash it again
while true; do
    read -p "Do you want to flash it again (yes=default/no)? " yn
    yn=${yn:-yes} # Default to yes if enter is pressed
    case $yn in
    [Yy]*)
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
        ;;
    [Nn]*)
        echo "Exiting."
        exit
        ;;
    *) echo "Please answer yes or no." ;;
    esac
done

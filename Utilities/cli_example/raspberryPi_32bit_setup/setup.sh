#!/bin/bash
# Setup script for hsdatalog under Linux on Raspberry Pi 32-bit
# NOTE: unplug the board before running the script
echo "NOTE: unplug the board before running the script"

# Copy the shared library
if [ -f ../lib/libhs_datalog/raspberryPi_32bit/libhs_datalog_v2.so ]; then
    sudo cp ../lib/libhs_datalog/raspberryPi_32bit/libhs_datalog_v2.so /usr/lib
    sudo ldconfig
else
    echo "Error: libhs_datalog_v2.so not found!"
    exit 1
fi

# Copy udev rules
if [ -f 30-hsdatalog.rules ]; then
    sudo cp 30-hsdatalog.rules /etc/udev/rules.d
else
    echo "Error: 30-hsdatalog.rules file not found!"
    exit 1
fi

# Check if the group already exists
if grep -q "hsdatalog" /etc/group; then
    echo "hsdatalog group exists"
else
    # Create hsdatalog group
    echo "Adding hsdatalog group"
    sudo addgroup hsdatalog || { echo "Failed to add group"; exit 1; }
    echo "Adding user to hsdatalog group"
    sudo usermod -aG hsdatalog $USER || { echo "Failed to add user to group"; exit 1; }
fi

# Reload udev rules
echo "Reloading udev rules"
sudo udevadm control --reload || { echo "Failed to reload udev rules"; exit 1; }

# Set permissions for the cli_example binary
if [ -f ../bin_raspberryPi_32bit/cli_example ]; then
    chmod 755 ../bin_raspberryPi_32bit/cli_example
else
    echo "Error: cli_example binary not found!"
    exit 1
fi

# Download, extract, and install libusb
wget https://github.com/libusb/libusb/releases/download/v1.0.28/libusb-1.0.28.tar.bz2 || { echo "Failed to download libusb"; exit 1; }
tar xjf libusb-1.0.28.tar.bz2 || { echo "Failed to extract libusb"; exit 1; }
cd libusb-1.0.28/ || { echo "Failed to enter libusb directory"; exit 1; }
sudo apt-get install -y libudev-dev || { echo "Failed to install libudev-dev"; exit 1; }
./configure || { echo "Failed to configure libusb"; exit 1; }
sudo make install -j 4 || { echo "Failed to install libusb"; exit 1; }
sudo ldconfig
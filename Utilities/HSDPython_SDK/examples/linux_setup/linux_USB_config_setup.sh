#!/bin/bash
# Setup script for hsdatalog under Linux
# NOTE: unplug the board before running the script
echo "NOTE: unplug the board before running the script"

sudo cp ../st_hsdatalog/HSD_link/communication/libhs_datalog/linux/libhs_datalog_v1.so /usr/lib
sudo cp ../st_hsdatalog/HSD_link/communication/libhs_datalog/linux/libhs_datalog_v2.so /usr/lib

sudo cp 30-hsdatalog.rules /etc/udev/rules.d

# Check if the group already exists
if grep -q "hsdatalog" /etc/group
then
	echo "hsdatalog group exists"
else
	#create hsdatalog group
	echo "Adding hsdatalog group"
	sudo addgroup hsdatalog
	echo "Adding user to hsdatalog group"
	sudo usermod -aG hsdatalog $USER
fi

# Then restart udev
echo "Reloading udev rules"
sudo udevadm control --reload



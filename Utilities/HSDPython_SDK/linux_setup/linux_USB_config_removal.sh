#!/bin/bash
# Removes udev rules, libhs_datalog_v1.so, libhs_datalog_v2.so lib and hsdatalog user group
# NOTE: unplug the board before running the script
echo "NOTE: unplug the board before running the script"

sudo rm /usr/lib/libhs_datalog_v1.so
sudo rm /usr/lib/libhs_datalog_v2.so
echo "libhs_datalog_v1.so and libhs_datalog_v2.so libraries removed from /usr/lib"

sudo sed -i '/hsdatalog/d' /etc/group
echo "hsdatalog entry removed from /etc/group file"

sudo rm /etc/udev/rules.d/30-hsdatalog.rules
echo "30-hsdatalog.rules file removed from /etc/udev/rules.d"

sudo udevadm control --reload
echo "udev reloaded"


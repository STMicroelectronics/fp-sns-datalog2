## __How to use cli_example in RaspberryPi environment__

To use it, follow the above procedure: 

- on a RaspberryPi running 32 bit OS do **cd raspberryPi_32bit_setup**, else do **cd raspberryPi_64bit_setup**

- **dos2unix 30-hsdatalog.rules setup.sh udev_removal.sh**

- **chmod 777 setup.sh udev_removal.sh**

- Launch **./setup.sh**

- Reboot to be sure to reload USB udev rules

- Reopen a terminal from cli_example folder

- Run **./bin_RaspberryPi_32bit/cli_example** or **./bin_RaspberryPi_64bit/cli_example**

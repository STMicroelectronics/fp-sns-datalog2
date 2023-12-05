## __How to use cli_example in Linux environment__

To use it, follow the above procedure: 

- on a RaspberryPi do **cd raspberryPi_setup**, else do **cd linux_setup**

- **dos2unix 30-hsdatalog.rules setup.sh udev_removal.sh**

- **chmod 777 setup.sh udev_removal.sh**

- Launch **./setup.sh**

- Reboot to be sure to reload USB udev rules

- Reopen a terminal from cli_example folder

- Run **./USB_Datalog_Run** or **./bin_linux/cli_example**

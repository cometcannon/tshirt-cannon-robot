Instructions for deploying code to the OpenWrt (Linux) chip on the Yun
===

These instructions are intended for use in a *nix environment. They have
been tested specifically on Ubuntu 14.04. Results may vary...

Steps 1-4 are obviously one-time tasks.

Step 1: clone the openwrt-yun source
---

$ git clone https://github.com/arduino/openwrt-yun.git
$ cd openwrt-yun/

Step 2: run set-up script to install necessary packages
---

$ ./FIRST_SETUP_debian_wheezy.sh

Step 3: customize the OpenWrt configuration to include the toolchain
---

$ make menuconfig

keep all defaults except for the starring the following:

[*] Build the OpenWrt based Toolchain

exit and save the configuration

Step 4: build the OpenWrt image (takes a long time, 1-3 hr)
---

$ make

** If problems occur during the build, disable any troublesome
** packages via make menuconfig - all you really need to build
** is the toolchain

Step 5: cd to the tshirt-cannon repo
---

$ cd ~/tshirt-cannon/src/atheros/

Step 6: edit the BASE_DIR variable
---

$ vi properties.mk

BASE_DIR := /path/to/openwrt-yun/repository

Step 7: build any desired files
---
$ make -f Makefile.crosscompile

Step 8: copy the executables to the yun
---

$ scp hello root@192.168.240.1:/usr/bin/

Step 9: test everything by logging in to yun and executing your program
---

$ ssh root@192.168.240.1
$ /usr/bin/hello

Debugging
---

In case the Atheros chip does not boot up see:
http://forum.arduino.cc/index.php?topic=295642.0

Also use the backup.ino sketch in the src/AVR/backup/ to access the atheros
chip through the serial port connected to the AVR chip.

For details:
http://arduino.cc/en/Reference/YunBridgeLibrary

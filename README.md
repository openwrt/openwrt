
![Creator logo](creatorlogo.png)

# Using OpenWrt on Creator (Ci40) Marduk platform

#### For more details about the platform please visit [Creator (Ci40) Marduk](https://community.imgtec.com/platforms/creator-ci40/)
----

## Overview

OpenWrt is a highly extensible GNU/Linux distribution for embedded devices. Instead of trying to create a single, static firmware,
OpenWrt provides a fully writable filesystem with optional package management. This frees you from the restrictions of the application
 selection and configuration provided by the vendor and allows you to use packages to customize an embedded device to suit any application.
For developers, OpenWrt provides a framework to build an application without having to create a complete firmware image and distribution around it.
For users, this means the freedom of full customisation, allowing the use of an embedded device in ways the vendor never envisioned.

Creator is MIPS based platform series from Imagination and Marduk is the latest board under that series. Pistachio is the name of Imagination's SoC based on MIPS.
This guide helps as a quick start but for full details please see the OpenWrt projects
own [wiki]("http://wiki.openwrt.org/doc/start").

## Revision History

| revision  | changes from previous revision |
|---------- |------------------------------- |
| 1.0.0     | Initial release                |



## Package Content

The release distribution is structured as documented in the upstream project however
there are some key paths where IMG have added solutions:

| Folder              				| Content                                              							|
| :----               				| :----                                                							|
| target/linux/pistachio			| IMG pistachio SoC based board configs and makefiles   						|
| target/linux/pistachio/config-4.1		| IMG pistachio SoC specific kernel config							        |
| target/linux/pistachio/marduk/profiles/marduk_cc2520.mk	| IMG Marduk platform profile with TI cc2520		|
| target/linux/pistachio/marduk/profiles/marduk_ca8210.mk	| IMG Marduk platform profile with cascoda ca8210  |
## Getting started

Firstly, to obtain a copy of the (Ci40) Marduk platform supported OpenWrt source code:

 * Sign up for a Github account

 * Install Git:  ```` sudo apt-get install git ````

 * Clone the repository: ```` git clone https://github.com/FlowM2M/openwrt.git ````

To simply make a build based on the IMG default config run the following commands:

    $ cd openwrt
    $ ./scripts/feeds update -a
    $ ./scripts/feeds install -a
_Ignore any "WARNING: No feed for package..." from the install feeds step._

Load Marduk platform specific openwrt configuration for Pistachio.

    $ make menuconfig

1. Select the "Target System" as IMG MIPS Pistachio

        Target System (Atheros AR7xxx/AR9xxx) --->(X) IMG MIPS Pistachio

2. Check the "Target Profile" is set to Basic platform profile for Marduk

        Target Profile (Basic platform profile for Marduk)  ---> (X) Basic platform profile for Marduk with TI cc2520
                                                                 ( ) Basic platform profile for Marduk with Cascoda ca8210

You can also load Marduk platform specific openwrt configuration for Pistachio by adding following into .config.

    $ echo "CONFIG_TARGET_pistachio=y" > .config
    $ echo "CONFIG_TARGET_pistachio_marduk_marduk_cc2520=y" >> .config


Now build OpenWrt in standard way:

    $ make V=s -j1

Once the build is completed, you will find the resulting output i.e. images, dtbs and rootfs at "bin/pistachio", depending upon the selected profile.
- openwrt-pistachio-pistachio_marduk_cc2520-uImage
- openwrt-pistachio-pistachio_marduk_cc2520-uImage-initramfs
- openwrt-pistachio-marduk-marduk_cc2520-rootfs.tar.gz
- pistachio_marduk_cc2520.dtb

## Customising your OpenWrt
You can configure OpenWrt from scratch but it's best to start from a base profile
e.g. the one for the IMG pistachio board as it has some useful defaults.

Simply select the package you need and add into the marduk profile:

    $ vi target/linux/pistachio/marduk/profiles/marduk_cc2520.mk

If you want to add new package, then simply add package name in the list of packages:

    define Profile/marduk
    NAME:=Basic platform profile for Marduk?
    PACKAGES:=kmod-i2c kmod-marduk-cc2520 kmod-sound-pistachio-soc \
                wpan-tools tcpdump uhttpd uboot-envtools \
                alsa-lib alsa-utils alsa-utils-tests

    endef


Please ensure to remove .config, tmp folders before running make menuconfig.

    $ make menuconfig

Verify that package name is selected. Just save and exit.

## Customising your linux kernel

Kernel configuration is saved at target/linux/pistachio/config-4.1
e.g. This the one for the IMG pistachio board as it has some useful defaults.

To customise to suit your requirements use the following command:

    $ make kernel_menuconfig

If you change any option you need and then save & quit GUI, the changed configuration will get written into target/linux/pistachio/config-4.1

For more details please refer [OpenWrt Build System]("http://wiki.openwrt.org/doc/howto/build")

## Serial console
Connect the Marduk board to development computer over serial port. Open a serial console on host PC.

- In Windows PuTTY, HyperTerminal, or any other free application can be used.

- In Linux, the simplest option is to use the miniterm.py application that comes bundled with Python.

First work out which device you want, you can see them by running:

    $ ls /dev/ttyUSB*

Then connect to it (remember to choose the correct device listed in above command):

    $ sudo miniterm.py /dev/ttyUSB0 -b 115200

## Instructions for putting root filesystem on usb and booting from usb drive
Firstly you need to format USB drive into ext4 parition. Partition your media with one big ext4 partion (use dos partition table)
You can check the partition number by following way:


`$ mount | grep /media
/dev/sdx1 on /media/user/43e3311e-7b95-4693-bfcd-b07fa4590a0d type ext4 (rw,nosuid,nodev,uhelper=udisks2)`

Alternatively, you can also check the output of the dmesg command.

Next you need to ensure that it has a DOS partition table containing a Linux ext4 partition.
<b>IMPORTANT! Make sure that you replace /dev/sdx for the right device name, e.g. /dev/sdb</b>

Unmount the filesystem before creating new partition table:

    $ sudo umount /dev/sdx?

Create new partition table:

    $ sudo fdisk /dev/sdx
    > enter 'o' to create a new empty DOS partition table
    > enter 'n' to create a new partition; just press return at every prompt to accept the default values
    > enter 'w' to write the new partition table and exit
    $ sudo mkfs.ext4 /dev/sdx1

Mount the usb drive:

    $ sudo mount /dev/sdx1 /mnt/

To put filesystem on USB you will need 'openwrt-pistachio-marduk-marduk_cc2520-rootfs.tar.gz' which is available at "openwrt/bin/pistachio"

Extract the openwrt-pistachio-marduk-marduk_cc2520-rootfs.tar.gz onto the partition you just created

    $ sudo rm -rf /mnt/*
    $ sudo tar -xf bin/pistachio/openwrt-pistachio-marduk-marduk_cc2520-rootfs.tar.gz -C /mnt/
    $ sudo umount /mnt/


Run "sync" command to synchronize the data properly on the USB drive.

    $ sync


Connect the USB drive and power on the board.

Press Enter on the serial console to cancel the autoboot and to get the uboot prompt. Check if the all the environment variables are setup properly on the uboot prompt or not

    $ pistachio # printenv

Type following command to boot from usb.

    $ pistachio # run usbboot

## Boot from SD Card
Similar to the instructions for Boot from usb drive, you need to format the SD card into ext4 parition. Follow the steps mentioned above by making subtle changes


`$ mount | grep /media
/dev/sdc1 on /media/user/5c0dac14-1d47-4a39-8b06-e27861473670 type ext4 (rw,nosuid,nodev,uhelper=udisks2)'

SD media essentially mounts at /dev/sdc1 on a computer.  However, make sure to double-check if it gets somehow mounted differently on your computer, in which case, <b>IMPORTANT! Make sure that you replace /dev/sdc with /dev/sdx for the right device name</b>

Unmount the filesystem before creating new partition table:

    $ sudo umount /dev/sdc1

Create new partition table:

    $ sudo fdisk /dev/sdc
    > enter 'o' to create a new empty DOS partition table
    > enter 'n' to create a new partition; just press return at every prompt to accept the default values
    > enter 'w' to write the new partition table and exit
    $ sudo mkfs.ext4 /dev/sdc1

Mount the SD media:

    $ sudo mount /dev/sdc1 /mnt/

To put filesystem on SD you will need 'openwrt-pistachio-marduk-marduk_cc2520-rootfs.tar.gz' which is available at "openwrt/bin/pistachio"

Extract the openwrt-pistachio-marduk-marduk_cc2520-rootfs.tar.gz onto the partition you just created

    $ sudo rm -rf /mnt/*
    $ sudo tar -xf bin/pistachio/openwrt-pistachio-marduk-marduk_cc2520-rootfs.tar.gz -C /mnt/
    $ sudo umount /mnt/

Run "sync" command to synchronize the data properly on the SD card.

    $ sync

Connect the SD card in the slot and power on the board.

Press Enter on the serial console to cancel the autoboot and to get the uboot prompt. Check if the all the environment variables are setup properly on the uboot prompt or not

    $ pistachio # printenv

Type following command to boot from SD.

    $ pistachio # run mmcboot

## TFTP Boot
For TFTP boot, we need TFTP server serving kernel image (uImage), dtb (*.dtb)
and initramfs filesystem.

    $ sudo cp bin/pistachio/openwrt-pistachio-pistachio_marduk_cc2520-uImage-initramfs /tftpboot/uImage
    $ sudo cp bin/pistachio/pistachio_marduk_cc2520.dtb /tftpboot

### Setting up TFTP Server

####Install tftpd package:

    $ sudo apt-get install tftpd

Create /etc/xinetd.d/tftp with following contents:
    
    service tftp
    {
     protocol        = udp
     port            = 69
     socket_type     = dgram
     wait            = yes
     user            = root
     server          = /usr/sbin/in.tftpd
     server_args     = /tftpboot
     disable         = no
    }

Now restart service:

    $ sudo service xinetd restart

####Configure Uboot

Now use Serial Console to connect device to host PC. Switch on device and press any key in first 2 seconds. After that you will drop to a u-boot console.

To use tftp boot, set following environment variables.

    $ pistachio # setenv ethaddr <xx:xx:xx:xx:xx:xx> # Set mac address for Ethernet
(To generate mac address use command 

    "echo "e2:"`od /dev/random -N5 -t x1 -An | sed 's/ /:/g' | cut -c1 --complement`"

 on host machine)
However beware of running out of IP addresses if we keep on switching the mac addresses.

    $ pistachio # setenv serverip <server_ip> # Server IP address where TFTP and an NFS server is running
    $ pistachio # saveenv # Save environment variables

_note: No need to set these environment variables for next boot since these are already saved._

Now start tftp boot:

    $ pistachio # run ethboot


You should see the logs on the console as below:


~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
  BusyBox v1.24.1 (2016-03-04 17:43:09 IST) built-in shell (ash)

  _______                     ________        __
 |       |.-----.-----.-----.|  |  |  |.----.|  |_
 |   -   ||  _  |  -__|     ||  |  |  ||   _||   _|
 |_______||   __|_____|__|__||________||__|  |____|
          |__| W I R E L E S S   F R E E D O M
 -----------------------------------------------------
 DESIGNATED DRIVER (Bleeding Edge, r48138)
 -----------------------------------------------------
  * 2 oz. Orange Juice         Combine all juices in a
  * 2 oz. Pineapple Juice      tall glass filled with
  * 2 oz. Grapefruit Juice     ice, stir well.
  * 2 oz. Cranberry Juice

$root@OpenWrt:/#
$root@OpenWrt:/#
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


You can check "ifconfig -a" to check list of interfaces. Ethernet, WiFi and 6loWPAN should be up.

####Note:

1. 6loWPAN IP has been hardcoded to 2001:1418:0100::1/48. You can change that by editing /etc/config/network script and restarting it.

        $root@OpenWrt:/# /etc/init.d/network restart

2. You can set ssid and password for WiFi either at compile time from file target/linux/pistachio/base-files/etc/uci-defaults/config/wireless


        config wifi-iface
            option device       radio0
            option network      sta
            option mode         sta
            option ssid         <XYZ>
            option encryption   psk2
            option key          <Password>

    OR after booting update /etc/config/wireless as above and restart the network by running following command from CLI.

        $root@OpenWrt:/# /etc/init.d/network restart

### Known Issues:

- Cleaned up kernel patches will be upstreamed soon.
- Booting from flash is not included.
- OPKG support is not implemented.



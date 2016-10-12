
![Creator logo](creatorlogo.png)

# Using OpenWrt on Creator Ci40 (Marduk) platform

#### For more high-level details about the platform please visit the [Creator Ci40 website](https://community.imgtec.com/platforms/creator-ci40/)
----

## Overview

OpenWrt is a highly extensible GNU/Linux distribution for embedded devices. Instead of trying to create a single, static firmware,
OpenWrt provides a fully writable filesystem with optional package management. This frees you from the restrictions of the application
 selection and configuration provided by the vendor and allows you to use packages to customize an embedded device to suit any application.
For developers, OpenWrt provides a framework to build an application without having to create a complete firmware image and distribution around it.
For users, this means the freedom of full customisation, allowing the use of an embedded device in ways the vendor never envisioned.

Creator is a MIPS based developer program from Imagination Technologies. Creator Ci40 (Marduk) is the latest development board in the Creator program. Pistachio is the name of the Imagination Technologies SoC that Ci40 is uses as it's core.
This guide helps as a quick start but for full details about OpenWrt please see the [OpenWrt project wiki](http://wiki.openwrt.org/doc/start).



## This readme contains the basics of building and modifying OpenWrt for Ci40. **You will find much more documentation on the [Creator Documentation website](https://docs.creatordev.io/ci40/guides/openwrt-platform/).**



## Package Content

The release distribution is structured as documented in the upstream project however
there are some key paths where IMG have added solutions:

| Folder              				| Content                                              							|
| :----               				| :----                                                							|
| target/linux/pistachio			| IMG pistachio SoC based board configs and makefiles   						|
| target/linux/pistachio/config-4.1		| IMG pistachio SoC specific kernel config							        |
| target/linux/pistachio/marduk/profiles/marduk_cc2520.mk	| IMG Marduk platform profile with TI cc2520	|
| target/linux/pistachio/marduk/profiles/marduk_ca8210.mk	| IMG Marduk platform profile with Cascoda ca8210  |

***Note that mass production Ci40 boards use the Cascoda ca8210 chip, so assume the use of this profile unless you have been personally informed otherwise***

## Getting started

Firstly, to obtain a copy of the (Ci40) Marduk platform supported OpenWrt source code:

 * Sign up for a GitHub account

 * Install Git:  ```` sudo apt-get install git ````

 * Clone the repository: ```` git clone https://github.com/CreatorDev/openwrt.git ````

Install build dependencies:

    $ sudo apt-get install libncurses5-dev libncursesw5-dev zlib1g-dev libssl-dev gawk subversion device-tree-compiler

To make a build based on the default Ci40 config, run the following commands:

    $ cd openwrt
    $ ./scripts/feeds update -a
    $ ./scripts/feeds install -a

_Ignore any "WARNING: No feed for package..." from the install feeds step._

Set the default configuration for Ci40 by running the following command (this sets version numbers and OPKG URLs):

    $ cat target/linux/pistachio/creator-platform-default-cascoda.config > .config

Complete the configuration process by running menuconfig:

    $ make menuconfig

1. Check that "Target System" is IMG MIPS Pistachio

        Target System (IMG MIPS Pistachio)  --->

2. Check the "Target Profile" is set to Basic platform profile for Marduk with Cascoda ca8210

        Target Profile (Basic platform profile for Marduk with Cascoda ca8210)  --->

3. Save and Exit menuconfig

Optionally, you can add a git revision number as DISTRIB_REVISION in the openwrt image by doing the following:

    $ getver.sh . > version

Now build OpenWrt with one of the following commands (see the gnu make documentation for more options):

    $ make

or

    $ make V=s                       // Verbose mode

Once the build is completed, you will find the resulting output i.e. images, dtbs and rootfs at "bin/pistachio".

Example output for V0.9.4:

- openwrt-0.9.4-pistachio-pistachio_marduk_ca8210-uImage
- openwrt-0.9.4-pistachio-pistachio_marduk_ca8210-uImage-initramfs
- openwrt-0.9.4-pistachio-marduk-marduk_ca8210-rootfs.tar.gz
- pistachio_marduk_ca8210.dtb

By default the version number is blank if you do not use the creator-platform-default-cascoda.config for loading the configuration.

## Customising your OpenWrt
You can configure OpenWrt from scratch but it's best to start from a base profile
e.g. the one for the IMG pistachio board as it has some useful defaults.

Simply select the package you need and add into the marduk profile:

    $ vi target/linux/pistachio/marduk/profiles/marduk_ca8210.mk

If you want to add new package, then simply add package name in the list of packages:

    define Profile/marduk
    NAME:=Basic platform profile for Marduk with Cascoda ca8210
    PACKAGES:=kmod-i2c kmod-ca8210 kmod-sound-pistachio-soc \
                wpan-tools tcpdump uhttpd uboot-envtools \
                alsa-lib alsa-utils alsa-utils-tests \
                iw hostapd wpa-supplicant kmod-uccp420wlan kmod-cfg80211
    endef

Please ensure to remove .config, tmp folders before running make menuconfig.

    $ make menuconfig

Verify that package name is selected. Just save and exit.

## Customising your Linux Kernel

Kernel configuration is saved at target/linux/pistachio/config-4.1
e.g. This the one for the IMG pistachio board as it has some useful defaults.

To customise to suit your requirements use the following command:

    $ make kernel_menuconfig

If you change any option you need and then save & quit GUI, the changed configuration will get written into target/linux/pistachio/config-4.1

For more details please refer to [OpenWrt Build System]("http://wiki.openwrt.org/doc/howto/build")

## Adding Linux Kernel patches
Linux kernel patches are added at :-

    target/linux/pistachio/patches-<kernel_version>/

* All kernel patches are created from the linux kernel hosted [here](https://github.com/CreatorDev/linux).
* Kernel patches for specific kernel version used in OpenWrt are prepared from corresponding branches in [linux](https://github.com/CreatorDev/linux) repo. e.g.

        target/linux/pistachio/patches-4.1/

contains patches created from [openwrt-4.1.13](https://github.com/CreatorDev/linux/tree/openwrt-4.1.13) branch.
* For adding the kernel patch in OpenWrt, create a PR in [linux](https://github.com/CreatorDev/linux) with the change, and also create a PR with the patch of the change in OpenWrt repository.
* Following command can be used for creating the kernel patch :-

        git format-patch <commit_id> --keep-subject --start-number <number>

*NOTE :* Number should the next one from the last patch already added in OpenWrt.

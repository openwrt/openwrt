
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


## Using this documentation

This readme contains the basics of building and modifying OpenWrt for Ci40. *You will find much more documentation on the [Creator Documentation website](https://docs.creatordev.io/ci40/introduction/dev-guide/).*

## Package Content

The release distribution is structured as documented in the upstream project however
there are some key paths where IMG have added solutions:

| Folder              				| Content                                              							|
| :----               				| :----                                                							|
| target/linux/pistachio			| IMG pistachio SoC based board configs and makefiles   						|
| target/linux/pistachio/config-4.1		| IMG pistachio SoC specific kernel config							        |
| target/linux/pistachio/marduk/profiles/marduk_cc2520.mk	| IMG Marduk platform profile with TI cc2520	|
| target/linux/pistachio/marduk/profiles/marduk_ca8210.mk	| IMG Marduk platform profile with Cascoda ca8210  |

## Getting started

Firstly, to obtain a copy of the (Ci40) Marduk platform supported OpenWrt source code:

 * Sign up for a GitHub account

 * Install Git:  ```` sudo apt-get install git ````

 * Clone the repository: ```` git clone https://github.com/CreatorDev/openwrt.git ````

To simply make a build based on the IMG default config run the following commands:

    $ cd openwrt
    $ ./scripts/feeds update -a
    $ ./scripts/feeds install -a

_Ignore any "WARNING: No feed for package..." from the install feeds step._

Load Marduk platform specific OpenWrt configuration for Pistachio.

    $ make menuconfig

1. Select the "Target System" as IMG MIPS Pistachio

        Target System (Atheros AR7xxx/AR9xxx) --->(X) IMG MIPS Pistachio

2. Check the "Target Profile" is set to Basic platform profile for Marduk

        Target Profile (Basic platform profile for Marduk)  --->
            (X) Basic platform profile for Marduk with TI cc2520
            ( ) Basic platform profile for Marduk with Cascoda ca8210

Alternatively, you can use default configuration for Marduk platform with TI cc2520 specific OpenWrt configuration for IMG Pistachio by copying following into .config file:

    $ cat target/linux/pistachio/creator-platform-default.config > .config

Similarly, you can do the same for Marduk platform with Cascoda ca8210 specific OpenWrt configuration:

    $ cat target/linux/pistachio/creator-platform-cascoda-default.config > .config

You can add git revision number as DISTRIB_REVISION in the openwrt image by doing following:

    $ getver.sh . > version

Now build OpenWrt in standard way:

    $ make V=s -j1

Once the build is completed, you will find the resulting output i.e. images, dtbs and rootfs at "bin/pistachio", depending upon the selected profile.
Where PROFILE can be marduk_cc2520, marduk_ca8210, marduk_cc2520_wifi and VERSION is whatever mentioned in CONFIG_VERSION_NUMBER.
By default VERSION is blank if you do not use the creator-platform-default.config for loading the configuration.

- openwrt-$(VERSION)-pistachio-pistachio_$(PROFILE)-uImage
- openwrt-$(VERSION)-pistachio-pistachio_$(PROFILE)-uImage-initramfs
- openwrt-$(VERSION)-pistachio-marduk-$(PROFILE)-rootfs.tar.gz
- pistachio_marduk_cc2520.dtb (for marduk_cc2520 board) or
- pistachio_marduk_ca8210.dtb (for marduk_ca8210 board)

For simplicity, let's assume that marduk_cc2520 PROFILE has been selected and VERSION as 1.0.0 hence the filenames will be as follows: 
- openwrt-1.0.0-pistachio-pistachio_marduk_cc2520-uImage
- openwrt-1.0.0-pistachio-pistachio_marduk_cc2520-uImage-initramfs
- openwrt-1.0.0-pistachio-marduk-marduk_cc2520-rootfs.tar.gz
- pistachio_marduk_cc2520.dtb

## Customising your OpenWrt
You can configure OpenWrt from scratch but it's best to start from a base profile
e.g. the one for the IMG pistachio board as it has some useful defaults.

Simply select the package you need and add into the marduk profile:

    $ vi target/linux/pistachio/marduk/profiles/marduk_cc2520.mk

If you want to add new package, then simply add package name in the list of packages:

    define Profile/marduk
    NAME:=Basic platform profile for Marduk with TI cc2520
    PACKAGES:=kmod-i2c kmod-marduk-cc2520 kmod-sound-pistachio-soc \
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

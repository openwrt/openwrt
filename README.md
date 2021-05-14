Overview
========

This repo is maintained by GL.iNet team, which is used to release stock firmware.

Feature
=======

- Support latest device of GL.iNet
- Support kernel driver which isn't support by kernel-tree
- Keep updating with stock firmware

Branches Introduction
=======
- **openwrt-18.06-siflower** Only supports SF1200

- **openwrt-18.06-s1300** Support compiling openwrt firmware for S1300

    If you need to flash OpenWrt firmware on S1300, you need to modify the partition table using the intermediate firmware in this branch

- **openwrt-18.06** Compile versions before 3.105 firmware based on this source code

- **openwrt-18.06.5** Compile version 3.105 firmware based on this source code

- **openwrt-19.07.7** Compile version 3.201 firmware based on this source code

- **openwrt-trunk** Compile S1300 firmware supporting emmc

**For example, if you want to use openwrt-19.07.7 to compile the production firmware, you need to use *```git checkout openwrt-19.07.7```* command to switch openwrt-19.07.7 branch.**

Product Branch Relationship Table
=======
**Support Branch:** Branches that support this product

**Official OpenWrt :** Official OpenWrt supports this product from the current release
| Product | Support Branch | Official OpenWrt | Remark |
| :-----| :----- | :---- | :---- |
| AR150 | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | >17.01 |  |
| MIFI | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | >17.01 |  |
| AR300M | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | 17.01~19.07(nor)^<br>>21.02(nor+nand)^ | |
| MT300N-V2 | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | >18.06 | GL fireware wifi drivers are closed source, we do not guarantee that OpenWrt drivers are stable |
| B1300 | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | >18.06 | GL fireware use QSDK, if you use openwrt to compile firmware, there isn't mesh function|
| USB150 | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | >18.06 |  |
| AR750 | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | >18.06 |  |
| AR750S | openwrt-18.06<br>openwrt-18.06.5<br>openwrt-19.07.7 | 19.07(nor)^<br>>21.02(nor+nand)^ | |
| X750 | openwrt-18.06<br>openwrt-19.07.7 | >19.07 |  |
| S1300 | openwrt-19.07.7(nor)^<br>openwrt-trunk(emmc)^ | >21.02(nor)^ | |
| N300 | openwrt-18.06<br>openwrt-19.07.7 | >21.02 | GL fireware wifi drivers are closed source, we do not guarantee that OpenWrt drivers are stable |
| X1200 | openwrt-18.06<br>openwrt-19.07.7 | N | Must choose ath10k-firmware-qca9888-ct-htt and kmod-ath10k-ct packages |
| MV1000 | openwrt-19.07.7 | >21.02 |  |
| E750 | openwrt-18.06<br>openwrt-19.07.7 | >21.02 |  |
| AP1300 | We don't make patch for this project, so you must use the official OpenWrt | >21.02 |  |
| B2200 |  |  |  |
| MT1300 | openwrt-19.07.7 | >21.02 | GL fireware wifi drivers are closed source, we do not guarantee that OpenWrt drivers are stable |
| XE300 | openwrt-19.07.7 | N |  |
| X300B | openwrt-18.06.5<br>openwrt-19.07.7 | N |  |
| SF1200 |  |  |  |
| AX1800 |  |  |  |

^nor: Compiled firmware can only run on nor flash

^nor+nand: Can compile the firmware that runs on nor flash and nand flash

^nor+emmc: Can compile the firmware that runs on nor flash and emmc

Prerequisites
=============

To build your own firmware you need to have access to a Linux, BSD or MacOSX system (case-sensitive filesystem required). Cygwin will not be supported because of the lack of case sensitiveness in the file system. Ubuntu is usually recommended.

Installing Packages
-------------------

```bash
$ sudo apt-get update
$ sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev
```

Downloading Source
------------------

```
$ git clone https://github.com/gl-inet/openwrt.git openwrt
$ cd openwrt
```

Updating Feeds
--------------

```
$ ./scripts/feeds update -a
$ ./scripts/feeds install -a
```

Note that if you have all the source already, just put them in your *openwrt/dl* folder and you save time to download resource.

Clear temp buffer
-----------------

```
$ rm ./tmp -rf
```


Compile firmware for NOR flash
=======
Suitable for all products

Select target
-------------
Issueing **make menuconfig** to select a GL.iNet device, for example AR300M.

```
$ make menuconfig
```

Please select options as following:

	Target System (Atheros AR7xxx/AR9xxx)  --->

	Subtarget (Generic)  --->

	Target Profile (GL-AR300M)  --->

Then select common software package (as you need),such as USB driver as following,

    GL.iNet packages choice shortcut  --->

       [ ] Select basic packages
           Select VPN  --->
       [*] Support storage
       [*] Support USB
       [ ] Support webcam
       [ ] Support rtc

If the package you want to brush depends on the GL base package, please Select **Select basic packages** as well. Which packages the GL base package contains can be found in *config/config-glinet.in*

Compile
-------
Simply running **make V=s -j5** will build your own firmware. It will download all sources, build the cross-compile toolchain, the kernel and all choosen applications which is spent on several hours.

```
$ make V=s -j5
```


Notice **V=s**, this parameter is purpose to check info when compile.
**-j5**, this parameter is for choosing the cpu core number, 5 means using 4 cores.
If there’s error, please use **make V=s -j1** to recompile, and check the error.

Target file location for NOR flash
-----------------------------------
The final firmware file is **bin/ar71xx/openwrt-ar71xx-generic-gl-ar300m-squashfs-sysupgrade.bin**
so this file is the firmware we need, please update firmware again.
Please refer to other instructions for further operations. Such as flash the firmware, etc.


Compile firmware for NAND flash
=====
Applicable to GL-AR300M GL-AR750S GL-E750 GL-X1200 GL-X750

Select target
-------------
Issueing **make menuconfig** to select a GL.iNet device, for example AR300M.

```
$ make menuconfig
```

Please select options as following:

	Target System (Atheros AR7xxx/AR9xxx)  --->

	Subtarget (Generic devices with NAND flash)  --->

	Target Profile (GL-AR300M NAND)  --->

Then select common software package (as you need),such as USB driver as following,

    GL.iNet packages choice shortcut  --->

       [ ] Select basic packages
           Select VPN  --->
       [*] Support storage
       [*] Support USB
       [ ] Support webcam
       [ ] Support rtc

If the package you want to brush depends on the GL base package, please Select **Select basic packages** as well. Which packages the GL base package contains can be found in *config/config-glinet.in*

Compile
-------
Simply running **make V=s -j5** will build your own firmware. It will download all sources, build the cross-compile toolchain, the kernel and all choosen applications which is spent on several hours.

```
$ make V=s -j5
```

Notice **V=s**, this parameter is purpose to check info when compile.
**-j5**, this parameter is for choosing the cpu core number, 5 means using 4 cores.
If there’s error, please use **make V=s -j1** to recompile, and check the error.

Target file location for NAND flash
-----------------------------------

The final firmware file is:

**bin/ar71xx/openwrt-ar71xx-nand-gl-ar300m-rootfs-squashfs.ubi**

**bin/ar71xx/openwrt-ar71xx-nand-gl-ar300m-squashfs-sysupgrade.tar**

So this file is the firmware we need, please update firmware again.
Please refer to other instructions for further operations. Such as flash the firmware, etc.

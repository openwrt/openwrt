```
  _______                     ________        __
 |       |.-----.-----.-----.|  |  |  |.----.|  |_
 |   -   ||  _  |  -__|     ||  |  |  ||   _||   _|
 |_______||   __|_____|__|__||________||__|  |____|
          |__| W I R E L E S S   F R E E D O M
 -----------------------------------------------------
		     ____  ______________    ____ 
		    / __ \/ ___/_  __/   |  / __ \
		   / / / /\__ \ / / / /| | / /_/ /
		  / /_/ /___/ // / / ___ |/ _, _/ 
		  \____//____//_/ /_/  |_/_/ |_| MMDVM Suite
  
```

This is the buildsystem for the OpenWrt Linux distribution.

To build your own firmware you need a Linux, BSD or MacOSX system (case
sensitive filesystem required). Cygwin is unsupported because of the lack
of a case sensitive file system.

You need gcc, binutils, bzip2, flex, python, perl, make, find, grep, diff,
unzip, gawk, getopt, subversion, libz-dev and libc headers installed.

1. Run "./scripts/feeds update -a" to obtain all the latest package definitions
defined in feeds.conf / feeds.conf.default
```bash
cp feeds.conf.default feeds.conf
./scripts/feeds update -a

```

2. Run "./scripts/feeds install -a" to install symlinks for all obtained
packages into package/feeds/
```bash
./scripts/feeds install luci-base
./scripts/feeds install luci-compat
./scripts/feeds install luci-mod-status
./scripts/feeds install luci-mod-system
./scripts/feeds install luci-proto-ipv6
./scripts/feeds install luci-app-firewal
./scripts/feeds install luci-i18n-firewall-zh-cn
./scripts/feeds install stm32flash
./scripts/feeds install -a -pmmdvm

```

Optional for MT76x8 
```
./scripts/feeds install travelmate
./scripts/feeds install luci-app-travelmate
./scripts/feeds install luci-i18n-travelmate-zh-cn
```

Optional for X86_64
```
./scripts/feeds luci-mod-admin-full
```

3. Run "make menuconfig" to select your preferred configuration for the
toolchain, target system & firmware packages.
```bash
cp config.mt76x8 .config
make menuconfig
```

4. Run "make" to build your firmware. This will download all sources, build
the cross-compile toolchain and then cross-compile the Linux kernel & all
chosen applications for your target system.
```bash
make -j4
```

73, Michael BD7MQB

Sunshine!

## License 

This software is licenced under the GPL v2 and is primarily intended for amateur and educational use.

Disclaimer: This software was written as a personal hobby and I am not responsible for products that use this software. Contributions from anyone are welcome.

免责声明：编写此软件纯属个人爱好，我不对使用此软件的产品负责


我的APP源码：https://github.com/Lienol/openwrt-package

本源码生成的固件禁止使用在任何非法、商业用途！

[OpenWRT讨论群](https://t.me/joinchat/IUAfG09LxGyUlfBz8YZNWA)

编译命令如下（引用大雕的README）:

1. 首先装好 Ubuntu 64bit，推荐  Ubuntu  14 LTS x64

2. 命令行输入 sudo apt-get update ，然后输入
sudo apt-get -y install build-essential asciidoc binutils bzip2 gawk gettext git libncurses5-dev libz-dev patch unzip zlib1g-dev lib32gcc1 libc6-dev-i386 subversion flex uglifyjs git-core gcc-multilib p7zip p7zip-full msmtp libssl-dev texinfo libglib2.0-dev xmlto qemu-utils upx libelf-dev autoconf automake libtool autopoint device-tree-compiler

3. git clone -b dev-19.07 https://github.com/Lienol/openwrt 命令下载好源代码，然后 cd openwrt 进入目录

4. ./scripts/feeds clean

   ./scripts/feeds update -a
   
   ./scripts/feeds install -a
   
   make menuconfig 

5. 最后选好你要的路由，输入 make -j1 V=s （-j1 后面是线程数。第一次编译推荐用单线程，国内请尽量全局科学上网）即可开始编译你要的固件了。

你可以自由使用，但源码编译二次发布请注明我的 GitHub 仓库链接。谢谢合作！
 
 -----------------------------------------------------
 
My app source code: https://github.com/Lienol/openwrt-package

This is the buildsystem for the OpenWrt Linux distribution.

To build your own firmware you need a Linux, BSD or MacOSX system (case
sensitive filesystem required). Cygwin is unsupported because of the lack
of a case sensitive file system.

You need gcc, binutils, bzip2, flex, python, perl, make, find, grep, diff,
unzip, gawk, getopt, subversion, libz-dev and libc headers installed.

1. Run "./scripts/feeds update -a" to obtain all the latest package definitions
defined in feeds.conf / feeds.conf.default

2. Run "./scripts/feeds install -a" to install symlinks for all obtained
packages into package/feeds/

3. Run "make menuconfig" to select your preferred configuration for the
toolchain, target system & firmware packages.

4. Run "make" to build your firmware. This will download all sources, build
the cross-compile toolchain and then cross-compile the Linux kernel & all
chosen applications for your target system.

Sunshine!
	Your OpenWrt Community
	http://www.openwrt.org



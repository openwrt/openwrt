# Original

This is the buildsystem for the LEDE Linux distribution.

Please use "make menuconfig" to choose your preferred
configuration for the toolchain and firmware.

You need to have installed gcc, binutils, bzip2, flex, python, perl, make,
find, grep, diff, unzip, gawk, getopt, subversion, libz-dev and libc headers.

Run "./scripts/feeds update -a" to get all the latest package definitions
defined in feeds.conf / feeds.conf.default respectively
and "./scripts/feeds install -a" to install symlinks of all of them into
package/feeds/.

Use "make menuconfig" to configure your image.

Simply running "make" will build your firmware.
It will download all sources, build the cross-compile toolchain, 
the kernel and all choosen applications.

To build your own firmware you need to have access to a Linux, BSD or MacOSX system
(case-sensitive filesystem required). Cygwin will not be supported because of
the lack of case sensitiveness in the file system.


Sunshine!
	Your LEDE Community
	http://www.lede-project.org


# Add

## 编译示例

本 fork 将 LEDE 移植到 NXP 的 i.MX6ULL 上。

以下示例操作所使用的操作系统为 Ubuntu 18.04 LTS：

1. 安装编译环境
	```shell
	sudo apt-get install gcc git libssl-dev openssl libz-dev asciidoc gawk zlib1g-dev ncurses-term libncurses5-dev subversion sharutils unzip texinfo gettext autoconf make bison flex bzip2 patch binutils g++ lzop
	```

2. 下载源码
	```shell
	git clone https://github.com/Shanyaoxing12/openwrt.git
	```

3. 进入源码目录
	```shell
	cd openwrt
	```

4. 更新并安装软件包
	```shell
	./scripts/feeds update -a
	./scripts/feeds install -a 
	```

5. 选择编译内容
	```shell
	make menuconfig
	```

6. 开始编译
	```shell
	make V=s
	```

编译成功后的文件位于 bin 文件夹中

## 编译错误

若出现
```shell
	in6.h:32:8: error: redefinition of 'struct in6_addr'
 	struct in6_addr {
	...
```
则将 .../openwrt/staging_dir/toolchain-arm\_cortex-a7+neon-vfpv4\_gcc-5.4.0\_musl-1.1.16\_eabi/include/linux/if\_bridge.h 中的 \#include <linux/in6.h> 修改为 \#include <netinet/in.h>


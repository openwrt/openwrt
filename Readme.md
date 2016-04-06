This is the buildsystem for the OpenWrt Linux distribution.

You need to have installed the following tools
```
$ sudo apt-get install gcc binutils bzip2 flex python perl 
$ sudo apt-get isntall make findutils grep diffutils unzip gawk subversion zlib1g-dev build-essential
```
Run to get all the latest package definitions defined in feeds.conf / feeds.conf.default respectively, and install symlinks of all of them into
package/feeds/.
```
$ ./scripts/feeds update -a
$ ./scripts/feeds install -a 
```
Use "make menuconfig" to configure your image. 'intorobot-default.config' is provided for you reference
```
$ make defconfig & make prereq
$ cp intorobot-default.config .config
$ make menuconfig
```

Simply running "make" will build your firmware. It will download all sources, build the cross-compile toolchain, 
the kernel and all choosen applications. **Note: Please make sure you can access google, cause some source files need to be downloaded (翻翻翻）**
```
$ make
```


**If you have difficulty to download files, please tell us  (chy at molmc dot com)**

You can use "scripts/flashing/flash.sh" for remotely updating your embedded
system via tftp.

The OpenWrt system is documented in docs/. You will need a LaTeX distribution
and the tex4ht package to build the documentation. Type "make -C docs/" to build it.

To build your own firmware you need to have access to a Linux, BSD or MacOSX system
(case-sensitive filesystem required). Cygwin will not be supported because of
the lack of case sensitiveness in the file system.


Sunshine!
	Your OpenWrt Project
	http://openwrt.org



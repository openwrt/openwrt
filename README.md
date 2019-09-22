Overview  
======== 

This repo is a fork for Private use of GL.iNet team's https://github.com/gl_inet/openwrt, which is used to release stock firmware.  

Prerequisites  
=============  

```bash  
Ubuntu is recommended. tested on 18.04LTS in Azure

Installing Packages 
=============  

```bash  
$ sudo apt-get update
$ sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev unzip
$ sudo upgrade
```  

Downloading Source  
------------------  

```  
$ git clone https://github.com/mattytap/openwrt.git openwrt
$ cd openwrt
$ git pull
Should return something like 
  From https://github.com/mattytap/openwrt
  6609299fd3..54bf236dc6  mattytap-18.06-patch-1 -> origin/mattytap-18.06-patch-1
  Already up to date.

git checkout mattytap-18.06-patch-1

```  

Updating Feeds  
--------------  

```
$ ./scripts/feeds update -a
run this again a couple of times just to be sure
$ ./scripts/feeds install -a
run this again a couple of times jto clear the warnings (because packages appear initially to be installed out of order)

Left with these issues
WARNING: Makefile 'package/feeds/routing/hnetd/Makefile' has a dependency on 'odhcp6c', which does not exist
WARNING: Makefile 'package/feeds/routing/hnetd/Makefile' has a dependency on 'odhcp6c', which does not exist
WARNING: Makefile 'package/feeds/gli_pub/luci-app-chinadns/Makefile' has a dependency on 'redsocks2', which does not exist
WARNING: Makefile 'package/feeds/gli_pub/luci-app-chinadns/Makefile' has a dependency on 'ShadowVPN', which does not exist
WARNING: Makefile 'package/feeds/routing/nat46/Makefile' has a dependency on 'map', which does not exist
WARNING: Makefile 'package/feeds/gli_pub/shadowsocks-libev/Makefile' has a build dependency on 'libpcre', which does not exist
WARNING: Makefile 'package/feeds/gli_pub/shadowsocks-libev/Makefile' has a build dependency on 'libmbedtls', which does not exist

```  

Compile  
=======  

make defconfig
make menuconfig
make download

Issuing **make menuconfig** to select a GL.iNet device, and then *exit* with *save*. Simply running **make** will build your own firmware. It will download all sources, build the cross-compile toolchain, the kernel and all choosen applications which is spent on several hours.   

Note that if you have all the source already, just put them in your *openwrt/dl* folder and you save time to download resource.  

Build Errors
make V=s 2>&1 | tee build.log | grep -i '[^_-"a-z]error[^_-.a-z]' 



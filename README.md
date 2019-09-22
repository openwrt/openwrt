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

```  

Compile  
=======  

make difconfig
make menuconfig
make download

Issuing **make menuconfig** to select a GL.iNet device, and then *exit* with *save*. Simply running **make** will build your own firmware. It will download all sources, build the cross-compile toolchain, the kernel and all choosen applications which is spent on several hours.   

Note that if you have all the source already, just put them in your *openwrt/dl* folder and you save time to download resource.  



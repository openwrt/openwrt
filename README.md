Overview  
======== 

This repo is a fork for Private use of GL.iNet team's https://github.com/gl_inet/openwrt, which is used to release stock firmware.  

Prerequisites  
=============  

```bash  
Ubuntu is recommended. tested on 18.04LTS in Azure

Instsalling Packages 
=============  

```bash  
$ sudo apt-get update
$ sudo apt-get install build-essential subversion libncurses5-dev zlib1g-dev gawk gcc-multilib flex git-core gettext libssl-dev
$ sudo upgrade
```  

Downloading Source  
------------------  

```  
$ git clone https://github.com/mattytap/openwrt.git openwrt
```  

Updating Feeds  
--------------  

```
$ ./scripts/feeds update -a
$ ./scripts/feeds install -a
```  

Compile  
=======  

make difconfig
make menuconfig
make download

Issuing **make menuconfig** to select a GL.iNet device, and then *exit* with *save*. Simply running **make** will build your own firmware. It will download all sources, build the cross-compile toolchain, the kernel and all choosen applications which is spent on several hours.   

Note that if you have all the source already, just put them in your *openwrt/dl* folder and you save time to download resource.  



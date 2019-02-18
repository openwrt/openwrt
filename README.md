Overview  
======== 

This repo is maintained by GL.iNet team, which is used to release stock firmware.  

Feature  
=======  

- Support latest device of GL.iNet 
- Support kernel driver which isn't support by kernel-tree  
- Keep updating with stock firmware  

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
```  

Updating Feeds  
--------------  

```
$ ./scripts/feeds update -a
$ ./scripts/feeds install -a
```  

Compile  
=======  

Issueing **make menuconfig** to select a GL.iNet device, and then *exit* with *save*. Simply running **make** will build your own firmware. It will download all sources, build the cross-compile toolchain, the kernel and all choosen applications which is spent on several hours.   

Note that if you have all the source already, just put them in your *openwrt/dl* folder and you save time to download resource.  



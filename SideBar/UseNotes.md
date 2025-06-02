# Quick Notes for Use #

## Images ##

## kmod packages ##
https://apk.tahomasoft.com/packages.adb

https://tahomasoft.com/OpenWRT/LinkStar/images/index.html

https://tahomasoft.com/OpenWRT/LinkStar/images/openwrt-rockchip-armv8-seeed_linkstar-h68k-1432v1-ext4-sysupgrade.img.gz

https://tahomasoft.com/OpenWRT/LinkStar/images/openwrt-rockchip-armv8-seeed_linkstar-h68k-1432v1-squashfs-sysupgrade.img.gz


## After First Boot ##

* apk update
* apk install pciutils
* apk install usbutils
* apk install kmod-mt7921-firmware
* apk install mt7921bt-firmware
* apk install kmod-mt7921-common
* apk install kmod-mt7921e

* reboot

### run lspci. Should see ###
* 0000:00:00.0 PCI bridge: Rockchip Electronics Co., Ltd RK3568 Remote Signal Processor (rev 01)
* 0000:01:00.0 Network controller: MEDIATEK Corp. MT7921 802.11ax PCI Express Wireless Network Adapter
* 0002:20:00.0 PCI bridge: Rockchip Electronics Co., Ltd RK3568 Remote Signal Processor (rev 01)

edit /etc/config/wireless to enable wifi
/etc/init.d/network restart

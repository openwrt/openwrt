#
# Copyright (C) 2006 - 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Es
  NAME:=es
  PACKAGES:=-iptables -ppp -ppp-mod-pppoe -kmod-ipt-nathelper \
		-firewall -mtd -hotplug2 -udevtrigger -dropbear \
		usb udev kmod-bluetooth ntpclient wpa-supplicant \
		wpa-cli \
		bluez-utils bluez-hcidump usbutils \
		kmod-usb-serial kmod-usb-serial-cp210x kmod-usb-serial-ftdi \
		kmod-usb-serial-pl2303 kmod-usb-uhci kmod-cfg80211 \
		kmod-lib80211 kmod-libertas-sd kmod-mac80211 libnl-tiny \
		crda iw kmod-usb-hid \
		kmod-usb-storage kmod-usb-storage-extras kmod-fs-vfat \
		uboot-omap35xx-omap3_overo \
		es estest mtd-utils uboot-envtools
endef

define Profile/Es/Description
        Gumstix www.gumstix.com
endef
$(eval $(call Profile,Es))

#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WifiAP
  NAME:=WifiAP
  PACKAGES:=kmod-madwifi wireless-tools \
	kmod-scsi-core \
	kmod-ata-core kmod-ata-artop \
	kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-storage \
	kmod-fs-ext2 kmod-fs-ext3 kmod-fs-vfat gpioctl openslp hostapd wpa-supplicant
endef

define Profile/WifiAP/Description
	Full featured image for flash based ixp4xx devices used as industrial APs
endef
$(eval $(call Profile,WifiAP))


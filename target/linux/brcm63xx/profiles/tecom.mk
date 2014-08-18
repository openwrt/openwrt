#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/GW6000
  NAME:=Tecom GW6000
  PACKAGES:=kmod-brcm-wl kmod-usb-ohci kmod-usb-storage \
	kmod-fs-ext4 kmod-nls-cp437 kmod-nls-iso8859-1 e2fsprogs \
	kmod-ipt-nathelper-extra wlc
endef
define Profile/GW6000/Description
  Package set optimized for GW6000.
endef
$(eval $(call Profile,GW6000))

define Profile/GW6200
  NAME:=Tecom GW6200
  PACKAGES:=kmod-brcm-wl kmod-usb-ohci kmod-usb-storage \
	kmod-fs-ext4 kmod-nls-cp437 kmod-nls-iso8859-1 e2fsprogs \
	kmod-ipt-nathelper-extra wlc
endef
define Profile/GW6200/Description
  Package set optimized for GW6200.
endef
$(eval $(call Profile,GW6200))

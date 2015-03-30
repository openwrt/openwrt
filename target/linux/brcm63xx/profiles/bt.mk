#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BTHOMEHUB2A
  NAME:=BT Home Hub 2A
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/BTHOMEHUB2A/Description
  Package set optimized for BTHOMEHUB2A.
endef
$(eval $(call Profile,BTHOMEHUB2A))

define Profile/BTV2110
  NAME:=BT Voyager V2110
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/BTV2500V/Description
  Package set optimized for BTV2110.
endef
$(eval $(call Profile,BTV2110))

define Profile/BTV2500V
  NAME:=BT Voyager V2500V
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/BTV2500V/Description
  Package set optimized for BTV2500V.
endef
$(eval $(call Profile,BTV2500V))

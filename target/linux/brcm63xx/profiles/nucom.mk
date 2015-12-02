#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/R5010UNV2
  NAME:=NuCom R5010UN v2
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/R5010UNV2/Description
  Package set optimized for R5010UNV2.
endef
$(eval $(call Profile,R5010UNV2))

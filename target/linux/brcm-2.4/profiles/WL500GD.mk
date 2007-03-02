#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WL500GD
  NAME:=ASUS WL-500g Deluxe
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-uhci kmod-usb2 wlc nas kmod-wlcompat
endef

define Profile/WL500GD/Description
	Package set optimized for the WL-500g Deluxe with USB support
endef
$(eval $(call Profile,WL500GD))


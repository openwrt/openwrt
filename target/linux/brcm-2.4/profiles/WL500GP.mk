#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WL500GP
  NAME:=ASUS WL-500g Premium
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-uhci-iv kmod-usb2 wlc nas kmod-wlcompat
endef

define Profile/WL500GP/Description
	Package set optimized for the WL-500g Premium with USB support
endef
$(eval $(call Profile,WL500GP))


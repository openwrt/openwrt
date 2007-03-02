#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WL500G
  NAME:=ASUS WL-500g
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-lp wlc nas kmod-wlcompat
endef

define Profile/WL500G/Description
	Package set optimized for the WL-500g with USB and parallel port support
endef
$(eval $(call Profile,WL500G))


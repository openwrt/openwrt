#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
  NAME:=Default Profile
  PACKAGES:= \
	kmod-usb-core kmod-usb-ohci
endef

define Profile/Default/Description
	Default package set compatible with most EP93xx-based boards.
endef
$(eval $(call Profile,Default))

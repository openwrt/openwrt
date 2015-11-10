#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/EA3500
  NAME:=Linksys EA3500
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	swconfig wpad-mini
endef

define Profile/EA3500/Description
 Package set compatible with Linksys EA3500 board.
endef

$(eval $(call Profile,EA3500))

define Profile/EA4500
  NAME:=Linksys EA4500
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	swconfig wpad-mini
endef

define Profile/EA4500/Description
 Package set compatible with Linksys EA4500 board.
endef

$(eval $(call Profile,EA4500))

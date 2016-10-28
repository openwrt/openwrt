#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DW33D
	NAME:=DomyWifi DW33D
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev kmod-ath10k
endef

define Profile/DW33D/Description
	Package set optimized for the DomyWifi DW33D.
endef

$(eval $(call Profile,DW33D))

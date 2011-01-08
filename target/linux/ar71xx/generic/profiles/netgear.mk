#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WNDR3700
	NAME:=NETGEAR WNDR3700
	PACKAGES:=kmod-ath9k wpad-mini kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/WNDR3700/Description
	Package set optimized for the NETGEAR WNDR3700
endef

$(eval $(call Profile,WNDR3700))

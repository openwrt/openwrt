#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/UBNTRS
	NAME:=Ubiquiti RouterStation
	PACKAGES:=kmod-usb-ohci kmod-usb2
endef

define Profile/UBNTRS/Description
	Package set optimized for the Ubiquiti RouterStation
endef

$(eval $(call Profile,UBNTRS))

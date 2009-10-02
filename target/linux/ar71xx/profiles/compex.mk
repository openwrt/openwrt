#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WP543
	NAME:=Compex WP543
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/WP543/Description
	Package set optimized for the Compex WP543.
endef

$(eval $(call Profile,WP543))

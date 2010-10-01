#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/JWAP003
	NAME:=jjPlus JWAP0003
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/JWAP003/Description
	Package set optimized for the jjPlus JWAP003 board.
endef

$(eval $(call Profile,JWAP003))

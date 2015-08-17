#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RTN15
	NAME:=Asus RT-N15
	PACKAGES:=kmod-switch-rtl8366s kmod-swconfig swconfig
endef

define Profile/RTN15/Description
	Package set for Asus RT-N15 board
endef
$(eval $(call Profile,RTN15))

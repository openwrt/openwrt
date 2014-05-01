#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/A13_OLinuXino
	NAME:=A13 OLinuXino
	PACKAGES:=\
		uboot-sunxi-A13-OLinuXino kmod-rtl8192cu
endef

define Profile/A13_OLinuXino/Description
	Package set optimized for the Olimex A13 OLinuXino
endef

$(eval $(call Profile,A13_OLinuXino))

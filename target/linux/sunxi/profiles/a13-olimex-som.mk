#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OLIMEX_A13_SOM
	NAME:=Olimex A13 SOM
	PACKAGES:=\
		uboot-sunxi-OLIMEX_A13_SOM kmod-rtl8192cu
endef

define Profile/OLIMEX_A13_SOM/Description
	Package set optimized for the Olimex A13 SOM
endef

$(eval $(call Profile,OLIMEX_A13_SOM))

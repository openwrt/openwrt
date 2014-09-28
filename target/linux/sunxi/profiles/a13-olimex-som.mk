#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OLIMEX-A13-SOM
	NAME:=Olimex A13 SOM
	PACKAGES:=\
		uboot-sunxi-OLIMEX-A13-SOM kmod-rtl8192cu
endef

define Profile/OLIMEX-A13-SOM/Description
	Package set optimized for the Olimex A13 SOM
endef

$(eval $(call Profile,OLIMEX-A13-SOM))

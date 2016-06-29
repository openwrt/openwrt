#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WL-351
	NAME:=Sitecom WL-351 v1
	PACKAGES:=\
		kmod-switch-rtl8366rb kmod-swconfig swconfig
endef

define Profile/WL-351/Description
	Package set for Sitecom WL-351 v1
endef

$(eval $(call Profile,WL-351))

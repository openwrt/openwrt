#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Mele_M9
	NAME:=Mele_M9
	PACKAGES:=\
		uboot-sunxi-Mele_M9 kmod-sun4i-emac kmod-rtc-sunxi kmod-rtl8192cu
endef

define Profile/Mele_M9/Description
	Package set optimized for the Mele M9
endef

$(eval $(call Profile,Mele_M9))

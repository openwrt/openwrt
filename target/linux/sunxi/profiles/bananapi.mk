#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Bananapi
	NAME:=Bananapi
	PACKAGES:=\
		uboot-sunxi-Bananapi kmod-rtc-sunxi kmod-ata-core kmod-ata-sunxi
endef

define Profile/Bananapi/Description
	Package set optimized for the Bananapi
endef

$(eval $(call Profile,Bananapi))

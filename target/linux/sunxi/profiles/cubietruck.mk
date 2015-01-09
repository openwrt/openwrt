#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Cubietruck
	NAME:=Cubietruck
	PACKAGES:=\
		uboot-sunxi-Cubietruck kmod-ata-sunxi kmod-rtc-sunxi kmod-brcmfmac
endef

define Profile/Cubietruck/Description
	Package set optimized for the Cubietruck
endef

$(eval $(call Profile,Cubietruck))

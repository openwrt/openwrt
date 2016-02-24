#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Bananapro
	NAME:=Bananapro
	PACKAGES:=\
		uboot-sunxi-Bananapro kmod-rtc-sunxi kmod-ata-core kmod-ata-sunxi \
		kmod-brcmfmac
endef

define Profile/Bananapro/Description
	Package set optimized for the Bananapro
endef

$(eval $(call Profile,Bananapro))

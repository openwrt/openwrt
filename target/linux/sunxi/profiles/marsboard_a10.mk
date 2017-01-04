#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Marsboard_A10
	NAME:=Marsboard A10
	PACKAGES:=\
		uboot-sunxi-Marsboard_A10 kmod-ata-core kmod-ata-sunxi \
		kmod-sun4i-emac kmod-rtc-sunxi sound-soc-sunxi
endef

define Profile/Marsboard_A10/Description
	Package set optimized for the HAOYU Electronics MarsBoard A10
endef

$(eval $(call Profile,Marsboard_A10))

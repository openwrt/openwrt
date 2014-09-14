#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Linksprite_pcDuino3
	NAME:=pcDuino3
	PACKAGES:=\
		uboot-sunxi-Linksprite_pcDuino3 kmod-sun4i-emac kmod-rtc-sunxi kmod-net-rtl8188eu
endef

define Profile/Linksprite_pcDuino3/Description
	Package set optimized for the pcDuino3
endef

$(eval $(call Profile,Linksprite_pcDuino3))

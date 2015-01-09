#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Linksprite_pcDuino
	NAME:=Linksprite_pcDuino
	PACKAGES:=\
		uboot-sunxi-Linksprite_pcDuino kmod-sun4i-emac kmod-rtc-sunxi kmod-rtl8192cu
endef

define Profile/Linksprite_pcDuino/Description
	Package set optimized for the Linksprite pcDuino
endef

$(eval $(call Profile,Linksprite_pcDuino))

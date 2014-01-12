#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/pcDuino
	NAME:=pcDuino
	PACKAGES:=\
		uboot-sunxi-pcDuino kmod-sun4i-emac kmod-rtc-sunxi
endef

define Profile/pcDuino/Description
	Package set optimized for the pcDuino
endef

$(eval $(call Profile,pcDuino))

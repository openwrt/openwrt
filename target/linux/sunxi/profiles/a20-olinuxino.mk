#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/A20-OLinuXino-Lime
	NAME:=A20 OLinuXino Lime
	PACKAGES:=\
		uboot-sunxi-A20-OLinuXino-Lime kmod-ata-core kmod-ata-sunxi \
		kmod-rtc-sunxi
endef

define Profile/A20-OLinuXino-Lime/Description
	Package set optimized for the Olimex A20 OLinuXino Lime
endef

$(eval $(call Profile,A20-OLinuXino-Lime))


define Profile/A20-OLinuXino-Lime2
	NAME:=A20 OLinuXino Lime2
	PACKAGES:=\
		uboot-sunxi-A20-OLinuXino-Lime2 kmod-ata-core kmod-ata-sunxi \
		kmod-rtc-sunxi
endef

define Profile/A20-OLinuXino-Lime2/Description
	Package set optimized for the Olimex A20 OLinuXino Lime2
endef

$(eval $(call Profile,A20-OLinuXino-Lime2))


define Profile/A20-OLinuXino_MICRO
	NAME:=A20 OLinuXino Micro
	PACKAGES:=\
		uboot-sunxi-A20-OLinuXino_MICRO kmod-ata-core kmod-ata-sunxi \
		kmod-sun4i-emac kmod-rtc-sunxi
endef

define Profile/A20-OLinuXino_MICRO/Description
	Package set optimized for the Olimex A20 OLinuXino micro
endef

$(eval $(call Profile,A20-OLinuXino_MICRO))

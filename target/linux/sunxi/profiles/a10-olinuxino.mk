#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/A10-OLinuXino-Lime
	NAME:=A10 OLinuXino LIME
	PACKAGES:=\
		uboot-sunxi-A10-OLinuXino-Lime kmod-ata-core kmod-ata-sunxi \
		kmod-sun4i-emac kmod-rtc-sunxi
endef

define Profile/A10-OLinuXino-Lime/Description
	Package set optimized for the Olimex A10 OLinuXino LIME
endef

$(eval $(call Profile,A10-OLinuXino-Lime))

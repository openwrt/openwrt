#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Cubieboard2
	NAME:=Cubieboard2
	PACKAGES:=\
		uboot-sunxi-Cubieboard2 kmod-ata-core kmod-ata-sunxi \
		kmod-sun4i-emac kmod-rtc-sunxi
endef

define Profile/Cubieboard2/Description
	Package set optimized for the Cubieboard2
endef

$(eval $(call Profile,Cubieboard2))

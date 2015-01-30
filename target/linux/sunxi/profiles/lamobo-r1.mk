#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Lamobo_R1
	NAME:=Lamobo R1
	PACKAGES:=\
		uboot-sunxi-Lamobo_R1 kmod-ata-sunxi kmod-rtl8192cu \
		swconfig wpad-mini
endef

define Profile/Lamobo_R1/Description
	Package set optimized for the Lamobo R1
endef

$(eval $(call Profile,Lamobo_R1))

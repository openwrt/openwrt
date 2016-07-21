#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/orangepi_2
	NAME:=orangepi_2
	PACKAGES:=\
		kmod-rtc-sunxi uboot-sunxi-orangepi_2
endef

define Profile/orangepi_2/Description
	Package set optimized for the OrangePi 2 (H3)
endef

$(eval $(call Profile,orangepi_2))

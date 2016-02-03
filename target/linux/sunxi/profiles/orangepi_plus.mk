#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/orangepi_plus
	NAME:=orangepi_plus
	PACKAGES:=\
		kmod-rtc-sunxi uboot-sunxi-orangepi_plus
endef

define Profile/orangepi_plus/Description
	Package set optimized for the OrangePi Plus (H3)
endef

$(eval $(call Profile,orangepi_plus))

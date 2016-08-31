#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ls1043ardb-64bit
	NAME:=ls1043ardb 64bit Profile
	PACKAGES += kmod-usb3 kmod-usb-dwc3 kmod-usb-storage \
		kmod-i2c-core kmod-i2c-imx \
		kmod-mmc kmod-sdhci kmod-sdhci-of-esdhc \
		kmod-e1000e kmod-fs-vfat
endef

define Profile/ls1043ardb-64bit/Description
	ls1043ardb 64bit Description
endef

$(eval $(call Profile,ls1043ardb-64bit))

#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ZBT-WG2626
	NAME:=ZBT-WG2626 Device
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-sdhci-mt7620 \
		kmod-ledtrig-usbdev kmod-ata-core kmod-ata-ahci
endef

define Profile/ZBT-WG2626/Description
	Package set for ZBT-WG2626 device
endef
$(eval $(call Profile,ZBT-WG2626))

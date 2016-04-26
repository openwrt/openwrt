#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CS-QR10
	NAME:=Planex CS-QR10
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci \
		i2c-core kmod-i2c-ralink kmod-sound-core kmod-sound-mtk \
		kmod-sdhci-mt7620
endef

define Profile/CS-QR10/Description
	Package set optimized for the Planex CS-QR10.
endef
$(eval $(call Profile,CS-QR10))


define Profile/MZK-750DHP
	NAME:=Planex MZK-750DHP
	PACKAGES:= kmod-mt76 kmod-mt7610e
endef

define Profile/MZK-750DHP/Description
	Package set optimized for the Planex MZK-750DHP.
endef
$(eval $(call Profile,MZK-750DHP))


define Profile/MZK-EX300NP
	NAME:=Planex MZK-EX300NP
	PACKAGES:=
endef

define Profile/MZK-EX300NP/Description
	Package set optimized for the Planex MZK-EX300NP.
endef
$(eval $(call Profile,MZK-EX300NP))

#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ANTMINERS1
	NAME:=Antminer-S1
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-crypto-manager kmod-i2c-gpio-custom kmod-usb-hid
endef


define Profile/ANTMINERS1/Description
	Package set optimized for the Bitmain Antminer S1.
endef
$(eval $(call Profile,ANTMINERS1))

define Profile/ANTMINERS3
	NAME:=Antminer-S3
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-crypto-manager kmod-i2c-gpio-custom kmod-usb-hid
endef


define Profile/ANTMINERS3/Description
	Package set optimized for the Bitmain Antminer S3.
endef
$(eval $(call Profile,ANTMINERS3))

define Profile/ANTROUTERR1
	NAME:=Antrouter-R1
	PACKAGES:=kmod-usb-core kmod-usb2
endef


define Profile/ANTROUTERR1/Description
	Package set optimized for the Bitmain Antrouter R1.
endef
$(eval $(call Profile,ANTROUTERR1))

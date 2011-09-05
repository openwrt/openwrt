#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/JA76PF
	NAME:=jjPlus JA76PF
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-hwmon-core kmod-i2c-core kmod-hwmon-lm75
endef

define Profile/JA76PF/Description
	Package set optimized for the jjPlus JA76PF board.
endef

$(eval $(call Profile,JA76PF))

define Profile/JWAP003
	NAME:=jjPlus JWAP0003
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/JWAP003/Description
	Package set optimized for the jjPlus JWAP003 board.
endef

$(eval $(call Profile,JWAP003))

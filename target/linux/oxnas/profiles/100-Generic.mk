#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/POGOPLUG_PRO
  NAME:=PogoPlug Pro
endef

define Profile/POGOPLUG_PRO/Description
 Profile with built-in ox820 PogoPlug Pro device-tree
 (board with miniPCIe slot)
endef

define Profile/POGOPLUG_V3
  NAME:=PogoPlug V3
endef

define Profile/POGOPLUG_V3/Description
 Profile with built-in ox820 PogoPlug V3 device-tree
 (board without miniPCIe slot)
endef

define Profile/STG212
  NAME:=MitraStar STG-212
endef

define Profile/STG212/Description
 Profile with built-in ox820 STG-212 device-tree
endef

define Profile/KD20
  NAME:=Shuttle KD20
  PACKAGES:= \
	kmod-usb3 kmod-usb-storage kmod-i2c-gpio kmod-rtc-pcf8563 \
	kmod-gpio-beeper kmod-hwmon-core kmod-hwmon-gpiofan
endef

define Profile/KD20/Description
 Profile with built-in ox820 KD20 device-tree
endef

$(eval $(call Profile,POGOPLUG_PRO))
$(eval $(call Profile,POGOPLUG_V3))
$(eval $(call Profile,STG212))
$(eval $(call Profile,KD20))

#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Pcengines
  NAME:=PCEngines WRAP
  PACKAGES:=kmod-i2c-scx200 kmod-natsemi kmod-leds-wrap kmod-scx200-gpio kmod-scx200-wdt kmod-hwmon-pc87360
endef

define Profile/Pcengines/Description
	Package set compatible with the PCEngines WRAP. Contains I2C/LEDS/GPIO/Sensors support
endef
$(eval $(call Profile,Pcengines))

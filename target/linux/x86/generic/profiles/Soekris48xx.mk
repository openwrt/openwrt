#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Soekris48xx
  NAME:=Soekris Net48xx
  PACKAGES:=kmod-i2c-scx200 kmod-natsemi kmod-leds-net48xx kmod-scx200-gpio kmod-scx200-wdt kmod-hwmon-pc87360
endef

define Profile/Soekris48xx/Description
        Package set compatible with the Soekris Net4801. Contains I2C/LEDS/GPIO/Sensors support
endef
$(eval $(call Profile,Soekris48xx))

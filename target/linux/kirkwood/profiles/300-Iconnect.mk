#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Iconnect
  NAME:=Iomega iConnect Wireless
  PACKAGES:=kmod-i2c-mv64xxx kmod-hwmon-core kmod-hwmon-lm63
endef

define Profile/Iconnect/Description
        Iomega iConnect Wireless
endef

$(eval $(call Profile,Iconnect))

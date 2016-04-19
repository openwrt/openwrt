# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AKITIO
  NAME:=Akitio MyCloud mini / Silverstone DC01
  PACKAGES:= \
	kmod-usb-storage kmod-i2c-gpio kmod-rtc-ds1307
endef

define Profile/AKITIO/Description
 Profile for the Akitio MyCloud mini aka. Silverstone DC01
endef

$(eval $(call Profile,AKITIO))

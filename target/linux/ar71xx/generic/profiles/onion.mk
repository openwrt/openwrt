#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OMEGA
   NAME:=Onion Omega
   PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage kmod-i2c-core kmod-i2c-gpio-custom kmod-spi-bitbang kmod-spi-dev kmod-spi-gpio kmod-spi-gpio-custom kmod-usb-serial
endef

define Profile/OMEGA/Description
   Package set optimized for the Onion Omega development platform.
endef
$(eval $(call Profile,OMEGA))

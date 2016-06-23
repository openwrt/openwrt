#
# Copyright (C) 2014-2015 Chris Blake (chrisrblake93@gmail.com)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MR12
	NAME:=Meraki MR12
	PACKAGES:=kmod-spi-gpio kmod-ath9k
endef

define Profile/MR12/description
	Package set optimized for the Cisco Meraki MR12 Access Point.
endef

define Profile/MR16
	NAME:=Meraki MR16
	PACKAGES:=kmod-spi-gpio kmod-ath9k
endef

define Profile/MR16/description
	Package set optimized for the Cisco Meraki MR16 Access Point.
endef

$(eval $(call Profile,MR12))
$(eval $(call Profile,MR16))
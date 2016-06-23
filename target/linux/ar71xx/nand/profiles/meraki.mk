#
# Copyright (C) 2014-2015 Chris Blake (chrisrblake93@gmail.com)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MR18
	NAME:=Meraki MR18
	PACKAGES:=kmod-spi-gpio kmod-ath9k
endef

define Profile/MR18/description
	Package set optimized for the Cisco Meraki MR18 Access Point.
endef

$(eval $(call Profile,MR18))

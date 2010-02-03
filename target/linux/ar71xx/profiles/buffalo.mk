#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WZRHPG300NH
	NAME:=Buffalo WZR-HP-G300NH
	PACKAGES:=kmod-ath9k wpad-mini kmod-usb-core kmod-usb2
endef

define Profile/WZRHPG300NH/Description
	Package set optimized for the Buffalo WZR-HP-G300NH
endef

$(eval $(call Profile,WZRHPG300NH))

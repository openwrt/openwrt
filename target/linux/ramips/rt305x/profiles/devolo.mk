#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MT2681
	NAME:=Devolo WiFi Extender MT2681
	PACKAGES:=rssileds
endef

define Profile/MT2681/Description
	Package set for Devolo MT2681
endef
$(eval $(call Profile,MT2681))


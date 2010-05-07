#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DefaultNoWifi
	NAME:=Default Profile (no WiFi)
	PACKAGES:=
endef

define Profile/DefaultNoWifi/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,DefaultNoWifi))

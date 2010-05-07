#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Madwifi
	NAME:=Atheros WiFi (madwifi)
	PACKAGES:=kmod-madwifi wpad-mini
endef

define Profile/Madwifi/Description
	Package set compatible with hardware using Atheros WiFi cards.
endef
$(eval $(call Profile,Madwifi))

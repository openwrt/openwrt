#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Atheros
  NAME:=Atheros WiFi (default)
  PACKAGES:=kmod-ath5k kmod-ath9k wpad-mini
endef

define Profile/Atheros/Description
	Package set compatible with hardware using Atheros WiFi cards
endef
$(eval $(call Profile,Atheros))


#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ath5k
	NAME:=Atheros 802.11abg WiFi (ath5k)
	PACKAGES:=kmod-ath5k wpad-mini
endef

define Profile/ath5k/Description
	Package set compatible with hardware using Atheros 802.11abg cards.
endef
$(eval $(call Profile,ath5k))

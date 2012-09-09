#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Ath5k
	NAME:=Atheros WiFi (ath5k)
	PACKAGES:=kmod-ath5k -kmod-ath9k
endef

define Profile/Ath5k/Description
	Package set compatible with hardware using Atheros WiFi cards.
endef
$(eval $(call Profile,Ath5k))

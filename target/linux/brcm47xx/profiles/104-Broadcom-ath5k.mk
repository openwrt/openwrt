#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom-ath5k
  NAME:=Atheros WiFi (ath5k)
  PACKAGES:=kmod-ath5k
endef

define Profile/Atheros/Description
	Package set compatible with hardware using Atheros WiFi cards and
	b44 Ethernet driver.
endef
$(eval $(call Profile,Broadcom-ath5k))


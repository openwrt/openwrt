#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Bcm4705-b43
  NAME:=BCM4705/BCM4785, Broadcom BCM43xx WiFi (b43)
  PACKAGES:=kmod-b43 kmod-b43legacy kmod-tg3 kmod-tg3
endef

define Profile/Broadcom-b43/Description
	Package set compatible with hardware using Broadcom BCM43xx cards
	using the MAC80211 b43 and b43legacy drivers and tg3 Ethernet driver
endef

$(eval $(call Profile,Bcm4705-b43))


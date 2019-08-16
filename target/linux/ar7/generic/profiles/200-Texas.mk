#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/texas
  NAME:=Texas Instruments WiFi (mac80211)
  PACKAGES:=kmod-acx-mac80211
endef

define Profile/texas/Description
	Package set compatible with hardware using Texas Instruments WiFi cards
	using the mac80211 driver.
endef
$(eval $(call Profile,texas))


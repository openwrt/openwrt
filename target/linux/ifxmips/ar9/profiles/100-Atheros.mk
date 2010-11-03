#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Atheros
  NAME:=Atheros WiFi
  PACKAGES:=kmod-madwifi hostapd-mini
endef

$(eval $(call Profile,Atheros))


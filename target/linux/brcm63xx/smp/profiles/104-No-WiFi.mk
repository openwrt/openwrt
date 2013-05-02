#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NoWiFi
  NAME:=No WiFi
  PACKAGES:=
endef

define Profile/NoWiFi/description
  Package set compatible with BCM63xx routers without Wi-Fi.
endef

$(eval $(call Profile,NoWiFi))

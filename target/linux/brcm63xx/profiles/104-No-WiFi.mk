#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NoWiFi
  NAME:=No WiFi (BCM6338)
  PACKAGES:=kmod-bcm63xx-spi
endef

define Profile/NoWiFi/description
  Package set compatible with BCM63xx routers without Wi-Fi (e.g: BCM6338-based).
endef

$(eval $(call Profile,NoWiFi))

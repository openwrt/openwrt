#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Bcm4705-none
  NAME:=BCM4705/BCM4785, no WiFi
  PACKAGES:=-wpad-mini kmod-tg3
endef

define Profile/None/Description
	Package set without WiFi support and tg3 Ethernet driver.
endef
$(eval $(call Profile,Bcm4705-none))


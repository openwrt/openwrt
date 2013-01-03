#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom-none
  NAME:=No WiFi
  PACKAGES:=-wpad-mini kmod-b44
endef

define Profile/None/Description
	Package set without WiFi support and b44 Ethernet driver.
endef
$(eval $(call Profile,Broadcom-none))


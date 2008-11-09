#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom-b43
  NAME:=Broadcom BCM43xx WiFi (default)
  PACKAGES:=kmod-b43 kmod-b43legacy
endef

define Profile/Broadcom-b43/Description
	Package set compatible with hardware using Broadcom BCM43xx cards
endef
$(eval $(call Profile,Broadcom-b43))


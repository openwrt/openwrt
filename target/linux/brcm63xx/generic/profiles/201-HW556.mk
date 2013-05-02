#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HW556
  NAME:=HW556 WiFi
  PACKAGES:=kmod-rt2800-pci kmod-ath9k wpad-mini
endef

define Profile/HW556/Description
	Package set compatible with hardware using Atheros or Ralink WiFi cards (Huawei HG556a)
endef
$(eval $(call Profile,HW556))


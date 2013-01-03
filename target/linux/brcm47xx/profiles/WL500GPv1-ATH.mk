#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WL500GPv1
  NAME:=ASUS WL-500g Premium v1 (Atheros WiFi using ath5k)
  PACKAGES:=kmod-b44 kmod-ath5k kmod-usb-core kmod-usb-uhci kmod-usb2
endef

define Profile/WL500GPv1/Description
	Package set optimized for the WL-500g Premium v1 with USB and Atheros WiFi card support
endef
$(eval $(call Profile,WL500GPv1))

#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WGT634U
  NAME:=Netgear WGT634U
  PACKAGES:=kmod-ath5k kmod-usb-core kmod-usb2 kmod-ocf-ubsec-ssb
endef

define Profile/WGT634U/Description
	Package set compatible with the Netgear WGT634U. Contains USB support
endef
$(eval $(call Profile,WGT634U))


#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/mototech
  NAME:=Mototech evaluation board
  PACKAGES:=kmod-ath9k kmod-ata-core kmod-ata-sil24 kmod-usb-core kmod-usb2 \
		kmod-usb-octeon
endef

define Profile/mototech/Description
	Package set compatible with the Mototech Evaluation Board (CN5000F).
endef
$(eval $(call Profile,mototech))


#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/EA3500
  NAME:=Linksys EA3500
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	uboot-envtools
endef

define Profile/EA3500/Description
 Package set compatible with Linksys EA3500 board.
endef

EA3500_UBIFS_OPTS:="-m 512 -e 15872 -c 4096"
EA3500_UBI_OPTS:="-m 512 -p 16384 -s 256"

$(eval $(call Profile,EA3500))

define Profile/EA4500
  NAME:=Linksys EA4500
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	swconfig uboot-envtools
endef

define Profile/EA4500/Description
 Package set compatible with Linksys EA4500 board.
endef

EA4500_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
EA4500_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,EA4500))

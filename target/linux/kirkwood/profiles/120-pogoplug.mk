#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/POGOE02
  NAME:=Cloud Engines Pogoplug E02
  PACKAGES:= \
	kmod-usb2 kmod-usb-storage uboot-envtools
endef

define Profile/POGOE02/Description
 Package set compatible with Cloud Engines Pogoplug E02 board.
endef

POGOE02_UBIFS_OPTS:="-m 2048 -e 126KiB -c 4096"
POGOE02_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,POGOE02))

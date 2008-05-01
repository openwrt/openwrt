#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/FSG3
  NAME:=Freecom FSG-3
  PACKAGES:= \
	kmod-madwifi wireless-tools \
	kmod-usb-core kmod-usb-uhci kmod-usb2 kmod-usb-storage \
	kmod-fs-ext2 kmod-fs-ext3 kmod-fs-reiserfs
endef

define Profile/FSG3/Description
	Package set optimized for the Freecom FSG-3
endef
$(eval $(call Profile,FSG3))


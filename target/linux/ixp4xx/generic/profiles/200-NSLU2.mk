#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NSLU2
  NAME:=Linksys NSLU2
  PACKAGES:=-wpad-mini kmod-scsi-core \
	kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-storage \
	kmod-fs-ext2 kmod-fs-ext3
endef

define Profile/NSLU2/Description
	Package set optimized for the Linksys NSLU2
endef
$(eval $(call Profile,NSLU2))


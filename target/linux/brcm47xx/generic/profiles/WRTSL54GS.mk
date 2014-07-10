#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRTSL54GS
  NAME:=Linksys WRTSL54GS
  PACKAGES:=kmod-b44 kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-storage kmod-scsi-core kmod-fs-ext4 e2fsprogs kmod-b43
endef

define Profile/WRTSL54GS/Description
	Package set compatible with the Linksys WRTSL54GS. Contains USB support
endef
$(eval $(call Profile,WRTSL54GS))


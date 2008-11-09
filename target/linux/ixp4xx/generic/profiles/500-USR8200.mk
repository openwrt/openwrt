#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/USR8200
  NAME:=USRobotics USR8200
  PACKAGES:=kmod-ixp4xx-npe \
	kmod-scsi-core \
	kmod-usb-core kmod-usb-uhci kmod-usb2 kmod-usb-storage \
	kmod-fs-ext2 kmod-fs-ext3 \
	kmod-ieee1394 kmod-ohci1394 kmod-sbp2
endef

define Profile/USR8200/Description
	Package set optimized for the USRobotics USR8200
endef
$(eval $(call Profile,USR8200))


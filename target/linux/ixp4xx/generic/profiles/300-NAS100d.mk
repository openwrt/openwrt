#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NAS100d
  NAME:=Iomega NAS 100d
  PACKAGES:=kmod-madwifi wireless-tools \
	kmod-scsi-core \
	kmod-ata-core kmod-ata-artop \
	kmod-usb-core kmod-usb2 kmod-usb-storage \
	kmod-fs-ext2 kmod-fs-ext3
endef

define Profile/NAS100d/Description
	Package set optimized for the Iomega NAS 100d
endef
$(eval $(call Profile,NAS100d))


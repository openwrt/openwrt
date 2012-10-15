#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/bifferboard
	NAME:=Bifferboard
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 \
		kmod-usb-storage kmod-scsi-core kmod-fs-ext3
endef
$(eval $(call Profile,bifferboard))

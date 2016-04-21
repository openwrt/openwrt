#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRH-300CR
	NAME:=ELECOM WRH-300CR
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci kmod-usb-storage \
		kmod-scsi-core kmod-fs-ext4 block-mount
endef

define Profile/WRH-300CR/Description
	Package set optimized for the ELECOM WRH-300CR.
endef
$(eval $(call Profile,WRH-300CR))

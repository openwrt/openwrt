#
# Copyright (C) 2012-2013 Hiwifi Wireless
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HIWIFI_HC6361
	NAME:=HiWiFi HC6361
	PACKAGES:= kmod-usb-core kmod-usb2 kmod-usb-storage \
		kmod-crypto-deflate kmod-fs-ext4 kmod-ledtrig-gpio \
		kmod-nls-iso8859-1 e2fsprogs
endef

define Profile/HIWIFI_HC6361/description
	Package set optimized for the HiWiFi HC6361.
endef
$(eval $(call Profile,HIWIFI_HC6361))

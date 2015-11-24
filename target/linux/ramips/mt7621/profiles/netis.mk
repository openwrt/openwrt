#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WF-2881
	NAME:=NETIS WF-2881
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-usb-hid \
		kmod-ledtrig-usbdev
endef

define Profile/WF-2881/Description
	Package set compatible with the NETIS WF-2881 board.
endef

#-m <min io size> -e <LEB size> -c <Eraseblocks count>
WF-2881_UBIFS_OPTS:="-m 2048 -e 129024 -c 1024"
WF-2881_UBI_OPTS:="-m 2048 -p 128KiB -s 512"

$(eval $(call Profile,WF-2881))

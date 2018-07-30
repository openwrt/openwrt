#
# Copyright (C) 2017 Mathew McBride <matt@traverse.com.au>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

BOARD:=armserver
BOARDNAME:=ARM Server
FEATURES:=fpu pci rtc usb
FEATURES+=ext4 ramdisk targz
MAINTAINER:=Mathew McBride <matt@bionicmessage.net>
SUBTARGETS:=64
KERNEL_PATCHVER:=4.14

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-fs-vfat
$(eval $(call BuildTarget))

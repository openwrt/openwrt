#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=orion
BOARDNAME:=Marvell Orion
FEATURES:=squashfs
SUBTARGETS=generic harddisk

LINUX_VERSION:=2.6.30.10

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="uImage"

DEFAULT_PACKAGES += kmod-ath9k wpad-mini

$(eval $(call BuildTarget))

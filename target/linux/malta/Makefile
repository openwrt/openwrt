#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=malta
BOARDNAME:=MIPS Malta CoreLV board (qemu)
FEATURES:=ramdisk
CFLAGS:=-Os -pipe -mips32r2 -mtune=mips32r2 -funit-at-a-time

LINUX_VERSION:=2.6.32.8

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for MTI Malta CoreLV boards.
endef

$(eval $(call BuildTarget))

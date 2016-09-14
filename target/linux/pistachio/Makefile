#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=pistachio
BOARDNAME:=IMG MIPS Pistachio
FEATURES:=targz nand ubifs audio ramdisk
SUBTARGETS=marduk

KERNEL_PATCHVER:=4.4

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Pistachio board.
endef

$(eval $(call BuildTarget))

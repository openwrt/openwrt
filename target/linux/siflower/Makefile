#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=siflower
BOARDNAME:=MIPS Siflower board
SUBTARGETS:=sf16a18-fullmask sf19a28-fullmask
INITRAMFS_EXTRA_FILES:=
MAINTAINER:=Shanghai Siflower Communication Technology Co., Ltd.

KERNEL_PATCHVER:=4.14

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Siflower based boards.
endef

$(eval $(call BuildTarget))

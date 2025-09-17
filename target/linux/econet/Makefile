# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2009-2025 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=econet
BOARDNAME:=EcoNet EN75xx MIPS
FEATURES:=dt source-only squashfs nand
SUBTARGETS:=en751221

KERNEL_PATCHVER:=6.12

define Target/Description
	Build firmware image for EcoNet EN75xx MIPS based boards.
endef

# include the profiles
include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

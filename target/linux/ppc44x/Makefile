#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ppc44x
BOARDNAME:=AMCC/IBM PPC44x
FEATURES:=squashfs
CFLAGS:=-Os -pipe -funit-at-a-time -mcpu=440

LINUX_VERSION:=2.6.32.9

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for AMCC/IBM PPC44x based boards.
endef

$(eval $(call BuildTarget))

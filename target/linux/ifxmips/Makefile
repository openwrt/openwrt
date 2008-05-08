# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ifxmips
BOARDNAME:=Infineon Mips
FEATURES:=squashfs jffs2 broken
LINUX_VERSION:=2.6.25.1

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Infineon Mips Controllers
endef

$(eval $(call BuildTarget))

#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ifxmips
BOARDNAME:=Infineon Mips
FEATURES:=squashfs jffs2
SUBTARGETS:=generic nfs
LINUX_VERSION:=2.6.26.8
#LINUX_VERSION:=2.6.27.11

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES+=uboot-ifxmips hostapd-mini

define Target/Description
	Build firmware images for Infineon Mips Controllers
endef

$(eval $(call BuildTarget))

# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=ar531x
BOARDNAME:=Atheros AR531x
FEATURES:=broken jffs2

define Target/Description
	Build firmware images for Atheros AR531x based boards
	(e.g. Netgear WGT624, Linksys WRT55AG)
endef

LINUX_VERSION:=2.4.33.4
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=aca0257549c385a227088ad8853a72a2

include $(INCLUDE_DIR)/kernel-build.mk
$(eval $(call BuildKernel))

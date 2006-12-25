# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=atheros
BOARDNAME:=Atheros
FEATURES:=jffs2

define Target/Description
	Build firmware images for Atheros SoC boards (currently AR2315-AR2317)
endef

LINUX_VERSION:=2.6.19
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=443c265b57e87eadc0c677c3acc37e20

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += kmod-madwifi

$(eval $(call BuildKernel))

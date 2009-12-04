# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=amazon
BOARDNAME:=Infineon Amazon
FEATURES:=squashfs jffs2 broken
LINUX_VERSION:=2.6.30.10

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build firmware images for Infineon Amazon boards
endef

ifeq ($(KERNEL_PATCHVER),2.6.30)
  define Kernel/Prepare
	$(call Kernel/Prepare/Default)
	mv $(LINUX_DIR)/include/asm-mips/mach-amazon $(LINUX_DIR)/arch/mips/include/asm/mach-amazon
	mv $(LINUX_DIR)/drivers/char/watchdog/amazon_wdt.c $(LINUX_DIR)/drivers/watchdog/amazon_wdt.c
  endef
endif

$(eval $(call BuildTarget))

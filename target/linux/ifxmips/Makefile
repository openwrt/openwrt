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

LINUX_VERSION:=2.6.28.10

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES+=uboot-ifxmips hostapd-mini

define Target/Description
	Build firmware images for Infineon Mips Controllers
endef

ifeq ($(KERNEL_PATCHVER),2.6.28)
  define Kernel/Prepare
	$(call Kernel/Prepare/Default)
	mv $(LINUX_DIR)/include/asm-mips/* $(LINUX_DIR)/arch/mips/include/asm/
  endef
endif

$(eval $(call BuildTarget))

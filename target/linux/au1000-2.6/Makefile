# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=au1000
BOARDNAME:=AMD Alchemy AU1x00
FEATURES:=jffs2

define Target/Description
	Build firmware for AMD Alchemy 1500 boards
	(e.g. 4G-Systems Mesh/Access Cube ...)
endef

include $(INCLUDE_DIR)/kernel-build.mk
DEFAULT_PACKAGES += yamonenv

define Kernel/BuildImage
	$(call Kernel/BuildImage/Default)
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.flash.srec $(LINUX_DIR)/zImage.flash.srec
	$(CP) $(LINUX_DIR)/arch/mips/boot/compressed/images/zImage.srec $(LINUX_DIR)/zImage.srec
endef

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))


#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=cris
BOARD:=etrax
BOARDNAME:=Foxboard (ETRAX 100LX)
FEATURES:=squashfs jffs2
LINUX_VERSION:=2.6.30.7
DEVICE_TYPE=

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage"

define Target/Description
	Build firmware images for the FOXBOARD made by acmesystems.it
endef

define Kernel/Prepare
	$(call Kernel/Prepare/Default)
	mkdir -p $(LINUX_DIR)/include/asm-cris
	$(CP) $(LINUX_DIR)/arch/cris/include/asm/*.h $(LINUX_DIR)/include/asm-cris
	mkdir -p $(LINUX_DIR)/include/asm-cris/arch
	$(CP) $(LINUX_DIR)/arch/cris/include/arch-v10/arch/*.h $(LINUX_DIR)/include/asm-cris/arch
endef

$(eval $(call BuildTarget))

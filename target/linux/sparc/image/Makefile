#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	cp $(LINUX_DIR)/arch/sparc/boot/zImage $(KDIR)/zImage
endef

define Image/BuildKernel
	mkdir -p $(BIN_DIR)
	cp $(KDIR)/zImage $(BIN_DIR)/$(IMG_PREFIX)-zImage
#	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
endef

$(eval $(call BuildImage))

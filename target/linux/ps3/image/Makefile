#
# Copyright (C) 2007-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	cp $(LINUX_DIR)/arch/powerpc/boot/otheros.bld $(KDIR)/image
endef

define Image/BuildKernel
	mkdir -p $(BIN_DIR)
	cp $(KDIR)/image $(BIN_DIR)/otheros.bld
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	cp $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf $(TARGET_DIR)
endef

$(eval $(call BuildImage))

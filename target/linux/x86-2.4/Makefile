# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

LINUX_VERSION:=2.4.32
LINUX_RELEASE:=1
LINUX_KERNEL_MD5SUM:=38f4d0830e95a20f4bfed17622d5557c

include ./config
include $(INCLUDE_DIR)/kernel.mk

$(eval $(call KMOD_template,WD1100,wd1100,\
	$(MODULES_DIR)/kernel/drivers/char/wd1100.o \
,CONFIG_WD1100))

include $(INCLUDE_DIR)/kernel-build.mk

$(LINUX_DIR)/.patched: $(LINUX_DIR)/.unpacked
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches
	@$(CP) config $(LINUX_DIR)/.config
	touch $@

$(LINUX_BUILD_DIR)/bzImage: $(LINUX_DIR)/vmlinux
	$(MAKE) -C $(LINUX_DIR) TOPDIR=$(LINUX_DIR) ARCH=$(LINUX_KARCH) PATH=$(TARGET_PATH) bzImage
	$(CP) $(LINUX_DIR)/arch/i386/boot/bzImage $@
	touch $@

compile: $(LINUX_BUILD_DIR)/bzImage

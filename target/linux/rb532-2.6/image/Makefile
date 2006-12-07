# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include $(INCLUDE_DIR)/kernel.mk

LOADADDR = 0x81000000		# RAM start + 16M 
KERNEL_ENTRY = 0x80101000
RAMSIZE = 0x00100000		# 1MB
IMAGE_COPY = 1

LOADER_MAKEOPTS= \
		KDIR=$(KDIR) \
		LOADADDR=$(LOADADDR) \
		KERNEL_ENTRY=$(KERNEL_ENTRY) \
		RAMSIZE=$(RAMSIZE) \
		IMAGE_COPY=$(IMAGE_COPY)

define Build/Compile
	$(CC) -o $(STAGING_DIR)/bin/patch-cmdline $(PLATFORM_DIR)/src/patch-cmdline.c
endef

define Build/Clean
	$(MAKE) -C ../../generic-2.6/image/lzma-loader $(LOADER_MAKEOPTS) clean
endef

define Image/Prepare
	$(STAGING_DIR)/bin/patch-cmdline $(LINUX_DIR)/vmlinux 'root=/dev/cfa2 '
	$(KERNEL_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $(LINUX_DIR)/vmlinux $(LINUX_KERNEL)
endef

define Image/BuildKernel
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C ../../generic-2.6/image/lzma-loader $(LOADER_MAKEOPTS) clean compile
	$(CP) $(KDIR)/loader.elf $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinux
endef

define Image/cmdline/jffs2-64k
block2mtd.block2mtd=/dev/cfa2,65536 root=/dev/mtdblock0 rootfstype=jffs2
endef

define Image/cmdline/jffs2-128k
block2mtd.block2mtd=/dev/cfa2,131072 root=/dev/mtdblock0 rootfstype=jffs2
endef

define Image/cmdline/ext2
root=/dev/cfa2 rootfstype=ext2
endef

define Image/Build
	$(STAGING_DIR)/bin/patch-cmdline $(LINUX_DIR)/vmlinux '$(strip $(call Image/cmdline/$(1))) '
	./gen_image.sh $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).bin 4 $(LINUX_DIR)/vmlinux 16 $(KDIR)/root.$(1)
endef

$(eval $(call BuildImage))

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

define Build/Clean
	$(MAKE) -C $(GENERIC_PLATFORM_DIR)/image/lzma-loader $(LOADER_MAKEOPTS) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C $(GENERIC_PLATFORM_DIR)/image/lzma-loader $(LOADER_MAKEOPTS) clean compile
endef

define Image/BuildKernel
	$(CP) $(KDIR)/loader.elf $(BIN_DIR)/openwrt-$(BOARD)-vmlinux
endef

define Image/cmdline/jffs2-64k
block2mtd.block2mtd=/dev/cfa2,65536 root=/dev/mtdblock0 rootfstype=jffs2 init=/etc/preinit
endef

define Image/cmdline/jffs2-128k
block2mtd.block2mtd=/dev/cfa2,131072 root=/dev/mtdblock0 rootfstype=jffs2 init=/etc/preinit
endef

define Image/cmdline/ext2
root=/dev/cfa2 rootfstype=ext2 init=/etc/preinit
endef

define Image/cmdline/squashfs
block2mtd.block2mtd=/dev/cfa2,65536 root=/dev/mtdblock0 rootfstype=squashfs init=/etc/preinit
endef

define Image/cmdline/yaffs2
root=/dev/mtdblock1 rootfstype=yaffs2 init=/etc/preinit
endef

define Image/Build
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux.elf '$(strip $(call Image/cmdline/$(1))) '
	./gen_image.sh $(BIN_DIR)/openwrt-$(BOARD)-$(1).bin 4 $(KDIR)/vmlinux.elf $(CONFIG_TARGET_ROOTFS_FSPART) $(KDIR)/root.$(1)
endef

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  define Image/Prepare
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux '$(strip $(call Image/cmdline/yaffs2)) '
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C $(GENERIC_PLATFORM_DIR)/image/lzma-loader $(LOADER_MAKEOPTS) clean compile	
  endef
  define Image/BuildKernel
	$(CP) $(KDIR)/loader.elf $(BIN_DIR)/openwrt-$(BOARD)-vmlinux
  endef
endif

$(eval $(call BuildImage))

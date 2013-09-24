#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

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

VMLINUX:=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux-kernel

define Image/BuildKernel
	$(CP) $(KDIR)/loader.elf $(VMLINUX)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/loader.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs
endif
endef

define Image/cmdline/jffs2-64k
block2mtd.block2mtd=/dev/sda2,65536 root=/dev/mtdblock0 rootfstype=jffs2
endef

define Image/cmdline/jffs2-128k
block2mtd.block2mtd=/dev/sda2,131072 root=/dev/mtdblock0 rootfstype=jffs2
endef

define Image/cmdline/ext2
root=/dev/sda2 rootfstype=ext2
endef

define Image/cmdline/squashfs
block2mtd.block2mtd=/dev/sda2,65536 root=/dev/mtdblock0 rootfstype=squashfs
endef

define Image/cmdline/yaffs2
root=/dev/mtdblock1 rootfstype=yaffs2
endef

define Image/Build
	$(CP) $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-$(1).kernel
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(BIN_DIR)/$(IMG_PREFIX)-$(1).kernel '$(strip $(call Image/cmdline/$(1))) '
	./gen_image.sh $(BIN_DIR)/$(IMG_PREFIX)-combined-$(1).bin 4 $(BIN_DIR)/$(IMG_PREFIX)-$(1).kernel $(CONFIG_TARGET_ROOTFS_PARTSIZE) $(KDIR)/root.$(1)
endef

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  define Image/Prepare
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-initramfs '$(strip $(call Image/cmdline/yaffs2)) '
	cat $(KDIR)/vmlinux-initramfs | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux-initramfs.lzma
	$(MAKE) -C $(GENERIC_PLATFORM_DIR)/image/lzma-loader $(LOADER_MAKEOPTS) clean compile
	$(CP) $(KDIR)/loader.elf $(KDIR)/loader-initramfs.elf
  endef
endif

$(eval $(call BuildImage))

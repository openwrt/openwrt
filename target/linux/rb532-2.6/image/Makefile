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
	$(MAKE) -C ../../generic-2.6/image/lzma-loader $(LOADER_MAKEOPTS) clean
endef

CMDLINE_SIZE=512
CMDLINE_OFFSET=4112

define Image/Prepare
	echo 'root=/dev/cfa2 ' | \
		dd bs=$(CMDLINE_SIZE) count=1 conv=sync | \
		dd of=$(LINUX_DIR)/vmlinux bs=$(CMDLINE_OFFSET) conv=notrunc seek=1
	$(KERNEL_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $(LINUX_DIR)/vmlinux $(LINUX_KERNEL)
endef

define Image/BuildKernel
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C ../generic/lzma-loader $(LOADER_MAKEOPTS) clean compile
	$(CP) $(KDIR)/loader.elf $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinux
endef

PARTITION1=\x80\x01\x01\x00\x27\x01\x20\x7b\x20\x00\x00\x00\xe0\x1e\x00\x00# 4 MB  (kernel part)
PARTITION2=\x00\x00\x01\x7c\x83\x01\xa0\x64\x00\x1f\x00\x00\x40\x7a\x00\x00# 16 MB (rootfs part)

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
	echo '$(strip $(call Image/cmdline/$(1))) ' | \
		dd bs=$(CMDLINE_SIZE) count=1 conv=sync | \
		dd of=$(LINUX_DIR)/vmlinux bs=$(CMDLINE_OFFSET) conv=notrunc seek=1
	( \
		echo -ne OWRT | dd bs=$$$$((0x1be)) conv=sync; \
		( \
			echo -ne '$(strip $(PARTITION1))'; \
			echo -ne '$(strip $(PARTITION2))'; \
		) | dd bs=$$$$((0x40)) conv=sync; \
		echo -ne '\x55\xaa'; \
		dd if=/dev/zero bs=$$$$((0x3e00)) conv=sync count=1; \
		dd if=$(LINUX_DIR)/vmlinux bs=$$$$((0x3dc000)) conv=sync; \
		cat $(KDIR)/root.$(1); \
		echo -ne '\xde\xad\xc0\xde'; \
	) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).bin
endef

$(eval $(call BuildImage))

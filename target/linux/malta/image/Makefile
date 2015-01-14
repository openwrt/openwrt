#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define get_kernel_entry
0x$(shell $(TARGET_CROSS)nm $(1) 2>/dev/null | grep " kernel_entry" | cut -f1 -d ' ')
endef

define CompressLzma
  $(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define CompressGzip
	gzip -9 -c $(1) > $(2)
endef

define MkuImage
	mkimage -A mips -O linux -T kernel -a 0x80100000 -C $(1) $(2) \
		-e $(call get_kernel_entry,$(KDIR)/vmlinux.debug) -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
		-d $(3) $(4)
endef

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	$(call MkuImage,lzma,,$(KDIR)/vmlinux.bin.lzma,$(BIN_DIR)/$(IMG_PREFIX)-uImage-lzma)
	$(call CompressGzip,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.gz)
	$(call MkuImage,gzip,,$(KDIR)/vmlinux.bin.gz,$(BIN_DIR)/$(IMG_PREFIX)-uImage-gzip)
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/Initramfs
	cp $(KDIR)/vmlinux-initramfs.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.elf
	cp $(KDIR)/vmlinux-initramfs $(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs.bin
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))

#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

VMLINUX:=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux
UIMAGE:=$(IMG_PREFIX)-uImage
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	VMLINUX:=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux-initramfs
	UIMAGE:=$(IMG_PREFIX)-uImage-initramfs
endif

define kernel_entry
-a 0x80002000 -e 0x80002000
endef


define CompressGzip
	gzip -9 -c $(1) > $(2)
endef

define MkImage
	mkimage -A mips -O linux -T kernel  $(call kernel_entry) -C $(1) $(2) \
		-n "ADM8668 Linux Kernel(2.4.31)" \
		-d $(3) $(4)
endef

define Build/Clean
	$(MAKE) -C lzma-loader clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
	$(MAKE) -C lzma-loader \
		KDIR="$(KDIR)" \
		clean compile
	rm -f $(KDIR)/fs_mark
	touch $(KDIR)/fs_mark
	$(call prepare_generic_squashfs,$(KDIR)/fs_mark)
endef

define Image/Build
	./my-mkimage $(KDIR)/loader.bin $(KDIR)/root.squashfs	\
		$(KDIR)/fs_mark $(BIN_DIR)/$(IMG_PREFIX)-$(1).bin
endef

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(VMLINUX).elf
	cp $(KDIR)/vmlinux $(VMLINUX).bin
	$(call CompressGzip,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.gz)
	$(call MkImage,gzip,,$(KDIR)/vmlinux.bin.gz,$(BIN_DIR)/$(UIMAGE)-gzip.bin)
endef

$(eval $(call BuildImage))

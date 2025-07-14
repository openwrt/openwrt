#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk
include ./nand.mk

define get_kernel_entry
0x$(shell $(TARGET_CROSS)nm $(1) 2>/dev/null | grep " kernel_entry" | cut -f1 -d ' ')
endef

SIGN := $(CURDIR)/sign_image
export SIGN
ifeq ($(CONFIG_TARGET_SIGNATURE_IMAGE), y)
	BOOT_FLAG := SECURITY_BOOT
else
	BOOT_FLAG :=
endif

#  $(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
define CompressLzma
  lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define CompressGzip
	gzip -9 -c $(1) > $(2)
endef

define MkuImage
	if [ "${BOOT_FLAG}" = "SECURITY_BOOT" ]; then \
		sh $(SIGN)/sign.sh $1 $(call get_kernel_entry,$(KDIR)/vmlinux.debug) $(3) $(4);	\
	else	\
		mkimage -A mips -O linux -T kernel -a 0x80100000 -C $(1) $(2) \
			-e $(call get_kernel_entry,$(KDIR)/vmlinux.debug) -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)' \
			-d $(3) $(4); \
	fi
endef

define Image/Prepare
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	$(call MkuImage,lzma,,$(KDIR)/vmlinux.bin.lzma,$(KDIR)/uImage.lzma)
	$(call CompressGzip,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.gz)
	$(call MkuImage,gzip,,$(KDIR)/vmlinux.bin.gz,$(KDIR)/uImage.gz)
endef

define Image/BuildKernel
endef

#use lzma uimage and xt compressed suqashfs
define Image/MakeSysupgradeBinary
	$(eval output_name=$(IMG_PREFIX)-squashfs-sysupgrade.bin)
	$(call Image/mkfs/squashfs)
	cat $(KDIR)/uImage.lzma $(KDIR)/root.squashfs > $(KDIR)/$(output_name)
	$(call prepare_generic_squashfs,$(KDIR)/$(output_name))
	$(CP) $(KDIR)/$(output_name) $(BIN_DIR)/$(output_name)
	$(call Build/append-metadata,$(BIN_DIR)/$(output_name))
endef

define Image/Build/Initramfs
	$(call CompressLzma,$(KDIR)/vmlinux-initramfs,$(KDIR)/vmlinux-initramfs.bin.lzma)
	$(call MkuImage,lzma,,$(KDIR)/vmlinux-initramfs.bin.lzma,$(KDIR)/uImage-initramfs.lzma)
endef

define Image/Build
	$(call Image/Build/$(1))
	$(if $(TARGET_DEVICES),,$(call Image/MakeSysupgradeBinary))
endef

$(eval $(call BuildImage))

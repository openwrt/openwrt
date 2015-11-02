# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuilduImage
	cat $(LINUX_DIR)/arch/arm/boot/dts/mt7623-evb.dtb >> $(KDIR)/zImage$(1)
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/zImage$(1) $(KDIR)/zImage$(1).lzma
	mkimage -A arm -O linux -T kernel -C lzma -a 0x80008000 -e 0x80008000 -n 'MIPS OpenWrt Linux-$(LINUX_VERSION)'  -d $(KDIR)/zImage$(1).lzma $(KDIR)/uImage$(1)
	$(CP) $(KDIR)/uImage$(1) $(BIN_DIR)/$(IMG_PREFIX)-uImage$(1)
endef

define Image/BuildKernel
	$(call Image/BuilduImage)
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(call Image/BuilduImage,-initramfs)
endif
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))

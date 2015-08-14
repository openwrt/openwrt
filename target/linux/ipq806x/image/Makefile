# Copyright (c) 2014 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

UBIFS_OPTS = -m 2048 -e 124KiB -c 4096 -U -F
UBINIZE_OPTS = -m 2048 -p 128KiB

KERNEL_LOADADDR := 0x42208000

define Image/Prepare
	$(CP) $(LINUX_DIR)/vmlinux $(KDIR)/$(IMG_PREFIX)-vmlinux.elf
endef

define Image/BuildKernel
	$(CP) $(KDIR)/$(IMG_PREFIX)-vmlinux.elf $(BIN_DIR)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-$(1)-root.img bs=2k conv=sync
endef

define Build/append-dtb
	cat $(DTS_DIR)/$(DEVICE_DTS).dtb >> $@
endef

define Device/Default
	PROFILES := Default
	KERNEL_INITRAMFS_PREFIX := $$(IMG_PREFIX)-$(1)-initramfs
	DEVICE_DTS :=
	KERNEL_PREFIX := $$(IMAGE_PREFIX)
endef
DEVICE_VARS += DEVICE_DTS

define Device/LegacyImage
	KERNEL_SUFFIX := -zImage
	KERNEL = kernel-bin | append-dtb
	KERNEL_NAME := zImage
	KERNEL_INSTALL := 1
endef

define Device/FitImage
	KERNEL_SUFFIX := -fit-uImage.itb
	KERNEL = kernel-bin | gzip | fit gzip $$(DTS_DIR)/$$(DEVICE_DTS).dtb
	KERNEL_NAME := Image
	KERNEL_INSTALL := 1
endef

define Device/AP148
	$(call Device/FitImage)
	DEVICE_DTS := qcom-ipq8064-ap148
endef

define Device/AP148-legacy
	$(call Device/LegacyImage)
	DEVICE_DTS := qcom-ipq8064-ap148
endef

define Device/DB149
	$(call Device/FitImage)
	DEVICE_DTS := qcom-ipq8064-db149
endef

TARGET_DEVICES += AP148 AP148-legacy DB149

$(eval $(call BuildImage))

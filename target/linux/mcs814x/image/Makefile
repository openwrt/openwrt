#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

TARGET_DTBS := rbt-832 dlan-usb-extender

LOADADDR:=0x00008000

UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	UIMAGE:=$(BIN_DIR)/$(IMG_PREFIX)-uImage-initramfs
endif

define Image/Build/MkuImage
	mkimage -A arm -O linux -T kernel -a $(LOADADDR) -C none -e $(LOADADDR) \
	-n 'ARM OpenWrt Linux-$(LINUX_VERSION)' -d $(1) $(2);
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/$(ARCH)/boot/zImage $(KDIR)/zImage
endef

define Image/BuildKernel
	$(foreach dtb,$(TARGET_DTBS),cp $(KDIR)/zImage $(KDIR)/zImage-$(dtb);)
	$(foreach dtb,$(TARGET_DTBS),cat $(LINUX_DIR)/arch/$(ARCH)/boot/$(dtb).dtb >> $(KDIR)/zImage-$(dtb);)
	$(foreach dtb,$(TARGET_DTBS),$(call Image/Build/MkuImage,$(KDIR)/zImage-$(dtb),$(KDIR)/uImage-$(dtb)))
	$(foreach dtb,$(TARGET_DTBS),cp $(KDIR)/uImage-$(dtb) $(UIMAGE)-$(dtb);)
	$(call Image/Build/Initramfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))

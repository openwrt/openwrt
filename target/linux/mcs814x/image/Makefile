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

define Image/Build/DTB
	cp $(KDIR)/zImage $(KDIR)/zImage-$(1);
	cat $(LINUX_DIR)/arch/$(ARCH)/boot/$(1).dtb >> $(KDIR)/zImage-$(1);
	$(call Image/Build/MkuImage,$(KDIR)/zImage-$(1),$(KDIR)/uImage-$(1))
	cp $(KDIR)/uImage-$(1) $(UIMAGE)-$(1);
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/$(ARCH)/boot/zImage $(KDIR)/zImage
endef

define Image/Build/dlan
	dd if=$(KDIR)/uImage-dlan-usb-extender bs=1M conv=sync of=$(KDIR)/$(IMG_PREFIX)-dlan-usb-extender-upgrade-$(1).bin
	cat $(KDIR)/root.$(1) >> $(KDIR)/$(IMG_PREFIX)-dlan-usb-extender-upgrade-$(1).bin
	cp $(KDIR)/$(IMG_PREFIX)-dlan-usb-extender-upgrade-$(1).bin $(BIN_DIR)/
endef

define Image/BuildKernel
	$(foreach dtb,$(TARGET_DTBS),$(call Image/Build/DTB,$(dtb)))
	$(call Image/Build/Initramfs)
endef

define Image/Build
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
	$(call Image/Build/$(1))
	$(call Image/Build/dlan,$(1))
endef

$(eval $(call BuildImage))

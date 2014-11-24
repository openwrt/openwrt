#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	-rm -rf $(KDIR)/linux-system.axf
	cp $(LINUX_DIR)/arch/$(LINUX_KARCH)/boot/Image $(KDIR)/Image
	$(MAKE) -C boot-wrapper clean
	$(MAKE) -C boot-wrapper compile
endef

define Build/Clean
	$(MAKE) -C boot-wrapper clean
endef

define Image/Build/QemuVirt
	$(CP) $(KDIR)/Image $(BIN_DIR)/openwrt-$(BOARD)-qemu-virt.Image
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
	$(CP) $(KDIR)/Image-initramfs $(BIN_DIR)/openwrt-$(BOARD)-qemu-virt-initramfs.Image
endif
endef

define Image/Build/VexpressFoundation
	$(CP) $(KDIR)/linux-system.axf $(BIN_DIR)/openwrt-$(BOARD)-vexpress-foundation.axf
endef

define Image/BuildKernel
	$(call Image/Build/QemuVirt)
	$(call Image/Build/VexpressFoundation)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))

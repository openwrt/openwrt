#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	cp $(KDIR)/vmlinuz $(BIN_DIR)/$(IMG_PREFIX)-zImage
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/cris/boot/zImage $(KDIR)/vmlinuz
	$(MAKE) -C e100boot compile
	$(MAKE) -C mkfimage compile
	$(INSTALL_BIN) ./boot_linux $(BIN_DIR)
endef

define Image/Build/generic
	mkfimage $(KDIR)/vmlinuz $(KDIR)/vmlinuz.tmp
	cat $(KDIR)/vmlinuz.tmp $(KDIR)/root.$(1) > $(KDIR)/fimage.$(1).tmp
	dd if=$(KDIR)/fimage.$(1).tmp of=$(KDIR)/fimage.$(1) bs=$(2) conv=sync
	cp $(KDIR)/fimage.$(1) $(BIN_DIR)/$(IMG_PREFIX)-$(1)-fimage
endef

define Image/Build/jffs2-64k
	$(call prepare_generic_jffs-64k,$(KDIR)/root.jff2-64k)
	$(call Image/Build/generic,$(1),4194304)
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	$(call Image/Build/generic,$(1),4194304)
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))

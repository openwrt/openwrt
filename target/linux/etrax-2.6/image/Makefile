# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

FOXBOARD:=custom MCM 416 816 832
FOXBOARD_4MB:=MCM 416 
FOXBOARD_8MB:=custom 816 832

define Image/BuildKernel
	for f in $(FOXBOARD); do \
		cp $(KDIR)/vmlinuz_$$$$f $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-zImage_$$$$f; \
	done
endef

define Image/Prepare
	for f in $(FOXBOARD); do \
		cp $(LINUX_DIR)/arch/cris/boot/zImage_$$$$f $(KDIR)/vmlinuz_$$$$f; \
	done
	$(MAKE) -C ./e100boot/
	$(MAKE) -C ./mkfimage/
	$(INSTALL_BIN) ./boot_linux $(BIN_DIR)
endef

define Image/Build/generic
	for f in $(2); do \
		mkfimage $(KDIR)/vmlinuz_$$$$f $(KDIR)/vmlinuz_$$$$f.tmp ; \
		cat $(KDIR)/vmlinuz_$$$$f.tmp $(KDIR)/root.$(1) > $(KDIR)/fimage.$(1)_$$$$f.tmp; \
		dd if=$(KDIR)/fimage.$(1)_$$$$f.tmp of=$(KDIR)/fimage.$(1)_$$$$f bs=$(3) conv=sync; \
		cp $(KDIR)/fimage.$(1)_$$$$f $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1)-fimage_$$$$f; \
	done	
endef

define Image/Build/jffs2-64k
	$(call prepare_generic_jffs-64k,$(KDIR)/root.jff2-64k)
	$(call Image/Build/generic,$(1),$(FOXBOARD_4MB),4194304)
	$(call Image/Build/generic,$(1),$(FOXBOARD_8MB),8388608)	
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
	$(call Image/Build/generic,$(1),$(FOXBOARD_4MB),4194304)
	$(call Image/Build/generic,$(1),$(FOXBOARD_8MB),8388608)	
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
endef

$(eval $(call BuildImage))

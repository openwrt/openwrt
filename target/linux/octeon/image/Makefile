#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	# Workaround pre-SDK-1.9.0 u-boot versions not handling the .notes section
	$(TARGET_CROSS)strip -R .notes $(KDIR)/vmlinux.elf -o $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux.lzma bs=65536 conv=sync
	$(CP) $(KDIR)/vmlinux.elf $(BIN_DIR)/vmlinux.64
	md5sum  $(BIN_DIR)/vmlinux.64 | cut -d " " -f 1 | tee $(BIN_DIR)/vmlinux.64.md5
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/ext4/erlite
	mkdir -p $(KDIR)/sysupgrade-erlite/
	echo "BOARD=erlite" > $(KDIR)/sysupgrade-erlite/CONTROL
	$(CP) $(KDIR)/vmlinux.elf $(KDIR)/sysupgrade-erlite/kernel
	$(CP) $(KDIR)/root.ext4 $(KDIR)/sysupgrade-erlite/root
	(cd $(KDIR); $(TAR) cvf \
		$(BIN_DIR)/$(IMG_PREFIX)-erlite-sysupgrade.tar sysupgrade-erlite)
endef

define Image/Build/ext4
    $(call Image/Build/ext4/erlite)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))

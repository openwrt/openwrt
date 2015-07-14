#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
	gzip -9n -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/$(IMG_PREFIX)-vmlinux.gz bs=65536 conv=sync
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=128k conv=sync

	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B XS2 -v XS2.ar2316.OpenWrt.$(REVISION) \
		-k $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.lzma \
		-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		-o $(BIN_DIR)/$(IMG_PREFIX)-ubnt2-$(1).bin

	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B XS5 -v XS5.ar2313.OpenWrt.$(REVISION) \
		-k $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.lzma \
		-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		-o $(BIN_DIR)/$(IMG_PREFIX)-ubnt5-$(1).bin

	-$(STAGING_DIR_HOST)/bin/mkfwimage \
		-B XS2-8 -v XS2.ar2316.OpenWrt.$(REVISION) \
		-k $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.lzma \
		-r $(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		-o $(BIN_DIR)/$(IMG_PREFIX)-ubnt2-pico2-$(1).bin

	-$(STAGING_DIR_HOST)/bin/mkmylofw -B np25g \
		-p0x020000:0x130000:ah:0x80041000:linux:$(KDIR)/vmlinux.bin.gz  \
		-p0x150000:0x2a0000:::rootfs:$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		$(BIN_DIR)/$(IMG_PREFIX)-np25g-$(1).bin

	-$(STAGING_DIR_HOST)/bin/mkmylofw -B wpe53g \
		-p0x020000:0x130000:ah:0x80041000:linux:$(KDIR)/vmlinux.bin.gz  \
		-p0x150000:0x2a0000:::rootfs:$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) \
		$(BIN_DIR)/$(IMG_PREFIX)-wpe53g-$(1).bin

	-sh $(TOPDIR)/scripts/combined-image.sh \
		"$(BIN_DIR)/$(IMG_PREFIX)-vmlinux.lzma" \
		"$(BIN_DIR)/$(IMG_PREFIX)-root.$(1)" \
		"$(BIN_DIR)/$(IMG_PREFIX)-combined.$(1).img"
endef

$(eval $(call BuildImage))

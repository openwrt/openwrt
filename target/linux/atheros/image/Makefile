# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.elf
	gzip -9 -c $(KDIR)/vmlinux > $(KDIR)/vmlinux.bin.gz
	$(STAGING_DIR_HOST)/bin/lzma e $(KDIR)/vmlinux $(KDIR)/vmlinux.bin.l7
	dd if=$(KDIR)/vmlinux.bin.l7 of=$(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma bs=65536 conv=sync
	dd if=$(KDIR)/vmlinux.bin.gz of=$(BIN_DIR)/openwrt-$(BOARD)-vmlinux.gz bs=65536 conv=sync
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	$(call Image/Build/$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-root.$(1) bs=128k conv=sync

	echo -en "kernel\t\t0x01\t0xBE030000\t0x" > $(BIN_DIR)/ls5-parts
	echo -en "$(shell printf "%08X\n" $(shell cat $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma | wc -c))" >> $(BIN_DIR)/ls5-parts
	echo -e "\t0x80041000\t0x80041000\t$(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma" >> $(BIN_DIR)/ls5-parts 
	echo -en "rootfs\t\t0x02\t0x" >> $(BIN_DIR)/ls5-parts
	echo -en "$(shell printf "%08X\n" $(shell expr $(shell cat $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma | wc -c) + 3187867648 ) )" >> $(BIN_DIR)/ls5-parts
	echo -en "\t0x$(shell printf "%08X\n" $(shell expr 3735552 - $(shell cat $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma | wc -c) ) )" >> $(BIN_DIR)/ls5-parts
	echo -e "\t0x00000000\t0x00000000\t$(BIN_DIR)/openwrt-$(BOARD)-root.jffs2-64k" >> $(BIN_DIR)/ls5-parts
	
	echo -en "kernel\t\t0x01\t0xBFC30000\t0x" > $(BIN_DIR)/ls2-parts
	echo -en "$(shell printf "%08X\n" $(shell cat $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma | wc -c))" >> $(BIN_DIR)/ls2-parts
	echo -e "\t0x80041000\t0x80041000\t$(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma" >> $(BIN_DIR)/ls2-parts
	echo -en "rootfs\t\t0x02\t0x" >> $(BIN_DIR)/ls2-parts
	echo -en "$(shell printf "%08X\n" $(shell expr $(shell cat $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma | wc -c) + 3217227776 ) )" >> $(BIN_DIR)/ls2-parts
	echo -en "\t0x$(shell printf "%08X\n" $(shell expr 3866624 - $(shell cat $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.lzma | wc -c) ) )" >> $(BIN_DIR)/ls2-parts
	echo -e "\t0x00000000\t0x00000000\t$(BIN_DIR)/openwrt-$(BOARD)-root.$(1)" >> $(BIN_DIR)/ls2-parts

	$(STAGING_DIR_HOST)/bin/mkfwimage -i $(BIN_DIR)/ls2-parts -v XS2.ar2316.OpenWRT -o $(BIN_DIR)/openwrt-$(BOARD)-litestation2-$(1).bin
	$(STAGING_DIR_HOST)/bin/mkfwimage -i $(BIN_DIR)/ls5-parts -v XS5.ar2313.OpenWRT -o $(BIN_DIR)/openwrt-$(BOARD)-litestation5-$(1).bin
	cp $(BIN_DIR)/openwrt-$(BOARD)-litestation5-$(1).bin $(BIN_DIR)/openwrt-$(BOARD)-nanostation5-$(1).bin
	cp $(BIN_DIR)/openwrt-$(BOARD)-litestation2-$(1).bin $(BIN_DIR)/openwrt-$(BOARD)-nanostation2-$(1).bin
	rm $(BIN_DIR)/ls2-parts $(BIN_DIR)/ls5-parts
endef

$(eval $(call BuildImage))

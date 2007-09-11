#
# Copyright (C) 2006,2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADER_MAKE := $(NO_TRACE_MAKE) -C lzma-loader KDIR=$(KDIR)
IMGNAME := $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)

define imgname
$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(patsubst jffs2-%,jffs2,$(1))-$(2)
endef

define Build/Clean
	$(LOADER_MAKE) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR_HOST)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
endef

define Image/Build/Loader
	$(LOADER_MAKE) LOADER=loader-$(1).$(2) LOADER_DATA="" \
		LZMA_TEXT_START=$(3) LZMA_STARTUP_ORG=$(4) \
		CONFIG_PASS_KARGS=$(5) CONFIG_BOARD=$(6) \
		compile loader.$(2)
endef

define Image/Build/LZMAKernel
	$(LOADER_MAKE) TARGET_DIR=$(BIN_DIR) \
		LOADER=openwrt-$(BOARD)-$(KERNEL)-ramfs-lzma-$(1).$(2) \
		LOADER_DATA=$(KDIR)/vmlinux.lzma \
		LZMA_TEXT_START=$(3) LZMA_STARTUP_ORG=$(4) \
		CONFIG_PASS_KARGS=$(5) CONFIG_BOARD=$(6) \
		compile loader.$(2)
endef

define Image/Build/LZMAKernel/Generic
	$(call Image/Build/LZMAKernel,$(1),$(2),0x80500000,0)
endef

define Image/Build/LZMAKernel/Admboot
	$(call Image/Build/LZMAKernel,$(1),$(2),0x80500000,0x6D8,y,$(1))
endef

define Image/Build/LZMAKernel/KArgs
	$(call Image/Build/LZMAKernel,$(1),$(2),0x80500000,0,y,$(1))
endef

define trxalign/jffs2-128k
-a 0x20000 -f $(KDIR)/root.jffs2-128k
endef
define trxalign/jffs2-64k
-a 0x10000 -f $(KDIR)/root.jffs2-64k
endef
define trxalign/squashfs
-a 1024 -f $(KDIR)/root.squashfs
endef

define Image/Build/TRX
	$(STAGING_DIR_HOST)/bin/trx -o $(1) -f $(3) -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(2))
endef

define Image/Build/TRXNoloader
	$(STAGING_DIR_HOST)/bin/trx -o $(1) -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(2))
endef

include $(SUBTARGET).mk

define Image/Build
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

define Image/Build/Initramfs
	$(call Image/Build/Profile/$(PROFILE),Initramfs)
endef

$(eval $(call BuildImage))

#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

IMGNAME = $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)
LOADER_MAKE = $(NO_TRACE_MAKE) -C lzma-loader KDIR=$(KDIR)

define Image/Build/Loader
	$(LOADER_MAKE) LOADER=loader-$(1).$(2) LOADER_DATA="" \
		LZMA_TEXT_START=$(3) LZMA_STARTUP_ORG=$(4) \
		CONFIG_PASS_KARGS=$(5) CONFIG_BOARD=$(6) \
		compile loader.$(2)
endef

define Build/Clean
	$(LOADER_MAKE) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
endef

define trxalign/jffs2-128k
-a 0x20000
endef
define trxalign/jffs2-64k
-a 0x10000
endef
define trxalign/squashfs
-a 1024
endef

define Image/Build/TRX
	$(STAGING_DIR)/bin/trx -o $(1) -f $(3) -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(2)) -f $(KDIR)/root.$(2)
endef

define Image/Build/TRXNoloader
	$(STAGING_DIR)/bin/trx -o $(1) -f $(KDIR)/vmlinux.lzma \
		$(call trxalign/$(2)) -f $(KDIR)/root.$(2)
endef

define Image/Build/ZyXEL
	$(call Image/Build/Loader,loader-$(2),gz,0x80500000,0)
	$(call Image/Build/TRXNoloader,$(IMGNAME)-$(3)-$(2).trx,$(1))
	#TODO: firmware generation tool needed
endef

define Image/Build
	#FIXME: No firmware images yet
endef

define Image/Build/LZMAKernel
	$(LOADER_MAKE) TARGET_DIR=$(BIN_DIR) \
		LOADER=openwrt-$(BOARD)-$(KERNEL)-ramfs-lzma-$(1).$(2) \
		LOADER_DATA=$(KDIR)/vmlinux.lzma \
		LZMA_TEXT_START=$(3) LZMA_STARTUP_ORG=$(4) \
		CONFIG_PASS_KARGS=$(5) CONFIG_BOARD=$(6) \
		compile loader.$(2)
endef

define Image/Build/LZMAKernel/ZyXEL
	$(call Image/Build/LZMAKernel,$(1),$(2),0x80500000,0,y,$(1))
endef

define Image/Build/Initramfs
	$(call Image/Build/LZMAKernel/ZyXEL,p-334wt,bin)
	$(call Image/Build/LZMAKernel/ZyXEL,p-335,bin)
endef

$(eval $(call BuildImage))


#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

LOADER_MAKE = $(MAKE) -C lzma-loader KDIR=$(KDIR)

define CompileLoader
	$(LOADER_MAKE) LOADER=$(1) LOADER_DATA="" \
		LZMA_TEXT_START=$(2) LZMA_STARTUP_ORG=$(3) \
		compile
endef

define CompileLZMAKernel
	$(LOADER_MAKE) LOADER=vmlinux-lzma-$(1) LOADER_DATA=$(KDIR)/vmlinux.lzma \
		LZMA_TEXT_START=$(2) LZMA_STARTUP_ORG=$(3) \
		compile
endef

define Build/Clean
	$(LOADER_MAKE) clean
endef

define Image/Prepare
	cat $(KDIR)/vmlinux | $(STAGING_DIR)/bin/lzma e -si -so -eos -lc1 -lp2 -pb2 > $(KDIR)/vmlinux.lzma
ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	$(call CompileLoader,zyxel,0x80500000,0)
else	
	$(call CompileLZMAKernel,zyxel,0x80500000,0)
endif
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

define Image/Build/ZyXEL
	$(CP) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1)-noloader.trx $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).trx
endef

define Image/Build
	$(STAGING_DIR)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1)-noloader.trx -f $(KDIR)/vmlinux.lzma $(call trxalign/$(1)) -f $(KDIR)/root.$(1)
ifneq ($(1),jffs2-128K)
	#FIXME: firware images yet
endif
endef

define Image/Build/LZMAKernel
	$(CP) $(KDIR)/loader-vmlinux-lzma-$(2).$(3) \
	    $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinux-lzma-$(1).$(3)
endef

define Image/Build/Initramfs
	$(call Image/Build/LZMAKernel,p-334wt,zyxel,bin)
	$(call Image/Build/LZMAKernel,p-335wt,zyxel,bin)
endef

$(eval $(call BuildImage))


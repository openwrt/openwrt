# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Build/Compile
	rm -f $(KDIR)/loader.gz
	$(MAKE) -C lzma-loader \
		BUILD_DIR="$(KDIR)" \
		TARGET="$(KDIR)" \
		install
	echo -ne "\\x00" >> $(KDIR)/loader.gz
endef

define Build/Clean
	$(MAKE) -C lzma-loader clean
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

define Image/Build/Compex
	$(CP) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).trx
endef

define Image/Build/Edimax
	# FIXME : not tested (hardware ?)
	$(STAGING_DIR)/bin/mksyshdr $(KDIR)/csys $(KDIR)/vmlinux
	cat $(KDIR)/csys $(KDIR)/loader.gz $(KDIR)/vmlinux.lzma $(KDIR)/root.$(1) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).img
endef

define Image/Build/MyLoader
	$(CP) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1)-noloader.trx $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).trx
	$(STAGING_DIR)/bin/mkmylofw -B $(4) \
		-p0x20000:0x10000:ahp:0x80001000 \
		-p0x30000:0 \
		-b0x20000:0x10000:h:$(KDIR)/loader.gz \
		-b0x30000:0::$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).trx \
		$(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).bin
endef

define Image/Build
	$(STAGING_DIR)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx -f $(KDIR)/loader.gz -f $(KDIR)/vmlinux.lzma $(call trxalign/$(1)) -f $(KDIR)/root.$(1)
	$(STAGING_DIR)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1)-noloader.trx -f $(KDIR)/vmlinux.lzma $(call trxalign/$(1)) -f $(KDIR)/root.$(1)
ifneq ($(1),jffs2-128K)
	$(call Image/Build/Compex,$(1),wp54g-wrt,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Edimax,$(1),br6104,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/MyLoader,$(1),np27g,$(patsubst jffs2-%,jffs2,$(1)),NP27G)
	$(call Image/Build/MyLoader,$(1),np28g,$(patsubst jffs2-%,jffs2,$(1)),NP28G)
	$(call Image/Build/MyLoader,$(1),np28ghs,$(patsubst jffs2-%,jffs2,$(1)),NP28GHS)
	$(call Image/Build/MyLoader,$(1),wp54g,$(patsubst jffs2-%,jffs2,$(1)),WP54G)
	$(call Image/Build/MyLoader,$(1),wp54ag,$(patsubst jffs2-%,jffs2,$(1)),WP54AG)
	$(call Image/Build/MyLoader,$(1),wpp54g,$(patsubst jffs2-%,jffs2,$(1)),WPP54G)
	$(call Image/Build/MyLoader,$(1),wpp54ag,$(patsubst jffs2-%,jffs2,$(1)),WPP54AG)
endif
endef

$(eval $(call BuildImage))

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
	$(STAGING_DIR)/bin/mksyshdr csys $(KDIR)/vmlinux
	cat csys $(KDIR)/vmlinux $(KDIR)/root.$(1) > $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).img
endef

define Image/Build
	$(STAGING_DIR)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx -f $(KDIR)/loader.gz -f $(KDIR)/vmlinux.lzma $(call trxalign/$(1)) -f $(KDIR)/root.$(1)
ifneq ($(1),jffs2-128K)
	$(call Image/Build/Compex,$(1),wp54g,$(patsubst jffs2-%,jffs2,$(1)))
	$(call Image/Build/Edimax,$(1),br6104,$(patsubst jffs2-%,jffs2,$(1)))
endif
endef

$(eval $(call BuildImage))

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

define Image/Build
	$(STAGING_DIR)/bin/trx -o $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).trx -f $(KDIR)/loader.gz -f $(KDIR)/vmlinux.lzma $(call trxalign/$(1)) -f $(KDIR)/root.$(1)
endef

$(eval $(call BuildImage))

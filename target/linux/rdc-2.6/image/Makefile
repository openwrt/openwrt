# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

define Image/Prepare
	$(CP) $(LINUX_DIR)/arch/i386/boot/bzImage $(KDIR)/bzImage
endef

define trxalign/jffs2-128k
bs=128k
endef
define trxalign/jffs2-64k
bs=64k
endef
define trxalign/squashfs
bs=1024
endef

define Image/Build/Airlink
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1)-$(2).img
	mv $(KDIR)/root.$(1) $(KDIR)/root.tmp
	dd of=$(KDIR)/root.$(1) if=$(KDIR)/root.tmp $(call trxalign/$(1)) conv=sync
	rm -f $(KDIR)/root.tmp
	$(STAGING_DIR)/bin/airlink -b 1 -j $(shell bash -c 'echo $$[$(3)]') $(KDIR)/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1)-$(2).img
endef

define Image/Build
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).bzImage
	$(call Image/Build/Airlink,$(1),$(PROFILE),$(patsubst jffs2-%k,%,$(1)))
endef

$(eval $(call BuildImage))

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
-a 0x20000
endef
define trxalign/jffs2-64k
-a 0x10000
endef
define trxalign/squashfs
-a 1024
endef

define Image/Build/Airlink
	touch $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).img
ifneq ($(1),squashfs)
	$(STAGING_DIR)/bin/airlink -b 1 -j 64 $(KDIR)/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).img
else
	$(STAGING_DIR)/bin/airlink -b 1 $(KDIR)/bzImage $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(3)-$(2).img
endif
endef

define Image/Build
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL).bzImage
ifneq ($(1),jffs2-128K)
	$(call Image/Build/Airlink,$(1),ar525w,$(patsubst jffs2-%,jffs2,$(1)))
endif
endef

$(eval $(call BuildImage))

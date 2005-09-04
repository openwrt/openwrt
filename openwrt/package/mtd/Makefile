# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := mtd
PKG_RELEASE := 1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

ifeq ($(LINUX_VERSION),2.4.30)
VER=-DLINUX_24
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MTD,$(PKG_NAME),$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $@
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) -I$(LINUX_DIR)/include $(VER) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/mtd mtd.c
	touch $@

$(IPKG_MTD): 
	install -d -m0755 $(IDIR_MTD)/sbin
	install -m0755 $(PKG_BUILD_DIR)/mtd $(IDIR_MTD)/sbin
	$(RSTRIP) $(IDIR_MTD)/sbin/*
	$(IPKG_BUILD) $(IDIR_MTD) $(PACKAGE_DIR)

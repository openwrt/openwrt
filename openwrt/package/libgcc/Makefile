# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := libgcc
PKG_VERSION := $(BR2_GCC_VERSION)
PKG_RELEASE := 1

PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR :=$(PKG_BUILD_DIR)/ipkg/

.NOTPARALLEL:

$(PKG_IPK): $(STAGING_DIR)/lib/libpthread.so
	rm -rf $(PKG_BUILD_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/lib
	cp -fp $(STAGING_DIR)/lib/libgcc_s.so.* $(PKG_IPK_DIR)/lib/
	$(STRIP) $(PKG_IPK_DIR)/lib/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source:
prepare:
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/$(PKG_NAME).list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)

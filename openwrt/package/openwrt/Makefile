# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := openwrt-utils
PKG_RELEASE := 2
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

SHARED_INCLUDE:=${shell pwd}/include

libshared-compile libnvram-compile:
	@$(MAKE) -C $(patsubst %-compile,%,$@) compile
	
libshared-install libnvram-install: libshared-compile libnvram-compile
	@$(MAKE) -C $(patsubst %-install,%,$@) install

libshared-clean libnvram-clean wlc-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean

$(PKG_IPK_DIR)/sbin/mtd: mtd.c
	mkdir -p $(PKG_IPK_DIR)/sbin
	$(TARGET_CC) -o $@ $<

$(PKG_IPK_DIR)/sbin/jffs2root: jffs2root.c
	mkdir -p $(PKG_IPK_DIR)/sbin
	$(TARGET_CC) -o $@ $<

$(PKG_IPK_DIR)/usr/sbin/wlc: wlc.c libshared-install
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	$(TARGET_CC) -o $@ $< -lshared -L./libshared -I$(SHARED_INCLUDE)

$(PKG_IPK): $(PKG_IPK_DIR)/sbin/mtd $(PKG_IPK_DIR)/sbin/jffs2root $(PKG_IPK_DIR)/usr/sbin/wlc
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_RELEASE) $(ARCH)
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	$(STRIP) $(PKG_IPK_DIR)/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/$(PKG_NAME).list: $(PKG_IPK)
	$(IPKG) install $<

source: 
prepare: 
compile: prepare libnvram-compile libshared-compile $(PKG_IPK) $(PKG_WLCOMPAT)
	mkdir -p $(STAGING_DIR)/usr/include
	cp -a ./include/* $(STAGING_DIR)/usr/include/
install: libnvram-install libshared-install \
	 $(IPKG_STATE_DIR)/info/$(PKG_NAME).list \

clean: libshared-clean libnvram-clean
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)

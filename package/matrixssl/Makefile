# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := matrixssl
PKG_VERSION := 1.2.4
PKG_RELEASE := 1
PKG_MD5SUM := 771c1489488e62668d673478311d63ca

PKG_SOURCE_SITE := http://nbd.vd-s.ath.cx/openwrt \
		   http://openwrt.openbsd-geek.de/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/lib$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/src/libmatrixssl.so: $(PKG_BUILD_DIR)/.patched
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
	  $(TARGET_CONFIGURE_OPTS) \
	  DFLAGS="$(TARGET_CFLAGS) -Wall"

$(STAGING_DIR)/lib/libmatrixssl.so: $(PKG_BUILD_DIR)/src/libmatrixssl.so
	cp -a $(PKG_BUILD_DIR)/src/libmatrixssl.so* $(STAGING_DIR)/lib
	mkdir -p $(STAGING_DIR)/include/matrixSsl
	cp $(PKG_BUILD_DIR)/matrixSsl.h $(STAGING_DIR)/include/matrixSsl
	ln -sf matrixSsl/matrixSsl.h $(STAGING_DIR)/include/matrixSsl.h

$(PKG_IPK): $(STAGING_DIR)/lib/libmatrixssl.so
	mkdir -p $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BUILD_DIR)/ipkg/matrixssl matrixssl.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_BUILD_DIR)/ipkg/matrixssl/usr/lib
	cp -a $(PKG_BUILD_DIR)/src/libmatrixssl.so* $(PKG_BUILD_DIR)/ipkg/matrixssl/usr/lib
	$(STRIP) $(PKG_BUILD_DIR)/ipkg/matrixssl/usr/lib/*
	$(IPKG_BUILD) $(PKG_BUILD_DIR)/ipkg/matrixssl $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/libmatrixssl.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/lib/libmatrixssl.so $(PKG_IPK)
install: $(PKG_ZLIB) $(IPKG_STATE_DIR)/info/libmatrixssl.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/libmatrixssl* $(STAGING_DIR)/include/matrixssl*
	rm -f $(PKG_IPK)


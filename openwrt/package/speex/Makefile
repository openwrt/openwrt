# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := speex
PKG_VERSION := 1.1.7
PKG_RELEASE := 1
PKG_MD5SUM := a900d009f937efbb896f1f816193742f

PKG_SOURCE_SITE := http://us.speex.org/download 
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/ \
		--exec-prefix=/ \
		--bindir=/bin \
		--sbindir=/sbin \
		--libexecdir=/lib \
		--sysconfdir=/etc \
		--localstatedir=/var \
		--enable-shared \
		--disable-oggtest \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/libspeex/libspeex.la: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  bin_PROGRAMS=""

$(STAGING_DIR)/lib/libspeex.a: $(PKG_BUILD_DIR)/libspeex/libspeex.la
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  CC=$(TARGET_CC) \
	  DESTDIR="$(STAGING_DIR)" \
	  bin_PROGRAMS="" \
	  install

$(PKG_IPK): $(STAGING_DIR)/lib/libspeex.a
	mkdir -p $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BUILD_DIR)/ipkg/speex speex.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_BUILD_DIR)/ipkg/speex/usr/lib
	cp -a $(STAGING_DIR)/lib/libspeex.so.* $(PKG_BUILD_DIR)/ipkg/speex/usr/lib
	$(STRIP) $(PKG_BUILD_DIR)/ipkg/speex/usr/lib/*
	$(IPKG_BUILD) $(PKG_BUILD_DIR)/ipkg/speex $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/speex.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(STAGING_DIR)/lib/libspeex.a $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/speex.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/libspeex*
	rm -f $(PACKAGE_DIR)/speex\*.ipk

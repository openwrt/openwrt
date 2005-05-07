# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := libpng
PKG_VERSION := 1.2.8
PKG_RELEASE := 1
PKG_MD5SUM := e5a39003eed16103cbbd3b6a8bc6b1f9

PKG_SOURCE_SITE := @SF/libpng
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION)-config.tar.gz
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)-config
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_CAT := zcat
PKG_IPK_DIR := $(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE_FILE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE_FILE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=$(STAGING_DIR)/usr \
			--with-zlib=$(STAGING_DIR)/usr/ \
			$(DISABLE_NLS) \
	)
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.libs/libpng.so: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib" \
		all

$(STAGING_DIR)/usr/lib/libpng.so: $(PKG_BUILD_DIR)/.libs/libpng.so
	$(MAKE) -C $(PKG_BUILD_DIR) \
		install
#		DESTDIR="$(STAGING_DIR)" \
#		install-libLTLIBRARIES install-includeHEADERS install-binSCRIPTS

$(PKG_IPK): $(PKG_BUILD_DIR)/.libs/libpng.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp -a $(PKG_BUILD_DIR)/.libs/*.so* $(PKG_IPK_DIR)/usr/lib/
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*
	find $(PKG_IPK_DIR) -name CVS | xargs rm -rf
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/libpng.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(STAGING_DIR)/usr/lib/libpng.so $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/libpng.list

clean:  
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/usr/bin/libpng*
	rm -f $(STAGING_DIR)/usr/lib/libpng*
	rm -f $(PKG_IPK)

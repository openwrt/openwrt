# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME := gd
PKG_VERSION := 2.0.33
PKG_RELEASE := 1
PKG_MD5SUM := be0a6d326cd8567e736fbc75df0a5c45

PKG_SOURCE_SITE := http://www.boutell.com/gd/http
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_CAT := zcat
PKG_IPK_DIR := $(PKG_BUILD_DIR)/ipkg

$(DL_DIR)/$(PKG_SOURCE_FILE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE_FILE) $(PKG_MD5SUM) $(PKG_SOURCE_SITE)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE_FILE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR) ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		LIBPNG12_CONFIG="$(STAGING_DIR)/usr/bin/libpng12-config" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		sys_lib_dlsearch_path_spec="$(STAGING_DIR)/lib $(STAGING_DIR)/usr/lib" \
		sys_lib_search_path_spec="$(STAGING_DIR)/lib $(STAGING_DIR)/usr/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix="$(STAGING_DIR)/usr" \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--sbindir=/usr/sbin \
			--libexecdir=/usr/lib \
			--datadir=/usr/share \
			--infodir=/usr/share/info \
			--mandir=/usr/share/man \
			--localstatedir=/var \
			--sysconfdir=/etc \
			--disable-rpath \
			--without-x \
			--with-png=$(STAGING_DIR)/usr \
			$(DISABLE_NLS) \
	)
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.libs/libgd.so: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	CC=$(TARGET_CC) \
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -L$(STAGING_DIR)/usr/lib" \
	LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
	libdir="$(STAGING_DIR)/usr/lib" \
	all

$(STAGING_DIR)/usr/lib/libgd.so: $(PKG_BUILD_DIR)/.libs/libgd.so
	$(MAKE) -C $(PKG_BUILD_DIR) \
	DESTDIR="$(STAGING_DIR)" \
	install
	rm -f $(STAGING_DIR)/usr/lib/libgd*.la

$(PKG_IPK): $(PKG_BUILD_DIR)/.libs/libgd.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) libgd.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/lib
	cp -a $(PKG_BUILD_DIR)/.libs/*.so* $(PKG_IPK_DIR)/usr/lib/
	$(STRIP) $(PKG_IPK_DIR)/usr/lib/*
	find $(PKG_IPK_DIR) -name CVS | xargs rm -rf
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/libgd.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(STAGING_DIR)/usr/lib/libgd.so $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/libgd.list

clean:  
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/usr/bin/libgd*
	rm -f $(STAGING_DIR)/usr/lib/libgd*
	rm -f $(PKG_IPK)

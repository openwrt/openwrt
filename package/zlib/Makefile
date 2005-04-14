# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=zlib
PKG_VERSION:=1.2.2
PKG_RELEASE:=1
PKG_MD5SUM:=68bd51aaa6558c3bc3fd4890e53413de

PKG_SOURCE_URL:=\
	http://www.zlib.net/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_ZLIB:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_ZLIB:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_ZLIB:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) . $(PKG_NAME).patch
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--prefix=/usr \
		--shared \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) $(CFLAGS_LARGEFILE)" \
		libz.a libz.so
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libz.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install

$(IPKG_ZLIB): $(STAGING_DIR)/usr/lib/libz.so
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_ZLIB) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(IDIR_ZLIB)/usr/lib
	cp -a $(STAGING_DIR)/usr/lib/libz.so.* $(IDIR_ZLIB)/usr/lib/
	$(STRIP) $(IDIR_ZLIB)/usr/lib/lib*.so*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_ZLIB) $(PACKAGE_DIR)

$(INFO_ZLIB): $(IPKG_ZLIB)
	$(IPKG) install $(IPKG_ZLIB)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_ZLIB)
install: $(INFO_ZLIB)

clean:
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		uninstall
	rm -rf $(PKG_BUILD_DIR) $(IPKG_ZLIB)

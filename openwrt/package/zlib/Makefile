include $(TOPDIR)/rules.mk

PKG_NAME := zlib
PKG_VERSION := 1.2.2
PKG_RELEASE := 1

PKG_SOURCE_SITE := http://www.zlib.net/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/zlib_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(PKG_SOURCE_SITE)/$(PKG_SOURCE_FILE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) . $(PKG_NAME).patch
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); \
	$(TARGET_CONFIGURE_OPTS) \
	./configure \
	  --prefix=/ \
	  --shared)	
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/libz.so: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  $(TARGET_CONFIGURE_OPTS) \
	  CFLAGS="$(TARGET_CFLAGS) $(CFLAGS_LARGEFILE)" \
	 libz.a libz.so

$(STAGING_DIR)/lib/libz.so: $(PKG_BUILD_DIR)/libz.so
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  DESTDIR="$(STAGING_DIR)" \
	  install

$(PKG_IPK): $(STAGING_DIR)/lib/libz.so
	mkdir -p $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BUILD_DIR)/ipkg/zlib zlib.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_BUILD_DIR)/ipkg/zlib/usr/lib
	cp -a $(PKG_BUILD_DIR)/libz.so* $(PKG_BUILD_DIR)/ipkg/zlib/usr/lib
	$(STRIP) $(PKG_BUILD_DIR)/ipkg/zlib/usr/lib/*
	$(IPKG_BUILD) $(PKG_BUILD_DIR)/ipkg/zlib $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/zlib.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/lib/libz.so $(PKG_IPK)
install: $(PKG_ZLIB) $(IPKG_STATE_DIR)/info/zlib.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/libz* $(STAGING_DIR)/include/zlib*
	rm -f $(PACKAGE_DIR)/zlib\*.ipk


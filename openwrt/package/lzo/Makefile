include $(TOPDIR)/rules.mk

PKG_NAME := lzo
PKG_VERSION := 1.08
PKG_RELEASE := 1

PKG_SOURCE_SITE := http://www.oberhumer.com/opensource/lzo/download/
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk


$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(PKG_SOURCE_SITE)/$(PKG_SOURCE_FILE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) . ./patches
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
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
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/src/liblzo.la: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)

$(STAGING_DIR)/lib/liblzo.a: $(PKG_BUILD_DIR)/src/liblzo.la
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  CC=$(TARGET_CC) \
	  DESTDIR="$(STAGING_DIR)" \
	  install

$(PKG_IPK): $(STAGING_DIR)/lib/liblzo.a
	mkdir -p $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BUILD_DIR)/ipkg/lzo lzo.control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_BUILD_DIR)/ipkg/lzo/usr/lib
	cp -a $(STAGING_DIR)/lib/liblzo.so* $(PKG_BUILD_DIR)/ipkg/lzo/usr/lib
	$(STRIP) $(PKG_BUILD_DIR)/ipkg/lzo/usr/lib/*
	$(IPKG_BUILD) $(PKG_BUILD_DIR)/ipkg/lzo $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/lzo.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/lib/liblzo.a $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/lzo.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/liblzo*
	rm -f $(PACKAGE_DIR)/lzo\*.ipk


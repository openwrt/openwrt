include $(TOPDIR)/rules.mk

PKG_NAME := openssl
PKG_VERSION := 0.9.7e
PKG_RELEASE := 1

PKG_SOURCE_SITE := http://www.openssl.org/source
PKG_SOURCE_FILE := $(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_CAT := zcat
PKG_SOURCE_DIR := $(PKG_NAME)-$(PKG_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_SOURCE_DIR)
PKG_IPK := $(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk

OPENSSL_NO_CIPHERS:=no-idea no-mdc2 no-rc5 no-rc2 no-md2 no-md4

$(DL_DIR)/$(PKG_SOURCE_FILE):
	mkdir -p $(DL_DIR)
	$(WGET) -P $(DL_DIR) $(PKG_SOURCE_SITE)/$(PKG_SOURCE_FILE)

$(PKG_BUILD_DIR)/.patched: $(DL_DIR)/$(PKG_SOURCE_FILE)
	mkdir -p $(TOOL_BUILD_DIR)
	$(PKG_SOURCE_CAT) $(DL_DIR)/$(PKG_SOURCE_FILE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	$(SED) 's,/CFLAG=,/CFLAG= $(TARGET_SOFT_FLOAT) ,g' \
		$(PKG_BUILD_DIR)/Configure
	touch $(PKG_BUILD_DIR)/.patched

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.patched
	(cd $(PKG_BUILD_DIR); \
		CFLAGS="-DOPENSSL_NO_KRB5 -DOPENSSL_NO_IDEA -DOPENSSL_NO_MDC2 -DOPENSSL_NO_RC5" \
        PATH=$(TARGET_PATH) ./Configure linux-$(ARCH) --prefix=/ \
		--openssldir=/usr/lib/ssl -L$(STAGING_DIR)/lib -ldl \
		-I$(STAGING_DIR)/include $(OPENSSL_NO_CIPHERS) \
		shared no-krb5 no-ec no-engine zlib-dynamic no-hw no-threads )
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/apps/openssl: $(PKG_BUILD_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(PKG_BUILD_DIR) all build-shared
	# Work around openssl build bug to link libssl.so with libcrypto.so.
	-rm $(PKG_BUILD_DIR)/libssl.so.*.*.*
	$(MAKE) CC=$(TARGET_CC) -C $(PKG_BUILD_DIR) do_linux-shared

$(STAGING_DIR)/lib/libssl.so: $(PKG_BUILD_DIR)/apps/openssl
	$(MAKE) CC=$(TARGET_CC) INSTALL_PREFIX=$(STAGING_DIR) -C $(PKG_BUILD_DIR) install_sw
	cp -fa $(PKG_BUILD_DIR)/libcrypto.so* $(STAGING_DIR)/lib/
	chmod a-x $(STAGING_DIR)/lib/libcrypto.so.0.9.7
	(cd $(STAGING_DIR)/lib; ln -fs libcrypto.so.0.9.7 libcrypto.so)
	(cd $(STAGING_DIR)/lib; ln -fs libcrypto.so.0.9.7 libcrypto.so.0)
	cp -fa $(PKG_BUILD_DIR)/libssl.so* $(STAGING_DIR)/lib/
	chmod a-x $(STAGING_DIR)/lib/libssl.so.0.9.7
	(cd $(STAGING_DIR)/lib; ln -fs libssl.so.0.9.7 libssl.so)
	(cd $(STAGING_DIR)/lib; ln -fs libssl.so.0.9.7 libssl.so.0)

$(PACKAGE_DIR)/$(PKG_IPK): $(STAGING_DIR)/lib/libssl.so
	mkdir -p $(PACKAGE_DIR)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_BUILD_DIR)/ipkg/$(PKG_NAME) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)/usr/lib
	cp -a $(STAGING_DIR)/lib/libssl.so* $(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)/usr/lib
	cp -a $(STAGING_DIR)/lib/libcrypto.so* $(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)/usr/lib
	$(STRIP) $(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)/usr/lib/*
	$(IPKG_BUILD) $(PKG_BUILD_DIR)/ipkg/$(PKG_NAME) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/openssl.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE_FILE)
prepare: $(PKG_BUILD_DIR)/.patched
compile: $(STAGING_DIR)/lib/libssl.so $(PACKAGE_DIR)/$(PKG_IPK)
install: $(PKG_ZLIB) $(IPKG_STATE_DIR)/info/openssl.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(STAGING_DIR)/lib/libssl*
	rm -f $(STAGING_DIR)/lib/libcrypto*
	rm -f $(PKG_IPK)


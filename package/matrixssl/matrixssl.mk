## matrixssl

MATRIXSSL_VERSION := 1.2.1
MATRIXSSL_RELEASE := 1

MATRIXSSL_SOURCE := matrixssl-1-2-1.tar.gz
#MATRIXSSL_SITE := http://www.matrixssl.org/
MATRIXSSL_SITE := http://nthill.free.fr/openwrt/sources/
MATRIXSSL_DIR := $(BUILD_DIR)/matrixssl
MATRIXSSL_CAT := zcat

MATRIXSSL_PATCH_DIR := $(SOURCE_DIR)/openwrt/patches/matrixssl

MATRIXSSL_BUILD_DIR := $(BUILD_DIR)/libmatrixssl_$(MATRIXSSL_VERSION)-$(MATRIXSSL_RELEASE)
MATRIXSSL_IPK_DIR := $(OPENWRT_IPK_DIR)/libmatrixssl
MATRIXSSL_IPK := $(MATRIXSSL_BUILD_DIR)_$(ARCH).ipk


$(DL_DIR)/$(MATRIXSSL_SOURCE):
	 $(WGET) -P $(DL_DIR) $(MATRIXSSL_SITE)/$(MATRIXSSL_SOURCE)


$(MATRIXSSL_DIR)/.stamp-unpacked: $(DL_DIR)/$(MATRIXSSL_SOURCE)
	$(MATRIXSSL_CAT) $(DL_DIR)/$(MATRIXSSL_SOURCE) | tar -C $(BUILD_DIR) -xvf -

	touch $(MATRIXSSL_DIR)/.stamp-unpacked


$(MATRIXSSL_DIR)/.stamp-patched: $(MATRIXSSL_DIR)/.stamp-unpacked
	$(SOURCE_DIR)/patch-kernel.sh $(MATRIXSSL_DIR) $(MATRIXSSL_PATCH_DIR)

	touch $(MATRIXSSL_DIR)/.stamp-patched


$(MATRIXSSL_DIR)/.stamp-built: $(MATRIXSSL_DIR)/.stamp-patched
	cd  $(MATRIXSSL_DIR) ; \
	$(MAKE) -C src \
	  CC=$(TARGET_CC) \
	  DFLAGS="$(TARGET_CFLAGS)" \
	  STRIP=/bin/true \
	 all

	touch $(MATRIXSSL_DIR)/.stamp-built


$(MATRIXSSL_BUILD_DIR): $(MATRIXSSL_DIR)/.stamp-built
	mkdir -p $(MATRIXSSL_BUILD_DIR)
	 
	install -m0755 -d $(STAGING_DIR)/include/matrixSsl
	install -m0644 $(MATRIXSSL_DIR)/matrixSsl.h $(STAGING_DIR)/include/matrixSsl/
	ln -fs matrixSsl/matrixSsl.h $(STAGING_DIR)/include/
	install -m0755 $(MATRIXSSL_DIR)/src/libmatrixssl.so $(STAGING_DIR)/lib/libmatrixssl.so.$(MATRIXSSL_VERSION)
	ln -fs libmatrixssl.so.$(MATRIXSSL_VERSION) $(STAGING_DIR)/lib/libmatrixssl.so.1.2
	ln -fs libmatrixssl.so.1.2 $(STAGING_DIR)/lib/libmatrixssl.so

	mkdir -p $(MATRIXSSL_BUILD_DIR)/usr/lib
	cp -a $(STAGING_DIR)/lib/libmatrixssl.so.* $(MATRIXSSL_BUILD_DIR)/usr/lib/

	$(STRIP) $(MATRIXSSL_BUILD_DIR)/usr/lib/libmatrixssl.so.*


$(MATRIXSSL_IPK): $(MATRIXSSL_BUILD_DIR)
	cp -a $(MATRIXSSL_IPK_DIR)/CONTROL $(MATRIXSSL_BUILD_DIR)/
	perl -pi -e "s/^Vers.*:.*$$/Version: $(MATRIXSSL_VERSION)-$(MATRIXSSL_RELEASE)/" $(MATRIXSSL_BUILD_DIR)/CONTROL/control
	perl -pi -e "s/^Arch.*:.*$$/Architecture: $(ARCH)/" $(MATRIXSSL_BUILD_DIR)/CONTROL/control
	cd $(BUILD_DIR); $(IPKG_BUILD) $(MATRIXSSL_BUILD_DIR)


matrixssl-ipk: ipkg-utils libpthread-ipk $(MATRIXSSL_IPK)

matrixssl-clean:
	$(MAKE) -C $(MATRIXSSL_DIR) clean

matrixssl-clean-all:
	rm -rf $(MATRIXSSL_DIR)
	rm -rf $(MATRIXSSL_BUILD_DIR)
	rm -rf $(MATRIXSSL_IPK)
	rm -rf $(STAGING_DIR)/include/pcap*.h
	rm -rf $(STAGING_DIR)/lib/libpcap.a
	rm -rf $(STAGING_DIR)/lib/libpcap.so*

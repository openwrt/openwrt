#############################################################
#
# openssl
#
#############################################################

# TARGETS
OPENSSL_SITE:=http://www.openssl.org/source
OPENSSL_SOURCE:=openssl-0.9.7d.tar.gz
OPENSSL_DIR:=$(BUILD_DIR)/openssl-0.9.7d
OPENSSL_PATCH=$(SOURCE_DIR)/openssl.patch

$(DL_DIR)/$(OPENSSL_SOURCE):
	$(WGET) -P $(DL_DIR) $(OPENSSL_SITE)/$(OPENSSL_SOURCE)

$(OPENSSL_DIR)/.unpacked: $(DL_DIR)/$(OPENSSL_SOURCE) $(OPENSSL_PATCH)
	gunzip -c $(DL_DIR)/$(OPENSSL_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	cat $(OPENSSL_PATCH) | patch -p1 -d $(OPENSSL_DIR)
	# sigh... we have to resort to this just to set a gcc flag.
	$(SED) 's,/CFLAG=,/CFLAG= $(TARGET_SOFT_FLOAT) ,g' \
		$(OPENSSL_DIR)/Configure
	touch  $(OPENSSL_DIR)/.unpacked

$(OPENSSL_DIR)/Makefile: $(OPENSSL_DIR)/.unpacked
	(cd $(OPENSSL_DIR); \
	CFLAGS="-DOPENSSL_NO_KRB5 -DOPENSSL_NO_IDEA -DOPENSSL_NO_MDC2 -DOPENSSL_NO_RC5" \
	PATH=$(TARGET_PATH) ./Configure linux-$(ARCH) --prefix=/ \
		--openssldir=/usr/lib/ssl -L$(STAGING_DIR)/lib -ldl \
		-I$(STAGING_DIR)/include $(OPENSSL_OPTS) no-threads \
		shared no-idea no-mdc2 no-rc5)

$(OPENSSL_DIR)/apps/openssl: $(OPENSSL_DIR)/Makefile
	$(MAKE) CC=$(TARGET_CC) -C $(OPENSSL_DIR) all build-shared
	# Work around openssl build bug to link libssl.so with libcrypto.so.
	-rm $(OPENSSL_DIR)/libssl.so.*.*.*
	$(MAKE) CC=$(TARGET_CC) -C $(OPENSSL_DIR) do_linux-shared

$(STAGING_DIR)/lib/libcrypto.a: $(OPENSSL_DIR)/apps/openssl
	$(MAKE) CC=$(TARGET_CC) INSTALL_PREFIX=$(STAGING_DIR) -C $(OPENSSL_DIR) install
	cp -fa $(OPENSSL_DIR)/libcrypto.so* $(STAGING_DIR)/lib/
	chmod a-x $(STAGING_DIR)/lib/libcrypto.so.0.9.7
	(cd $(STAGING_DIR)/lib; ln -fs libcrypto.so.0.9.7 libcrypto.so)
	(cd $(STAGING_DIR)/lib; ln -fs libcrypto.so.0.9.7 libcrypto.so.0)
	cp -fa $(OPENSSL_DIR)/libssl.so* $(STAGING_DIR)/lib/
	chmod a-x $(STAGING_DIR)/lib/libssl.so.0.9.7
	(cd $(STAGING_DIR)/lib; ln -fs libssl.so.0.9.7 libssl.so)
	(cd $(STAGING_DIR)/lib; ln -fs libssl.so.0.9.7 libssl.so.0)

$(TARGET_DIR)/usr/lib/libcrypto.so.0.9.7: $(STAGING_DIR)/lib/libcrypto.a
	mkdir -p $(TARGET_DIR)/usr/lib
	cp -fa $(STAGING_DIR)/lib/libcrypto.so* $(TARGET_DIR)/usr/lib/
	cp -fa $(STAGING_DIR)/lib/libssl.so* $(TARGET_DIR)/usr/lib/
	#cp -fa $(STAGING_DIR)/bin/openssl  $(TARGET_DIR)/bin/
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libssl.so.0.9.7
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libcrypto.so.0.9.7

$(TARGET_DIR)/usr/lib/libssl.a: $(STAGING_DIR)/lib/libcrypto.a
	mkdir -p $(TARGET_DIR)/usr/include 
	cp -a $(STAGING_DIR)/include/openssl $(TARGET_DIR)/usr/include/
	cp -dpf $(STAGING_DIR)/lib/libssl.a $(TARGET_DIR)/usr/lib/
	cp -dpf $(STAGING_DIR)/lib/libcrypto.a $(TARGET_DIR)/usr/lib/
	touch -c $(TARGET_DIR)/usr/lib/libssl.a

openssl-headers: $(TARGET_DIR)/usr/lib/libssl.a

openssl: uclibc $(TARGET_DIR)/usr/lib/libcrypto.so.0.9.7

openssl-source: $(DL_DIR)/$(OPENSSL_SOURCE)

openssl-clean: 
	rm -f $(STAGING_DIR)/bin/openssl  $(TARGET_DIR)/bin/openssl
	rm -f $(STAGING_DIR)/lib/libcrypto.so* $(TARGET_DIR)/lib/libcrypto.so*
	rm -f $(STAGING_DIR)/lib/libssl.so* $(TARGET_DIR)/lib/libssl.so*
	$(MAKE) -C $(OPENSSL_DIR) clean

openssl-dirclean: 
	rm -rf $(OPENSSL_DIR) 


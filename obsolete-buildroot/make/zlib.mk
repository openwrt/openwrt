#############################################################
#
# zlib
#
#############################################################
ZLIB_SOURCE=zlib-1.1.4.tar.bz2
ZLIB_SITE=http://aleron.dl.sourceforge.net/sourceforge/libpng
ZLIB_DIR=$(BUILD_DIR)/zlib-1.1.4
ZLIB_CFLAGS= $(TARGET_CFLAGS) -fPIC
ifeq ($(strip $(BUILD_WITH_LARGEFILE)),true)
ZLIB_CFLAGS+= -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
endif

$(DL_DIR)/$(ZLIB_SOURCE):
	$(WGET) -P $(DL_DIR) $(ZLIB_SITE)/$(ZLIB_SOURCE)

$(ZLIB_DIR)/.source: $(DL_DIR)/$(ZLIB_SOURCE)
	bzcat $(DL_DIR)/$(ZLIB_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(ZLIB_DIR)/.source

$(ZLIB_DIR)/.configured: $(ZLIB_DIR)/.source
	(cd $(ZLIB_DIR); \
		./configure \
		--shared \
		--prefix=/usr \
		--exec-prefix=$(STAGING_DIR)/usr/bin \
		--libdir=$(STAGING_DIR)/lib \
		--includedir=$(STAGING_DIR)/include \
	);
	touch $(ZLIB_DIR)/.configured;

$(ZLIB_DIR)/libz.so.1.1.4: $(ZLIB_DIR)/.configured
	$(MAKE) LDSHARED="$(TARGET_CROSS)ld -shared -soname,libz.so.1" \
		CFLAGS="$(ZLIB_CFLAGS)" CC=$(TARGET_CC) -C $(ZLIB_DIR) all libz.a;
	touch -c $(ZLIB_DIR)/libz.so.1.1.4

$(STAGING_DIR)/lib/libz.so.1.1.4: $(ZLIB_DIR)/libz.so.1.1.4
	cp -dpf $(ZLIB_DIR)/libz.a $(STAGING_DIR)/lib;
	cp -dpf $(ZLIB_DIR)/zlib.h $(STAGING_DIR)/include;
	cp -dpf $(ZLIB_DIR)/zconf.h $(STAGING_DIR)/include;
	cp -dpf $(ZLIB_DIR)/libz.so* $(STAGING_DIR)/lib;
	(cd $(STAGING_DIR)/lib; ln -fs libz.so.1.1.4 libz.so.1);
	chmod a-x $(STAGING_DIR)/lib/libz.so.1.1.4
	touch -c $(STAGING_DIR)/lib/libz.so.1.1.4

$(TARGET_DIR)/lib/libz.so.1.1.4: $(STAGING_DIR)/lib/libz.so.1.1.4
	cp -dpf $(STAGING_DIR)/lib/libz.so* $(TARGET_DIR)/lib;
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/lib/libz.so*
	touch -c $(TARGET_DIR)/lib/libz.so.1.1.4

$(TARGET_DIR)/usr/lib/libz.a: $(STAGING_DIR)/lib/libz.so.1.1.4
	mkdir -p $(TARGET_DIR)/usr/include
	cp -dpf $(STAGING_DIR)/include/zlib.h $(TARGET_DIR)/usr/include/
	cp -dpf $(STAGING_DIR)/include/zconf.h $(TARGET_DIR)/usr/include/
	cp -dpf $(STAGING_DIR)/lib/libz.a $(TARGET_DIR)/usr/lib/
	rm -f $(TARGET_DIR)/lib/libz.so
	(cd $(TARGET_DIR)/usr/lib; ln -fs /lib/libz.so.1.1.4 libz.so)
	touch -c $(TARGET_DIR)/usr/lib/libz.a

zlib-headers: $(TARGET_DIR)/usr/lib/libz.a

zlib: uclibc $(TARGET_DIR)/lib/libz.so.1.1.4

zlib-source: $(DL_DIR)/$(ZLIB_SOURCE)

zlib-clean:
	rm -f $(TARGET_DIR)/lib/libz.so*
	-$(MAKE) -C $(ZLIB_DIR) clean

zlib-dirclean:
	rm -rf $(ZLIB_DIR)


#############################################################
#
# newt
#
#############################################################
NEWT_SOURCE=newt-0.51.0.tar.bz2
NEWT_SITE=http://www.uclibc.org/
NEWT_DIR=$(BUILD_DIR)/newt-0.51.0
NEWT_VERSION=0.51.0
ifeq ($(strip $(BUILD_WITH_LARGEFILE)),true)
NEWT_CFLAGS=-Os -g -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
else
NEWT_CFLAGS=-Os -g
endif
NEWT_CFLAGS+=-fPIC

$(DL_DIR)/$(NEWT_SOURCE):
	$(WGET) -P $(DL_DIR) $(NEWT_SITE)/$(NEWT_SOURCE)

$(NEWT_DIR)/.source: $(DL_DIR)/$(NEWT_SOURCE)
	bzcat $(DL_DIR)/$(NEWT_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(NEWT_DIR)/.source;

$(NEWT_DIR)/.configured: $(NEWT_DIR)/.source
	(cd $(NEWT_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--sbindir=/usr/sbin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
	);
	touch $(NEWT_DIR)/.configured;

$(NEWT_DIR)/libnewt.so.$(NEWT_VERSION): $(NEWT_DIR)/.configured
	$(MAKE) CFLAGS="$(NEWT_CFLAGS)" CC=$(TARGET_CC) -C  $(NEWT_DIR)
	touch -c $(NEWT_DIR)/libnewt.so.$(NEWT_VERSION)

$(STAGING_DIR)/lib/libnewt.a: $(NEWT_DIR)/libnewt.so.$(NEWT_VERSION)
	cp -a $(NEWT_DIR)/libnewt.a $(STAGING_DIR)/lib;
	cp -a $(NEWT_DIR)/newt.h $(STAGING_DIR)/include;
	cp -a $(NEWT_DIR)/libnewt.so* $(STAGING_DIR)/lib;
	(cd $(STAGING_DIR)/lib; ln -fs libnewt.so.$(NEWT_VERSION) libnewt.so);
	(cd $(STAGING_DIR)/lib; ln -fs libnewt.so.$(NEWT_VERSION) libnewt.so.0.51);
	touch -c $(STAGING_DIR)/lib/libnewt.a

$(TARGET_DIR)/lib/libnewt.so.$(NEWT_VERSION): $(STAGING_DIR)/lib/libnewt.a
	cp -a $(STAGING_DIR)/lib/libnewt.so* $(TARGET_DIR)/lib;
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/lib/libnewt.so*
	touch -c $(TARGET_DIR)/lib/libnewt.so.$(NEWT_VERSION)

newt: uclibc slang $(TARGET_DIR)/lib/libnewt.so.$(NEWT_VERSION)

newt-source: $(DL_DIR)/$(NEWT_SOURCE)

newt-clean:
	rm -f $(TARGET_DIR)/lib/libnewt.so*
	-$(MAKE) -C $(NEWT_DIR) clean

newt-dirclean: slang-dirclean
	rm -rf $(NEWT_DIR)


#############################################################
#
# bzip2
#
#############################################################
BZIP2_SOURCE:=bzip2-1.0.2.tar.gz
BZIP2_SITE:=ftp://sources.redhat.com/pub/bzip2/v102
BZIP2_DIR:=$(BUILD_DIR)/bzip2-1.0.2
BZIP2_CAT:=zcat
BZIP2_BINARY:=$(BZIP2_DIR)/bzip2
BZIP2_TARGET_BINARY:=$(TARGET_DIR)/usr/bin/bzmore

$(DL_DIR)/$(BZIP2_SOURCE):
	 $(WGET) -P $(DL_DIR) $(BZIP2_SITE)/$(BZIP2_SOURCE)

bzip2-source: $(DL_DIR)/$(BZIP2_SOURCE)

$(BZIP2_DIR)/.unpacked: $(DL_DIR)/$(BZIP2_SOURCE)
	$(BZIP2_CAT) $(DL_DIR)/$(BZIP2_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	$(SED) "s,ln \$$(,ln -sf \$$(,g" $(BZIP2_DIR)/Makefile
	$(SED) "s,ln -s (lib.*),ln -sf \$$1 ; ln -sf libbz2.so.1.0.2 libbz2.so,g" \
	    $(BZIP2_DIR)/Makefile-libbz2_so
ifeq ($(strip $(BUILD_WITH_LARGEFILE)),false)
	$(SED) "s,^BIGFILES,#BIGFILES,g" $(BZIP2_DIR)/Makefile
	$(SED) "s,^BIGFILES,#BIGFILES,g" $(BZIP2_DIR)/Makefile-libbz2_so
endif
	touch $(BZIP2_DIR)/.unpacked

$(STAGING_DIR)/lib/libbz2.so.1.0.2: $(BZIP2_DIR)/.unpacked
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) CC=$(TARGET_CC) -C $(BZIP2_DIR) -f Makefile-libbz2_so
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) CC=$(TARGET_CC) -C $(BZIP2_DIR) libbz2.a
	cp $(BZIP2_DIR)/bzlib.h $(STAGING_DIR)/include/ 
	cp $(BZIP2_DIR)/libbz2.so.1.0.2 $(STAGING_DIR)/lib/ 
	cp $(BZIP2_DIR)/libbz2.a $(STAGING_DIR)/lib/ 
	(cd $(STAGING_DIR)/lib/; ln -sf libbz2.so.1.0.2 libbz2.so) 
	(cd $(STAGING_DIR)/lib/; ln -sf libbz2.so.1.0.2 libbz2.so.1.0) 

$(BZIP2_BINARY): $(STAGING_DIR)/lib/libbz2.so.1.0.2
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) CC=$(TARGET_CC) -C $(BZIP2_DIR) bzip2 bzip2recover

$(BZIP2_TARGET_BINARY): $(BZIP2_BINARY)
	(cd $(TARGET_DIR)/usr/bin; \
	rm -f bzip2 bunzip2 bzcat bzip2recover bzgrep bzegrep bzfgrep bzmore bzless bzdiff bzcmp);
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) PREFIX=$(TARGET_DIR)/usr -C $(BZIP2_DIR) install
	rm -f $(TARGET_DIR)/usr/lib/libbz2.a
	rm -f $(TARGET_DIR)/usr/include/bzlib.h
	cp $(BZIP2_DIR)/libbz2.so.1.0.2 $(TARGET_DIR)/usr/lib/
	(cd $(TARGET_DIR)/usr/lib; \
	ln -sf libbz2.so.1.0.2 libbz2.so.1.0; \
	ln -sf libbz2.so.1.0.2 libbz2.so)
	(cd $(TARGET_DIR)/usr/bin; \
	ln -sf bzip2 bunzip2; \
	ln -sf bzip2 bzcat; \
	ln -sf bzdiff bzcmp; \
	ln -sf bzmore bzless; \
	ln -sf bzgrep bzegrep; \
	ln -sf bzgrep bzfgrep;)
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

$(TARGET_DIR)/usr/lib/libbz2.a: $(STAGING_DIR)/lib/libbz2.a
	mkdir -p $(TARGET_DIR)/usr/include 
	cp $(STAGING_DIR)/include/bzlib.h $(TARGET_DIR)/usr/include/
	cp $(STAGING_DIR)/lib/libbz2.a $(TARGET_DIR)/usr/lib/ 
	rm -f $(TARGET_DIR)/lib/libbz2.so
	(cd $(TARGET_DIR)/usr/lib; \
		ln -fs /usr/lib/libbz2.so.1.0 libbz2.so; \
	)
	-$(STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libbz2.so.1.0
	touch -c $(TARGET_DIR)/usr/lib/libbz2.a

bzip2-headers: $(TARGET_DIR)/usr/lib/libbz2.a

bzip2: uclibc $(BZIP2_TARGET_BINARY)

bzip2-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(BZIP2_DIR) uninstall
	-$(MAKE) -C $(BZIP2_DIR) clean

bzip2-dirclean:
	rm -rf $(BZIP2_DIR)


#############################################################
#
# flex
#
#############################################################
FLEX_SOURCE:=flex_2.5.4a.orig.tar.gz
FLEX_PATCH:=flex_2.5.4a-24.diff.gz
FLEX_SITE:=http://ftp.debian.org/debian/pool/main/f/flex
FLEX_DIR:=$(BUILD_DIR)/flex-2.5.4
FLEX_CAT:=zcat
FLEX_BINARY:=flex
FLEX_TARGET_BINARY:=usr/bin/flex

$(DL_DIR)/$(FLEX_SOURCE):
	 $(WGET) -P $(DL_DIR) $(FLEX_SITE)/$(FLEX_SOURCE)

$(DL_DIR)/$(FLEX_PATCH):
	 $(WGET) -P $(DL_DIR) $(FLEX_SITE)/$(FLEX_PATCH)

flex-source: $(DL_DIR)/$(FLEX_SOURCE) $(DL_DIR)/$(FLEX_PATCH)

$(FLEX_DIR)/.unpacked: $(DL_DIR)/$(FLEX_SOURCE) $(DL_DIR)/$(FLEX_PATCH)
	$(FLEX_CAT) $(DL_DIR)/$(FLEX_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	#$(SOURCE_DIR)/patch-kernel.sh $(FLEX_DIR) $(DL_DIR) $(FLEX_PATCH)
	touch $(FLEX_DIR)/.unpacked

$(FLEX_DIR)/.configured: $(FLEX_DIR)/.unpacked
	(cd $(FLEX_DIR); autoconf; rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		$(DISABLE_LARGEFILE) \
	);
	touch  $(FLEX_DIR)/.configured

$(FLEX_DIR)/$(FLEX_BINARY): $(FLEX_DIR)/.configured
	$(MAKE) -C $(FLEX_DIR)

$(TARGET_DIR)/$(FLEX_TARGET_BINARY): $(FLEX_DIR)/$(FLEX_BINARY)
	$(MAKE) \
	    prefix=$(TARGET_DIR)/usr \
	    exec_prefix=$(TARGET_DIR)/usr \
	    bindir=$(TARGET_DIR)/usr/bin \
	    sbindir=$(TARGET_DIR)/usr/sbin \
	    libexecdir=$(TARGET_DIR)/usr/lib \
	    datadir=$(TARGET_DIR)/usr/share \
	    sysconfdir=$(TARGET_DIR)/etc \
	    sharedstatedir=$(TARGET_DIR)/usr/com \
	    localstatedir=$(TARGET_DIR)/var \
	    libdir=$(TARGET_DIR)/usr/lib \
	    infodir=$(TARGET_DIR)/usr/info \
	    mandir=$(TARGET_DIR)/usr/man \
	    includedir=$(TARGET_DIR)/usr/include \
	    -C $(FLEX_DIR) install;
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	(cd $(TARGET_DIR)/usr/bin; ln -s flex lex)

flex: uclibc $(TARGET_DIR)/$(FLEX_TARGET_BINARY)

flex-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(FLEX_DIR) uninstall
	-$(MAKE) -C $(FLEX_DIR) clean

flex-dirclean:
	rm -rf $(FLEX_DIR)


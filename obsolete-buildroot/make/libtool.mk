#############################################################
#
# libtool
#
#############################################################
LIBTOOL_SOURCE:=libtool_1.4.3.orig.tar.gz
LIBTOOL_SITE:=http://ftp.debian.org/debian/pool/main/libt/libtool
LIBTOOL_CAT:=zcat
LIBTOOL_DIR:=$(BUILD_DIR)/libtool-1.4.3
LIBTOOL_BINARY:=libtool
LIBTOOL_TARGET_BINARY:=usr/bin/libtool

$(DL_DIR)/$(LIBTOOL_SOURCE):
	 $(WGET) -P $(DL_DIR) $(LIBTOOL_SITE)/$(LIBTOOL_SOURCE)

libtool-source: $(DL_DIR)/$(LIBTOOL_SOURCE)

$(LIBTOOL_DIR)/.unpacked: $(DL_DIR)/$(LIBTOOL_SOURCE)
	$(LIBTOOL_CAT) $(DL_DIR)/$(LIBTOOL_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(LIBTOOL_DIR)/.unpacked

$(LIBTOOL_DIR)/.configured: $(LIBTOOL_DIR)/.unpacked
	(cd $(LIBTOOL_DIR); rm -rf config.cache; \
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
	);
	touch  $(LIBTOOL_DIR)/.configured

$(LIBTOOL_DIR)/$(LIBTOOL_BINARY): $(LIBTOOL_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(LIBTOOL_DIR)
	touch -c $(LIBTOOL_DIR)/$(LIBTOOL_BINARY)

$(TARGET_DIR)/$(LIBTOOL_TARGET_BINARY): $(LIBTOOL_DIR)/$(LIBTOOL_BINARY)
	$(MAKE) \
	    prefix=$(TARGET_DIR)/usr \
	    exec_prefix=$(TARGET_DIR)/usr \
	    bindir=$(TARGET_DIR)/usr/bin \
	    sbindir=$(TARGET_DIR)/usr/sbin \
	    libexecdir=$(TARGET_DIR)/usr/lib \
	    datadir=$(TARGET_DIR)/usr/share \
	    sysconfdir=$(TARGET_DIR)/etc \
	    localstatedir=$(TARGET_DIR)/var \
	    libdir=$(TARGET_DIR)/usr/lib \
	    infodir=$(TARGET_DIR)/usr/info \
	    mandir=$(TARGET_DIR)/usr/man \
	    includedir=$(TARGET_DIR)/usr/include \
	    -C $(LIBTOOL_DIR) install;
	$(STRIP) $(TARGET_DIR)//usr/lib/libltdl.so.*.*.* > /dev/null 2>&1
	$(SED) "s,^CC.*,CC=\"/usr/bin/gcc\"," $(TARGET_DIR)/usr/bin/libtool
	$(SED) "s,^LD.*,LD=\"/usr/bin/ld\"," $(TARGET_DIR)/usr/bin/libtool
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

libtool: uclibc $(TARGET_DIR)/$(LIBTOOL_TARGET_BINARY)

libtool-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(LIBTOOL_DIR) uninstall
	-$(MAKE) -C $(LIBTOOL_DIR) clean

libtool-dirclean:
	rm -rf $(LIBTOOL_DIR)


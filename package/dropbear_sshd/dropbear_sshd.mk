#############################################################
#
# dropbear_sshd
#
#############################################################
DROPBEAR_SSHD_SOURCE:=dropbear-0.44.tar.bz2
DROPBEAR_SSHD_SITE:=http://matt.ucc.asn.au/dropbear/releases/
DROPBEAR_SSHD_DIR:=$(BUILD_DIR)/dropbear-0.44
DROPBEAR_SSHD_CAT:=bzcat
DROPBEAR_SSHD_BINARY:=dropbearmulti
DROPBEAR_SSHD_TARGET_BINARY:=usr/sbin/dropbear


$(DL_DIR)/$(DROPBEAR_SSHD_SOURCE):
	 $(WGET) -P $(DL_DIR) $(DROPBEAR_SSHD_SITE)/$(DROPBEAR_SSHD_SOURCE)

dropbear_sshd-source: $(DL_DIR)/$(DROPBEAR_SSHD_SOURCE)

$(DROPBEAR_SSHD_DIR)/.unpacked: $(DL_DIR)/$(DROPBEAR_SSHD_SOURCE)
	$(DROPBEAR_SSHD_CAT) $(DL_DIR)/$(DROPBEAR_SSHD_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	toolchain/patch-kernel.sh $(DROPBEAR_SSHD_DIR) package/dropbear_sshd/ dropbear-\*.patch
	$(SED) 's,^/\* #define DROPBEAR_MULTI.*,#define DROPBEAR_MULTI,g' $(DROPBEAR_SSHD_DIR)/options.h
	touch $(DROPBEAR_SSHD_DIR)/.unpacked

$(DROPBEAR_SSHD_DIR)/.configured: $(DROPBEAR_SSHD_DIR)/.unpacked
	(cd $(DROPBEAR_SSHD_DIR); rm -rf config.cache; \
		autoconf; \
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
		--with-shared \
	);
	touch  $(DROPBEAR_SSHD_DIR)/.configured

$(DROPBEAR_SSHD_DIR)/$(DROPBEAR_SSHD_BINARY): $(DROPBEAR_SSHD_DIR)/.configured
	$(MAKE) $(TARGET_CONFIGURE_OPTS) LD=$(TARGET_CC) \
		PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" \
		MULTI=1 SCPPROGRESS=1 -C $(DROPBEAR_SSHD_DIR)

$(TARGET_DIR)/$(DROPBEAR_SSHD_TARGET_BINARY): $(DROPBEAR_SSHD_DIR)/$(DROPBEAR_SSHD_BINARY)
	#$(MAKE) DESTDIR=$(TARGET_DIR) $(TARGET_CONFIGURE_OPTS) \
	#	LD=$(TARGET_CC) -C $(DROPBEAR_SSHD_DIR) install
	#rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
	#	$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	install -d -m 755 $(TARGET_DIR)/usr/sbin
	install -d -m 755 $(TARGET_DIR)/usr/bin
	install -m 755 $(DROPBEAR_SSHD_DIR)/$(DROPBEAR_SSHD_BINARY) \
		$(TARGET_DIR)/$(DROPBEAR_SSHD_TARGET_BINARY)
	ln -sf ../sbin/dropbear $(TARGET_DIR)/usr/bin/scp
	ln -sf ../sbin/dropbear $(TARGET_DIR)/usr/bin/ssh
	ln -sf ../sbin/dropbear $(TARGET_DIR)/usr/bin/dropbearkey
	ln -sf ../sbin/dropbear $(TARGET_DIR)/usr/bin/dropbearconvert
	cp $(DROPBEAR_SSHD_DIR)/S50dropbear $(TARGET_DIR)/etc/init.d/
	chmod a+x $(TARGET_DIR)/etc/init.d/S50dropbear

dropbear_sshd: uclibc zlib $(TARGET_DIR)/$(DROPBEAR_SSHD_TARGET_BINARY)

dropbear_sshd-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) $(TARGET_CONFIGURE_OPTS) \
		LD=$(TARGET_CC) -C $(DROPBEAR_SSHD_DIR) uninstall
	-$(MAKE) -C $(DROPBEAR_SSHD_DIR) clean

dropbear_sshd-dirclean:
	rm -rf $(DROPBEAR_SSHD_DIR)


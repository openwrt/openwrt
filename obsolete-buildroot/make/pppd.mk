#############################################################
#
# pppd
#
#############################################################
PPPD_SOURCE:=ppp-2.4.1.tar.gz
PPPD_SITE:=ftp://ftp.samba.org/pub/ppp
PPPD_DIR:=$(BUILD_DIR)/ppp-2.4.1
PPPD_CAT:=zcat
PPPD_BINARY:=pppd/pppd
PPPD_TARGET_BINARY:=usr/sbin/pppd


$(DL_DIR)/$(PPPD_SOURCE):
	 $(WGET) -P $(DL_DIR) $(PPPD_SITE)/$(PPPD_SOURCE)

pppd-source: $(DL_DIR)/$(PPPD_SOURCE)

$(PPPD_DIR)/.unpacked: $(DL_DIR)/$(PPPD_SOURCE)
	$(PPPD_CAT) $(DL_DIR)/$(PPPD_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	$(SED) 's/ -DIPX_CHANGE -DHAVE_MULTILINK -DHAVE_MMAP//' $(PPPD_DIR)/pppd/Makefile.linux
	$(SED) 's/$(INSTALL) -s/$(INSTALL)/' $(PPPD_DIR)/*/Makefile.linux
	$(SED) 's/ -o root//' $(PPPD_DIR)/*/Makefile.linux
	$(SED) 's/ -g daemon//' $(PPPD_DIR)/*/Makefile.linux
	touch $(PPPD_DIR)/.unpacked

$(PPPD_DIR)/.configured: $(PPPD_DIR)/.unpacked
	(cd $(PPPD_DIR); rm -rf config.cache; \
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
	touch  $(PPPD_DIR)/.configured

$(PPPD_DIR)/$(PPPD_BINARY): $(PPPD_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(PPPD_DIR)

$(TARGET_DIR)/$(PPPD_TARGET_BINARY): $(PPPD_DIR)/$(PPPD_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(PPPD_DIR) install
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

pppd: uclibc $(TARGET_DIR)/$(PPPD_TARGET_BINARY)

pppd-clean:
	rm -f  $(TARGET_DIR)/usr/sbin/pppd
	rm -f  $(TARGET_DIR)/usr/sbin/chat
	rm -rf $(TARGET_DIR)/etc/ppp
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(PPPD_DIR) uninstall
	-$(MAKE) -C $(PPPD_DIR) clean

pppd-dirclean:
	rm -rf $(PPPD_DIR)



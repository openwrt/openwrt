#############################################################
#
# patch
#
#############################################################
GNUPATCH_SOURCE:=patch_2.5.9.orig.tar.gz
GNUPATCH_SITE:=http://ftp.debian.org/debian/pool/main/p/patch
GNUPATCH_CAT:=zcat
GNUPATCH_DIR:=$(BUILD_DIR)/patch-2.5.9
GNUPATCH_BINARY:=patch
GNUPATCH_TARGET_BINARY:=usr/bin/patch

$(DL_DIR)/$(GNUPATCH_SOURCE):
	 $(WGET) -P $(DL_DIR) $(GNUPATCH_SITE)/$(GNUPATCH_SOURCE)

patch-source: $(DL_DIR)/$(GNUPATCH_SOURCE)

$(GNUPATCH_DIR)/.unpacked: $(DL_DIR)/$(GNUPATCH_SOURCE)
	$(GNUPATCH_CAT) $(DL_DIR)/$(GNUPATCH_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(GNUPATCH_DIR)/.unpacked

$(GNUPATCH_DIR)/.configured: $(GNUPATCH_DIR)/.unpacked
	(cd $(GNUPATCH_DIR); rm -rf config.cache; \
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
	touch  $(GNUPATCH_DIR)/.configured

$(GNUPATCH_DIR)/$(GNUPATCH_BINARY): $(GNUPATCH_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(GNUPATCH_DIR)

$(TARGET_DIR)/$(GNUPATCH_TARGET_BINARY): $(GNUPATCH_DIR)/$(GNUPATCH_BINARY)
	rm -f $(TARGET_DIR)/$(GNUPATCH_TARGET_BINARY)
	cp -a $(GNUPATCH_DIR)/$(GNUPATCH_BINARY) $(TARGET_DIR)/$(GNUPATCH_TARGET_BINARY)

patch: uclibc $(TARGET_DIR)/$(GNUPATCH_TARGET_BINARY)

patch-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(GNUPATCH_DIR) uninstall
	-$(MAKE) -C $(GNUPATCH_DIR) clean

patch-dirclean:
	rm -rf $(GNUPATCH_DIR)


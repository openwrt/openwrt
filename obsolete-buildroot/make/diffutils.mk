#############################################################
#
# diffutils
#
#############################################################
DIFFUTILS_SOURCE:=diffutils-2.8.4.tar.gz
DIFFUTILS_SITE:=ftp://alpha.gnu.org/gnu/diffutils/
DIFFUTILS_CAT:=zcat
DIFFUTILS_DIR:=$(BUILD_DIR)/diffutils-2.8.4
DIFFUTILS_BINARY:=src/diff
DIFFUTILS_TARGET_BINARY:=usr/bin/diff

$(DL_DIR)/$(DIFFUTILS_SOURCE):
	 $(WGET) -P $(DL_DIR) $(DIFFUTILS_SITE)/$(DIFFUTILS_SOURCE)

diffutils-source: $(DL_DIR)/$(DIFFUTILS_SOURCE)

$(DIFFUTILS_DIR)/.unpacked: $(DL_DIR)/$(DIFFUTILS_SOURCE)
	$(DIFFUTILS_CAT) $(DL_DIR)/$(DIFFUTILS_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(DIFFUTILS_DIR)/.unpacked

$(DIFFUTILS_DIR)/.configured: $(DIFFUTILS_DIR)/.unpacked
	(cd $(DIFFUTILS_DIR); rm -rf config.cache; \
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
	touch  $(DIFFUTILS_DIR)/.configured

$(DIFFUTILS_DIR)/$(DIFFUTILS_BINARY): $(DIFFUTILS_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(DIFFUTILS_DIR)

$(TARGET_DIR)/$(DIFFUTILS_TARGET_BINARY): $(DIFFUTILS_DIR)/$(DIFFUTILS_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(DIFFUTILS_DIR) install
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

diffutils: uclibc $(TARGET_DIR)/$(DIFFUTILS_TARGET_BINARY)

diffutils-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(DIFFUTILS_DIR) uninstall
	-$(MAKE) -C $(DIFFUTILS_DIR) clean

diffutils-dirclean:
	rm -rf $(DIFFUTILS_DIR)


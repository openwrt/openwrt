#############################################################
#
# gawk
#
#############################################################
GAWK_SOURCE:=gawk-3.1.2.tar.gz
GAWK_SITE:=ftp://ftp.gnu.org/gnu/gawk
GAWK_CAT:=zcat
GAWK_DIR:=$(BUILD_DIR)/gawk-3.1.2
GAWK_BINARY:=gawk
GAWK_TARGET_BINARY:=usr/bin/gawk

$(DL_DIR)/$(GAWK_SOURCE):
	 $(WGET) -P $(DL_DIR) $(GAWK_SITE)/$(GAWK_SOURCE)

gawk-source: $(DL_DIR)/$(GAWK_SOURCE)

$(GAWK_DIR)/.unpacked: $(DL_DIR)/$(GAWK_SOURCE)
	$(GAWK_CAT) $(DL_DIR)/$(GAWK_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(GAWK_DIR)/.unpacked

$(GAWK_DIR)/.configured: $(GAWK_DIR)/.unpacked
	(cd $(GAWK_DIR); rm -rf config.cache; autoconf; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		ac_cv_func_getpgrp_void=yes \
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
	touch  $(GAWK_DIR)/.configured

$(GAWK_DIR)/$(GAWK_BINARY): $(GAWK_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(GAWK_DIR)

$(TARGET_DIR)/$(GAWK_TARGET_BINARY): $(GAWK_DIR)/$(GAWK_BINARY)
	rm -f $(TARGET_DIR)/usr/bin/awk
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(GAWK_DIR) install
	rm -f $(TARGET_DIR)/usr/bin/gawk-*
	(cd $(TARGET_DIR)/usr/bin; ln -sf gawk awk) 
	$(STRIP) $(TARGET_DIR)/usr/lib/awk/* > /dev/null 2>&1
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

gawk: uclibc $(TARGET_DIR)/$(GAWK_TARGET_BINARY)

gawk-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(GAWK_DIR) uninstall
	-$(MAKE) -C $(GAWK_DIR) clean

gawk-dirclean:
	rm -rf $(GAWK_DIR)


#############################################################
#
# gzip
#
#############################################################
GZIP_SOURCE:=gzip-1.3.5.tar.gz
GZIP_SITE:=ftp://alpha.gnu.org/gnu/gzip
GZIP_DIR:=$(BUILD_DIR)/gzip-1.3.5
GZIP_CAT:=zcat
GZIP_BINARY:=$(GZIP_DIR)/gzip
GZIP_TARGET_BINARY:=$(TARGET_DIR)/bin/zmore

ifeq ($(strip $(BUILD_WITH_LARGEFILE)),false)
GZIP_LARGEFILE="--disable-largefile"
endif

$(DL_DIR)/$(GZIP_SOURCE):
	 $(WGET) -P $(DL_DIR) $(GZIP_SITE)/$(GZIP_SOURCE)

gzip-source: $(DL_DIR)/$(GZIP_SOURCE)

$(GZIP_DIR)/.unpacked: $(DL_DIR)/$(GZIP_SOURCE)
	$(GZIP_CAT) $(DL_DIR)/$(GZIP_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	touch $(GZIP_DIR)/.unpacked

$(GZIP_DIR)/.configured: $(GZIP_DIR)/.unpacked
	(cd $(GZIP_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=/usr \
		--exec-prefix=/ \
		--bindir=/bin \
		--sbindir=/bin \
		--libexecdir=/usr/lib \
		--sysconfdir=/etc \
		--datadir=/usr/share/misc \
		--localstatedir=/var \
		--mandir=/usr/man \
		--infodir=/usr/info \
		$(DISABLE_NLS) \
		$(GZIP_LARGEFILE) \
	);
	touch  $(GZIP_DIR)/.configured

$(GZIP_BINARY): $(GZIP_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) -C $(GZIP_DIR)

$(GZIP_TARGET_BINARY): $(GZIP_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(GZIP_DIR) install
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc
	(cd $(TARGET_DIR)/bin; \
	ln -sf gzip gunzip; \
	ln -sf gzip zcat; \
	ln -sf zdiff zcmp; \
	ln -sf zgrep zegrep; \
	ln -sf zgrep zfgrep;)

gzip: uclibc $(GZIP_TARGET_BINARY)

gzip-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) CC=$(TARGET_CC) -C $(GZIP_DIR) uninstall
	-$(MAKE) -C $(GZIP_DIR) clean

gzip-dirclean:
	rm -rf $(GZIP_DIR)


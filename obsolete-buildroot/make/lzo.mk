#############################################################
#
# lzo
#
# Note: this builds only a static library, it does not provide
#       anything to be installed into the target system.
#
#############################################################
LZO_SOURCE:=lzo_1.08.orig.tar.gz
LZO_SITE:=http://ftp.debian.org/debian/pool/main/l/lzo
#LZO_SOURCE:=lzo-1.08.tar.bz2
#LZO_SITE:=http://www.oberhumer.com/opensource/lzo/download
LZO_DIR:=$(BUILD_DIR)/lzo-1.08
LZO_CAT:=zcat
LZO_PATCH:=$(SOURCE_DIR)/lzo-cross-compile.patch

$(DL_DIR)/$(LZO_SOURCE):
	 $(WGET) -P $(DL_DIR) $(LZO_SITE)/$(LZO_SOURCE)

lzo-source: $(DL_DIR)/$(LZO_SOURCE)

$(LZO_DIR)/.unpacked: $(DL_DIR)/$(LZO_SOURCE)
	$(LZO_CAT) $(DL_DIR)/$(LZO_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	cat $(LZO_PATCH) | patch -p1 -d $(LZO_DIR)
	touch $(LZO_DIR)/.unpacked

LZO_CONFIG_SHARED:=--disable-shared
#LZO_CONFIG_SHARED:=--enable-shared

$(LZO_DIR)/.configured: $(LZO_DIR)/.unpacked
	(cd $(LZO_DIR); rm -rf config.cache; \
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
		$(LZO_CONFIG_SHARED) \
	);
	touch  $(LZO_DIR)/.configured

$(LZO_DIR)/src/liblzo.la: $(LZO_DIR)/.configured
	$(MAKE) -C $(LZO_DIR)

$(STAGING_DIR)/lib/liblzo.a: $(LZO_DIR)/src/liblzo.la
	$(MAKE) CC=$(TARGET_CC) DESTDIR=$(STAGING_DIR) -C $(LZO_DIR) install

lzo: uclibc $(STAGING_DIR)/lib/liblzo.a

lzo-clean:
	$(MAKE) DESTDIR=$(STAGING_DIR) -C $(LZO_DIR) uninstall
	-$(MAKE) -C $(LZO_DIR) clean

lzo-dirclean:
	rm -rf $(LZO_DIR)


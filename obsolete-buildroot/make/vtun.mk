#############################################################
#
# vtun
#
# NOTE: Uses start-stop-daemon in init script, so be sure
# to enable that within busybox
#
#############################################################
VTUN_SOURCE:=vtun-2.6.tar.gz
VTUN_SITE:=http://aleron.dl.sourceforge.net/sourceforge/vtun/
VTUN_DIR:=$(BUILD_DIR)/vtun-2.6
VTUN_CAT:=zcat
VTUN_BINARY:=vtund
VTUN_TARGET_BINARY:=usr/sbin/vtund
VTUN_PATCH:=$(SOURCE_DIR)/vtun.patch

$(DL_DIR)/$(VTUN_SOURCE):
	 $(WGET) -P $(DL_DIR) $(VTUN_SITE)/$(VTUN_SOURCE)

vtun-source: $(DL_DIR)/$(VTUN_SOURCE)

$(VTUN_DIR)/.unpacked: $(DL_DIR)/$(VTUN_SOURCE)
	$(VTUN_CAT) $(DL_DIR)/$(VTUN_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	mv $(BUILD_DIR)/vtun $(VTUN_DIR)
	cat $(VTUN_PATCH) | patch -p1 -d $(VTUN_DIR)
	touch $(VTUN_DIR)/.unpacked

$(VTUN_DIR)/.configured: $(VTUN_DIR)/.unpacked zlib lzo openssl
	(cd $(VTUN_DIR); rm -rf config.cache; \
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
		--with-ssl-headers=$(STAGING_DIR)/include/openssl \
		--with-lzo-headers=$(STAGING_DIR)/include \
	);
	touch  $(VTUN_DIR)/.configured

$(VTUN_DIR)/$(VTUN_BINARY): $(VTUN_DIR)/.configured
	$(MAKE) -C $(VTUN_DIR)

$(TARGET_DIR)/$(VTUN_TARGET_BINARY): $(VTUN_DIR)/$(VTUN_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(VTUN_DIR) install
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

vtun: uclibc $(TARGET_DIR)/$(VTUN_TARGET_BINARY)

vtun-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(VTUN_DIR) uninstall
	-$(MAKE) -C $(VTUN_DIR) clean

vtun-dirclean:
	rm -rf $(VTUN_DIR)


#############################################################
#
# openvpn
#
# NOTE: Uses start-stop-daemon in init script, so be sure
# to enable that within busybox
#
#############################################################
OPENVPN_SOURCE:=openvpn-1.5.0.tar.gz
OPENVPN_SITE:=http://aleron.dl.sourceforge.net/sourceforge/openvpn/
OPENVPN_DIR:=$(BUILD_DIR)/openvpn-1.5.0
OPENVPN_CAT:=zcat
OPENVPN_BINARY:=openvpn
OPENVPN_TARGET_BINARY:=usr/sbin/openvpn
#OPENVPN_PATCH:=$(SOURCE_DIR)/openvpn.patch

$(DL_DIR)/$(OPENVPN_SOURCE):
	 $(WGET) -P $(DL_DIR) $(OPENVPN_SITE)/$(OPENVPN_SOURCE)

openvpn-source: $(DL_DIR)/$(OPENVPN_SOURCE)

$(OPENVPN_DIR)/.unpacked: $(DL_DIR)/$(OPENVPN_SOURCE)
	$(OPENVPN_CAT) $(DL_DIR)/$(OPENVPN_SOURCE) | tar -C $(BUILD_DIR) -xvf -
	#cat $(OPENVPN_PATCH) | patch -p1 -d $(OPENVPN_DIR)
	touch $(OPENVPN_DIR)/.unpacked

$(OPENVPN_DIR)/.configured: $(OPENVPN_DIR)/.unpacked
	(cd $(OPENVPN_DIR); rm -rf config.cache; \
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
		--program-prefix="" \
		--enable-pthread \
	);
	touch  $(OPENVPN_DIR)/.configured

$(OPENVPN_DIR)/$(OPENVPN_BINARY): $(OPENVPN_DIR)/.configured
	$(MAKE) -C $(OPENVPN_DIR)

$(TARGET_DIR)/$(OPENVPN_TARGET_BINARY): $(OPENVPN_DIR)/$(OPENVPN_BINARY)
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(OPENVPN_DIR) install
	mkdir -p $(TARGET_DIR)/etc/openvpn
	cp $(SOURCE_DIR)/openvpn $(TARGET_DIR)/etc/init.d/openvpn
	rm -rf $(TARGET_DIR)/share/locale $(TARGET_DIR)/usr/info \
		$(TARGET_DIR)/usr/man $(TARGET_DIR)/usr/share/doc

openvpn: uclibc lzo openssl $(TARGET_DIR)/$(OPENVPN_TARGET_BINARY)

openvpn-clean:
	$(MAKE) DESTDIR=$(TARGET_DIR) -C $(OPENVPN_DIR) uninstall
	-$(MAKE) -C $(OPENVPN_DIR) clean

openvpn-dirclean:
	rm -rf $(OPENVPN_DIR)


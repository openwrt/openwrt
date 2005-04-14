# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openvpn
PKG_VERSION:=2.0_rc21
PKG_RELEASE:=1
PKG_MD5SUM:=949c51cb6c69da7054613d80042dc5a6

PKG_SOURCE_URL:=http://openvpn.net/release @SF/openvpn
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_CAT:=zcat
PKG_IPK:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
PKG_IPK_DIR:=$(PKG_BUILD_DIR)/ipkg

ifneq ($(BR2_PACKAGE_OPENVPN_LZO),y)
DISABLE_LZO:=--disable-lzo
endif

ifneq ($(BR2_PACKAGE_OPENVPN_SERVER),y)
DISABLE_SERVER:=--disable-server
endif

$(DL_DIR)/$(PKG_SOURCE):
	 $(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.unpacked: $(DL_DIR)/$(PKG_SOURCE)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.unpacked

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.unpacked
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
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
		--disable-pthread \
		--disable-debug \
		--disable-plugins \
		--disable-management \
		--disable-socks \
		--disable-http \
		$(DISABLE_LZO) \
		$(DISABLE_SERVER) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/openvpn: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)

$(PKG_IPK): $(PKG_BUILD_DIR)/openvpn
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(PKG_IPK_DIR) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	mkdir -p $(PKG_IPK_DIR)/usr/sbin
	cp $(PKG_BUILD_DIR)/openvpn $(PKG_IPK_DIR)/usr/sbin/
	$(STRIP) $(PKG_IPK_DIR)/usr/sbin/*
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(PKG_IPK_DIR) $(PACKAGE_DIR)

$(IPKG_STATE_DIR)/info/openvpn.list: $(PKG_IPK)
	$(IPKG) install $(PKG_IPK)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.unpacked
compile: $(PKG_IPK)
install: $(IPKG_STATE_DIR)/info/openvpn.list

clean:
	rm -rf $(PKG_BUILD_DIR)
	rm -f $(PKG_IPK)

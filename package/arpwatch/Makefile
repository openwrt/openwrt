# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=arpwatch
PKG_VERSION:=2.1a11
PKG_RELEASE:=1
PKG_MD5SUM:=9ecd00db892737431b0e524f3e52e19e

PKG_SOURCE_URL:= \
	http://nbd.vd-s.ath.cx/openwrt \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)/$(PKG_NAME)-$(PKG_VERSION)

IPKG_ARPWATCH:=$(PACKAGE_DIR)/$(PKG_NAME)_$(PKG_VERSION)-$(PKG_RELEASE)_$(ARCH).ipk
IDIR_ARPWATCH:=$(PKG_BUILD_DIR)/ipkg/$(PKG_NAME)
INFO_ARPWATCH:=$(IPKG_STATE_DIR)/info/$(PKG_NAME).list

$(DL_DIR)/$(PKG_SOURCE):
	$(SCRIPT_DIR)/download.pl $(DL_DIR) $(PKG_SOURCE) $(PKG_MD5SUM) $(PKG_SOURCE_URL)

$(PKG_BUILD_DIR)/.prepared: $(DL_DIR)/$(PKG_SOURCE)
	mkdir -p $(BUILD_DIR)/$(PKG_NAME)
	$(PKG_CAT) $(DL_DIR)/$(PKG_SOURCE) | tar -C $(BUILD_DIR)/$(PKG_NAME) $(TAR_OPTIONS) -
	touch $(PKG_BUILD_DIR)/.prepared

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--program-prefix="" \
		--program-suffix="" \
		--prefix=/usr \
		--exec-prefix=/usr \
		--bindir=/usr/bin \
		--datadir=/usr/share \
		--includedir=/usr/include \
		--infodir=/usr/share/info \
		--libdir=/usr/lib \
		--libexecdir=/usr/lib \
		--localstatedir=/var \
		--mandir=/usr/share/man \
		--sbindir=/usr/sbin \
		--sysconfdir=/etc \
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/$(PKG_NAME): $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  	ARPDIR=/etc/arpwatch \
		CCOPT="$(TARGET_CFLAGS)" \
		INCLS="-I. -I$(STAGING_DIR)/usr/include" \
		LIBS="-L$(STAGING_DIR)/usr/lib -lpcap" \
	  

$(IPKG_ARPWATCH): $(PKG_BUILD_DIR)/$(PKG_NAME)
	$(SCRIPT_DIR)/make-ipkg-dir.sh $(IDIR_ARPWATCH) $(PKG_NAME).control $(PKG_VERSION)-$(PKG_RELEASE) $(ARCH)
	cp arpwatch.conffiles $(IDIR_ARPWATCH)/CONTROL/conffiles
	mkdir -p $(IDIR_ARPWATCH)/usr/sbin
	cp $(PKG_BUILD_DIR)/arpwatch $(IDIR_ARPWATCH)/usr/sbin/
	cp $(PKG_BUILD_DIR)/arpsnmp $(IDIR_ARPWATCH)/usr/sbin/
	$(STRIP) $(IDIR_ARPWATCH)/usr/sbin/*
	mkdir -p $(IDIR_ARPWATCH)/etc/arpwatch
	cp $(PKG_BUILD_DIR)/arp.dat $(IDIR_ARPWATCH)/etc/arpwatch/
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_ARPWATCH) $(PACKAGE_DIR)

$(INFO_ARPWATCH): $(IPKG_ARPWATCH)
	$(IPKG) install $(IPKG_ARPWATCH)

source: $(DL_DIR)/$(PKG_SOURCE)
prepare: $(PKG_BUILD_DIR)/.prepared
compile: $(IPKG_ARPWATCH)
install: $(INFO_ARPWATCH)

clean:
	rm -rf $(PKG_BUILD_DIR) $(IPKG_ARPWATCH)

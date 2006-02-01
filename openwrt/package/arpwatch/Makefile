# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=arpwatch
PKG_VERSION:=2.1a11
PKG_RELEASE:=1
PKG_MD5SUM:=9ecd00db892737431b0e524f3e52e19e

PKG_SOURCE_URL:=http://nbd.vd-s.ath.cx/openwrt
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ARPWATCH,arpwatch,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
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
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
	  	ARPDIR=/etc/arpwatch \
		CCOPT="$(TARGET_CFLAGS)" \
		INCLS="-I. -I$(STAGING_DIR)/usr/include" \
		LIBS="-L$(STAGING_DIR)/usr/lib -lpcap"
	touch $@

$(IPKG_ARPWATCH):
	mkdir -p $(IDIR_ARPWATCH)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/arpwatch $(IDIR_ARPWATCH)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/arpsnmp $(IDIR_ARPWATCH)/usr/sbin/
	$(STRIP) $(IDIR_ARPWATCH)/usr/sbin/*
	mkdir -p $(IDIR_ARPWATCH)/etc/arpwatch
	$(CP) $(PKG_BUILD_DIR)/arp.dat $(IDIR_ARPWATCH)/etc/arpwatch/
	$(IPKG_BUILD) $(IDIR_ARPWATCH) $(PACKAGE_DIR)

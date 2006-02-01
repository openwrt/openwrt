# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=arpd
PKG_VERSION:=0.2
PKG_RELEASE:=1
PKG_MD5SUM:=e2911fa9de1b92ef50deda1489ae944d

PKG_SOURCE_URL:=http://niels.xtdnet.nl/honeyd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ARPD,arpd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		--enable-shared \
		--disable-static \
		--with-libdnet=$(STAGING_DIR)/usr \
		--with-libevent=$(STAGING_DIR)/usr \
		--with-libpcap=$(STAGING_DIR)/usr \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CCOPT="$(TARGET_CFLAGS)" \
		INCLS="-I. -I$(STAGING_DIR)/usr/include" \
		LIBS="-L$(STAGING_DIR)/usr/lib -lpcap -ldnet -levent"
	touch $@

$(IPKG_ARPD):
	mkdir -p $(IDIR_ARPD)/usr/sbin
	$(CP)  $(PKG_BUILD_DIR)/arpd $(IDIR_ARPD)/usr/sbin/
	$(STRIP) $(IDIR_ARPD)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_ARPD) $(PACKAGE_DIR)

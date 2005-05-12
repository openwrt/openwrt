# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cups
PKG_VERSION:=1.1.23
PKG_RELEASE:=1
PKG_MD5SUM:=4ce09b1dce09b6b9398af0daae9adf63

PKG_SOURCE_URL:=http://ftp.funet.fi/pub/mirrors/ftp.easysw.com/ftp.easysw.com/pub/cups/$(PKG_VERSION) \
	http://ftp.rz.tu-bs.de/pub/mirror/ftp.easysw.com/ftp/pub/cups/$(PKG_VERSION) \
	ftp://ftp3.easysw.com/pub/cups/$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-source.tar.bz2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install
PKG_CAT:=bzcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CUPS,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		ac_cv_path_STRIP="$(STRIP)" \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
		--target=$(GNU_TARGET_NAME) \
		--host=$(GNU_TARGET_NAME) \
		--build=$(GNU_HOST_NAME) \
		--prefix=$(CUPS_IPK_DIR)/usr \
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
		--with-gnu-ld \
		--with-cups-user=root \
		--with-cups-group=root \
		--without-perl \
		--without-python \
		--without-php \
		--disable-slp \
		--disable-gnutls \
		--disable-openssl \
		--disable-cdsassl \
		--disable-ssl \
		--disable-slp \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR)
	$(MAKE) STRIP=$(STRIP) DSTROOT=$(PKG_INSTALL_DIR) -C $(PKG_BUILD_DIR) install
	cp -a $(PKG_INSTALL_DIR)/usr/lib/* $(STAGING_DIR)/lib/
	cp -a $(PKG_INSTALL_DIR)/usr/include/* $(STAGING_DIR)/include/
	touch $@

$(IPKG_CUPS):
	cp -a $(PKG_INSTALL_DIR)/* $(IDIR_CUPS)/
	rm -f $(IDIR_CUPS)/usr/bin/cups-config
	rm -f $(IDIR_CUPS)/usr/lib/*.a
	rm -f $(IDIR_CUPS)/usr/share/doc/cups/* || true
	rm -rf $(IDIR_CUPS)/usr/share/doc/cups/??
	rm -rf $(IDIR_CUPS)/usr/share/locale
	rm -rf $(IDIR_CUPS)/usr/share/cups/banners
	rm -rf $(IDIR_CUPS)/usr/share/cups/charsets
	rm -rf $(IDIR_CUPS)/usr/share/cups/fonts
	rm -rf $(IDIR_CUPS)/usr/share/cups/model
	rm -rf $(IDIR_CUPS)/usr/share/cups/data
	rm -rf $(IDIR_CUPS)/usr/share/cups/templates/??
	rm -rf $(IDIR_CUPS)/usr/include
	rm -rf $(IDIR_CUPS)/var
	rm -rf $(IDIR_CUPS)/etc/*.d
	$(STRIP) $(IDIR_CUPS)/usr/bin/*
	$(STRIP) $(IDIR_CUPS)/usr/lib/cups/backend/*
	$(STRIP) $(IDIR_CUPS)/usr/lib/cups/cgi-bin/*
	$(STRIP) $(IDIR_CUPS)/usr/lib/cups/daemon/*
	cp -a ./files/* $(IDIR_CUPS)/
	find $(IDIR_CUPS) -name CVS | xargs rm -rf
	$(IPKG_BUILD) $(IDIR_CUPS) $(PACKAGE_DIR)


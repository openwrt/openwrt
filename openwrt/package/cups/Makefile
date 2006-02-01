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
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,CUPS,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
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
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DSTROOT="$(PKG_INSTALL_DIR)" \
		STRIP="/bin/true" \
		all install
	touch $@

$(IPKG_CUPS):
	rm -rf $(IDIR_CUPS)/etc/cups
	install -d -m0755 $(IDIR_CUPS)/etc/cups
	$(CP) $(PKG_INSTALL_DIR)/etc/cups/* $(IDIR_CUPS)/etc/cups/
	rm -rf $(IDIR_CUPS)/etc/cups/certs
	ln -sf /tmp $(IDIR_CUPS)/etc/cups/certs
	install -d -m0755 $(IDIR_CUPS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(IDIR_CUPS)/usr/bin/
	rm -f $(IDIR_CUPS)/usr/bin/cups-config
	install -d -m0755 $(IDIR_CUPS)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcups.so.* $(IDIR_CUPS)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/cups $(IDIR_CUPS)/usr/lib/
	install -d -m0755 $(IDIR_CUPS)/usr/share/cups/templates
	$(CP) $(PKG_INSTALL_DIR)/usr/share/cups/templates/*.tmpl $(IDIR_CUPS)/usr/share/cups/templates/
	install -d -m0755 $(IDIR_CUPS)/usr/share/doc/cups
	$(CP) $(PKG_INSTALL_DIR)/usr/share/doc/cups/index.html $(IDIR_CUPS)/usr/share/doc/cups/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/doc/cups/images $(IDIR_CUPS)/usr/share/doc/cups/
	install -d -m0755 $(IDIR_CUPS)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/* $(IDIR_CUPS)/usr/sbin/
	# overwrite default config with our own
	$(CP) ./files/etc/cups/* $(IDIR_CUPS)/etc/cups/
	# install initscript with priority 60
	install -d -m0755 $(IDIR_CUPS)/etc/init.d
	install -m0755 ./files/cupsd.init $(IDIR_CUPS)/etc/init.d/S60cupsd
	find $(IDIR_CUPS) -name CVS | xargs rm -rf
	$(RSTRIP) $(IDIR_CUPS)
	$(IPKG_BUILD) $(IDIR_CUPS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libcups.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/cups-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/cups $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcups.{a,so*} $(STAGING_DIR)/usr/lib/

install-dev: $(STAGING_DIR)/usr/lib/libcups.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/bin/cups-config \
	  $(STAGING_DIR)/usr/include/cups \
	  $(STAGING_DIR)/usr/lib/libcups.{a,so*}

compile-targets: install-dev
clean-targets: uninstall-dev


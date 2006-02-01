# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gmediaserver
PKG_VERSION:=0.9.0
PKG_RELEASE:=1
PKG_MD5SUM:=93de0f8dcbc1353a0d0dd5b5ab4b3142

PKG_SOURCE_URL:=http://savannah.nongnu.org/download/gmediaserver/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,GMEDIASERVER,gmediaserver,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS=" -lixml -lthreadutil -lupnp -luClibc++ -lz" \
		am_cv_func_iconv=no \
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
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--disable-static \
			--disable-rpath \
			--with-gnu-ld \
			--with-id3lib="$(STAGING_DIR)/usr" \
			--with-libupnp="$(STAGING_DIR)/usr" \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_GMEDIASERVER):
	install -m0755 -d $(IDIR_GMEDIASERVER)/etc/default
	install -m0644 ./files/gmediaserver.default $(IDIR_GMEDIASERVER)/etc/default/gmediaserver
	install -m0755 -d $(IDIR_GMEDIASERVER)/etc/init.d
	install -m0755 ./files/gmediaserver.init $(IDIR_GMEDIASERVER)/etc/init.d/gmediaserver
	install -m0755 -d $(IDIR_GMEDIASERVER)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/gmediaserver $(IDIR_GMEDIASERVER)/usr/sbin/
	$(RSTRIP) $(IDIR_GMEDIASERVER)
	$(IPKG_BUILD) $(IDIR_GMEDIASERVER) $(PACKAGE_DIR)


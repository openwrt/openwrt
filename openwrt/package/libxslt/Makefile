# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libxslt
PKG_VERSION:=1.1.14
PKG_RELEASE:=1
PKG_MD5SUM:=db71660bb7d01ccd4e6be990af8d813b

PKG_SOURCE_URL:=http://xmlsoft.org/sources/ \
	ftp://fr.rpmfind.net/pub/libxml/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBXSLT,libxslt,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/libxml2" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
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
			--enable-static \
			--disable-rpath \
			--with-gnu-ld \
			--with-libxml-prefix="$(STAGING_DIR)/usr" \
			--with-libxml-include-prefix="$(STAGING_DIR)/usr/include" \
			--with-libxml-libs-prefix="$(STAGING_DIR)/usr/lib" \
			--without-python \
			--without-crypto \
			--without-debug \
			--without-mem-debug \
			--without-debugger \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBXSLT):
	install -m0755 -d $(IDIR_LIBXSLT)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxslt.so.* $(IDIR_LIBXSLT)/usr/lib/
	$(RSTRIP) $(IDIR_LIBXSLT)
	$(IPKG_BUILD) $(IDIR_LIBXSLT) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libxslt.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/xslt-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libxslt $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxslt.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libxslt.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/bin/xslt-config
	$(SED) 's,-I$(STAGING_DIR)/usr/include,,g' $(STAGING_DIR)/usr/bin/xslt-config
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/bin/xslt-config
	$(SED) 's,-L$(STAGING_DIR)/usr/lib,,g' $(STAGING_DIR)/usr/bin/xslt-config
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/libxslt.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/libxslt.pc
	$(SED) 's,-L$(STAGING_DIR)/usr/lib,,g' $(STAGING_DIR)/usr/lib/pkgconfig/libxslt.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libxslt.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/xslt-config \
		$(STAGING_DIR)/usr/include/libxslt \
		$(STAGING_DIR)/usr/lib/libxslt.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/libxslt.pc \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built


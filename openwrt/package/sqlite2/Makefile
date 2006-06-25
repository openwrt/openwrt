# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=sqlite2
PKG_VERSION:=2.8.17
PKG_RELEASE:=1
PKG_MD5SUM:=838dbac20b56d2c4292e98848505a05b

PKG_SOURCE_URL:=http://www.sqlite.org/
PKG_SOURCE:=sqlite-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/sqlite-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBSQLITE2,libsqlite2,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQLITE2_CLI,sqlite2-cli,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		config_BUILD_CC="$(HOSTCC)" \
		config_BUILD_CFLAGS="-O2" \
		config_TARGET_CC="$(TARGET_CC)" \
		config_TARGET_CFLAGS="$(TARGET_CFLAGS)" \
		config_TARGET_READLINE_INC="-I$(STAGING_DIR)/usr/include" \
		config_TARGET_READLINE_LIBS="-L$(STAGING_DIR)/usr/lib -lreadline -lncurses" \
		config_TARGET_TCL_INC="-DNO_TCL=1" \
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
			--enable-shared \
			--enable-static \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBSQLITE2):
	install -d -m0755 $(IDIR_LIBSQLITE2)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite.so.* $(IDIR_LIBSQLITE2)/usr/lib/
	$(RSTRIP) $(IDIR_LIBSQLITE2)
	$(IPKG_BUILD) $(IDIR_LIBSQLITE2) $(PACKAGE_DIR)

$(IPKG_SQLITE2_CLI):
	install -d -m0755 $(IDIR_SQLITE2_CLI)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/sqlite $(IDIR_SQLITE2_CLI)/usr/bin/
	$(RSTRIP) $(IDIR_SQLITE2_CLI)
	$(IPKG_BUILD) $(IDIR_SQLITE2_CLI) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libsqlite.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/sqlite.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/sqlite.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/sqlite.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/sqlite.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libsqlite.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/sqlite.h \
		$(STAGING_DIR)/usr/lib/libsqlite.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/sqlite.pc \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built 

# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=sqlite
PKG_VERSION:=3.3.3
PKG_RELEASE:=1
PKG_MD5SUM:=eb5d9d7e7853b3af78e767d709b7fced

PKG_SOURCE_URL:=http://www.sqlite.org/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBSQLITE,libsqlite,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,SQLITE_CLI,sqlite-cli,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
			--disable-tcl \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBSQLITE):
	install -d -m0755 $(IDIR_LIBSQLITE)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite3.so.* $(IDIR_LIBSQLITE)/usr/lib/
	$(RSTRIP) $(IDIR_LIBSQLITE)
	$(IPKG_BUILD) $(IDIR_LIBSQLITE) $(PACKAGE_DIR)

$(IPKG_SQLITE_CLI):
	install -d -m0755 $(IDIR_SQLITE_CLI)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/sqlite3 $(IDIR_SQLITE_CLI)/usr/bin/
	$(RSTRIP) $(IDIR_SQLITE_CLI)
	$(IPKG_BUILD) $(IDIR_SQLITE_CLI) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libsqlite3.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/sqlite3.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsqlite3.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/sqlite3.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/sqlite3.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/sqlite3.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libsqlite3.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/sqlite3.h \
		$(STAGING_DIR)/usr/lib/libsqlite3.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/sqlite3.pc \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built 

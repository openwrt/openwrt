# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libdb
PKG_VERSION:=4.2.52
PKG_RELEASE:=1
PKG_MD5SUM:=x

PKG_SOURCE_URL:=ftp://ftp.sleepycat.com/releases/ \
	http://downloads.sleepycat.com/
PKG_SOURCE:=db-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/db-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBDB,libdb,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR)/build_unix ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		../dist/configure \
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
		--enable-static \
		--disable-java \
		--enable-cxx \
		--disable-posixmutexes \
		--disable-uimutexes \
		--disable-tcl \
		--disable-rpc \
		--enable-compat185 \
		--enable-debug \
		--enable-statistics \
		--enable-replication \
		--enable-cryptography \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR)/build_unix \
		DESTDIR="$(PKG_INSTALL_DIR)" install
	touch $@

$(IPKG_LIBDB): 
	install -m0755 -d $(IDIR_LIBDB)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdb*.so $(IDIR_LIBDB)/usr/lib/
	$(RSTRIP) $(IDIR_LIBDB)
	$(IPKG_BUILD) $(IDIR_LIBDB) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libdb.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/db.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libdb*.{a,so} $(STAGING_DIR)/usr/lib/
	touch $(STAGING_DIR)/usr/lib/libdb.so

install-dev: $(STAGING_DIR)/usr/lib/libdb.so

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/include/db.h
	rm -rf $(STAGING_DIR)/usr/lib/libdb*.{a,so}

compile-targets: install-dev
clean-targets: uninstall-dev

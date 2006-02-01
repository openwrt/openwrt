# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libgpg-error
PKG_VERSION:=1.0
PKG_RELEASE:=0
PKG_MD5SUM:=ff409db977e4a4897aa09ea420a28a2f

PKG_SOURCE_URL:=ftp://ftp.gnupg.org/gcrypt/libgpg-error/ \
	http://mirrors.rootmode.com/ftp.gnupg.org/libgpg-error/ \
	http://gulus.usherbrooke.ca/pub/appl/GnuPG/libgpg-error/ \
	http://gnupg.unixmexico.org/ftp/libgpg-error/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBGPG_ERROR,libgpg-error,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
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
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		--enable-shared \
		--enable-static \
		--disable-rpath \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBGPG_ERROR):
	install -m0755 -d $(IDIR_LIBGPG_ERROR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgpg-error.so.* $(IDIR_LIBGPG_ERROR)/usr/lib/
	$(RSTRIP) $(IDIR_LIBGPG_ERROR)
	$(IPKG_BUILD) $(IDIR_LIBGPG_ERROR) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libgpg-error.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/gpg-error-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gpg-error.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgpg-error.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/share/aclocal
	$(CP) $(PKG_INSTALL_DIR)/usr/share/aclocal/gpg-error.m4 $(STAGING_DIR)/usr/share/aclocal/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libgpg-error.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/gpg-error-config \
		$(STAGING_DIR)/usr/include/gpg-error.h \
		$(STAGING_DIR)/usr/lib/libgpg-error.{a,so*} \
		$(STAGING_DIR)/usr/share/aclocal/gpg-error.m4 \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built

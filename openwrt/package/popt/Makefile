# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=popt
PKG_VERSION:=1.7
PKG_RELEASE:=4
PKG_MD5SUM:=5988e7aeb0ae4dac8d83561265984cc9

PKG_SOURCE_URL:= \
	ftp://ftp.rpm.org/pub/rpm/dist/rpm-4.1.x/ \
	http://nthill.free.fr/openwrt/sources/$(PKG_NAME)/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/templates.mk

$(eval $(call PKG_template,LIBPOPT,libpopt,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

include $(TOPDIR)/package/rules.mk

COMPILE_TARGETS += install-dev
CLEAN_TARGETS += uninstall-dev

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
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
		--enable-shared \
		--enable-static \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libpopt.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		install
	rm -f $(STAGING_DIR)/usr/lib/libpopt.la

$(IPKG_LIBPOPT): $(IDIR_LIBPOPT)/CONTROL/control $(STAGING_DIR)/usr/lib/libpopt.so
	mkdir -p $(IDIR_LIBPOPT)/usr/lib
	cp -fpR $(STAGING_DIR)/usr/lib/libpopt.so.* $(IDIR_LIBPOPT)/usr/lib/
	$(RSTRIP) $(IDIR_LIBPOPT)
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBPOPT) $(PACKAGE_DIR)

install-dev: $(STAGING_DIR)/usr/lib/libpopt.so

uninstall-dev:
	-$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(STAGING_DIR)" \
		uninstall

# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=pcre
PKG_VERSION:=5.0
PKG_RELEASE:=3
PKG_MD5SUM:=53976b62032fd5c76678274e5fd196b4

PKG_SOURCE_URL:=ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBPCRE,libpcre,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status}; \
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
		--enable-utf8 \
	);
	touch $(PKG_BUILD_DIR)/.configured

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC_FOR_BUILD="$(HOSTCC)" \
		LINK_FOR_BUILD="$(HOSTCC)" \
		CFLAGS_FOR_BUILD="" \
		dftables
	$(MAKE) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)"
	touch $(PKG_BUILD_DIR)/.built

$(STAGING_DIR)/usr/lib/libpcre.so: $(PKG_BUILD_DIR)/.built
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		DESTDIR="$(STAGING_DIR)" \
		install
	rm -f $(STAGING_DIR)/usr/lib/libpcre*.la

$(IPKG_LIBPCRE): $(IDIR_LIBPCRE)/CONTROL/control $(STAGING_DIR)/usr/lib/libpcre.so
	mkdir -p $(IDIR_LIBPCRE)/usr/lib
	cp -fpR $(STAGING_DIR)/usr/lib/libpcre*.so.* $(IDIR_LIBPCRE)/usr/lib/
	$(RSTRIP) $(IDIR_LIBPCRE)
	mkdir -p $(PACKAGE_DIR)
	$(IPKG_BUILD) $(IDIR_LIBPCRE) $(PACKAGE_DIR)

install-dev: $(STAGING_DIR)/usr/lib/libpcre.so

uninstall-dev:
	rm -rf \
	  $(STAGING_DIR)/usr/bin/pcre-config \
	  $(STAGING_DIR)/usr/include/pcre* \
	  $(STAGING_DIR)/usr/lib/libpcre* \
	  $(STAGING_DIR)/usr/lib/pkgconfig/libpcre.pc \
	  $(STAGING_DIR)/usr/share/man/man*/pcre* \

compile: install-dev
clean: uninstall-dev

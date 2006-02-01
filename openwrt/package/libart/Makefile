# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libart
PKG_VERSION:=2.3.17
PKG_RELEASE:=1
PKG_MD5SUM:=f1f6b3e5490f382535dccb288f5d8b14

PKG_SOURCE_URL:=http://ftp.gnome.org/pub/gnome/sources/libart_lgpl/2.3
PKG_SOURCE:=$(PKG_NAME)_lgpl-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_lgpl-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBART,libart,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		$(TARGET_CONFIGURE_OPTS) \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--sbindir=/usr/sbin \
			--libexecdir=/usr/lib \
			--sysconfdir=/etc \
			--datadir=/usr/share \
			--localstatedir=/var \
			--mandir=/usr/man \
			--infodir=/usr/info \
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
			--enable-shared=yes \
			--enable-static=yes \
			--disable-rpath \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		HOSTCC="$(HOSTCC)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBART):
	install -d -m0755 $(IDIR_LIBART)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libart_lgpl_2.so.* $(IDIR_LIBART)/usr/lib/
	$(RSTRIP) $(IDIR_LIBART)
	$(IPKG_BUILD) $(IDIR_LIBART) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libart_lgpl_2.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/libart2-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libart-2.0 $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libart_lgpl_2.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libart-2.0.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libart_lgpl_2.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/libart2-config \
		$(STAGING_DIR)/usr/include/libart-2.0 \
		$(STAGING_DIR)/usr/lib/libart_lgpl_2.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/libart-2.0.pc \

compile-targets: install-dev
clean-targets: uninstall-dev

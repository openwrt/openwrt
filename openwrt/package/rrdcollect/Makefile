# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=rrdcollect
PKG_VERSION:=0.2.3
PKG_RELEASE:=1
PKG_MD5SUM:=5e4305c612bc3cccbaf802c275c81a11

PKG_SOURCE_URL:=@SF/rrdcollect
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,RRDCOLLECT,rrdcollect,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DSOCKET_COMM" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		LIBS="-lart_lgpl_2 -lfreetype -lpng -lz" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
		ac_cv_func_malloc_0_nonnull=yes \
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
			--enable-shared \
			--disable-static \
			--enable-exec \
			--without-rrdtool \
			--with-librrd \
			--without-libpcre \
			--without-libpcap \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_RRDCOLLECT):
	install -d -m0755 $(IDIR_RRDCOLLECT)/etc
	install -m0644 ./files/rrdcollect.conf $(IDIR_RRDCOLLECT)/etc
	install -d -m0755 $(IDIR_RRDCOLLECT)/etc/init.d
	install -m0755 ./files/rrdcollect.init $(IDIR_RRDCOLLECT)/etc/init.d/rrdcollect
	ln -sf rrdcollect $(IDIR_RRDCOLLECT)/etc/init.d/S98rrdcollect
	install -d -m0755 $(IDIR_RRDCOLLECT)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/rrdcollect $(IDIR_RRDCOLLECT)/usr/sbin/
	$(RSTRIP) $(IDIR_RRDCOLLECT)
	$(IPKG_BUILD) $(IDIR_RRDCOLLECT) $(PACKAGE_DIR)


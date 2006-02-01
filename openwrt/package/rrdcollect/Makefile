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

ifneq ($(BR2_PACKAGE_LIBRRD),)
RRDTOOL_LIBS:=-lart_lgpl_2 -lfreetype -lpng
endif

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,RRDCOLLECT,rrdcollect,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,RRDCOLLECT_EXAMPLE,rrdcollect-example,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -DSOCKET_COMM" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		LIBS="$(RRDTOOL_LIBS) -lz" \
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
			--disable-rpath \
			--with-gnu-ld \
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
	install -d -m0755 $(IDIR_RRDCOLLECT)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/rrdcollect $(IDIR_RRDCOLLECT)/usr/sbin/
	$(RSTRIP) $(IDIR_RRDCOLLECT)
	$(IPKG_BUILD) $(IDIR_RRDCOLLECT) $(PACKAGE_DIR)

$(IPKG_RRDCOLLECT_EXAMPLE):
	install -d -m0755 $(IDIR_RRDCOLLECT_EXAMPLE)/etc
	install -m0644 ./files/rrd.conf $(IDIR_RRDCOLLECT_EXAMPLE)/etc/
	install -m0644 ./files/rrdcollect.conf $(IDIR_RRDCOLLECT_EXAMPLE)/etc/
	install -d -m0755 $(IDIR_RRDCOLLECT_EXAMPLE)/etc/init.d
	install -m0755 ./files/rrdcollect.init $(IDIR_RRDCOLLECT_EXAMPLE)/etc/init.d/S98rrdcollect
	install -d -m0755 $(IDIR_RRDCOLLECT_EXAMPLE)/usr/bin
	install -m0755 ./files/rrd.sh $(IDIR_RRDCOLLECT_EXAMPLE)/usr/bin/
	install -d -m0755 $(IDIR_RRDCOLLECT_EXAMPLE)/www/cgi-bin
	ln -sf /var/lib/rrdcollect/rrd.cgi $(IDIR_RRDCOLLECT_EXAMPLE)/www/cgi-bin/rrd.cgi
	ln -sf /var/lib/rrdcollect/img $(IDIR_RRDCOLLECT_EXAMPLE)/www/img
	$(RSTRIP) $(IDIR_RRDCOLLECT_EXAMPLE)
	$(IPKG_BUILD) $(IDIR_RRDCOLLECT_EXAMPLE) $(PACKAGE_DIR)


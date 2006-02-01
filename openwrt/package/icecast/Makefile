# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=icecast
PKG_VERSION:=2.3.0
PKG_RELEASE:=1
PKG_MD5SUM:=35256fbc4a93571662af2ed18fbbfcc5

PKG_SOURCE_URL:=http://downloads.us.xiph.org/releases/icecast/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ICECAST,icecast,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include/libxml2 " \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \
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
			--disable-static \
			--disable-rpath \
			--with-gnu-ld \
			--enable-yp \
			--with-curl="yes" \
			--with-curl-config="$(STAGING_DIR)/usr/bin/curl-config" \
			--with-ogg="$(STAGING_DIR)/usr" \
			--with-speex="no" \
			--with-theora="no" \
			--with-vorbis="$(STAGING_DIR)/usr" \
			--with-xslt-config="$(STAGING_DIR)/usr/bin/xslt-config" \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_ICECAST):
	install -d -m0755 $(IDIR_ICECAST)/etc
	$(CP) $(PKG_INSTALL_DIR)/etc/icecast.xml $(IDIR_ICECAST)/etc/
	install -d -m0755 $(IDIR_ICECAST)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/icecast $(IDIR_ICECAST)/usr/bin/
	install -d -m0755 $(IDIR_ICECAST)/usr/share/icecast
	$(CP) $(PKG_INSTALL_DIR)/usr/share/icecast/admin $(IDIR_ICECAST)/usr/share/icecast/
	$(CP) $(PKG_INSTALL_DIR)/usr/share/icecast/web $(IDIR_ICECAST)/usr/share/icecast/
	$(RSTRIP) $(IDIR_ICECAST)
	$(IPKG_BUILD) $(IDIR_ICECAST) $(PACKAGE_DIR)

# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nocatsplash
PKG_VERSION:=0.92
PKG_RELEASE:=1
PKG_MD5SUM:=e0fcc0effe52e63bc31070d16d0b4a81

PKG_SOURCE_URL:=http://wifi.rulez.org/~josephus/
PKG_SOURCE:=NoCatSplash-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/NoCatSplash-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,NOCATSPLASH,nocatsplash,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
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
		  --with-firewall=iptables \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
		  --with-glib-prefix="$(STAGING_DIR)/usr" \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_NOCATSPLASH):
	mkdir -p $(IDIR_NOCATSPLASH)
	$(CP) $(PKG_INSTALL_DIR)/* $(IDIR_NOCATSPLASH)/ 
	$(RSTRIP) $(IDIR_NOCATSPLASH)
	$(IPKG_BUILD) $(IDIR_NOCATSPLASH) $(PACKAGE_DIR)


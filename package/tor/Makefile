# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=tor
PKG_VERSION:=0.1.0.17
PKG_RELEASE:=1
PKG_MD5SUM:=83c4afe29fa82473afcb2ec7e17771b9

PKG_SOURCE_URL:=http://tor.eff.org/dist/ \
	http://ftp.se.linux.org/crypto/tor/ \
	http://tor.meulie.net/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,TOR,tor,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		ac_cv_libevent_normal=yes \
		ac_cv_openssldir="$(STAGING_DIR)/usr" \
		tor_cv_null_is_zero=yes \
		tor_cv_unaligned_ok=yes \
		./configure \
		  --target=$(GNU_TARGET_NAME) \
		  --host=$(GNU_TARGET_NAME) \
		  --build=$(GNU_HOST_NAME) \
		  --program-prefix="" \
		  --program-suffix="" \
		  --prefix=/usr \
		  --exec-prefix=/usr \
		  --bindir=/usr/sbin \
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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_TOR):
	install -d -m0755 $(IDIR_TOR)/etc/init.d
	install -m0755 ./files/tor.init $(IDIR_TOR)/etc/init.d/tor
	install -d -m0755 $(IDIR_TOR)/etc/tor
	install -m0644 ./files/torrc $(IDIR_TOR)/etc/tor/torrc
	install -d -m0755 $(IDIR_TOR)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/tor $(IDIR_TOR)/usr/sbin/
	$(RSTRIP) $(IDIR_TOR)
	$(IPKG_BUILD) $(IDIR_TOR) $(PACKAGE_DIR)


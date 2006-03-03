# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dsniff
PKG_VERSION:=2.4b1
PKG_RELEASE:=2
PKG_MD5SUM:=2f761fa3475682a7512b0b43568ee7d6

PKG_SOURCE_URL:=http://www.monkey.org/~dugsong/dsniff/beta
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-2.4
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DSNIFF,dsniff,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		  --without-db \
		  --without-x \
		  --with-gdbm="$(STAGING_DIR)/usr" \
		  --with-libnids="$(STAGING_DIR)/usr" \
		  --with-libpcap="$(STAGING_DIR)/usr" \
		  --with-libnet="$(STAGING_DIR)/usr" \
		  --with-openssl="$(STAGING_DIR)/usr" \
		  $(DISABLE_LARGEFILE) \
		  $(DISABLE_NLS) \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		install_prefix="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_DSNIFF):
	install -d -m0755 $(IDIR_DSNIFF)/usr/lib
	install -d -m0755 $(IDIR_DSNIFF)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/* $(IDIR_DSNIFF)/usr/sbin
	$(CP)  $(PKG_INSTALL_DIR)/usr/lib/* $(IDIR_DSNIFF)/usr/lib
	$(RSTRIP) $(IDIR_DSNIFF)
	$(IPKG_BUILD) $(IDIR_DSNIFF) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built

# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=osiris
PKG_VERSION:=4.2.0
PKG_RELEASE:=1
PKG_MD5SUM:=ad30995660e506ee6d1d6460601f6107

PKG_SOURCE_URL:=http://www.hostintegrity.com/osiris/data/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,OSIRISD,osirisd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		ac_cv_openssldir="$(STAGING_DIR)/usr" \
		ac_cv_have_accrights_in_msghdr=no \
		ac_cv_have_control_in_msghdr=yes \
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
		  --with-osiris-user=root \
		  --with-root-dir=/var/lib/osiris \
		  --with-readline=no \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		ar=$(TARGET_CROSS)ar \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INTERACTIVE=0 \
		all install
	touch $@

$(IPKG_OSIRISD):
	install -d -m0755 $(IDIR_OSIRISD)/etc/init.d
	install -m0755 ./files/osirisd.init $(IDIR_OSIRISD)/etc/init.d/osirisd
	install -d -m0755 $(IDIR_OSIRISD)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/osirisd $(IDIR_OSIRISD)/usr/sbin/
	$(RSTRIP) $(IDIR_OSIRISD)
	$(IPKG_BUILD) $(IDIR_OSIRISD) $(PACKAGE_DIR)

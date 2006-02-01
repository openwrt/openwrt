# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ipkg
PKG_VERSION:=0.99.154
PKG_RELEASE:=1
PKG_MD5SUM:=b60aeea3b86cecf0eb26b161d122fbcc

PKG_SOURCE_URL:=http://www.handhelds.org/packages/ipkg \
	http://www.gtlib.gatech.edu/pub/handhelds.org/packages/ipkg \
	http://ftp.gwdg.de/pub/linux/handhelds/packages/ipkg
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IPKG,ipkg,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/bin \
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
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_IPKG):
	install -d -m0755 $(IDIR_IPKG)/bin
	$(CP) $(PKG_INSTALL_DIR)/bin/ipkg-cl $(IDIR_IPKG)/bin/ipkg
	install -d -m0755 $(IDIR_IPKG)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libipkg.so.* $(IDIR_IPKG)/usr/lib/
	$(RSTRIP) $(IDIR_IPKG)
	$(IPKG_BUILD) $(IDIR_IPKG) $(PACKAGE_DIR)

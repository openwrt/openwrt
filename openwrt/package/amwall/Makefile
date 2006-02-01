# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=amwall
PKG_VERSION:=0.1.0
PKG_RELEASE:=1
PKG_MD5SUM:=a8b6438e7fd9ccadc9f68824ae62349c

PKG_SOURCE_URL:=ftp://ftp.amselinux.net/source/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,AMWALL,amwall,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		V= \
		LDIR=$(PKG_BUILD_DIR) \
		AMSEL_INCLUDE="$(STAGING_DIR)/usr/include" \
		OPT_LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		PREFIX="/usr" \
		CONFIG="$(PKG_BUILD_DIR)/etc/amsel" \
		DESTDIR=$(PKG_INSTALL_DIR) \
		all install
	touch $@

$(IPKG_AMWALL):
	install -d -m0755 $(IDIR_AMWALL)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/* $(IDIR_AMWALL)/usr/bin	
	install -d -m0755 $(IDIR_AMWALL)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/* $(IDIR_AMWALL)/usr/sbin	
	install -d -m0755 $(IDIR_AMWALL)/etc/amsel
	$(CP) $(PKG_INSTALL_DIR)/etc/amsel* $(IDIR_AMWALL)/etc	
	$(RSTRIP) $(IDIR_AMWALL)					
	$(IPKG_BUILD) $(IDIR_AMWALL) $(PACKAGE_DIR)


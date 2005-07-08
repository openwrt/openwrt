# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=dhcp
PKG_VERSION:=3.0.2
PKG_RELEASE:=1
PKG_MD5SUM:=04800a111521e7442749b2ce883f962b

PKG_SOURCE_URL:=ftp://ftp.isc.org/isc/dhcp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,DHCP,dhcp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		./configure \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_DHCP):
	install -d -m0755 $(IDIR_DHCP)/usr/sbin
	cp -fpR $(PKG_INSTALL_DIR)/usr/sbin/dhcpd $(IDIR_DHCP)/usr/sbin
	$(RSTRIP) $(IDIR_DHCP)
	$(IPKG_BUILD) $(IDIR_DHCP) $(PACKAGE_DIR)

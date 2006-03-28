# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=hostapd
PKG_VERSION:=0.5.2
PKG_RELEASE:=1
PKG_MD5SUM:=b7f4739ca4758798f8dc3b1ea9fd00d0

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,HOSTAPD,hostapd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(CP) ./files/config $(PKG_BUILD_DIR)/.config
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/madwifi" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		all
	touch $@

$(IPKG_HOSTAPD):
	install -m0755 -d $(IDIR_HOSTAPD)/etc
	install -m0600 $(PKG_BUILD_DIR)/madwifi.conf $(IDIR_HOSTAPD)/etc/hostapd.conf
	install -m0755 -d $(IDIR_HOSTAPD)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/hostapd $(IDIR_HOSTAPD)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/hostapd_cli $(IDIR_HOSTAPD)/usr/sbin/
	$(RSTRIP) $(IDIR_HOSTAPD)
	$(IPKG_BUILD) $(IDIR_HOSTAPD) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built


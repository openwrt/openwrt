# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wpa_supplicant
PKG_VERSION:=0.4.9
PKG_RELEASE:=1
PKG_MD5SUM:=98498c152cbfc388c306fd9bbf7d57d4

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WPA_SUPPLICANT,wpa-supplicant,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(CP) ./files/config $(PKG_BUILD_DIR)/.config
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include/madwifi" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		OPT_FLAGS="$(TARGET_CFLAGS)" \
		KERNEL=$(LINUX_DIR) \
		all
	touch $@

$(IPKG_WPA_SUPPLICANT):
	install -m0755 -d $(IDIR_WPA_SUPPLICANT)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/wpa_cli $(IDIR_WPA_SUPPLICANT)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/wpa_passphrase $(IDIR_WPA_SUPPLICANT)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/wpa_supplicant $(IDIR_WPA_SUPPLICANT)/usr/sbin/
	$(RSTRIP) $(IDIR_WPA_SUPPLICANT)
	$(IPKG_BUILD) $(IDIR_WPA_SUPPLICANT) $(PACKAGE_DIR)

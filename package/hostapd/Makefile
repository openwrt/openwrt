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

define PKG_build

ifneq ($(BR2_PACKAGE_$(1)),)
BUILD_TARGETS += $(PKG_INSTALL_DIR)/$(2)/hostapd
endif

$(PKG_INSTALL_DIR)/$(2)/hostapd: $(PKG_BUILD_DIR)/.prepared
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	$(CP) ./files/$(2).config $(PKG_BUILD_DIR)/.config
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include/madwifi" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		hostapd
	mkdir -p $(PKG_INSTALL_DIR)/$(2)
	$(CP) $(PKG_BUILD_DIR)/hostapd $(PKG_INSTALL_DIR)/$(2)/
	touch $$@

$$(IPKG_$(1)): $(PKG_INSTALL_DIR)/$(2)/hostapd
	install -m0755 -d $$(IDIR_$(1))/etc
	install -m0600 $(PKG_BUILD_DIR)/madwifi.conf $$(IDIR_$(1))/etc/hostapd.conf
	install -m0755 -d $$(IDIR_$(1))/usr/sbin
	install -m0755 $(PKG_INSTALL_DIR)/$(2)/hostapd $$(IDIR_$(1))/usr/sbin/
	$(RSTRIP) $$(IDIR_$(1))
	$(IPKG_BUILD) $$(IDIR_$(1)) $(PACKAGE_DIR)

endef

$(eval $(call PKG_template,HOSTAPD,hostapd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,HOSTAPD_MINI,hostapd-mini,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,HOSTAPD_UTILS,hostapd-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(eval $(call PKG_build,HOSTAPD,hostapd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_build,HOSTAPD_MINI,hostapd-mini,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(CP) ./files/hostapd.config $(PKG_BUILD_DIR)/.config
	touch $@

$(PKG_BUILD_DIR)/.built: $(BUILD_TARGETS)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include/madwifi" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		hostapd_cli
	touch $@

$(IPKG_HOSTAPD_UTILS): $(PKG_BUILD_DIR)/.built
	install -m0755 -d $(IDIR_HOSTAPD_UTILS)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/hostapd_cli $(IDIR_HOSTAPD_UTILS)/usr/sbin/
	$(RSTRIP) $(IDIR_HOSTAPD_UTILS)
	$(IPKG_BUILD) $(IDIR_HOSTAPD_UTILS) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built


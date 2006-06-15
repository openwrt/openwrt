# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=hostapd
PKG_VERSION:=0.5.3
PKG_RELEASE:=1
PKG_MD5SUM:=4e3134e8b0d86e831230f8c620fd81bb
PKG_BUILDDEP:=madwifi openssl

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk


define Build/ConfigureTarget
	$(CP) $(PKG_BUILD_DIR) $(PKG_BUILD_DIR)_$(1)
	$(CP) ./files/$(1).config $(PKG_BUILD_DIR)_$(1)/.config
endef

define Build/CompileTarget
	$(MAKE) -C $(PKG_BUILD_DIR)_$(1) \
		$(TARGET_CONFIGURE_OPTS) \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include/madwifi" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		hostapd hostapd_cli
	$(CP) $(PKG_BUILD_DIR)_$(1)/hostapd_cli $(PKG_BUILD_DIR)/
endef

define Package/InstallTemplate
	if [ \! -f "$(PKG_BUILD_DIR)_$(2)/hostapd" ]; then \
		rm -f $(PKG_BUILD_DIR)/.built; \
		$(MAKE) $(PKG_BUILD_DIR)/.built; \
	fi
	install -m0755 -d $$(1)/etc
	install -m0600 $(PKG_BUILD_DIR)_$(2)/madwifi.conf $$(1)/etc/hostapd.conf
	install -m0755 -d $$(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)_$(2)/hostapd $$(1)/usr/sbin/
endef

define Package/Template
 ifneq ($(CONFIG_PACKAGE_$(1)),)
  define Build/Configure/$(2)
	$(call Build/ConfigureTarget,$(2))
  endef
  define Build/Compile/$(2)
	$(call Build/CompileTarget,$(2))
  endef
 endif

 define Package/$(1)/install
	$(call Package/InstallTemplate,$(1),$(2))
 endef
endef

define Build/Configure
	$(call Build/Configure/default)
	$(call Build/Configure/mini)
endef

define Build/Compile
	$(call Build/Compile/default)
	$(call Build/Compile/mini)
endef

define Build/Clean
	rm -rf $(PKG_BUILD_DIR)_default
	rm -rf $(PKG_BUILD_DIR)_mini
endef

define Package/hostapd
SECTION:=net
CATEGORY:=Network
DEPENDS:=+libopenssl
TITLE:=IEEE 802.1x Authenticator
DESCRIPTION:=IEEE 802.1x/WPA/EAP/RADIUS Authenticator
URL:=http://hostap.epitest.fi/
endef

define Package/hostapd-mini
$(call Package/hostapd)
TITLE:=IEEE 802.1x Authenticator (WPA-PSK only)
DEPENDS:=
DESCRIPTION:=IEEE 802.1x/WPA/EAP/RADIUS Authenticator (WPA-PSK only)
endef

define Package/hostapd-utils
$(call Package/hostapd)
DEPENDS:=@PACKAGE_hostapd||PACKAGE_hostapd-mini
TITLE:=IEEE 802.1x Authenticator (utils)
DESCRIPTION:=IEEE 802.1x/WPA/EAP/RADIUS Authenticator (command line utils)
endef

define Package/hostapd-utils/install
	install -m0755 -d $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/hostapd_cli $(1)/usr/sbin/
endef


$(eval $(call Package/Template,hostapd,default))
$(eval $(call Package/Template,hostapd-mini,mini))

$(eval $(call BuildPackage,hostapd))
$(eval $(call BuildPackage,hostapd-mini))
$(eval $(call BuildPackage,hostapd-utils))


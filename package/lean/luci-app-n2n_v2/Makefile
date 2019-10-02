include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-n2n_v2
PKG_VERSION:=2.7.3
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=luci
  CATEGORY:=LuCI
  SUBMENU:=3. Applications
  TITLE:=LuCI support for N2N_V2.
  DEPENDS:=+n2n_v2 +libc +wget +ca-certificates
  PKGARCH:=all
endef

define Package/$(PKG_NAME)/description
	LuCI support for N2N_V2.
endef

define Build/Prepare
	$(foreach po,$(wildcard ${CURDIR}/files/usr/lib/lua/luci/i18n/*.po), \
		po2lmo $(po) $(PKG_BUILD_DIR)/$(patsubst %.po,%.lmo,$(notdir $(po)));)
endef

define Build/Compile
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
[ -n "${IPKG_INSTROOT}" ] || {
	(. /etc/uci-defaults/luci-n2n_v2) && rm -f /etc/uci-defaults/luci-n2n_v2
	exit 0
}
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/etc/uci-defaults/luci-n2n_v2 $(1)/etc/uci-defaults/luci-n2n_v2
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) ./files/usr/lib/lua/luci/controller/n2n_v2.lua $(1)/usr/lib/lua/luci/controller/n2n_v2.lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/n2n_v2.*.lmo $(1)/usr/lib/lua/luci/i18n/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DATA) ./files/usr/lib/lua/luci/model/cbi/n2n_v2.lua $(1)/usr/lib/lua/luci/model/cbi/n2n_v2.lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/view/n2n_v2
	$(INSTALL_DATA) ./files/usr/lib/lua/luci/view/n2n_v2/*.htm $(1)/usr/lib/lua/luci/view/n2n_v2/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/n2n_v2.config $(1)/etc/config/n2n_v2
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/n2n_v2.init $(1)/etc/init.d/n2n_v2
	$(INSTALL_DIR) $(1)/usr/share/n2n_v2
	$(INSTALL_BIN) ./files/n2n_v2.watchdog $(1)/usr/share/n2n_v2/n2n_v2-watchdog
	$(INSTALL_BIN) ./files/n2n_v2.check $(1)/usr/share/n2n_v2/n2n_v2-check
endef

$(eval $(call BuildPackage,$(PKG_NAME)))

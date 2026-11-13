include $(TOPDIR)/rules.mk

PKG_NAME:=glinet-panel-ui
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/blogic/glinet-panel-ui.git
PKG_MIRROR_HASH:=4c0b6172033d99be3a3fb3f44719064e11f93d8983e82a90e95e58a0c9fc8e12
PKG_SOURCE_DATE:=2026-10-22
PKG_SOURCE_VERSION:=61df0a1297c3831e2b89940efe9d41ecade70a2f

PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE.txt
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/glinet-panel-ui
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=GL.iNet TFT panel UI
  DEPENDS:=@TARGET_mediatek_filogic +ucode-mod-lvgl +ucode +ucode-mod-ubus +ucode-mod-uloop \
	+ucode-mod-uci +ucode-mod-fs +ucode-mod-math +ucode-mod-nl80211
endef

define Package/glinet-panel-ui/description
An LVGL based user interface for the TFT panels on the GL.iNet GL-BE10000
and GL-BE14000.
endef

define Build/Compile
endef

define Package/glinet-panel-ui/conffiles
/etc/config/glinet_panel
endef

define Package/glinet-panel-ui/install
	$(INSTALL_DIR) $(1)/usr/share/ucode/glinet-panel-ui $(1)/usr/share/glinet-panel-ui
	$(CP) $(PKG_BUILD_DIR)/ucode/. $(1)/usr/share/ucode/glinet-panel-ui/
	$(CP) $(PKG_BUILD_DIR)/assets/. $(1)/usr/share/glinet-panel-ui/
	$(INSTALL_DIR) $(1)/etc/init.d $(1)/etc/config $(1)/etc/glinet-panel-ui $(1)/lib/preinit
	$(INSTALL_BIN) ./files/etc/init.d/glinet-panel-ui $(1)/etc/init.d/
	$(INSTALL_CONF) ./files/etc/config/glinet_panel $(1)/etc/config/
	$(INSTALL_DATA) ./files/etc/glinet-panel-ui/preinit.conf $(1)/etc/glinet-panel-ui/
	$(INSTALL_DATA) ./files/lib/preinit/01_glinet-panel-boot $(1)/lib/preinit/
endef

$(eval $(call BuildPackage,glinet-panel-ui))

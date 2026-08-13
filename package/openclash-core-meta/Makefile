include $(TOPDIR)/rules.mk

PKG_NAME:=openclash-core-meta
PKG_VERSION:=1.19.29
PKG_RELEASE:=1

PKG_SOURCE:=mihomo-linux-arm64-v$(PKG_VERSION).gz
PKG_SOURCE_URL:=https://github.com/MetaCubeX/mihomo/releases/download/v$(PKG_VERSION)/
PKG_HASH:=9a868b5e4e0ad91d9d71e1b41b0cfce78aaba44360c30df74a723f8e3926a86c

PKG_MAINTAINER:=ZJJCKA
PKG_LICENSE:=GPL-3.0

include $(INCLUDE_DIR)/package.mk

define Package/openclash-core-meta
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Mihomo Meta core for OpenClash
  URL:=https://github.com/MetaCubeX/mihomo
  DEPENDS:=@aarch64
endef

define Package/openclash-core-meta/description
  Preinstalled Mihomo Meta core for luci-app-openclash.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	gzip -dc $(DL_DIR)/$(PKG_SOURCE) > $(PKG_BUILD_DIR)/clash_meta
endef

define Build/Compile
endef

define Package/openclash-core-meta/install
	$(INSTALL_DIR) $(1)/etc/openclash/core
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/clash_meta $(1)/etc/openclash/core/clash_meta
endef

$(eval $(call BuildPackage,openclash-core-meta))

include $(TOPDIR)/rules.mk

PKG_NAME:=wasp_downloader
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/wasp_downloader
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=AVM WASP firmware downloader
  URL:=https://github.com/andy/boeh/wasp_downloader.git
endef

define Package/wasp_downloader/description
   Download configuration files to AVM WASP devices
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/wasp_downloader/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wasp_downloader $(1)/usr/bin/
	[ -d $(PKG_BUILD_DIR)/files ] && $(CP) $(PKG_BUILD_DIR)/files/* $(1)/
endef

$(eval $(call BuildPackage,wasp_downloader))

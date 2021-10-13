include $(TOPDIR)/rules.mk

PKG_NAME:=bluez-ibeacon
PKG_RELEASE:=1

PKG_SOURCE_URL=https://github.com/blogic/bluez-ibeacon
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2022-10-31
PKG_SOURCE_VERSION:=07c082bf3e139ce061ff62a42b7876860256f4ea

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=MIT

include $(INCLUDE_DIR)/package.mk

define Package/bluez-ibeacon
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=bluez-ibeacon
  DEPENDS:=+bluez-libs
endef

define Build/Compile
	$(MAKE_VARS) $(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/bluez-beacon $(MAKE_FLAGS)
endef

define Package/bluez-ibeacon/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bluez-beacon/ibeacon $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/ibeacon $(1)/etc/init.d/ibeacon
endef

$(eval $(call BuildPackage,bluez-ibeacon))

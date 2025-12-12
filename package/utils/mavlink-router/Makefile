include $(TOPDIR)/rules.mk

PKG_NAME:=mavlinkrouter
PKG_RELEASE:=0.0.0

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://github.com/mavlink-router/mavlink-router
PKG_SOURCE_VERSION:=master

PKG_BUILD_DIR=$(BUILD_DIR)/$(PKG_NAME)/$(PKG_NAME)-$(PKG_VERSION)

# do we need this?
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/meson.mk

MESON_ARGS+=-Dsystemdsystemunitdir=/usr/lib/systemd/system

define Package/mavlinkrouter
 SECTION:=net
 CATEGORY:=Network
 TITLE:=mavlinkrouter
 DEPENDS:=+libstdcpp
endef

define Package/mavlinkrouter/description
	Route mavlink packets between endpoints
endef

define Package/mavlinkrouter/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/mavlink-routerd $(1)/usr/bin
endef

$(eval $(call BuildPackage,mavlinkrouter))

include $(TOPDIR)/rules.mk

PKG_NAME:=bluewan-auto
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/bluewan-auto
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Auto configure BLUE4WAN PPPoE + firewall
  DEPENDS:=+ppp +ppp-mod-pppoe
endef

define Package/bluewan-auto/description
 This package automatically configures a BLUE4WAN PPPoE interface
 and ensures it is always part of the WAN firewall zone.
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/bluewan-auto/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,bluewan-auto))

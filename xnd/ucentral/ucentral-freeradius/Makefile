include $(TOPDIR)/rules.mk

PKG_NAME:=ucentral-freeradius
PKG_RELEASE:=1

PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/ucentral-freeradius
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=ucentral-freeradius cfg wrapper
  DEPENDS:=+freeradius3-mod-eap +freeradius3-mod-files +freeradius3-mod-eap-pwd
endef

define Package/ucentral-freeradius/description
	Allow Wireless client rate limiting
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/ucentral-freeradius/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,ucentral-freeradius))

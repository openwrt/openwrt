include $(TOPDIR)/rules.mk

PKG_NAME:=chilli-redirect
PKG_RELEASE:=1

PKG_LICENSE:=BSD-3-Clause
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/chilli-redirect
  SUBMENU:=Captive Portals
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Chilli-Redirect
endef

define Package/chilli-redirect/description
 Chilli Captive portal redirect support.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/chilli-redirect/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,chilli-redirect))

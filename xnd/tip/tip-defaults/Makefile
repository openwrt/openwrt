include $(TOPDIR)/rules.mk

PKG_NAME:=tip-defaults
PKG_RELEASE:=1

PKG_LICENSE:=BSD-3-Clause
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/tip-defaults
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=tip-defaults
endef

define Package/tip-defaults/description
 The default configuration of the AP.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/tip-defaults/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,tip-defaults))

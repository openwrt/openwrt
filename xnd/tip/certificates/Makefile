include $(TOPDIR)/rules.mk

PKG_NAME:=certificates
PKG_RELEASE:=1

PKG_LICENSE:=BSD-3-Clause
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/certificates
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=TIP DigiCer certificate store
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/certificates/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,certificates))

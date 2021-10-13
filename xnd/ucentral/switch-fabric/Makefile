include $(TOPDIR)/rules.mk

PKG_NAME:=switch-fabric
PKG_RELEASE:=1

PKG_LICENSE:=BSD-3-Clause
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk

define Package/switch-fabric
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=switch-fabric
endef

define Package/switch-fabric/description
 Allow configuring HW specific switch fabric features.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/switch-fabric/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,switch-fabric))

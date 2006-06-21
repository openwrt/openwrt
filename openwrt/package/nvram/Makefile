# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=nvram
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/nvram
SECTION:=base
DEPENDS:=@LINUX_2_4_BRCM
TITLE:=Broadcom config utility
DESCRIPTION:=control utility for broadcom's 'nvram' config area
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Package/nvram/install
	install -d -m0755 $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib{nvram,shared}*.so $(1)/usr/lib/
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/nvram $(1)/usr/sbin/
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/lib{nvram,shared}*.so $(STAGING_DIR)/usr/lib/
endef

define Build/UninstallDev
	rm -f $(STAGING_DIR)/usr/lib/lib{nvram,shared}*.so
endef

$(eval $(call BuildPackage,nvram))

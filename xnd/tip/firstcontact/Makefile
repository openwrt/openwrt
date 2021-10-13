include $(TOPDIR)/rules.mk

PKG_NAME:=firstcontact
PKG_RELEASE:=1

PKG_LICENSE:=BSD-3-Clause
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/firstcontact
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=TIP DigiCert firstcontact
  DEPENDS:=+libubox +libcurl +libopenssl +certificates
endef

define Package/firstcontact/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firstcontact $(1)/usr/sbin/
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,firstcontact))

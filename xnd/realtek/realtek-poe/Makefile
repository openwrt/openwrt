include $(TOPDIR)/rules.mk

PKG_NAME:=realtek-poe
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/realtek-poe
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Realtek PoE Switch Port daemon
  DEPENDS:=+libubox +libubus +libuci
endef

define Package/realtek-poe/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/realtek-poe $(1)/usr/bin/
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,realtek-poe))

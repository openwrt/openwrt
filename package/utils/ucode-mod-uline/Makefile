include $(TOPDIR)/rules.mk

PKG_NAME:=ucode-mod-uline
PKG_RELEASE:=$(AUTORELEASE)
PKG_LICENSE:=GPL-2.0-or-later
PKG_MAINTAINER:=Felix Fietkau <nbd@nbd.name>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_INSTALL := 1

define Package/ucode-mod-uline
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ucode module for terminal line editing
  DEPENDS:=+libucode +libubox
endef

CMAKE_OPTIONS += -DUSE_SYSTEM_WCHAR=ON

define Package/ucode-mod-uline/description
This module provides similar functionality as libreadline for ucode, without
depending on other libraries like ncurses.
endef

define Package/ucode-mod-uline/install
	$(INSTALL_DIR) $(1)/usr/lib/ucode
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/ucode/uline.so $(1)/usr/lib/ucode/
endef

$(eval $(call BuildPackage,ucode-mod-uline))

include $(TOPDIR)/rules.mk

PKG_NAME:=zmsh-bootconfig
PKG_VERSION:=0.0.1
PKG_RELEASE:=1

PKG_MAINTAINER:=Paul Geraedts <p.f.j.geraedts@gmail.com>
PKG_LICENSE:=GPL-2.0-or-later OR MIT
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/zmsh-bootconfig
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=ZTE Bootconfig Manager
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/zmsh-bootconfig/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/zmsh-bootconfig $(1)/usr/bin/
endef

$(eval $(call BuildPackage,zmsh-bootconfig))

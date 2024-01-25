# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=poe-common
PGK_VERSION:=1
PKG_RELEASE:=1

PKG_MAINTAINER:=Sander Vanheule <sander@svanheule.net>
PKG_LICENSE:=GPL-2.0-or-later

include $(INCLUDE_DIR)/package.mk

define Package/poe-common
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Common base files for PoE
  DEPENDS:=+libubox +libuci
endef

define Package/poe-common/conffiles
/etc/config/poe
endef

define Package/poe-common/description
 Common base files for PoE configuration and control
endef

define Package/poe-common/install
	$(INSTALL_DIR) $(1)/etc/uci-defaults/
	$(INSTALL_CONF) ./files/etc/uci-defaults/30_poe $(1)/etc/uci-defaults/
endef

define Build/Compile
endef

$(eval $(call BuildPackage,poe-common))

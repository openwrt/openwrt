include $(TOPDIR)/rules.mk

PKG_NAME:=dynamic-vlan
PKG_RELEASE:=1

#PKG_SOURCE_URL=https://github.com/blogic/dynamic-vlan.git
#PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-06-04
PKG_SOURCE_VERSION:=55d78d3e7215b601084980d922349bcfdcf9cf20

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=BSD-3-Clause

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/dynamic-vlan
  SECTION:=ucentral
  CATEGORY:=uCentral
  TITLE:=dynamic VLAN netifd helper
  DEPENDS:=+libubox +libubus
endef

define Package/dynamic-vlan/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/init.d 
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dynamic-vlan $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/dynamic-vlan $(1)/etc/init.d/
endef

$(eval $(call BuildPackage,dynamic-vlan))

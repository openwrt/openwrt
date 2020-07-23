include $(TOPDIR)/rules.mk

PKG_NAME:=vfconfig
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/vfconfig
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
  TITLE:=UCI configuration support for VLAN filtering rules
  DEPENDS:= +ip-bridge +ip-full
  PKGARCH:=all
endef

define Package/vfconfig/description
 This package provides UCI configuration abstraction for VLAN filter rules
 on top of Linux bridge interfaces.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/vfconfig/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/vfconfig.hotplug $(1)/etc/hotplug.d/iface/01-vfconfig

	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_DATA) ./files/vfconfig.include $(1)/lib/network/vfconfig.sh

	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) ./files/vfconfig.sh $(1)/sbin/vfconfig
endef

$(eval $(call BuildPackage,vfconfig))

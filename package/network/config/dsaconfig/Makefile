include $(TOPDIR)/rules.mk

PKG_NAME:=dsaconfig
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/dsaconfig
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
  TITLE:=UCI configuration support for DSA switch VLAN filtering
  DEPENDS:= +ip-bridge +ip-full
  PKGARCH:=all
endef

define Package/dsaconfig/description
 This package provides UCI configuration abstraction for VLAN filter rules
 on top of DSA switches.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/dsaconfig/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/dsaconfig.hotplug $(1)/etc/hotplug.d/iface/01-dsaconfig

	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_DATA) ./files/dsaconfig.include $(1)/lib/network/dsaconfig.sh

	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) ./files/dsaconfig.sh $(1)/sbin/dsaconfig
endef

$(eval $(call BuildPackage,dsaconfig))

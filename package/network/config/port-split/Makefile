include $(TOPDIR)/rules.mk

PKG_NAME:=port-split
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/port-split
  SECTION:=net
  CATEGORY:=Network
  MAINTAINER:=Til Kaiser <mail@tk154.de>
  TITLE:=Port split config support
  DEPENDS:=+devlink
endef

define Package/port-split/description
 Port split config support in /etc/config/network
 to split front panel ports using the devlink utility.
endef

Build/Compile=

define Package/port-split/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/port_split $(1)/etc/init.d/
endef

$(eval $(call BuildPackage,port-split))

# Copyright 2019 Shun Li <riverscn@gmail.com>
#
# Licensed to the public under the GNU General Public License v3.

include $(TOPDIR)/rules.mk

PKG_NAME:=iptvhelper
PKG_VERSION:=0.1.1
PKG_RELEASE:=1

PKG_MAINTAINER:=Shun Li <riverscn@gmail.com>
PKG_LICENSE:=GPL-3.0

include $(INCLUDE_DIR)/package.mk

define Package/iptvhelper
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Routing and Redirection
  DEPENDS:= +ipset +iptables
  TITLE:=Scripts for configure IPTV easily
  PKGARCH:=all
endef

define Package/iptvhelper/description
  Scripts for configure IPTV easily
endef

define Package/iptvhelper/conffiles
/etc/config/iptvhelper
/etc/firewall.iptvhelper
endef

define Build/Compile
endef

define Package/iptvhelper/install
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,iptvhelper))

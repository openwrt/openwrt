#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=firewall

PKG_VERSION:=1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/firewall
  SECTION:=net
  CATEGORY:=Base system
  URL:=http://openwrt.org/
  TITLE:=OpenWrt firewall
  DEPENDS:=+iptables
endef

define Package/firewall/description
 firewall for openwrt, you can select if you want to use the old version or the new uci based script
endef

define Package/firewall/config
	source "$(SOURCE)/Config.in"
endef

define Build/Compile
	true
endef

ifeq ($(CONFIG_FIREWALL_NEW),y)
define Package/firewall/conffiles
/etc/config/firewall
endef

define Package/firewall/install
	$(INSTALL_DIR) $(1)/lib/firewall
	$(INSTALL_DATA) ./files/new/uci_firewall.sh $(1)/lib/firewall
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/new/firewall.config $(1)/etc/config/firewall
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/new/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/new/20-firewall $(1)/etc/hotplug.d/iface
endef

else

define Package/firewall/conffiles
/etc/firewall.config
/etc/firewall.user
endef

define Package/firewall/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/old/firewall.config $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/old/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_BIN) ./files/old/firewall.user $(1)/etc/
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DATA) ./files/old/firewall.awk $(1)/usr/lib
endef
endif
$(eval $(call BuildPackage,firewall))

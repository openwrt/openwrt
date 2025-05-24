include $(TOPDIR)/rules.mk

PKG_NAME:=udhcpsnoop
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/udhcpsnoop
  SECTION:=net
  CATEGORY:=Network
  TITLE:=DHCP Snooping Daemon
  DEPENDS:=+libubox +libubus +kmod-ifb +tc
endef

define Package/udhcpsnoop/install
	$(INSTALL_DIR) \
		$(1)/usr/sbin \
		$(1)/etc/init.d \
		$(1)/etc/config \
		$(1)/etc/hotplug.d/net \
		$(1)/etc/uci-defaults

	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udhcpsnoop $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/dhcpsnoop.init $(1)/etc/init.d/dhcpsnoop
	$(INSTALL_DATA) ./files/dhcpsnoop.conf $(1)/etc/config/dhcpsnoop
	$(INSTALL_DATA) ./files/dhcpsnoop.defaults $(1)/etc/uci-defaults/50-dhcpsnoop
	$(INSTALL_DATA) ./files/dhcpsnoop.hotplug $(1)/etc/hotplug.d/net/10-dhcpsnoop
endef

$(eval $(call BuildPackage,udhcpsnoop))

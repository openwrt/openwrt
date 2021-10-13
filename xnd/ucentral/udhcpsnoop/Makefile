include $(TOPDIR)/rules.mk

PKG_NAME:=udhcpsnoop
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

PKG_SOURCE_URL=https://github.com/blogic/udhcpsnoop.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-04-12
PKG_SOURCE_VERSION:=b86639904147a40be32ac43cd89c21109ffc3543

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/udhcpsnoop
  SECTION:=net
  CATEGORY:=Network
  TITLE:=DHCP Snooping Daemon
  DEPENDS:=+libubox +libubus +libuci
endef

define Package/udhcpsnoop/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udhcpsnoop $(1)/usr/sbin/
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,udhcpsnoop))

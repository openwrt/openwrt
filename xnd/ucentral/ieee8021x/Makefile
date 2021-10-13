include $(TOPDIR)/rules.mk

PKG_NAME:=ieee8021x
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=John Crispin <john@phrozen.org>

#PKG_SOURCE_URL=https://github.com/blogic/ieee8021x.git
#PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2021-04-12
PKG_SOURCE_VERSION:=c1f36559dc0ed2deeac0531a3d5854f1955ae928

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ieee8021x
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Wired 802.1x
  DEPENDS:=+libubox +libubus +libuci
endef

define Package/ieee8021x/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ieee8021x $(1)/usr/sbin/
	$(CP) ./files/* $(1)
endef

$(eval $(call BuildPackage,ieee8021x))

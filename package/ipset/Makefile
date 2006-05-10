# $Id$
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ipset
PKG_VERSION:=2.2.3
PKG_RELEASE:=1
PKG_MD5SUM:=1709424cc2cdb925d4fb6fd5fcaefc26

PKG_BUILD_DIR:=$(BUILD_DIR)/ipset-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-20050914.tar.bz2
PKG_SOURCE_URL:=http://ipset.netfilter.org
PKG_CAT:=bzcat

include $(TOPDIR)/package/rules.mk

define Package/ipset
SECTION:=base
CATEGORY:=Network
DEFAULT:=y
DEPENDS:=kmod-iptables-extra
TITLE:=Netfilter ip sets administration utility
DESCRIPTION:=Netfilter ip sets administration utility
URL:=http://ipset.netfilter.org/
endef

define Package/ipset/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR=$(IDIR_IPSET) PREFIX=/usr install
	rm -rf $(IDIR_IPSET)/usr/man
endef

$(eval $(call BuildPackage,ipset))
